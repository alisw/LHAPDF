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
    if (verbosity > 0)
      ss << set().name() << " PDF set, member #" << memberID()
         << ", version " << dataversion() << "; "
         << "LHAPDF ID = " << lhapdfID();
    if (verbosity > 2 && set().description().size() > 0)
      ss << "\n" << set().description();
    if (verbosity > 1 && description().size() > 0)
      ss << "\n" << description();
    if (verbosity > 2)
      ss << "\n" << "Flavor content = " << to_str(flavors());
    os << ss.str() << endl;
  }


}
