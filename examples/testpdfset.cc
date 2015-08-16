#include "LHAPDF/LHAPDF.h"

#include <string>
#include <vector>
#include <map>
using namespace std;

int main() {

  // Get a PDF set object and use it to get a vector of heap-allocated PDFs
  LHAPDF::PDFSet set("CT10nlo");
  vector<LHAPDF::PDF*> pdfs = set.mkPDFs();
  BOOST_FOREACH (LHAPDF::PDF* p, pdfs) {
    const double xf_g = p->xfxQ(21, 1e-3, 126.0);
    cout << xf_g << endl;
    delete p; //< Manual deletion required
  }

  // Directly get a PDF vector using smart pointers for memory handling.
  // NB. C++11 isn't essential: it just supplies unique_ptr (and shared_ptr)
  #if __cplusplus > 199711L
  #ifndef BOOST_NO_CXX11_SMART_PTR
  typedef unique_ptr<LHAPDF::PDF> PDFPtr;
  vector<PDFPtr> smartpdfs = set.mkPDFs<PDFPtr>();
  #endif
  #endif

  return 0;
}
