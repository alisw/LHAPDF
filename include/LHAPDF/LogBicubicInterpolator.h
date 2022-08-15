// -*- C++ -*-
//
// This file is part of LHAPDF
// Copyright (C) 2012-2022 The LHAPDF collaboration (see AUTHORS for details)
//
#pragma once
#ifndef LHAPDF_LogBicubicInterpolator_H
#define LHAPDF_LogBicubicInterpolator_H

#include "LHAPDF/Interpolator.h"

namespace LHAPDF {


  /// @brief Implementation of bicubic interpolation
  ///
  /// This class will interpolate in 2D using a bicubic hermite spline.
  class LogBicubicInterpolator : public Interpolator {
    
  public:
    LogBicubicInterpolator(){ setType("logcubic"); }
    
    /// Implementation of (x,Q2) interpolation
    double _interpolateXQ2(const KnotArray& subgrid, double x, size_t ix, double q2, size_t iq2, int id) const;

    void _interpolateXQ2(const KnotArray& subgrid, double x, size_t ix, double q2, size_t iq2, std::vector<double>& ret) const;

    
  };

}

#endif
