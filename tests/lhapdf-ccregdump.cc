#include "LHAPDF/LHAPDF.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cmath>
#include <cstdlib>
using namespace std;


int main(int argc, char** argv) {
  if (argc != 4) {
    cerr << "Usage: lhapdf-ccregdump <pdffile> <pdfmember> <refdumpfile>" << endl;
    exit(1);
  }
  string pdffile = argv[1];
  int pdfmember  = atoi(argv[2]);
  string reffile = argv[3];

  LHAPDF::setVerbosity(LHAPDF::SILENT);
  LHAPDF::initPDFSet(pdffile, pdfmember);
  
  ifstream fref(reffile.c_str(), fstream::in);
  string line;
  while (getline(fref, line)) {
    istringstream iss(line);
    double x, Q;
    iss >> x;
    iss >> Q;
    vector<double> thisxfx = LHAPDF::xfx(x, Q);
    cout << scientific << x << "\t" << scientific << Q;
    for (vector<double>::const_iterator v = thisxfx.begin(); v != thisxfx.end(); ++v) {
      cout << "\t" << scientific << *v;
    }
    cout << endl;
  }
  
  return EXIT_SUCCESS;
}
