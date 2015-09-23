#include "LHAPDF/LHAPDF.h"
#include "LHAPDF/LHAPDFfw.h"
#include <iterator>
#include <iostream>
#include <cstdlib>
using namespace std;

int main() {
  cout << "Prefix path:   " << LHAPDF::prefixPath() << endl;
  cout << "Index path:    " << LHAPDF::pdfsetsIndexPath() << endl;
  cout << "PDF sets path: " << LHAPDF::pdfsetsPath() << endl;
  return EXIT_SUCCESS;
}

#include "LHAPDF/FortranWrappers.h"
#ifdef FC_DUMMY_MAIN
int FC_DUMMY_MAIN() { return 1; }
#endif
