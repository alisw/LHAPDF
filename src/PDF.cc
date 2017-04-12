// -*- C++ -*-
//
// This file is part of LHAPDF
// Copyright (C) 2012-2014 The LHAPDF collaboration (see AUTHORS for details)
//
#include "LHAPDF/PDF.h"
#include "LHAPDF/PDFSet.h"
using namespace std;

namespace LHAPDF {


  void PDF::print(std::ostream& os, int verbosity) const {
    stringstream ss;
    if (verbosity > 0) {
      ss << set().name() << " PDF set, member #" << memberID()
         << ", version " << dataversion();
      if (lhapdfID() > 0)
        ss << "; LHAPDF ID = " << lhapdfID();
    }
    if (verbosity > 2 && set().description().size() > 0)
      ss << "\n" << set().description();
    if (verbosity > 1 && description().size() > 0)
      ss << "\n" << description();
    if (verbosity > 2)
      ss << "\n" << "Flavor content = " << to_str(flavors());
    os << ss.str() << endl;
  }


  int PDF::lhapdfID() const {
    //return set().lhapdfID() + memberID()
    /// @todo Add failure tolerance if pdfsets.index not found
    try {
      return lookupLHAPDFID(_setname(), memberID());
    } catch (const Exception&) {
      return -1; //< failure
    }
  }


  double PDF::quarkMass(int id) const {
    const unsigned int aid = std::abs(id);
    if (aid == 0 || aid > 6) return -1;
    const static string QNAMES[] = {"Down", "Up", "Strange", "Charm", "Bottom", "Top"}; ///< @todo Centralise?
    const size_t qid = aid - 1;
    const string qname = QNAMES[qid];
    return info().get_entry_as<double>("M" + qname, -1);
  }


  double PDF::quarkThreshold(int id) const {
    const unsigned int aid = std::abs(id);
    if (aid == 0 || aid > 6) return -1;
    const static string QNAMES[] = {"Down", "Up", "Strange", "Charm", "Bottom", "Top"}; ///< @todo Centralise?
    const size_t qid = aid - 1;
    const string qname = QNAMES[qid];
    return info().get_entry_as<double>("Threshold" + qname, quarkMass(id));
  }


}
