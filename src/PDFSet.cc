// -*- C++ -*-
//
// This file is part of LHAPDF
// Copyright (C) 2012-2014 The LHAPDF collaboration (see AUTHORS for details)
//
#include "LHAPDF/PDFSet.h"
#include <boost/math/distributions/chi_squared.hpp>

namespace LHAPDF {


  PDFSet::PDFSet(const string& setname) {
    /// @todo Hmm, this relies on the standard search path system ... no specific-path API.
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
    // Return -1 or similar invalid value if errorType is replica: requires changes in uncertainty code below.
    return get_entry_as<double>("ErrorConfLevel", (errorType() != "replicas") ? 100*boost::math::erf(1/sqrt(2)) : -1);
  }



  PDFUncertainty PDFSet::uncertainty(const vector<double>& values, double cl, bool alternative) const {
    if (size() <= 1)
      throw UserError("Error in LHAPDF::PDFSet::uncertainty. PDF set must contain more than just the central value.");
    if (values.size() != size())
      throw UserError("Error in LHAPDF::PDFSet::uncertainty. Input vector must contain values for all PDF members.");
    const size_t nmem = size()-1;

    // Get set- and requested conf levels (converted from %) and check sanity (req CL = set CL if cl < 0).
    // For replica sets, we internally use a nominal setCL corresponding to 1-sigma, since errorConfLevel() == -1.
    const double setCL = (errorType() != "replicas") ? errorConfLevel() / 100.0 : boost::math::erf(1/sqrt(2));
    const double reqCL = (cl >= 0) ? cl / 100.0 : setCL; // convert from percentage
    if (!in_range(reqCL, 0, 1) || !in_range(setCL, 0, 1))
      throw UserError("Error in LHAPDF::PDFSet::uncertainty. Requested or PDF set confidence level outside [0,1] range.");

    // Return value
    PDFUncertainty rtn;
    rtn.central = values[0];

    if (alternative && errorType() == "replicas") {

      // Compute median and requested CL directly from probability distribution of replicas.
      // Sort "values" into increasing order, ignoring zeroth member (average over replicas).
      vector<double> sorted = values;
      sort(sorted.begin()+1, sorted.end());
      const int nmem = size()-1;
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
      rtn.errsymm = 0.5*(rtn.errplus + rtn.errminus); // symmetrised

    } else if (alternative) {

      throw UserError("Error in LHAPDF::PDFSet::uncertainty. This PDF set is not in the format of replicas.");

    } else if (errorType() == "replicas") {

      // Calculate the average and standard deviation using Eqs. (2.3) and (2.4) of arXiv:1106.5788v2.
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

    } else if (errorType() == "symmhessian") {

      double errsymm = 0;
      for (size_t ieigen = 1; ieigen <= nmem; ieigen++)
	errsymm += sqr(values[ieigen]-values[0]);
      errsymm = sqrt(errsymm);
      rtn.errplus = rtn.errminus = rtn.errsymm = errsymm;

    } else if (errorType() == "hessian") {

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

    if (!alternative && (setCL != reqCL)) {
      // Apply scaling to Hessian sets or replica sets with alternative=false.

      // Calculate the qth quantile of the chi-squared distribution with one degree of freedom.
      // Examples: quantile(dist, q) = {0.988946, 1, 2.70554, 3.84146, 4} for q = {0.68, 1-sigma, 0.90, 0.95, 2-sigma}.
      boost::math::chi_squared dist(1);
      double qsetCL = boost::math::quantile(dist, setCL);
      double qreqCL = boost::math::quantile(dist, reqCL);
      // Scale uncertainties from the original set CL to the requested CL.
      const double scale = sqrt(qreqCL/qsetCL);
      rtn.scale = scale;
      rtn.errplus *= scale;
      rtn.errminus *= scale;
      rtn.errsymm *= scale;

    }

    return rtn;
  }



  double PDFSet::correlation(const vector<double>& valuesA, const vector<double>& valuesB) const {
    if (valuesA.size() != size() || valuesB.size() != size())
      throw UserError("Error in LHAPDF::PDFSet::correlation. Input vectors must contain values for all PDF members.");

    const PDFUncertainty errA = uncertainty(valuesA, -1);
    const PDFUncertainty errB = uncertainty(valuesB, -1);
    const size_t nmem = size()-1;

    double cor = 0.0;
    if (errorType() == "replicas" && nmem > 1) {

      // Calculate the correlation using Eq. (2.7) of arXiv:1106.5788v2.
      for (size_t imem = 1; imem <= nmem; imem++)
        cor += valuesA[imem] * valuesB[imem];
      cor = (cor/nmem - errA.central*errB.central) / (errA.errsymm*errB.errsymm) * nmem/(nmem-1.0);

    } else if (errorType() == "symmhessian") {

      for (size_t ieigen = 1; ieigen <= nmem; ieigen++)
        cor += (valuesA[ieigen]-errA.central) * (valuesB[ieigen]-errB.central);
      cor /= errA.errsymm * errB.errsymm;

    } else if (errorType() == "hessian") {

      // Calculate the correlation using Eq. (2.5) of arXiv:1106.5788v2.
      for (size_t ieigen = 1; ieigen <= nmem/2; ieigen++)
        cor += (valuesA[2*ieigen-1]-valuesA[2*ieigen]) * (valuesB[2*ieigen-1]-valuesB[2*ieigen]);
      cor /= 4.0 * errA.errsymm * errB.errsymm;

    }

    return cor;
  }


  double PDFSet::randomValueFromHessian(const vector<double>& values, const vector<double>& randoms, bool symmetrise) const {
    if (values.size() != size())
      throw UserError("Error in LHAPDF::PDFSet::randomValueFromHessian. Input vector must contain values for all PDF members.");

    double frand = 0.0;
    double scale = uncertainty(values).scale;
    size_t nmem = size()-1;

    // Allocate number of eigenvectors based on ErrorType.
    size_t neigen = 0;
    if (errorType() == "hessian") {
      neigen = nmem/2;
    } else if (errorType() == "symmhessian") {
      neigen = nmem;
    } else {
      throw UserError("Error in LHAPDF::PDFSet::randomValueFromHessian. This PDF set is not in the Hessian format.");
    }

    if (randoms.size() != neigen)
      throw UserError("Error in LHAPDF::PDFSet::randomValueFromHessian. Input vector must contain random numbers for all eigenvectors.");

    frand = values[0];

    if (errorType() == "symmhessian") {

      // Loop over number of eigenvectors.
      for (size_t ieigen = 1; ieigen <= neigen; ieigen++) {
        double r = randoms[ieigen-1]; // Gaussian random number
        frand += r*abs(values[ieigen]-values[0])*scale;
      }

    } else if (errorType() == "hessian") {

      // Use either Eq. (6.4) or Eq. (6.5) of arXiv:1205.4024v2.
      // Loop over number of eigenvectors.
      for (size_t ieigen = 1; ieigen <= neigen; ieigen++) {
        double r = randoms[ieigen-1]; // Gaussian random number
        if (symmetrise) {
          frand += 0.5*r*abs(values[2*ieigen-1]-values[2*ieigen]) * scale;
        } else { // not symmetrised
          if (r < 0.0) frand -= r*(values[2*ieigen]-values[0]) * scale; // negative direction
          else frand += r*(values[2*ieigen-1]-values[0]) * scale; // positive direction
        }
      }

    }

    return frand;
  }


}
