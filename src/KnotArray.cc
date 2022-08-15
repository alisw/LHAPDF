// -*- C++ -*-
//
// This file is part of LHAPDF
// Copyright (C) 2012-2022 The LHAPDF collaboration (see AUTHORS for details)
//
#include "LHAPDF/KnotArray.h"
#include <functional>

namespace LHAPDF {

  void KnotArray::initPidLookup(){
    _lookup.clear();
    if (_pids.size() == 0){
      // Is there a better LHAPDF error for that?
      std::cerr << "Internal error when constructing lookup table; need to fill pids before construction"<< std::endl;
      throw;
    }
    for (int i(-6); i<0; i++)
      _lookup.push_back(findPidInPids(i, _pids));
      
    _lookup.push_back(findPidInPids(21, _pids));
    for (int i(1); i<=6; i++)
      _lookup.push_back(findPidInPids(i, _pids));
    _lookup.push_back(findPidInPids(22, _pids));
  }

  void KnotArray::fillLogKnots() {
    _logxs.resize(_xs.size());
    for (size_t i(0); i<_xs.size(); ++i)
      _logxs[i] = log(_xs[i]);

    _logq2s.resize(_q2s.size());
    for (size_t i(0); i<_q2s.size(); ++i)
      _logq2s[i] = log(_q2s[i]);
  }
  
}
