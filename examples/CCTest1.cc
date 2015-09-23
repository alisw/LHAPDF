// Simple test program to test functionality of
// the LHAPDF C++ wrapper.
// Use this as a template for your program.
//
// Original author: Stefan Gieseke 2005
// Adapted for LHAPDF v5+ by Mike Whalley.
// Further updates for >= v5.4 by Andy Buckley.

#include "LHAPDF/LHAPDF.h"
#include <iterator>
#include <iostream>
#include <cstdlib>
using namespace std;

// NB. You might want to also have
// using namespace LHAPDF;
// for convenience.


/// Demo of using LHAPDF via the C++ wrapper functions.
int main() {
  const int SUBSET = 0;
  const string NAME = "CT10";

  LHAPDF::initPDFSet(NAME, LHAPDF::LHGRID, SUBSET);

  const double Q = 10.0, mz = 91.2;
  cout << "alphas(mz) = " << LHAPDF::alphasPDF(mz) << endl;
  cout << "qcdlam4    = " << LHAPDF::getLam4(SUBSET) << endl;
  cout << "qcdlam5    = " << LHAPDF::getLam5(SUBSET) << endl;
  cout << "orderPDF   = " << LHAPDF::getOrderPDF() << endl;
  cout << "xmin       = " << LHAPDF::getXmin(SUBSET) << endl;
  cout << "xmax       = " << LHAPDF::getXmax(SUBSET) << endl;
  cout << "q2min      = " << LHAPDF::getQ2min(SUBSET) << endl;
  cout << "q2max      = " << LHAPDF::getQ2max(SUBSET) << endl;
  cout << "orderalfas = " << LHAPDF::getOrderAlphaS() << endl;
  cout << "num flav   = " << LHAPDF::getNf() << endl;
  cout << "name       = " << NAME << endl;
  cout << "number     = " << LHAPDF::numberPDF() << endl;
  cout << endl;


  // LHAPDF::extrapolate();

  const int NUMBER = LHAPDF::numberPDF();


  for (int n = 0; n < NUMBER + 1; ++n) {
    cout << "Set number: " << n << endl;
    LHAPDF::initPDF(n);
    for (int ix = 1; ix < 11; ++ix) {
      const double x = (ix - 0.5) / 10.0;
      cout << "x=" << x << ", Q=" << Q << ", f=0: " << LHAPDF::xfx(x, Q, 0) << endl;
    }
    cout << endl;
  }


  for (int n = 0; n < NUMBER + 1; ++n) {
    cout << "Set number: " << n << endl;
    LHAPDF::initPDF(n);
    for (int ix = 1; ix < 11; ++ix) {
      const double x = (ix - 0.5) / 10.0;
      cout << "x=" << x << ", Q=" << Q << ": ";
      vector<double> result = LHAPDF::xfx(x, Q);
      std::copy(result.begin(), result.end(), ostream_iterator<double>(cout,"\t"));
      cout << endl;
    }
    cout << endl;
  }


  cout << string().insert(0, 40, '-') << endl << endl;
  cout << "Checking LHAPDF with package " << endl;
  cout << NAME << " set number " << SUBSET << "/" << LHAPDF::numberPDF() << endl << endl;
  cout << "LHAPDF::getDescription() gives:" << endl;
  LHAPDF::getDescription();

  return EXIT_SUCCESS;
}


#include "FortranWrappers.h"
#ifdef FC_DUMMY_MAIN
int FC_DUMMY_MAIN() { return 1; }
#endif
