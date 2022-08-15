// -*- C++ -*-
//
// This file is part of LHAPDF
// Copyright (C) 2012-2022 The LHAPDF collaboration (see AUTHORS for details)
//
#include "LHAPDF/GridPDF.h"
#include "LHAPDF/Interpolator.h"
#include "LHAPDF/Factories.h"
#include "LHAPDF/FileIO.h"
#include <iostream>
#include <sstream>
#include <string>
#include <stdexcept>
#include <cstring>

using namespace std;

namespace LHAPDF {


  void GridPDF::setInterpolator(Interpolator* ipol) {
    _interpolator.reset(ipol);
    _interpolator->bind(this);
    if (_interpolator->getType() == "logcubic"){
      _computePolynomialCoefficients(true);
    } else if (_interpolator->getType() == "cubic"){
      _computePolynomialCoefficients(false);
    }
  }

  void GridPDF::setInterpolator(const std::string& ipolname) {
    setInterpolator(mkInterpolator(ipolname));
  }

  void GridPDF::_loadInterpolator() {
    const string ipolname = info().get_entry("Interpolator");
    /// @todo What if there is no Interpolator key?
    setInterpolator(ipolname);
  }

  const Interpolator& GridPDF::interpolator() const {
    if (!hasInterpolator()) throw Exception("No Interpolator pointer set");
    return *_interpolator;
  }

  const vector<double>& GridPDF::xKnots() const {
    return data.xs();
  }

  const vector<double>& GridPDF::q2Knots() const {
    return data.q2s();
  }

  void GridPDF::setExtrapolator(Extrapolator* xpol) {
    _extrapolator.reset(xpol);
    _extrapolator->bind(this);
  }

  void GridPDF::setExtrapolator(const std::string& xpolname) {
    setExtrapolator(mkExtrapolator(xpolname));
  }

  void GridPDF::_loadExtrapolator() {
    const string xpolname = info().get_entry("Extrapolator");
    /// @todo What if there is no Extrapolator key?
    setExtrapolator(xpolname);
  }

  const Extrapolator& GridPDF::extrapolator() const {
    if (!hasExtrapolator()) throw Exception("No Extrapolator pointer set");
    return *_extrapolator;
  }

  double GridPDF::_xfxQ2(int id, double x, double q2) const {
    /// Decide whether to use interpolation or extrapolation... the sanity checks
    /// are done in the public PDF::xfxQ2 function.
    double xfx = 0;
    int _id = data.get_pid(id);
    if (_id == -1) return 0;
    if (inRangeXQ2(x, q2)) {
      xfx = interpolator().interpolateXQ2(_id, x, q2);
    } else {
      xfx = extrapolator().extrapolateXQ2(_id, x, q2);
    }
    return xfx;
  }

  void GridPDF::_xfxQ2(double x, double q2, std::vector<double>& ret) const {
    if (inRangeXQ2(x, q2)) {
      interpolator().interpolateXQ2(x, q2, ret);
    } else {
      for (int id = 0; id < 13; ++id) {
	int _id = data.get_pid(id - 6);
	if (_id == -1) {
	  ret[id] = 0;
	} else {
	  ret[id] = extrapolator().extrapolateXQ2(_id, x, q2);
	}
      }
    }
  }


  namespace {

    // A wrapper for std::strtod and std::strtol, for fast tokenizing when all
    // input is guaranteed to be numeric (as in this data block). Based very
    // closely on FastIStringStream by Gavin Salam.
    class NumParser {
    public:
      // Constructor from char*
      NumParser(const char* line=0) {
        reset(line);
      }
      // Constructor from std::string
      NumParser(const string& line) {
        reset(line);
      }

      // Re-init to new line as char*
      void reset(const char* line=0) {
        _next = const_cast<char*>(line);
        _new_next = _next;
        _error = false;
      }
      // Re-init to new line as std::string
      void reset(const string& line) { reset(line.c_str()); }

      // Tokenizing stream operator (forwards to double and int specialisations)
      template<class T> NumParser& operator>>(T& value) {
        _get(value);
        if (_new_next == _next) _error = true; // handy error condition behaviour!
        _next = _new_next;
        return *this;
      }

      // Allow use of operator>> in a while loop
      operator bool() const { return !_error; }


    private:

      void _get(double& x) { x = std::strtod(_next, &_new_next); }
      void _get(float& x) { x = std::strtof(_next, &_new_next); }
      void _get(int& i) { i = std::strtol(_next, &_new_next, 10); } // force base 10!

      char *_next, *_new_next;
      bool _error;

    };


    double _ddx(KnotArray& data, size_t ix, size_t iq2, int id, bool logspace){
      const size_t nxknots = data.xsize();
      double del1, del2;
      if (logspace){
	del1 = (ix == 0)           ? 0 : data.logxs(ix)   - data.logxs(ix-1);
	del2 = (ix == nxknots - 1) ? 0 : data.logxs(ix+1) - data.logxs(ix);
      } else {
	del1 = (ix == 0)           ? 0 : data.xs(ix)   - data.xs(ix-1);
	del2 = (ix == nxknots - 1) ? 0 : data.xs(ix+1) - data.xs(ix);
      }
      if (ix != 0 && ix != nxknots-1) { //< If central, use the central difference
	const double lddx = (data.xf(ix, iq2, id) - data.xf(ix-1, iq2, id)) / del1;
	const double rddx = (data.xf(ix+1, iq2, id) - data.xf(ix, iq2, id)) / del2;
	return (lddx + rddx) / 2.0;
      } else if (ix == 0) { //< If at leftmost edge, use forward difference
	return (data.xf(ix+1, iq2, id) - data.xf(ix, iq2, id)) / del2;
      } else if (ix == nxknots-1) { //< If at rightmost edge, use backward difference
	return (data.xf(ix, iq2, id) - data.xf(ix-1, iq2, id)) / del1;
      } else {
	throw LogicError("We shouldn't be able to get here!");
      }
    }


  } // End unnamed namespace



  void GridPDF::_computePolynomialCoefficients(bool logspace){
    const size_t nxknots = data.xsize();
    vector<size_t> shape{data.xsize()-1, data.q2size(), data.size(), 4};
    vector<double> coeffs;
    coeffs.resize(shape[0]*shape[1]*shape[2]*shape[3]);
    for (size_t ix(0); ix<nxknots-1; ++ix){
      for (size_t iq2(0); iq2<data.q2size(); ++iq2){
	for (size_t id(0); id<data.size(); ++id){
	  double dlogx;
	  if (logspace){
	    dlogx = data.logxs(ix+1) - data.logxs(ix);
	  } else{
	    dlogx = data.xs(ix+1) - data.xs(ix);
	  }
	  double VL  = data.xf (ix,   iq2, id);
	  double VH  = data.xf (ix+1, iq2, id);
	  double VDL = _ddx(data, ix,   iq2, id, logspace) * dlogx;
	  double VDH = _ddx(data, ix+1, iq2, id, logspace) * dlogx;

	  // polynomial coefficients
	  double a = VDH + VDL - 2*VH + 2*VL;
	  double b = 3*VH - 3*VL - 2*VDL - VDH;
	  double c = VDL;
	  double d = VL;

	  coeffs[ix*shape[1]*shape[2]*shape[3] + iq2*shape[2]*shape[3] + id*shape[3] + 0] = a;
	  coeffs[ix*shape[1]*shape[2]*shape[3] + iq2*shape[2]*shape[3] + id*shape[3] + 1] = b;
	  coeffs[ix*shape[1]*shape[2]*shape[3] + iq2*shape[2]*shape[3] + id*shape[3] + 2] = c;
	  coeffs[ix*shape[1]*shape[2]*shape[3] + iq2*shape[2]*shape[3] + id*shape[3] + 3] = d;
	}
      }
    }
    data.setCoeffs() = coeffs;
  }

  void GridPDF::_loadData(const string& mempath) {
    string line, prevline;
    int iblock(0), iblockline(0), iline(0);
    vector<double> xknots;
    vector<double> q2knots;

    vector<int> pids;
    vector<double> ipid_xfs;

    try{
      IFile file(mempath.c_str());
      NumParser nparser; double ftoken; int itoken;
      while (getline(*file, line)) {
        line = trim(line);

        // If the line is commented out, increment the line number but not the block line
        iline += 1;
        if (line.find("#") == 0) continue;
        iblockline += 1;

        if (line != "---") { // if we are not on a block separator line...
          // Block 0 is the metadata, which we ignore here
          if (iblock == 0) continue;

          // Parse the data lines
          nparser.reset(line);
          if (iblockline == 1) { // x knots line
	    if (iblock == 1){
	      while (nparser >> ftoken) xknots.push_back(ftoken);
	      if (xknots.empty())
		throw ReadError("Empty x knot array on line " + to_str(iline));
	    } else { // the x grid should be the same as for the fist i block
	      int tmp = 0;
	      while (nparser >> ftoken) {
		if (ftoken != xknots[tmp])
		  throw ReadError("Mismatch in the x-knots");
		++tmp;
	      }
	    }

          } else if (iblockline == 2) { // Q knots line
            while (nparser >> ftoken) q2knots.push_back(ftoken*ftoken); // note Q -> Q2
            if (q2knots.size() == 0)
              throw ReadError("Empty Q knot array on line " + to_str(iline));
          } else if (iblockline == 3) { // internal flavor IDs ordering line
	    if (iblock == 1){
	      while (nparser >> itoken) pids.push_back(itoken);
	    } else {
	      int tmp = 0;
	      while (nparser >> itoken) {
		if (itoken != pids[tmp])
		  throw ReadError("Mismatch in the pids");
		++tmp;
	      }
	    }
            // Check that each line has many tokens as there should be flavours
            if (pids.size() != flavors().size())
              throw ReadError("PDF grid data error on line " + to_str(iline) + ": " + to_str(pids.size()) +
                              " parton flavors declared but " + to_str(flavors().size()) + " expected from Flavors metadata");
            /// @todo Handle sea/valence representations via internal pseudo-PIDs
          }
	} else{
	  ++iblock;
	  iblockline = 0;
	}
      }
    } catch (Exception& e) {
      throw;
    } catch (std::exception& e) {
      throw ReadError("Read error while parsing " + mempath + " as a GridPDF data file");
    }

    iblock = 0; iblockline = 0; iline = 0;

    // fill the knots of Knotarray
    data.setxknots()  = xknots;
    data.setq2knots() = q2knots;
    data.fillLogKnots();

    // fill shape of Knotarray
    vector<size_t> shape(3);
    shape[0] = xknots.size();
    shape[1] = q2knots.size();
    shape[2] = pids.size();
    data.setShape() = shape;
    data.setPids() = pids;

    // create lookuptable to get index id from pid
    data.initPidLookup();

    // sets size of data vector
    ipid_xfs.resize(data.shape(0) * data.shape(1) * data.shape(2));

    int qloc(0), qtot(0);
    try {
      int index(0);
      int xindex(0);

      IFile file(mempath.c_str());
      NumParser nparser; double ftoken;
      while (getline(*file, line)) {

        // Trim the current line to ensure that there is no effect of leading spaces, etc.
        line = trim(line);
        prevline = line; // used to test the last line after the while loop fails

        // If the line is commented out, increment the line number but not the block line
        iline += 1;
        if (line.find("#") == 0) continue;
        iblockline += 1;

        if (line != "---") { // if we are not on a block separator line...
          // Block 0 is the metadata, which we ignore here
          if (iblock == 0) continue;
          nparser.reset(line);
	  if (iblockline == 2) { // Find out how many q values are there
	    qloc = 0;
            while (nparser >> ftoken) ++qloc;
	  } else if (iblockline < 4){
	    continue;
          } else {
            while (nparser >> ftoken) {
	      ipid_xfs[xindex*data.shape(2)*data.shape(1)
		       + qtot*data.shape(2)
		       + index] = ftoken;
	      ++index;
            }
	    if ( (iblockline != 3) && (iblockline - 3) % qloc == 0){
	      ++xindex;
	      index = 0;
	    }
            // Check that each line has many tokens as there should be flavours
            if (index % pids.size() != 0)
	      /// @todo Error message gives wrong output bc. index % pids.size() is not the number of pids
              throw ReadError("PDF grid data error on line " + to_str(iline) + ": " + to_str(index % pids.size()) +
                              " flavor entries seen but " + to_str(pids.size()) + " expected");
          }

        } else { // we *are* on a block separator line
          // Check that the expected number of data lines were seen in the last block
	  // Does not work anymore, how to translate?
	  /*
          if (iblock > 0 && iblockline - 1 != int(xs.size()*q2s.size()) + 3)
            throw ReadError("PDF grid data error on line " + to_str(iline) + ": " +
                            to_str(iblockline-1) + " data lines were seen in block " + to_str(iblock-1) +
                            " but " + to_str(xs.size()*q2s.size() + 3) + " expected");
	  */

          // Ignore block registration if we've just finished reading the 0th (metadata) block
          if (iblock > 0) {

            // Throw if the last subgrid block was of zero size
            if (ipid_xfs.empty())
              throw ReadError("Empty xf values array in data block " + to_str(iblock) + ", ending on line " + to_str(iline));
          }

          // Increment/reset the block and line counters, etc
          iblock += 1;
          iblockline = 0;
	  index = 0;
	  xindex = 0;
	  qtot += qloc;
        }

      }
      data.setGrid() = ipid_xfs;

      // File reading finished: complain if it was not properly terminated
      if (prevline != "---")
        throw ReadError("Grid file " + mempath + " is not properly terminated: .dat files MUST end with a --- separator line");
      // Error handling
    } catch (Exception& e) {
      throw;
    } catch (std::exception& e) {
      throw ReadError("Read error while parsing " + mempath + " as a GridPDF data file");
    }
  }


}
