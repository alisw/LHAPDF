// Test program for path searching machinery

#include "LHAPDF/Paths.h"
#include <iostream>
using namespace std;

int main() {
  BOOST_FOREACH (const string& p, LHAPDF::paths())
    cout << p << endl;

  cout << "@" << LHAPDF::findFile("lhapdf.conf") << "@" << endl;

  cout << "List of available PDFs:" << endl;
  BOOST_FOREACH (const string& s, LHAPDF::availablePDFSets())
    cout << " " << s << endl;

  return 0;
}
