// -*- C++ -*-
//
// This file is part of LHAPDF
// Copyright (C) 2012-2022 The LHAPDF collaboration (see AUTHORS for details)
//
#include "LHAPDF/PDFSet.h"

namespace LHAPDF {


  string PDFErrInfo::qpartName(size_t iq) const {
    const EnvParts& eparts = qparts[iq];
    if (eparts.size() == 1) return eparts[0].first;
    string qname = "[";
    for (size_t ie = 0; ie < eparts.size(); ++ie) {
      qname += (ie == 0 ? "" : ",") + eparts[ie].first;
    }
    qname += "]";
    return qname;
  }

  vector<string> PDFErrInfo::qpartNames() const {
    vector<string> rtn;  rtn.reserve(qparts.size());
    for (size_t iq = 0; iq < qparts.size(); ++iq) rtn.push_back(qpartName(iq));
    return rtn;
  }


  size_t PDFErrInfo::nmemCore() const {
    return qparts[0][0].second;
  }

  size_t PDFErrInfo::nmemPar() const {
    size_t rtn = 0;
    for (size_t i = 1; i < qparts.size(); ++i) {
      for (size_t j = 1; j < qparts[i].size(); ++j) {
        rtn += qparts[i][j].second;
      }
    }
    return rtn;
  }





  PDFSet::PDFSet(const string& setname) {
    /// @todo Hmm, this relies on the standard search path system ... currently no way to provide a absolute path
    _setname = setname;
    const string setinfopath = findpdfsetinfopath(setname);
    if (!file_exists(setinfopath))
      throw ReadError("Info file not found for PDF set '" + setname + "'");
    // Load info file
    load(setinfopath);
    /// @todo Check that some mandatory metadata keys have been set: _check() function.
  }


  void PDFSet::print(ostream& os, int verbosity) const {
    stringstream ss;
    if (verbosity > 0)
      ss << name() << ", version " << dataversion() << "; " << size() << " PDF members";
    if (verbosity > 1)
      ss << "\n" << description();
    os << ss.str() << endl;
  }


  double PDFSet::errorConfLevel() const {
    // Return -1 or similar invalid value if errorType is replicas
    return get_entry_as<double>("ErrorConfLevel", (!startswith(errorType(), "replicas")) ? CL1SIGMA : -1);
  }



  /// Parse extended error type syntax
  PDFErrInfo PDFSet::errorInfo() const {
    if (_errinfo.qparts.size()) return _errinfo;

    // If not already populated, create the _errinfo cache object
    PDFErrInfo::QuadParts qparts;

    // Loop over the quadrature parts, separated by +  signs, after extracting the core part
    vector<string> quadstrs = split(errorType(), "+");
    qparts.reserve(quadstrs.size());
    qparts.push_back({{quadstrs[0], 0}});
    size_t nmempar = 0;

    for (size_t iq = 1; iq < quadstrs.size(); ++iq) {
      PDFErrInfo::EnvParts eparts;

      // Loop over any envelope components, separated by * signs
      const string& qstr = quadstrs[iq];
      vector<string> envstrs = split(qstr, "*");
      for (const string& estr : envstrs) {
        // Determine if a pair or singleton variation
        size_t esize = (startswith(estr, "#") || startswith(estr, "$")) ? 1 : 2;
        string ename = estr;
        eparts.push_back({ename,esize});
        nmempar += esize;
      }

      qparts.push_back(eparts);
    }

    // Finally, compute and set the size of the core errors
    qparts[0][0].second = errSize() - nmempar;

    // Set the cache and return
    _errinfo = PDFErrInfo(qparts, errorConfLevel(), errorType());
    return _errinfo;
  }


  void PDFSet::uncertainty(PDFUncertainty& rtn, const vector<double>& values, double cl, bool alternative) const {
    // rtn.clear();

    if (values.size() != size())
      throw UserError("Error in LHAPDF::PDFSet::uncertainty. Input vector must contain values for all PDF members.");

    // PDF members labelled 0 to nmem, excluding possible parameter variations.
    PDFErrInfo errinfo = errorInfo(); ///< @todo Avoid expensive recomputations... cache structure on PDFSet?
    size_t nmem = errinfo.nmemCore();
    if (nmem <= 0)
      throw UserError("Error in LHAPDF::PDFSet::uncertainty. PDF set must contain more than just the central value.");

    // Get set- and requested conf levels (converted from %) and check sanity (req CL = set CL if cl < 0).
    // For replica sets, we internally use a nominal setCL corresponding to 1-sigma, since errorConfLevel() == -1.
    const double setCL = (errinfo.coreType() != "replicas") ? errorConfLevel()/100.0 : CL1SIGMA/100.0;
    const double reqCL = (cl >= 0) ? cl / 100.0 : setCL; // convert from percentage
    if (!in_range(reqCL, 0, 1) || !in_range(setCL, 0, 1))
      throw UserError("Error in LHAPDF::PDFSet::uncertainty. Requested or PDF set confidence level outside [0,1] range.");

    // Central value
    rtn.central = values[0];


    // Compute core uncertainty component
    if (errinfo.coreType() == "replicas") {

      if (alternative) {
        // Compute median and requested CL directly from probability distribution of replicas.
        // Sort "values" into increasing order, ignoring zeroth member (average over replicas).
        // Also ignore possible parameter variations included at the end of the set.
        vector<double> sorted(nmem);
        copy(values.begin()+1, values.begin()+1+nmem+1, sorted.begin());
        sort(sorted.begin(), sorted.end());
        // Define central value to be median.
        if (nmem % 2) { // odd nmem => one middle value
          rtn.central = sorted[nmem/2 + 1];
        } else { // even nmem => average of two middle values
          rtn.central = 0.5*(sorted[nmem/2] + sorted[nmem/2 + 1]);
        }
        // Define uncertainties via quantiles with a CL given by reqCL.
        const int upper = round(0.5*(1+reqCL)*nmem); // round to nearest integer
        const int lower = 1 + round(0.5*(1-reqCL)*nmem); // round to nearest integer
        rtn.errplus = sorted[upper] - rtn.central;
        rtn.errminus = rtn.central - sorted[lower];
        rtn.errsymm = (rtn.errplus + rtn.errminus)/2.0; // symmetrised

      } else {

        // Calculate the average and standard deviation using Eqs. (2.3) and (2.4) of arXiv:1106.5788v2
        double av = 0.0, sd = 0.0;
        for (size_t imem = 1; imem <= nmem; imem++) {
          av += values[imem];
          sd += sqr(values[imem]);
        }
        av /= nmem; sd /= nmem;
        sd = nmem/(nmem-1.0)*(sd-sqr(av));
        sd = (sd > 0.0 && nmem > 1) ? sqrt(sd) : 0.0;
        rtn.central = av;
        rtn.errplus = rtn.errminus = rtn.errsymm = sd;
      }

    } else if (errinfo.coreType() == "symmhessian") {

      double errsymm = 0;
      for (size_t ieigen = 1; ieigen <= nmem; ieigen++) {
        errsymm += sqr(values[ieigen]-values[0]);
      }
      errsymm = sqrt(errsymm);
      rtn.errplus = rtn.errminus = rtn.errsymm = errsymm;

    } else if (errinfo.coreType() == "hessian") {

      // Calculate the asymmetric and symmetric Hessian uncertainties
      // using Eqs. (2.1), (2.2) and (2.6) of arXiv:1106.5788v2.
      double errplus = 0, errminus = 0, errsymm = 0;
      for (size_t ieigen = 1; ieigen <= nmem/2; ieigen++) {
        errplus += sqr(max(max(values[2*ieigen-1]-values[0],values[2*ieigen]-values[0]), 0.0));
        errminus += sqr(max(max(values[0]-values[2*ieigen-1],values[0]-values[2*ieigen]), 0.0));
        errsymm += sqr(values[2*ieigen-1]-values[2*ieigen]);
      }
      rtn.errsymm = 0.5*sqrt(errsymm);
      rtn.errplus = sqrt(errplus);
      rtn.errminus = sqrt(errminus);

    } else {
      throw MetadataError("\"ErrorType: " + errorType() + "\" not supported by LHAPDF::PDFSet::uncertainty.");
    }


    // Apply scaling to Hessian sets or replica sets with alternative=false.
    if (setCL != reqCL) {

      // Calculate the qth quantile of the chi-squared distribution with one degree of freedom.
      // Examples: quantile(dist, q) = {0.988946, 1, 2.70554, 3.84146, 4} for q = {0.68, 1-sigma, 0.90, 0.95, 2-sigma}.
      double qsetCL = chisquared_quantile(setCL, 1);
      double qreqCL = chisquared_quantile(reqCL, 1);
      // Scale uncertainties from the original set CL to the requested CL.
      const double scale = sqrt(qreqCL/qsetCL);
      rtn.scale = scale;
      if (!alternative) {
        rtn.errplus *= scale;
        rtn.errminus *= scale;
        rtn.errsymm *= scale;
      }

    }

    // Store core variation uncertainties
    rtn.errplus_pdf = rtn.errplus;
    rtn.errminus_pdf = rtn.errminus;
    rtn.errsymm_pdf = rtn.errsymm;
    rtn.errparts.push_back({rtn.errplus_pdf, rtn.errminus_pdf}); ///< @note (+,-) pair-ordering


    // Compute signed parameter-variation errors
    double errsq_par_plus = 0, errsq_par_minus = 0;
    size_t index = nmem;
    for (size_t iq = 1; iq < errinfo.qparts.size(); ++iq) {
      const PDFErrInfo::EnvParts& eparts = errinfo.qparts[iq];

      // Build envelope from pairs, singletons, or symmetrised singletons
      double vmin = rtn.central, vmax = rtn.central;
      for (const PDFErrInfo::EnvPart& epart : eparts) {
        /// @todo Handle #/$ one-side / symm / standard pair-building

        const bool symm = startswith(epart.first, "$");
        for (size_t ie = 0; ie < epart.second; ++ie) {
          index += 1;
          if (!symm) {
            vmin = min(values[index], vmin);
            vmax = max(values[index], vmax);
          } else {
            const double delta = values[index] - rtn.central;
            vmin = min({values[index], rtn.central-delta, vmin});
            vmax = max({values[index], rtn.central-delta, vmax});
          }
        }
      }
      const double eplus = vmax - rtn.central;
      const double eminus = rtn.central - vmin;
      rtn.errparts.push_back({eplus,eminus});
      errsq_par_plus += sqr(eplus);
      errsq_par_minus += sqr(eminus);
    }

    // Add the parameter-variation uncertainty to total, with same scaling as for PDF uncertainty.
    rtn.errplus_par = rtn.scale * sqrt(errsq_par_plus);
    rtn.errminus_par = rtn.scale * sqrt(errsq_par_minus);
    rtn.errsymm_par = (rtn.errplus_par + rtn.errminus_par)/2.0;
    rtn.err_par = rtn.errsymm_par; ///< @todo Remove

    // Add parameter variation uncertainties in quadrature with PDF uncertainty.
    rtn.errplus = sqrt( sqr(rtn.errplus_pdf) + sqr(rtn.errplus_par) );
    rtn.errminus = sqrt( sqr(rtn.errminus_pdf) + sqr(rtn.errminus_par) );
    rtn.errsymm = (rtn.errplus + rtn.errminus)/2.0;
  }


  void PDFSet::uncertainties(vector<PDFUncertainty>& rtn,
                             const vector<vector<double>>& observables_values,
                             double cl, bool alternative) const {
    rtn.clear();
    rtn.reserve(observables_values.size());
    for (const vector<double>& values : observables_values) {
      rtn.push_back( uncertainty(values, cl, alternative) );
    }
  }


  double PDFSet::correlation(const vector<double>& valuesA, const vector<double>& valuesB) const {
    if (valuesA.size() != size() || valuesB.size() != size())
      throw UserError("Error in LHAPDF::PDFSet::correlation. Input vectors must contain values for all PDF members.");

    const PDFUncertainty errA = uncertainty(valuesA, -1);
    const PDFUncertainty errB = uncertainty(valuesB, -1);

    double cor = 0.0;
    const PDFErrInfo errinfo = errorInfo();
    if (errinfo.coreType() == "replicas" && errinfo.nmemCore() > 1) {

      // Calculate the correlation over replicas using Eq. (2.7) of arXiv:1106.5788
      for (size_t imem = 1; imem < errinfo.nmemCore()+1; imem++) {
        cor += valuesA[imem] * valuesB[imem];
      }
      cor = (cor/errinfo.nmemCore() - errA.central*errB.central) / (errA.errsymm_pdf*errB.errsymm_pdf);
      cor *= errinfo.nmemCore() / (errinfo.nmemCore()-1.0); //< bias correction

    } else if (errinfo.coreType() == "symmhessian") {

      // Calculate the correlation over symmetrised Hessian vectors
      for (size_t ieigen = 1; ieigen < errinfo.nmemCore()+1; ieigen++) {
        cor += (valuesA[ieigen]-errA.central) * (valuesB[ieigen]-errB.central);
      }
      cor /= errA.errsymm_pdf * errB.errsymm_pdf;

    } else if (errinfo.coreType() == "hessian") {

      // Calculate the correlation over asymm Hessian vectors using Eq. (2.5) of arXiv:1106.5788
      for (size_t ieigen = 1; ieigen < errinfo.nmemCore()/2 + 1; ieigen++) {
        cor += (valuesA[2*ieigen-1]-valuesA[2*ieigen]) * (valuesB[2*ieigen-1]-valuesB[2*ieigen]);
      }
      cor /= 4.0 * errA.errsymm_pdf * errB.errsymm_pdf;

    }

    return cor;
  }


  double PDFSet::randomValueFromHessian(const vector<double>& values, const vector<double>& randoms, bool symmetrise) const {
    if (values.size() != size())
      throw UserError("Error in LHAPDF::PDFSet::randomValueFromHessian. Input vector must contain values for all PDF members.");

    const PDFErrInfo errinfo = errorInfo();
    if (!contains(errinfo.coreType(), "hessian")) {
      throw UserError("Error in LHAPDF::PDFSet::randomValueFromHessian. This PDF set is not in the Hessian format.");
    }

    const size_t neigen = (errinfo.coreType() == "hessian") ? errinfo.nmemCore()/2 : errinfo.nmemCore();
    if (randoms.size() != neigen) {
      throw UserError("Error in LHAPDF::PDFSet::randomValueFromHessian. Input vector must contain random numbers for all eigenvectors.");
    }

    PDFUncertainty unc = uncertainty(values);
    double frand = values[0];
    if (errinfo.coreType() == "symmhessian") {

      // Loop over number of eigenvectors.
      for (size_t ieigen = 1; ieigen <= neigen; ieigen++) {
        double r = randoms[ieigen-1]; // Gaussian random number
        frand += r*(values[ieigen]-values[0]) * unc.scale;
      }

    } else if (errinfo.coreType() == "hessian") {

      // Use either Eq. (6.4) or corrected Eq. (6.5) of arXiv:1205.4024v2.
      // Loop over number of eigenvectors.
      for (size_t ieigen = 1; ieigen <= neigen; ieigen++) {
        double r = randoms[ieigen-1]; // Gaussian random number
        if (symmetrise) {
          frand += 0.5*r*(values[2*ieigen-1]-values[2*ieigen]) * unc.scale;
        } else { // not symmetrised
          if (r < 0.0) frand -= r*(values[2*ieigen]-values[0]) * unc.scale; // negative direction
          else frand += r*(values[2*ieigen-1]-values[0]) * unc.scale; // positive direction
        }
      }

    }

    return frand;
  }


  void PDFSet::_checkPdfType(const vector<string>& pdftypes) const {
    if (pdftypes.size() != size())
      throw UserError("Error in LHAPDF::PDFSet::checkPdfType. Input vector must contain values for all PDF members.");
    const PDFErrInfo errinfo = errorInfo();

    // Check that zeroth member has "PdfType: central".
    if (pdftypes[0] != "central")
      throw MetadataError("Member 0, \"PdfType: " + pdftypes[0] + "\" should be \"PdfType: central\".");

    // Check that PDF-error members have "PdfType: replica" or "PdfType: error".
    string exptype = "";
    if (errinfo.coreType() == "replicas") {
      exptype = "replica";
    } else if (contains(errorType(), "hessian")) {
      exptype = "error";
    } else {
      throw MetadataError("\"ErrorType: " + errorType() + "\" not supported by LHAPDF::PDFSet::checkPdfType.");
    }
    for (size_t imem = 1; imem < errinfo.nmemCore()+1; imem++) {
      if (pdftypes[imem] != exptype) {
        throw MetadataError("Member " + to_str(imem) + ", \"PdfType: " + pdftypes[imem] + "\" should be \"PdfType: " + exptype + "\".");
      }
    }

    // Check that possible parameter variations have "PdfType: central" or "PdfType: variation".
    for (size_t imem = errinfo.nmemCore()+1; imem < size(); imem++) {
      if (pdftypes[imem] != "central" && pdftypes[imem] != "variation") {
        throw MetadataError("Member " + to_str(imem) + ", \"PdfType: " + pdftypes[imem] + "\" should be \"PdfType: central\" or \"PdfType: variation\".");
      }
    }

    //cout << "Success: PdfType of each member matches the ErrorType of the set." << endl;
  }


}
