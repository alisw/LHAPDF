/////////////////////////////////////////////////////////////////
// Program to demonstrate usage of the MRST 2006 NNLO PDFs.    //
// to calculate errors.                                        //
/////////////////////////////////////////////////////////////////
#include "LHAPDF/LHAPDF.h"
#include <iostream>
#include <cmath>
#include <cstdio>
#include <cstdlib>
using namespace std;

using namespace LHAPDF;


double logdist_x(double xmin, double xmax, size_t ix, size_t nx) {
  const double log10xmin = log10(xmin);
  const double log10xmax = log10(xmax);
  const double log10x = log10xmin + (ix/static_cast<double>(nx-1))*(log10xmax-log10xmin);
  const double x = pow(10.0, log10x);
  return x;
}


int main() {
  // Show initialisation banner only once
  setVerbosity(LOWKEY); // or SILENT, for no banner at all

  // You could explicitly set the path to the PDFsets directory
  // setPDFPath("/home/whalley/local/share/lhapdf/PDFsets");
  
  // Initialize PDF sets
  const string NAME = "MRST2006nnlo";
  initPDFSetM(1, NAME, LHGRID);
  initPDFSetM(2, NAME, LHGRID);
  initPDFSetM(3, NAME, LHGRID);
  
  // Find the number of eigensets from numberPDF()
  const int neigen = numberPDFM(1)/2;
  cout << "Number of eigensets in this fit = " << neigen << endl;
  // Find the min and max values of x and Q2 
  const double xmin = getXmin(0);
  const double xmax = getXmax(0);
  cout << "Valid x-range = [" << xmin << ", " << xmax << "]" << endl;
  // Number of x values to sample
  const int nx = 10;
  // Set the Q scale and flavour
  double q = 10;
  int flav = 4;

  // Get x's and central PDF values
  initPDFM(1, 0);
  vector<double> fc(nx), x(nx);
  for (int ix = 0; ix < nx; ++ix) {
    x[ix] = logdist_x(xmin, 0.9*xmax, ix, nx);
    fc[ix] = xfxM(1, x[ix], q, flav);
  }

  // Sum over error contributions (two ways, depending on how LHDPAF was compiled)
  vector<double> summax(nx), summin(nx), sum(nx);
  #ifndef LHAPDF_LOWMEM
  // This is the normal, efficient, way to do this, with the error
  // sets being initialised the minimum number of times
  cout << "Using efficient set looping" << endl;
  for (int ieigen = 1; ieigen <= neigen; ++ieigen) {
    initPDFM(2, 2*ieigen-1);
    initPDFM(3, 2*ieigen);
    for (int ix = 0; ix < nx; ++ix) {
      // Find central and plus/minus values
      const double fp = xfxM(2, x[ix], q, flav);
      const double fm = xfxM(3, x[ix], q, flav);
      // Construct shifts
      const double plus = max(max(fp-fc[ix], fm-fc[ix]),0.0);
      const double minus = min(min(fp-fc[ix], fm-fc[ix]),0.0);
      const double diff = fp-fm;
      // Add it together
      summax[ix] += plus*plus;
      summin[ix] += minus*minus;
      sum[ix] += diff*diff;
    }
  }
  #else
  // In low memory mode, the sets need to be re-initialised with every 
  // change of member. Using the approach above gives wrong answers, and
  // reinitialising in all the nested loops is sloooooow! The best way is 
  // to calculate the values, plus and minus errors separately.
  cout << "Using low-mem mode set looping" << endl;
  for (int ieigen = 1; ieigen <= neigen; ++ieigen) {
    vector<double> fp(nx), fm(nx);
    initPDFM(2, 2*ieigen-1);
    for (int ix = 0; ix < nx; ++ix) {
      fp[ix] = xfxM(2, x[ix], q, flav);
    }
    initPDFM(3, 2*ieigen);
    for (int ix = 0; ix < nx; ++ix) {
      fm[ix] = xfxM(3, x[ix], q, flav);
    }
    for (int ix = 0; ix < nx; ++ix) {
      // Construct shifts
      const double plus = max(max(fp[ix]-fc[ix], fm[ix]-fc[ix]), 0.0);
      const double minus = min(min(fp[ix]-fc[ix], fm[ix]-fc[ix]), 0.0);
      const double diff = fp[ix]-fm[ix];
      // Add it together
      summax[ix] += plus*plus;
      summin[ix] += minus*minus;
      sum[ix] += diff*diff;
    }
  }
  #endif

  // Print out results
  cout << "flavour = " << flav << "               Asymmetric (%)   Symmetric (%)" << endl;
  cout << "     x    Q**2    xf(x)    plus    minus      +-      " << endl;
  for  (int ix = 0; ix < nx; ++ix) {
    printf("%0.7f %.0f %10.2E %8.2f %8.2f %8.2f \n",
           x[ix], q*q, fc[ix], 
           sqrt(summax[ix])*100/fc[ix],
           sqrt(summin[ix])*100/fc[ix],
           0.5*sqrt(sum[ix])*100/fc[ix]);
  }
  
  return EXIT_SUCCESS;
}



#include "FortranWrappers.h"
#ifdef FC_DUMMY_MAIN
int FC_DUMMY_MAIN() { return 1; }
#endif
