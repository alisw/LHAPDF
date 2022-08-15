// -*- C++ -*-
//
// This file is part of LHAPDF
// Copyright (C) 2012-2022 The LHAPDF collaboration (see AUTHORS for details)
//
#pragma once
#ifndef LHAPDF_BilinearInterpolator_H
#define LHAPDF_BilinearInterpolator_H

#include "LHAPDF/Interpolator.h"

namespace LHAPDF {


  /// Implementation of bilinear interpolation
  class BilinearInterpolator : public Interpolator {
  public:
    double _interpolateXQ2(const KnotArray& subgrid, double x, size_t ix, double q2, size_t iq2, int id) const;
    void _interpolateXQ2(const KnotArray& subgrid, double x, size_t ix, double q2, size_t iq2, std::vector<double>& ret) const;
  };


}
#endif
