#include "LHAPDF/LHAPDF.h"
#include <cstdlib>
using namespace std;

int main() {
  LHAPDF::initPDFSet("NNPDF21_nnlo_nf5_100.LHgrid");
  return EXIT_SUCCESS;
}


#include "FortranWrappers.h"
#ifdef FC_DUMMY_MAIN
int FC_DUMMY_MAIN() { return 1; }
#endif
