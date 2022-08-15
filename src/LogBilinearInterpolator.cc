// -*- C++ -*-
//
// This file is part of LHAPDF
// Copyright (C) 2012-2022 The LHAPDF collaboration (see AUTHORS for details)
//
#include "LHAPDF/LogBilinearInterpolator.h"

namespace LHAPDF {


  namespace { // Unnamed namespace
    struct shared_data{
      // manual cache some values
      double logx;
      double logq2;
      double logx0;
      double logx1;
    };

    shared_data fill(const KnotArray& grid, double x, double q2, size_t ix){
      // manual cache some values
      shared_data share;
      share.logq2 = log(q2);
      share.logx  = log(x);
      share.logx0 = grid.logxs(ix);
      share.logx1 = grid.logxs(ix+1);
      
      return share;
    }
    
    // One-dimensional linear interpolation for y(x)
    inline double _interpolateLinear(double x, double xl, double xh, double yl, double yh)	{
      assert(x >= xl);
      assert(xh >= x);
      return yl + (x - xl) / (xh - xl) * (yh - yl);
    }

    void _checkGridSize(const KnotArray &grid){
      if (grid.xsize() < 2)
	throw GridError("PDF subgrids are required to have at least 2 x-knots for use with BilinearInterpolator");
      if (grid.q2size() < 2)
	throw GridError("PDF subgrids are required to have at least 2 Q2-knots for use with BilinearInterpolator");
    }

    double _interpolate(const KnotArray& grid, size_t ix, size_t iq2, int id, shared_data _share){
      const double f_ql = _interpolateLinear(_share.logx, _share.logx0, _share.logx1, grid.xf(ix, iq2, id), grid.xf(ix+1, iq2, id));
      const double f_qh = _interpolateLinear(_share.logx, _share.logx0, _share.logx1, grid.xf(ix, iq2+1, id), grid.xf(ix+1, iq2+1, id));
      // Then interpolate in Q2, using the x-ipol results as anchor points
      return _interpolateLinear(_share.logq2, grid.logq2s(iq2), grid.logq2s(iq2+1), f_ql, f_qh);
    }
  }
  
  void LogBilinearInterpolator::_interpolateXQ2(const KnotArray& grid, double x, size_t ix, double q2, size_t iq2, std::vector<double>& ret) const{
    _checkGridSize(grid);
    shared_data shared = fill(grid, x, q2, ix);
    
    // First interpolate in x
    for (int pid(-6); pid <= 6; ++pid){
      int id = grid.lookUpPid(pid + 6);
      if (id == -1){
	ret[pid + 6] = 0;
      } else {
	ret[pid + 6] = _interpolate(grid, ix, iq2, id, shared);
      }
    }
  }

  
  double LogBilinearInterpolator::_interpolateXQ2(const KnotArray& grid, double x, size_t ix, double q2, size_t iq2, int id) const {
    _checkGridSize(grid);
    shared_data shared = fill(grid, x, q2, ix);
    return _interpolate(grid, ix, iq2, id, shared);
  }


}
