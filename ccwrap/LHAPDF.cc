// Adapted for LHAPDFv4 by Mike Whalley.
// Adapted for LHAPDFv5 by Craig Group & Mike Whalley.
// Fortran portability and string passing by Andy Buckley.

#include "LHAPDF/LHAPDF.h"
#include "LHAPDF/LHAPDFfw.h"

#include <cstring>
#include <string>
#include <iostream>
#include <fstream>
#include <istream>
#include <sstream>
#include <stdexcept>
using namespace std;

#define SIZE 999

namespace LHAPDF {


  string _getFileExtension(SetType type) {
    switch(type) {
    case LHPDF:
      return "LHpdf";
    case LHGRID:
      return "LHgrid";
    }
    // This should never happen: it's only here to remove compiler warnings.
    throw runtime_error("Unknown file extension enum value in LHAPDF::_getFileExtension");
    return "";
  }

  string _mkStringFromFortran(char* fstring, size_t length) {
    // NB. "length" is the length of the target C string: the Fortran char
    // array must be one element larger, so that the trailing null can be
    // appended, like so:
    fstring[length] = '\0';
    for (size_t i = length-1; i >= 0; --i) {
      //cout << i << ": `" << fstring[i] << "'" << endl;
      if (fstring[i] != ' ') break;
      //fstring[i] = '@';
      fstring[i] = '\0';
    }
    return string(fstring);
  }

  /////////////////


  void initLHAPDF() {
    finitlhapdf();
  }


  string getVersion() {
    char fversion[10];
    fversion[9] = '\0';
    fgetlhapdfversion(fversion, 9);
    string version(fversion);
    int lastchar = version.find_last_not_of(' ');
    return version.substr(0, lastchar+1);
  }


  int getMaxNumSets() {
    int rtn;
    fgetmaxnumsets(&rtn);
    return rtn;
  }


  void setVerbosity(Verbosity noiselevel) {
    switch (noiselevel) {
    case SILENT:
      fsilent();
      break;
    case LOWKEY:
      flowkey();
      break;
    default:
      fdefaultverb();
    }
  }


  void setPDFPath(const string& path) {
    char cpath[SIZE+1];
    strncpy(cpath, path.c_str(), SIZE);
    fsetpdfpath(cpath, path.length());
  }


  void setParameter(const string& parm) {
    char cparm[SIZE+1];
    strncpy(cparm, parm.c_str(), SIZE);
    fsetlhaparm(cparm, parm.length());
  }


  ///////////////


  PDFSetInfo _makePDFSetInfo(const string& idxline) {
    istringstream iss(idxline);
    PDFSetInfo thisinfo;
    iss >> thisinfo.id;
    iss >> thisinfo.pdflibNType;
    iss >> thisinfo.pdflibNGroup;
    iss >> thisinfo.pdflibNSet;
    iss >> thisinfo.file;
    iss >> thisinfo.memberId;
    iss >> thisinfo.lowQ2;
    iss >> thisinfo.highQ2;
    iss >> thisinfo.lowx;
    iss >> thisinfo.highx;
    iss >> thisinfo.description;
    return thisinfo;
  }


  PDFSetInfo getPDFSetInfo(const string& filename, int memid) {
    const string indexpath = pdfsetsIndexPath();
    ifstream ifs(indexpath.c_str(), ifstream::in);
    string thisline;
    while (getline(ifs, thisline)) {
      PDFSetInfo thisinfo = _makePDFSetInfo(thisline);
      if (thisinfo.file != filename) continue;
      if (thisinfo.memberId != memid) continue;
      ifs.close();
      return thisinfo;
    }
    ifs.close();
    ostringstream os;
    os << "Couldn't find PDF set " << filename << ":" << memid;
    throw runtime_error(os.str());
    return PDFSetInfo();
  }


  PDFSetInfo getPDFSetInfo(int id) {
    const string indexpath = pdfsetsIndexPath();
    ifstream ifs(indexpath.c_str(), ifstream::in);
    string thisline;
    while (getline(ifs, thisline)) {
      PDFSetInfo thisinfo = _makePDFSetInfo(thisline);
      if (thisinfo.id != id) continue;
      ifs.close();
      return thisinfo;
    }
    ifs.close();
    ostringstream os;
    os << "Couldn't find PDF set #" << id;
    throw runtime_error(os.str());
    return PDFSetInfo();
  }


  vector<PDFSetInfo> getAllPDFSetInfo() {
    const string indexpath = pdfsetsIndexPath();
    vector<PDFSetInfo> infos;
    infos.reserve(4000);
    ifstream ifs(indexpath.c_str(), ifstream::in);
    string thisline;
    while (getline(ifs, thisline)) {
      PDFSetInfo thisinfo = _makePDFSetInfo(thisline);
      //cout << thisinfo << endl;
      infos.push_back(thisinfo);
    }
    ifs.close();
    return infos;
  }


  ///////////////


  string prefixPath() {
    char str[SIZE+1];
    fgetprefixpath(str, SIZE);
    return _mkStringFromFortran(str, SIZE);
  }


  string pdfsetsPath() {
    char str[SIZE+1];
    fgetdirpath(str, SIZE);
    return _mkStringFromFortran(str, SIZE);
  }


  string pdfsetsIndexPath() {
    char str[SIZE+1];
    fgetindexpath(str, SIZE);
    return _mkStringFromFortran(str, SIZE);
  }


  ///////////////


  void initPDFSet(int setid, int member) {
    PDFSetInfo info = getPDFSetInfo(setid);
    initPDFSet(info.file, info.memberId);
  }


  void initPDFSet(int nset, int setid, int member) {
    PDFSetInfo info = getPDFSetInfo(setid);
    initPDFSet(nset, info.file, info.memberId);
  }


  void initPDFSet(const string& name, SetType type, int member) {
    initPDFSet(name + "." + _getFileExtension(type), member);
  }

  void initPDFSet(int nset, const string& name, SetType type, int member) {
    initPDFSet(nset, name + "." + _getFileExtension(type), member);
  }

  void initPDFSet(const string& filename, int member) {
    char cfilename[SIZE+1];
    strncpy(cfilename, filename.c_str(), SIZE);
    if (filename.find("/") == string::npos) {
      finitpdfsetbyname(cfilename, filename.length());
    } else {
      finitpdfset(cfilename, filename.length());
    }
    usePDFMember(member);
  }

  void initPDFSet(int nset, const string& filename, int member) {
    char cfilename[SIZE+1];
    strncpy(cfilename, filename.c_str(), SIZE);
    if (filename.find("/") == string::npos) {
      finitpdfsetbynamem(&nset, cfilename, filename.length());
    } else {
      finitpdfsetm(&nset, cfilename, filename.length());
    }
    usePDFMember(nset, member);
  }

  void usePDFMember(int member) {
    finitpdf(&member);
  }

  void usePDFMember(int nset, int member) {
    finitpdfm(&nset, &member);
  }


  ///////////////


  void initPDFByName(const string& name, SetType type, int memset) {
    initPDFSetByName(name, type);
    initPDF(memset);
  }
  void initPDFByName(int nset, const string& name, SetType type, int memset) {
    initPDFSetByName(nset, name, type);
    initPDF(nset, memset);
  }

  void initPDFByName(const string& filename, int memset) {
    initPDFSetByName(filename);
    initPDF(memset);
  }
  void initPDFByName(int nset, const string& filename, int memset) {
    initPDFSetByName(nset, filename);
    initPDF(nset, memset);
  }

  // void initPDFSet(const string& path) {
  //   char cpath[SIZE+1];
  //   strncpy(cpath, path.c_str(), SIZE);
  //   finitpdfset(cpath, path.length());
  // }
  // void initPDFSet(int nset, const string& path) {
  //   char cpath[SIZE+1];
  //   strncpy(cpath, path.c_str(), SIZE);
  //   finitpdfsetm(&nset, cpath, path.length());
  // }

  void initPDFSetByName(const string& name, SetType type) {
    initPDFSetByName(name + "." + _getFileExtension(type));
  }
  void initPDFSetByName(int nset, const string& name, SetType type) {
    initPDFSetByName(nset, name + "." + _getFileExtension(type));
  }

  void initPDFSetByName(const string& filename) {
    char cfilename[SIZE+1];
    strncpy(cfilename, filename.c_str(), SIZE);
    finitpdfsetbyname(cfilename, filename.length());
  }
  void initPDFSetByName(int nset, const string& filename) {
    char cfilename[SIZE+1];
    strncpy(cfilename, filename.c_str(), SIZE);
    finitpdfsetbynamem(&nset, cfilename, filename.length());
  }

  void initPDF(int memset) {
    finitpdf(&memset);
  }
  void initPDF(int nset, int memset) {
    finitpdfm(&nset, &memset);
  }


  //////////////////


  vector<double> xfx(double x, double Q) {
    vector<double> r(13);
    fevolvepdf(&x, &Q, &r[0]);
    return r;
  }
  vector<double> xfx(int nset, double x, double Q) {
    vector<double> r(13);
    fevolvepdfm(&nset, &x, &Q, &r[0]);
    return r;
  }

  void xfx(double x, double Q, double* results) {
    fevolvepdf(&x, &Q, results);
  }
  void xfx(int nset, double x, double Q, double* results) {
    fevolvepdfm(&nset, &x, &Q, results);
  }

  double xfx(double x, double Q, int fl) {
    vector<double> r(13);
    fevolvepdf(&x, &Q, &r[0]);
    return r[fl+6];
  }
  double xfx(int nset, double x, double Q, int fl) {
    vector<double> r(13);
    fevolvepdfm(&nset, &x, &Q, &r[0]);
    return r[fl+6];
  }



  vector<double> xfxp(double x, double Q, double P2, int ip) {
    vector<double> r(13);
    fevolvepdfp(&x, &Q, &P2, &ip, &r[0]);
    return r;
  }
  vector<double> xfxp(int nset, double x, double Q, double P2, int ip) {
    vector<double> r(13);
    fevolvepdfpm(&nset, &x, &Q, &P2,&ip, &r[0]);
    return r;
  }

  void xfxp(double x, double Q, double P2, int ip, double* results) {
    fevolvepdfp(&x, &Q, &P2, &ip, results);
  }
  void xfxp(int nset, double x, double Q, double P2, int ip, double* results) {
    fevolvepdfpm(&nset, &x, &Q, &P2, &ip, results);
  }

  double xfxp(double x, double Q, double P2, int ip, int fl) {
    vector<double> r(13);
    fevolvepdfp(&x, &Q, &P2, &ip, &r[0]);
    return r[fl+6];
  }
  double xfxp(int nset, double x, double Q, double P2, int ip, int fl) {
    vector<double> r(13);
    fevolvepdfpm(&nset, &x, &Q, &P2, &ip, &r[0]);
    return r[fl+6];
  }


  vector<double> xfxa(double x, double Q, double a) {
    vector<double> r(13);
    fevolvepdfa(&x, &Q, &a, &r[0]);
    return r;
  }
  vector<double> xfxa(int nset, double x, double Q, double a) {
    vector<double> r(13);
    fevolvepdfam(&nset, &x, &Q, &a, &r[0]);
    return r;
  }

  void xfxa(double x, double Q, double a, double* results) {
    fevolvepdfa(&x, &Q, &a, results);
  }
  void xfxa(int nset, double x, double Q, double a, double* results) {
    fevolvepdfam(&nset, &x, &Q, &a, results);
  }

  double xfxa(double x, double Q, double a, int fl) {
    vector<double> r(13);
    fevolvepdfa(&x, &Q, &a, &r[0]);
    return r[fl+6];
  }
  double xfxa(int nset, double x, double Q, double a, int fl) {
    vector<double> r(13);
    fevolvepdfam(&nset, &x, &Q, &a, &r[0]);
    return r[fl+6];
  }


  vector<double> xfxphoton(double x, double Q) {
    vector<double> r(13);
    double mphoton;
    fevolvepdfphoton(&x, &Q, &r[0], &mphoton );
    r.push_back(mphoton);
    return r;
  }
  vector<double> xfxphoton(int nset, double x, double Q) {
    vector<double> r(13);
    double mphoton;
    fevolvepdfphotonm(&nset, &x, &Q, &r[0], &mphoton );
    r.push_back(mphoton);
    return r;
  }

  void xfxphoton(double x, double Q, double* results) {
    fevolvepdfphoton(&x, &Q, results, results+13);
  }
  void xfxphoton(int nset, double x, double Q, double* results) {
    fevolvepdfphotonm(&nset, &x, &Q, results, results+13);
  }

  double xfxphoton(double x, double Q, int fl) {
    vector<double> r(13);
    double mphoton;
    fevolvepdfphoton(&x, &Q, &r[0], &mphoton);
    if (fl == 7) return mphoton;
    return r[fl+6];
  }
  double xfxphoton(int nset, double x, double Q, int fl) {
    vector<double> r(13);
    double mphoton;
    fevolvepdfphotonm(&nset, &x, &Q, &r[0], &mphoton);
    if ( fl == 7 ) return mphoton;
    return r[fl+6];
  }


  /////////////////////

  // Returns double central, double errplus, double errminus, double errsym
  // double[4] getPDFUncertainty(vector<double> values);
  // double[4] getPDFUncertainty(int nset, vector<double> values);
  // ? double[4] getPDFUncertainty(x, Q);
  // ? double[4] getPDFUncertainty(int nset, x, Q);

  /////////////////////


  void getDescription() {
    fgetdesc();
  }
  void getDescription(int nset) {
    fgetdescm(&nset);
  }

  bool hasPhoton() {
    int haspho;
    fhasphoton(&haspho);
    return (haspho == 1);
  }

  int numberPDF() {
    int N;
    fnumberpdf(&N);
    return N;
  }
  int numberPDF(int nset) {
    int N;
    fnumberpdfm(&nset, &N);
    return N;
  }

  double alphasPDF(double Q) {
    double a;
    falphaspdf(&Q, &a);
    return a;
  }
  double alphasPDF(int nset, double Q) {
    double a;
    falphaspdfm(&nset, &Q, &a);
    return a;
  }

  int getOrderPDF() {
    int N;
    fgetorderpdf(&N);
    return N;
  }
  int getOrderPDF(int nset) {
    int N;
    fgetorderpdfm(&nset, &N);
    return N;
  }

  int getOrderAlphaS() {
    int N;
    fgetorderas(&N);
    return N;
  }
  int getOrderAlphaS(int nset) {
    int N;
    fgetorderasm(&nset, &N);
    return N;
  }

  double getQMass(int f) {
    double m;
    fgetqmass(&f, &m);
    return m;
  }
  double getQMass(int nset, int f) {
    double m;
    fgetqmassm(&nset, &f, &m);
    return m;
  }

  double getThreshold(int f) {
    double m;
    fgetthreshold(&f, &m);
    return m;
  }
  double getThreshold(int nset, int f) {
    double m;
    fgetthresholdm(&nset, &f, &m);
    return m;
  }

  int getNf() {
    int N;
    fgetnf(&N);
    return N;
  }
  int getNf(int nset) {
    int N;
    fgetnfm(&nset, &N);
    return N;
  }

  double getLam4(int m) {
    double l;
    fgetlam4(&m, &l);
    return l;
  }
  double getLam4(int nset, int m) {
    double l;
    fgetlam4m(&nset, &m, &l);
    return l;
  }

  double getLam5(int m) {
    double l;
    fgetlam5(&m, &l);
    return l;
  }
  double getLam5(int nset, int m) {
    double l;
    fgetlam5m(&nset, &m, &l);
    return l;
  }

  double getXmin(int m) {
    double xmin;
    fgetxmin(&m, &xmin);
    return xmin;
  }
  double getXmin(int nset,int m) {
    double xmin;
    fgetxminm(&nset, &m, &xmin);
    return xmin;
  }

  double getXmax(int m) {
    double xmax;
    fgetxmax(&m, &xmax);
    return xmax;
  }
  double getXmax(int nset,int m) {
    double xmax;
    fgetxmaxm(&nset, &m, &xmax);
    return xmax;
  }

  double getQ2min(int m) {
    double q2min;
    fgetq2min(&m, &q2min);
    return q2min;
  }
  double getQ2min(int nset,int m) {
    double q2min;
    fgetq2minm(&nset, &m, &q2min);
    return q2min;
  }

  double getQ2max(int m) {
    double q2max;
    fgetq2max(&m, &q2max);
    return q2max;
  }

  void extrapolate(bool extrapolate) {
    if (extrapolate) {
      fextrapolateon();
    } else {
      fextrapolateoff();
    }
  }
  double getQ2max(int nset,int m) {
    double q2max;
    fgetq2maxm(&nset, &m, &q2max);
    return q2max;
  }


}
