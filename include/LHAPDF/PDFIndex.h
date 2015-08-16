// -*- C++ -*-
//
// This file is part of LHAPDF
// Copyright (C) 2012-2014 The LHAPDF collaboration (see AUTHORS for details)
//
#pragma once
#ifndef LHAPDF_PDFIndex_H
#define LHAPDF_PDFIndex_H

#include "LHAPDF/Paths.h"
#include "LHAPDF/Utils.h"
#include "LHAPDF/Exceptions.h"

namespace LHAPDF {


  /// @name Functions for PDF lookup by LHAPDF ID index file
  //@{

  /// Get the singleton LHAPDF set ID -> PDF index map
  inline std::map<int, std::string>& getPDFIndex() {
    static map<int, string> _lhaindex;
    if (_lhaindex.empty()) { // The map needs to be populated first
      string indexpath = findFile("pdfsets.index");
      if (indexpath.empty()) throw ReadError("Could not find a pdfsets.index file");
      try {
        ifstream file(indexpath.c_str());
        string line;
        while (getline(file, line)) {
          trim(line);
          if (line.empty() || line.find("#") == 0) continue;
          istringstream tokens(line);
          int id; string setname;
          tokens >> id;
          tokens >> setname;
          // cout << id << " -> " << _lhaindex[id] << endl;
          _lhaindex[id] = setname;
        }
      } catch (const std::exception& ex) {
        throw ReadError("Trouble when reading " + indexpath + ": " + ex.what());
      }
    }
    return _lhaindex;
  }


  /// Look up a PDF set name and member ID by the LHAPDF ID code
  ///
  /// The set name and member ID are returned as an std::pair.
  /// If lookup fails, a pair ("", -1) is returned.
  inline pair<std::string, int> lookupPDF(int lhaid) {
    map<int, string>::iterator it = getPDFIndex().upper_bound(lhaid);
    string rtnname = "";
    int rtnmem = -1;
    if (it != getPDFIndex().begin()) {
      --it; // upper_bound (and lower_bound) return the entry *above* lhaid: we need to step back
      rtnname = it->second; // name of the set that contains this ID
      rtnmem = lhaid - it->first; // the member ID is the offset from the lookup ID
    }
    return make_pair(rtnname, rtnmem);
  }


  /// Look up the member's LHAPDF index from the set name and member ID.
  ///
  /// If lookup fails, -1 is returned, otherwise the LHAPDF ID code.
  /// NB. This function is relatively slow, since it requires std::map reverse lookup.
  inline int lookupLHAPDFID(const std::string& setname, int nmem) {
    // const map<int, string>& = getPDFIndex();
    typedef pair<int, string> MapPair;
    BOOST_FOREACH (const MapPair& id_name, getPDFIndex()) {
      if (id_name.second == setname) return id_name.first + nmem;
    }
    return -1; //< failure value
  }


  /// @todo Add this when the decodePDFStr function is finalised and can be moved out of Factories.cc
  // inline int lookupLHAPDFID(const std::string& setname_nmem) {
  //   std::pair<std::string,int> idpair = decodePDFStr(setname_nmem);
  //   return lookupLHAPDFID(idpair.first, idpair.second);
  // }

  //@}

}
#endif
