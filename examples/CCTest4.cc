#include "LHAPDF/LHAPDF.h"
#include <iterator>
#include <iostream>
#include <string>
#include <cstdlib>
#include <vector>
using namespace std;

int main() {
  vector<LHAPDF::PDFSetInfo> infos = LHAPDF::getAllPDFSetInfo();
  for (vector<LHAPDF::PDFSetInfo>::const_iterator i = infos.begin();
       i != infos.end(); ++i) {
    cout << *i << endl;
  }
  return EXIT_SUCCESS;
}

#include "LHAPDF/FortranWrappers.h"
#ifdef FC_DUMMY_MAIN
int FC_DUMMY_MAIN() { return 1; }
#endif
