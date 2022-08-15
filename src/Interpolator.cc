// -*- C++ -*-
//
// This file is part of LHAPDF
// Copyright (C) 2012-2022 The LHAPDF collaboration (see AUTHORS for details)
//
#include "LHAPDF/Interpolator.h"
#include "LHAPDF/GridPDF.h"

namespace LHAPDF {


  double Interpolator::interpolateXQ2(int id, double x, double q2) const {
    /// @todo Why do we make the detour via the Interpolator, instead of makin
    ///       the below calls from the derived classes?
    const KnotArray& grid = pdf().knotarray();
    const size_t ix  = grid.ixbelow(x);
    const size_t iq2 = grid.iq2below(q2);
    
    /// Call the overloaded interpolation routine
    return _interpolateXQ2(grid, x, ix, q2, iq2, id);
  }

  void Interpolator::interpolateXQ2(double x, double q2, std::vector<double>& ret) const {
    const KnotArray& grid = pdf().knotarray();
    const size_t ix  = grid.ixbelow(x);
    const size_t iq2 = grid.iq2below(q2);
    
    /// Call the overloaded interpolation routine
    _interpolateXQ2(grid, x, ix, q2, iq2, ret);
  }
  

}
