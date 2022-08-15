// Program to test automatic calculation of PDF uncertainties.
//
// Written March 2014 by G. Watt <Graeme.Watt(at)durham.ac.uk>
// Extended April 2022 by A. Buckley <andy.buckley(at)cern.ch>
//
// Use formulae for PDF uncertainties and correlations in:
//   G. Watt, JHEP 1109 (2011) 069 [arXiv:1106.5788 [hep-ph]].
// Also see Section 6 of LHAPDF6 paper [arXiv:1412.7420 [hep-ph]].
// Extended in July 2015 for ErrorType values ending in "+as".
// Modified in September 2015 for more general ErrorType values:
// number of parameter variations determined by counting "+" symbols.

#include "LHAPDF/LHAPDF.h"
#include <random>
using namespace std;


// Helper function for computing and printing errors from the given vals vector
void printUncs(const LHAPDF::PDFSet& set, const vector<double>& vals, double cl, const string& varname, bool alt=false) {
  if (cl == 0) cl = LHAPDF::CL1SIGMA;
  cout << "PDF uncertainties on " << varname << " computed with " << set.name() << " at CL=" << cl << "%" << endl;
  const LHAPDF::PDFErrInfo errinfo = set.errorInfo();
  const LHAPDF::PDFUncertainty err = set.uncertainty(vals, cl, alt);
  if (cl >= 0) cout << "Scaled PDF uncertainties using scale = " << err.scale << endl;
  // Print summary numbers
  cout.precision(5);
  cout << varname << " = " << err.central << " +" << err.errplus << " -" << err.errminus << " (+-" << err.errsymm << ")" << endl;
  // Break down into quadrature-combined uncertainty components
  for (size_t i = 0; i < errinfo.qparts.size(); ++i) {
    //cout << "  " << errinfo.qpartName(i) << endl;
    cout << "  " << setw(12) << err.errparts[i].first << setw(12) << err.errparts[i].second << "  " << errinfo.qpartName(i) << endl;
  }
}



// Simple test program to demonstrate the PDFSet member functions.
//   set.errorInfo();
//   set.uncertainty(values, cl=68.268949..., alternative=false);
//   set.correlation(valuesA, valuesB);
//   set.randomValueFromHessian(values, randoms, symmetrise=true);
int main(int argc, char* argv[]) {

  if (argc < 2) {
    cerr << "You must specify a PDF set:  ./testpdfunc setname" << endl;
    return 1;
  }

  const string setname = argv[1];
  const LHAPDF::PDFSet set(setname);
  const size_t nmem = set.size()-1;

  double x = 0.1; // momentum fraction
  double Q = 100.0; // factorisation scale in GeV

  // Fill vectors xgAll and xuAll using all PDF members.
  // Could replace xg, xu by cross section, acceptance etc.
  const vector<LHAPDF::PDF*> pdfs = set.mkPDFs();
  vector<double> xgAll, xuAll;
  vector<string> pdftypes;
  for (size_t imem = 0; imem <= nmem; imem++) {
    xgAll.push_back(pdfs[imem]->xfxQ(21,x,Q)); // gluon distribution
    xuAll.push_back(pdfs[imem]->xfxQ(2,x,Q)); // up-quark distribution
    pdftypes.push_back(pdfs[imem]->type()); // PdfType of member
  }

  cout << endl;
  const LHAPDF::PDFErrInfo errinfo = set.errorInfo();
  cout << "ErrorType: " << errinfo.errtype << endl;
  cout << "ErrorConfLevel: " << errinfo.conflevel << endl;

  // Count number of parameter variations = number of '+' characters.
  const size_t npar = errinfo.nmemPar();
  if (npar > 0) cout << "Last " << npar << " members are parameter variations" << endl;
  cout << endl;

  // Check that the PdfType of each member matches the ErrorType of the set.
  // NB. "Hidden" expert-only functionality -- API may change
  set._checkPdfType(pdftypes);


  // Calculate PDF uncertainty on gluon distribution.
  cout << "Gluon distribution at Q = " << Q << " GeV (normal uncertainties)" << endl;
  printUncs(set, xgAll, -1, "xg"); //< -1 => same C.L. as set
  cout << endl;

  // Calculate PDF uncertainty on up-quark distribution.
  cout << "Up-quark distribution at Q = " << Q << " GeV (normal uncertainties)" << endl;
  printUncs(set, xuAll, -1, "xu"); //< -1 => same C.L. as set
  cout << endl;

  // Calculate sanity-check PDF self-correlation between gluon and gluon.
  const double autocorr = set.correlation(xgAll, xgAll);
  cout << "Self-correlation of xg = " << autocorr << endl;
  cout << endl;

  // Calculate PDF correlation between gluon and up-quark.
  // (This is the PDF-only correlation if npar > 0.)
  const double corr = set.correlation(xgAll, xuAll);
  cout << "Correlation between xg and xu = " << corr << endl;
  cout << endl;


  // Calculate gluon PDF uncertainty scaled to 90% C.L.
  cout << "Gluon distribution at Q = " << Q << " GeV (scaled uncertainties)" << endl;
  printUncs(set, xgAll, 90, "xg"); //< -1 => same C.L. as set
  cout << endl;

  // Calculate up-quark PDF uncertainty scaled to 1-sigma.
  cout << "Up-quark distribution at Q = " << Q << " GeV (scaled uncertainties)" << endl;
  printUncs(set, xuAll, 0, "xu"); //< -1 => same C.L. as set
  cout << endl;


  if (LHAPDF::startswith(set.errorType(), "replicas")) {

    // Calculate gluon PDF as median and 90% C.L. of replica probability distribution.
    cout << "Gluon distribution at Q = " << Q << " GeV (median and 90% C.L.)" << endl;
    printUncs(set, xgAll, 90, "xg", true);
    cout << endl;

    // Calculate up-quark PDF as median and 68% C.L. of replica probability distribution.
    cout << "Up-quark distribution at Q = " << Q << " GeV (median and 68% C.L.)" << endl;
    printUncs(set, xuAll, 68, "xu", true);
    cout << endl;

  } else if (LHAPDF::startswith(set.errorType(), "hessian") || LHAPDF::startswith(set.errorType(), "symmhessian")) {

    // Generate random values from Hessian best-fit and eigenvector values.
    // See: G. Watt and R.S. Thorne, JHEP 1208 (2012) 052 [arXiv:1205.4024 [hep-ph]].

    // If npar > 0 exclude the last 2*npar members (parameter variations).
    const int npdfmem = errinfo.nmemCore();
    const int neigen = (LHAPDF::startswith(set.errorType(), "hessian")) ? npdfmem/2 : npdfmem;
    const unsigned seed = 1234;
    default_random_engine generator(seed);
    normal_distribution<double> distribution; //< mean 0.0, s.d. = 1.0
    const int nrand = 5; // generate nrand random values
    for (int irand = 1; irand <= nrand; irand++) {
      // Fill vector "randoms" with neigen Gaussian random numbers.
      vector<double> randoms;
      for (int ieigen=1; ieigen <= neigen; ieigen++) {
        double r = distribution(generator);
        randoms.push_back(r);
      }
      // const bool symmetrise = false; // average differs from best-fit
      const bool symmetrise = true; // default: average tends to best-fit
      double xgrand = set.randomValueFromHessian(xgAll, randoms, symmetrise);
      // Pass *same* random numbers to preserve correlations between xg and xu.
      double xurand = set.randomValueFromHessian(xuAll, randoms, symmetrise);
      cout << "Random " << irand << ": xg = " << xgrand << ", xu = " << xurand << endl;
    }
    // Random values generated in this way can subsequently be used for
    // applications such as Bayesian reweighting or combining predictions
    // from different groups (as an alternative to taking the envelope).
    // See, for example, material at http://mstwpdf.hepforge.org/random/.
    // The "randomValueFromHessian" function is the basis of the program
    // "examples/hessian2replicas.cc" to convert a Hessian set to replicas.
    cout << endl;

  }

  return 0;

}
