// -*- C++ -*-
//
// This file is part of LHAPDF
// Copyright (C) 2012-2014 The LHAPDF collaboration (see AUTHORS for details)
//
#include "LHAPDF/Info.h"
#include "LHAPDF/Config.h"
#include "LHAPDF/PDFSet.h"
#include "LHAPDF/PDFInfo.h"
#include "LHAPDF/PDF.h"
#include "LHAPDF/GridPDF.h"
#include "LHAPDF/BilinearInterpolator.h"
#include "LHAPDF/BicubicInterpolator.h"
#include "LHAPDF/LogBilinearInterpolator.h"
#include "LHAPDF/LogBicubicInterpolator.h"
#include "LHAPDF/ErrExtrapolator.h"
#include "LHAPDF/NearestPointExtrapolator.h"
#include "LHAPDF/ContinuationExtrapolator.h"
#include "LHAPDF/AlphaS.h"

namespace LHAPDF {


  Info& getConfig() {
    return Config::get();
  }


  PDFSet& getPDFSet(const string& setname) {
    static map<string, PDFSet> _sets;
    map<string, PDFSet>::iterator it = _sets.find(setname);
    if (it != _sets.end()) return it->second;
    _sets[setname] = PDFSet(setname);
    return _sets[setname];
  }


  PDFInfo* mkPDFInfo(const std::string& setname, int member) {
    //return new Info(findpdfmempath(setname, member));
    return new PDFInfo(setname, member);
  }

  PDFInfo* mkPDFInfo(int lhaid) {
    const pair<string,int> setname_memid = lookupPDF(lhaid);
    return mkPDFInfo(setname_memid.first, setname_memid.second);
  }


  PDF* mkPDF(const string& setname, int member) {
    // Find the member data file and ensure that it exists
    const string searchpath = findpdfmempath(setname, member);
    if (searchpath.empty()) {
      const int setsize = getPDFSet(setname).size();
      if (member > setsize-1)
        throw UserError("PDF " + setname + "/" + to_str(member) + " is out of the member range of set " + setname);
      throw UserError("Can't find a valid PDF " + setname + "/" + to_str(member));
    }
    // First create an Info object to work out what format of PDF this is:
    Info info(searchpath);
    const string fmt = info.get_entry("Format");
    // Then use the format information to call the appropriate concrete PDF constructor:
    if (fmt == "lhagrid1") return new GridPDF(setname, member);
    /// @todo Throw a deprecation error if format version is too old or new
    throw FactoryError("No LHAPDF factory defined for format type '" + fmt + "'");
  }


  namespace { // Keep this in an unnamed namespace for now, until stable/final for API


    // /// @brief Expand a string describing a chain of PDFs with wildcards into a list of definite chain strings
    // ///
    // /// Operation:
    // ///   SETA/1 * SETB/*     or     SETA/1 * SETB
    // ///   ->
    // ///   [SETA/1, SETB/*]
    // ///   ->
    // ///   [SETA/1 * SETB/0, SETA/1 * SETB/1, ...] (via PDFSet)
    // ///
    // /// @todo Handle more complex wildcards such as member ID ranges
    // ///
    // vector<string> expandPDFsStr(const string& pdfsstr) {
    //   // Split into multiplicative parts
    //   vector<string> parts;
    //   iter_split(parts, pdfsstr, boost::algorithm::first_finder(" * "));
    //   BOOST_FOREACH (string& part, parts) trim(part);

    //   // Expand each part into a single PDF spec and append it
    //   vector<string> rtn;
    //   BOOST_FOREACH (string part, parts) {
    //     // No member number corresponds to whole-set wildcard expansion in this context
    //     if (!contains(part, "/")) part += "/*";

    //     // Make a list of strings corresponding to the expansion of the current part
    //     vector<string> expandedpart;
    //     if (endswith(part, "/*")) {
    //       const string setname = part.substr(0, part.find("/*"));
    //       const PDFSet& set = getPDFSet(setname);
    //       for (size_t i = 0; i < set.size(); ++i)
    //         expandedpart.push_back(setname + "/" + to_str(i));
    //     } else {
    //       expandedpart.push_back(part);
    //     }

    //     // Append to or outer-product the return vector as appropriate
    //     if (expandedpart.size() == 1) {
    //       // If there were no wildcards to expand, no point in playing outer-product games
    //       BOOST_FOREACH (string& basestr, rtn) {
    //         const string newstr = (!basestr.empty() ? basestr + " * " : "") + expandedpart[0];
    //       }
    //     } else {
    //       // There was a wildcard to expand, so we work harder...
    //       vector<string> tmp;
    //       tmp.reserve(expandedpart.size() * rtn.size());
    //       BOOST_FOREACH (const string& basestr, rtn) {
    //         BOOST_FOREACH (const string& pdfstr, expandedpart) {
    //           const string newstr = (!basestr.empty() ? basestr + " * " : "") + pdfstr;
    //           tmp.push_back(newstr);
    //         }
    //       }
    //       rtn = tmp; //< Update rtn with the next level of Cartesian product from wildcard expansion
    //     }
    //   }
    //   return rtn;
    // }


    /// @brief Decode a single PDF member ID string into a setname,memid pair
    ///
    ///  SETA/1      SETA
    ///   ->          ->
    ///   <SETA,1>    <SETA,0>
    pair<string,int> decodePDFStr(const string& pdfstr) {
      int nmem = 0;
      const size_t slashpos = pdfstr.find("/");
      const string setname = trim_copy(pdfstr.substr(0, slashpos));
      try {
        if (slashpos != string::npos) {
          const string smem = pdfstr.substr(slashpos+1);
          nmem = lexical_cast<int>(smem);
        }
      } catch (...) {
        throw UserError("Could not parse PDF identity string " + pdfstr);
      }
      return make_pair(setname, nmem);
    }


    // /// @brief Decode a multiple PDF member ID string into a list of setname,memid pairs
    // ///
    // ///   SETA/1 * SETB/0
    // ///   ->
    // ///   [<SETA,1>, <SETB,0>]
    // vector< pair<string,int> > decodePDFsStr(const string& pdfsstr) {
    //   // Split into multiplicative parts
    //   vector<string> parts;
    //   iter_split(parts, pdfsstr, boost::algorithm::first_finder(" * "));

    //   // Create the list of set/id pairs
    //   vector< pair<string,int> > rtn;
    //   BOOST_FOREACH (const string& part, parts) {
    //     rtn.push_back(decodePDFStr(part));
    //   }
    //   return rtn;
    // }


  }



  PDF* mkPDF(const string& setname_nmem) {
    const pair<string,int> idpair = decodePDFStr(setname_nmem);
    return mkPDF(idpair.first, idpair.second);
  }


  PDF* mkPDF(int lhaid) {
    const pair<string,int> setname_nmem = lookupPDF(lhaid);
    return mkPDF(setname_nmem.first, setname_nmem.second);
  }


  void mkPDFs(const string& setname, vector<PDF*>& pdfs) {
    getPDFSet(setname).mkPDFs(pdfs);
  }


  vector<PDF*> mkPDFs(const string& setname) {
    return getPDFSet(setname).mkPDFs();
  }


  Interpolator* mkInterpolator(const string& name) {
    // Convert name to lower case for comparisons
    const string iname = to_lower_copy(name);
    if (iname == "linear")
      return new BilinearInterpolator();
    else if (iname == "cubic")
      return new BicubicInterpolator();
    else if (iname == "log")
      return new LogBilinearInterpolator();
    else if (iname == "logcubic")
      return new LogBicubicInterpolator();
    else
      throw FactoryError("Undeclared interpolator requested: " + name);
  }


  Extrapolator* mkExtrapolator(const string& name) {
    // Convert name to lower case for comparisons
    const string iname = to_lower_copy(name);
    if (iname == "nearest")
      return new NearestPointExtrapolator();
    else if (iname == "error")
      return new ErrExtrapolator();
    else if (iname == "continuation")
      return new ContinuationExtrapolator();
    else
      throw FactoryError("Undeclared extrapolator requested: " + name);
  }


  AlphaS* mkAlphaS(const Info& info) {
    AlphaS* as = 0;

    const string itype = to_lower_copy(info.get_entry("AlphaS_Type"));
    if (itype == "analytic") as = new AlphaS_Analytic();
    else if (itype == "ode") as = new AlphaS_ODE();
    else if (itype == "ipol") as = new AlphaS_Ipol();
    else throw FactoryError("Undeclared AlphaS requested: " + itype);

    // Configure the QCD params on this AlphaS
    if (info.has_key("AlphaS_OrderQCD")) as->setOrderQCD(info.get_entry_as<int>("AlphaS_OrderQCD"));
    /// @todo Fall back to generic OrderQCD? (Default value is 4 at the moment --KN)

    /// Thresholds are where the flavor transition happens in the ODE/analytic solvers, AlphaS_MQ
    /// should be given as \bar{MQ} (\bar{MQ}) if it is to be used for thresholds away from the mass.
    /// Since you need the heavy quark mass to calculate the decoupling in the ODE solver
    /// when threshold =/= mass, I've implemented this as AlphaS_Threshold* -> Threshold*, AlphaS_M* -> M*

    if (info.has_key("AlphaS_ThresholdDown") && info.has_key("AlphaS_ThresholdUp") && info.has_key("AlphaS_ThresholdStrange")
      && info.has_key("AlphaS_ThresholdCharm") && info.has_key("AlphaS_ThresholdBottom") && info.has_key("AlphaS_ThresholdTop")) {
      as->setQuarkThreshold(1, info.get_entry_as<double>("AlphaS_ThresholdDown"));
      as->setQuarkThreshold(2, info.get_entry_as<double>("AlphaS_ThresholdUp"));
      as->setQuarkThreshold(3, info.get_entry_as<double>("AlphaS_ThresholdStrange"));
      as->setQuarkThreshold(4, info.get_entry_as<double>("AlphaS_ThresholdCharm"));
      as->setQuarkThreshold(5, info.get_entry_as<double>("AlphaS_ThresholdBottom"));
      as->setQuarkThreshold(6, info.get_entry_as<double>("AlphaS_ThresholdTop"));
    }
    else if (info.has_key("ThresholdDown") && info.has_key("ThresholdUp") && info.has_key("ThresholdStrange")
      && info.has_key("ThresholdCharm") && info.has_key("ThresholdBottom") && info.has_key("ThresholdTop")) {
      as->setQuarkThreshold(1, info.get_entry_as<double>("ThresholdDown"));
      as->setQuarkThreshold(2, info.get_entry_as<double>("ThresholdUp"));
      as->setQuarkThreshold(3, info.get_entry_as<double>("ThresholdStrange"));
      as->setQuarkThreshold(4, info.get_entry_as<double>("ThresholdCharm"));
      as->setQuarkThreshold(5, info.get_entry_as<double>("ThresholdBottom"));
      as->setQuarkThreshold(6, info.get_entry_as<double>("ThresholdTop"));
    }

    if (info.has_key("AlphaS_MDown") && info.has_key("AlphaS_MUp") && info.has_key("AlphaS_MStrange")
      && info.has_key("AlphaS_MCharm") && info.has_key("AlphaS_MBottom") && info.has_key("AlphaS_MTop")) {
      as->setQuarkMass(1, info.get_entry_as<double>("AlphaS_MDown"));
      as->setQuarkMass(2, info.get_entry_as<double>("AlphaS_MUp"));
      as->setQuarkMass(3, info.get_entry_as<double>("AlphaS_MStrange"));
      as->setQuarkMass(4, info.get_entry_as<double>("AlphaS_MCharm"));
      as->setQuarkMass(5, info.get_entry_as<double>("AlphaS_MBottom"));
      as->setQuarkMass(6, info.get_entry_as<double>("AlphaS_MTop"));
    }
    // This falls back to lhapdf.conf so should in theory never throw the MetadataError
    else if (info.has_key("MDown") && info.has_key("MUp") && info.has_key("MStrange")
      && info.has_key("MCharm") && info.has_key("MBottom") && info.has_key("MTop")) {
      as->setQuarkMass(1, info.get_entry_as<double>("MDown"));
      as->setQuarkMass(2, info.get_entry_as<double>("MUp"));
      as->setQuarkMass(3, info.get_entry_as<double>("MStrange"));
      as->setQuarkMass(4, info.get_entry_as<double>("MCharm"));
      as->setQuarkMass(5, info.get_entry_as<double>("MBottom"));
      as->setQuarkMass(6, info.get_entry_as<double>("MTop"));
    } else {
      throw MetadataError("All quark masses required (either as AlphaS_MQ or MQ) for AlphaS.");
    }

    const string fscheme = to_lower_copy(info.get_entry("AlphaS_FlavorScheme", info.get_entry("FlavorScheme", "variable"))); // default is VFNS
    const int nflavs = info.get_entry_as<int>("AlphaS_NumFlavors", info.get_entry_as<int>("NumFlavors", 5)); // default is 5 flavour evolution
    if (fscheme == "fixed") as->setFlavorScheme(AlphaS::FIXED, nflavs);
    else if (fscheme == "variable") as->setFlavorScheme(AlphaS::VARIABLE, nflavs);
    else as->setFlavorScheme(AlphaS::VARIABLE, 5); // default fallback mode

    // Required parameter settings for each calculation mode
    if (as->type() == "ode") {
      /// @todo Handle FFNS / VFNS
      if ( (!info.has_key("AlphaS_MZ") || !info.has_key("MZ")) || (!info.has_key("AlphaS_MassReference") || !info.has_key("AlphaS_Reference")) )
        throw MetadataError("Requested ODE AlphaS but there is no reference point given: define either AlphaS_MZ and MZ, or AlphaS_MassReference and AlphaS_Reference. The latter is given preference if both are defined.");
      if (info.has_key("AlphaS_MZ")) as->setAlphaSMZ(info.get_entry_as<double>("AlphaS_MZ"));
      if (info.has_key("MZ"))as->setMZ(info.get_entry_as<double>("MZ"));
      if (info.has_key("AlphaS_Reference"))as->setAlphaSReference(info.get_entry_as<double>("AlphaS_Reference"));
      if (info.has_key("AlphaS_MassReference"))as->setMassReference(info.get_entry_as<double>("AlphaS_MassReference"));
      if (info.has_key("AlphaS_Qs")) {
        AlphaS_ODE* as_o = dynamic_cast<AlphaS_ODE*>(as);
        if (info.has_key("AlphaS_Qs")) as_o->setQValues( info.get_entry_as< vector<double> >("AlphaS_Qs"));
      }
    }
    else if (as->type() == "analytic") {
      /// @todo Handle FFNS / VFNS
      if (!info.has_key("AlphaS_Lambda5") && !info.has_key("AlphaS_Lambda4") && !info.has_key("AlphaS_Lambda3") )
        throw MetadataError("Requested analytic AlphaS but the required parameters are not defined.");
      if (info.has_key("AlphaS_Lambda3")) as->setLambda(3, info.get_entry_as<double>("AlphaS_Lambda3"));
      if (info.has_key("AlphaS_Lambda4")) as->setLambda(4, info.get_entry_as<double>("AlphaS_Lambda4"));
      if (info.has_key("AlphaS_Lambda5")) as->setLambda(5, info.get_entry_as<double>("AlphaS_Lambda5"));
    }
    else if (as->type() == "ipol") {
      if (!info.has_key("AlphaS_Qs") || !info.has_key("AlphaS_Vals") )
        throw MetadataError("Requested ipol AlphaS but the required parameters are not defined.");
      AlphaS_Ipol* as_i = dynamic_cast<AlphaS_Ipol*>(as);
      if (info.has_key("AlphaS_Qs")) as_i->setQValues( info.get_entry_as< vector<double> >("AlphaS_Qs"));
      if (info.has_key("AlphaS_Vals")) as_i->setAlphaSValues( info.get_entry_as< vector<double> >("AlphaS_Vals"));
    }

    return as;
  }


  AlphaS* mkAlphaS(const std::string& setname) {
    return mkAlphaS(getPDFSet(setname));
  }


  AlphaS* mkAlphaS(const std::string& setname, int member) {
    unique_ptr<Info> info( mkPDFInfo(setname, member) );
    return mkAlphaS(*info);
  }


  AlphaS* mkAlphaS(int lhaid) {
    unique_ptr<Info> info( mkPDFInfo(lhaid) );
    return mkAlphaS(*info);
  }


}
