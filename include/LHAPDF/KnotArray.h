// -*- C++ -*-
//
// This file is part of LHAPDF
// Copyright (C) 2012-2022 The LHAPDF collaboration (see AUTHORS for details)
//
#pragma once
#ifndef LHAPDF_KnotArray_H
#define LHAPDF_KnotArray_H

#include "LHAPDF/Exceptions.h"
#include "LHAPDF/Utils.h"

namespace {

  
  // Hide some internal functions from outside API view

  // General function to find the knot below a given value
  size_t indexbelow(double value, const std::vector<double>& knots) {
    size_t i = upper_bound(knots.begin(), knots.end(), value) - knots.begin();
    if (i == knots.size()) i -= 1; // can't return the last knot index
    i -= 1;                // step back to get the knot <= x behaviour
    return i;
  }

  
  int findPidInPids(int pid, const std::vector<int>& pids) {
    std::vector<int>::const_iterator it = std::find(pids.begin(), pids.end(), pid);
    if (it == pids.end())
      return -1;
    else
      return std::distance(pids.begin(), it);
  }


}

namespace LHAPDF {

  
  /// @brief Internal storage class for PDF data point grids
  ///
  /// We use "array" to refer to the "raw" knot grid, while "grid" means a grid-based PDF.
  /// The "1F" means that this is a single-flavour array
  class KnotArray{
  public:
    
    /// How many flavours are stored in the grid stored
    size_t size() const { return _shape.back(); }

    /// How many x knots are there
    size_t xsize() const { return _shape[0]; }

    /// How many q2 knots are there
    size_t q2size() const { return _shape[1]; }
    
    /// Is this container empty?
    bool empty() const { return _grid.empty(); }
    
    /// find the largest grid index below given x, such that xknots[index] < x
    size_t ixbelow(double x) const { return indexbelow(x, _xs); }

    /// find the largest grid index below given q2, such that q2knots[index] < q2
    size_t iq2below(double q2) const { return indexbelow(q2, _q2s); }

    /// convenient accessor to the grid values
    double xf(int ix, int iq2, int ipid) const {
      return _grid[ix*_shape[2]*_shape[1] + iq2*_shape[2] + ipid];
    }

    /// convenient accessor to the polynomial coefficients, returns reference rather than value, to be able to read multiple adjacent at once
    const double& coeff(int ix, int iq2, int pid, int in) const {
      return _coeffs[ix*(_shape[1])*_shape[2]*4 + iq2*_shape[2]*4 + pid*4 + in];
    }

    /// accessor to the internal 'lookup table' for the pid's
    int lookUpPid(int id) const { return _lookup[id]; }

    double xs(int id) const { return _xs[id]; }

    double logxs(int id) const { return _logxs[id]; }
    
    double q2s(int id) const { return _q2s[id]; }

    double logq2s(int id) const { return _logq2s[id]; }
    
    size_t shape(int id) const { return _shape[id]; }

    /// check if value within the boundaries of xknots
    bool inRangeX(double x) const {
      if (x < _xs.front()) return false;
      if (x > _xs.back())  return false;
      return true;
    }

    /// check if value within the boundaries of q2knots
    bool inRangeQ2(double q2) const {
      if (q2 < _q2s.front()) return false;
      if (q2 > _q2s.back())  return false;
      return true;
    }

    inline int get_pid(int id) const {
      // hardcoded lookup table for particle ids
      // -6,...,-1,21/0,1,...,6,22
      // if id outside of this range, search in list of ids
      if (-6 <= id && id <= 6) return _lookup[id + 6];
      else if (id == 21) return _lookup[0 + 6];
      else if (id == 22) return _lookup[13];
      else return findPidInPids(id, _pids);
    }

    bool has_pid(int id) const {
      return get_pid(id) != -1;
    }
    
    void initPidLookup();

    void fillLogKnots();
    
    /// Const accessors to the internal data container
    const std::vector<double>& xs() const { return _xs; }

    const std::vector<double>& logxs() const { return _logxs; }

    const std::vector<double>& q2s() const { return _q2s; }
    
    const std::vector<double>& logq2s() const { return _logq2s; }

    /// Non const accessors for programmatic filling
    std::vector<double>& setCoeffs() { return _coeffs; }

    std::vector<double>& setGrid() { return _grid; }

    std::vector<double>& setxknots() { return _xs; }

    std::vector<double>& setq2knots() { return _q2s; }

    std::vector<size_t>& setShape(){ return _shape; }
    
    std::vector<int>&    setPids() { return _pids; }
    
  private:
    // Shape of the interpolation grid
    std::vector<size_t> _shape;
        
     // Gridvalues
    std::vector<double> _grid;

    // Storage for the precomputed polynomial coefficients
    std::vector<double> _coeffs;
    
    // order the pids are filled in
    std::vector<int> _pids;
    std::vector<int> _lookup;

    // knots
    std::vector<double> _xs;
    std::vector<double> _q2s;
    std::vector<double> _logxs;
    std::vector<double> _logq2s;

  };
  

  /// Internal storage class for alpha_s interpolation grids
  class AlphaSArray {
  public:

    /// @name Construction etc.
    ///@{

    /// Default constructor just for std::map insertability
    AlphaSArray() {}

    /// Constructor from Q2 knot values and alpha_s values
    AlphaSArray(const std::vector<double>& q2knots, const std::vector<double>& as)
      : _q2s(q2knots), _as(as)
    {
      _syncq2s();
    }

    ///@}


    /// @name Q2 stuff
    ///@{

    /// Q2 knot vector accessor
    const std::vector<double>& q2s() const { return _q2s; }

    /// log(Q2) knot vector accessor
    const std::vector<double>& logq2s() const { return _logq2s; }

    /// Get the index of the closest Q2 knot row <= q2
    ///
    /// If the value is >= q2_max, return i_max-1 (for polynomial spine construction)
    size_t iq2below(double q2) const {
      // Test that Q2 is in the grid range
      if (q2 < q2s().front()) throw AlphaSError("Q2 value " + to_str(q2) + " is lower than lowest-Q2 grid point at " + to_str(q2s().front()));
      if (q2 > q2s().back()) throw AlphaSError("Q2 value " + to_str(q2) + " is higher than highest-Q2 grid point at " + to_str(q2s().back()));
      /// Find the closest knot below the requested value
      size_t i = upper_bound(q2s().begin(), q2s().end(), q2) - q2s().begin();
      if (i == q2s().size()) i -= 1; // can't return the last knot index
      i -= 1; // have to step back to get the knot <= q2 behaviour
      return i;
    }

    /// Get the index of the closest logQ2 knot row <= logq2
    ///
    /// If the value is >= q2_max, return i_max-1 (for polynomial spine construction)
    size_t ilogq2below(double logq2) const {
      // Test that log(Q2) is in the grid range
      if (logq2 < logq2s().front()) throw GridError("logQ2 value " + to_str(logq2) + " is lower than lowest-logQ2 grid point at " + to_str(logq2s().front()));
      if (logq2 > logq2s().back()) throw GridError("logQ2 value " + to_str(logq2) + " is higher than highest-logQ2 grid point at " + to_str(logq2s().back()));
      /// Find the closest knot below the requested value
      size_t i = upper_bound(logq2s().begin(), logq2s().end(), logq2) - logq2s().begin();
      if (i == logq2s().size()) i -= 1; // can't return the last knot index
      i -= 1; // have to step back to get the knot <= q2 behaviour
      return i;
    }

    ///@}


    /// @name alpha_s values at Q2 points
    ///@{

    /// alpha_s value accessor (const)
    const std::vector<double>& alphas() const { return _as; }
    // /// alpha_s value accessor (non-const)
    // std::vector<double>& alphas() { return _as; }
    // /// alpha_s value setter
    // void setalphas(const valarray& xfs) { _as = as; }

    ///@}


    /// @name alpha_s derivatives vs (log)Q2, useful for interpolation
    ///@{

    /// Forward derivative w.r.t. logQ2
    double ddlogq_forward(size_t i) const {
      return (alphas()[i+1] - alphas()[i]) / (logq2s()[i+1] - logq2s()[i]);
    }

    /// Backward derivative w.r.t. logQ2
    double ddlogq_backward(size_t i) const {
      return (alphas()[i] - alphas()[i-1]) / (logq2s()[i] - logq2s()[i-1]);
    }

    /// Central (avg of forward and backward) derivative w.r.t. logQ2
    double ddlogq_central(size_t i) const {
      return 0.5 * (ddlogq_forward(i) + ddlogq_backward(i));
    }

    ///@}


  private:

    /// Synchronise the log(Q2) array from the Q2 one
    void _syncq2s() {
      _logq2s.resize(_q2s.size());
      for (size_t i = 0; i < _q2s.size(); ++i) _logq2s[i] = log(_q2s[i]);
    }

    /// List of Q2 knots
    std::vector<double> _q2s;
    /// List of log(Q2) knots
    std::vector<double> _logq2s;
    /// List of alpha_s values across the knot array
    std::vector<double> _as;

  };
}
#endif
