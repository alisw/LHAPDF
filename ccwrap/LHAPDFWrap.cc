// Adapted for LHAPDFv4 by Mike Whalley.
// Adapted for LHAPDFv5 by Craig Group & Mike Whalley.
// Fortran portability and string passing by Andy Buckley.

#include "LHAPDF/LHAPDFWrap.h"
#include "LHAPDF/LHAPDFfw.h"

#include <iostream>
#include <cstring>
using namespace std;


void DEPRECATED() {
  cerr << "WARNING: Class-based C++ LHAPDF wrapper 'libLHAPDFWrap' is deprecated.\n" 
       << "Please use the namespaced functions from LHAPDF.h instead." << endl;
}


LHAPDFWrap::LHAPDFWrap() {
  DEPRECATED();
}

LHAPDFWrap::LHAPDFWrap(const string& name, int memset) {
  DEPRECATED();
  initPDFSetByName(name);
  initPDF(memset);
}

LHAPDFWrap::LHAPDFWrap(const string& name) {
  DEPRECATED();
  initPDFSetByName(name);
}


//For multiple set use
LHAPDFWrap::LHAPDFWrap(int nset, const string& name, int memset) {
  DEPRECATED();
  initPDFSetByNameM(nset, name);
  initPDFM(nset, memset);
}

LHAPDFWrap::LHAPDFWrap(int nset, const string& name) {
  DEPRECATED();
  initPDFSetByNameM(nset, name);
}



void LHAPDFWrap::initPDFSet(const string& name) { 
  char cname[1000];
  strncpy(cname, name.c_str(), 1000);
  finitpdfset(cname, name.length());
}

void LHAPDFWrap::initPDFSetByName(const string& name) { 
  char cname[1000];
  strncpy(cname, name.c_str(), 1000);
  finitpdfsetbyname(cname, name.length());
}

void LHAPDFWrap::initPDF(int memset) { 
  finitpdf(&memset); 
}

void LHAPDFWrap::getDescription() {
  fgetdesc();
}



void LHAPDFWrap::initPDFSetM(int nset, const string& name) {
  char cname[1000];
  strncpy(cname, name.c_str(), 1000);
  finitpdfsetm(&nset, cname, name.length());
}

void LHAPDFWrap::initPDFSetByNameM(int nset, const string& name) {
  char cname[1000];
  strncpy(cname, name.c_str(), 1000);
  finitpdfsetbynamem(&nset, cname, name.length());
}

void LHAPDFWrap::initPDFM(int nset, int memset) {
  finitpdfm(&nset, &memset);
}

void LHAPDFWrap::getDescriptionM(int nset) {
  fgetdescm(&nset);
}


vector<double> LHAPDFWrap::xfx(double x, double Q) {  
  vector<double> r(13);
  fevolvepdf(&x, &Q, &r[0]);
  return r;
}

double LHAPDFWrap::xfx(double x, double Q, int fl) {  
  vector<double> r(13);
  fevolvepdf(&x, &Q, &r[0]);
  return r[fl+6];
}

vector<double> LHAPDFWrap::xfxp(double x, double Q, double P2, int ip) {  
  vector<double> r(13);
  fevolvepdfp(&x, &Q, &P2, &ip, &r[0]);
  return r;
}

double LHAPDFWrap::xfxp(double x, double Q, double P2, int ip, int fl) {  
  vector<double> r(13);
  fevolvepdfp(&x, &Q, &P2, &ip, &r[0]);
  return r[fl+6];
}

vector<double> LHAPDFWrap::xfxa(double x, double Q, double a) {  
  vector<double> r(13);
  fevolvepdfa(&x, &Q, &a, &r[0]);
  return r;
}

double LHAPDFWrap::xfxa(double x, double Q, double a, int fl) {  
  vector<double> r(13);
  fevolvepdfa(&x, &Q, &a, &r[0]);
  return r[fl+6];
}

vector<double> LHAPDFWrap::xfxphoton(double x, double Q) {  
  vector<double> r(13);  
  double mphoton;
  fevolvepdfphoton(&x, &Q, &r[0], &mphoton );
  r.push_back(mphoton);
  return r;
}

double LHAPDFWrap::xfxphoton(double x, double Q, int fl) {  
  vector<double> r(13);
  double mphoton;
  fevolvepdfphoton(&x, &Q, &r[0], &mphoton);
  if ( fl == 7 )
    return mphoton;
  else
    return r[fl+6];
}

int LHAPDFWrap::numberPDF() {
  int N;
  fnumberpdf(&N); 
  return N;
}

double LHAPDFWrap::alphasPDF(double Q) {
  double a;
  falphaspdf(&Q, &a);
  return a;
}

int LHAPDFWrap::getOrderPDF() {
  int N;
  fgetorderpdf(&N);
  return N;
}

int LHAPDFWrap::getOrderAlphaS() {
  int N;
  fgetorderas(&N);
  return N;
}

double LHAPDFWrap::getQMass(int f) {
  double m;
  fgetqmass(&f, &m);
  return m;
}

double LHAPDFWrap::getThreshold(int f) {
  double m;
  fgetthreshold(&f, &m);
  return m;
}

int LHAPDFWrap::getNf() {
  int N; 
  fgetnf(&N); 
  return N;
}

double LHAPDFWrap::getLam4(int m){
  double l;
  fgetlam4(&m, &l);
  return l;
}

double LHAPDFWrap::getLam5(int m){
  double l;
  fgetlam5(&m, &l);
  return l;
}

double LHAPDFWrap::getXmin(int m){
  double xmin;
  fgetxmin(&m, &xmin);
  return xmin;
}

double LHAPDFWrap::getXmax(int m){
  double xmax;
  fgetxmax(&m, &xmax);
  return xmax;
}

double LHAPDFWrap::getQ2min(int m){
  double q2min;
  fgetq2min(&m, &q2min);
  return q2min;
}

double LHAPDFWrap::getQ2max(int m){
  double q2max;
  fgetq2max(&m, &q2max);
  return q2max;
}


// Functions below are the same as above but with M appended to their name.
// They are for multiple set use in LHAPDF.  They also take an extra paramater
// which is a label for their memory location 


vector<double> LHAPDFWrap::xfxM(int nset, double x, double Q) {  
  vector<double> r(13);
  fevolvepdfm(&nset, &x, &Q, &r[0]);
  return r;
}

double LHAPDFWrap::xfxM(int nset, double x, double Q, int fl) {  
  vector<double> r(13);
  fevolvepdfm(&nset, &x, &Q, &r[0]);
  return r[fl+6];
}

vector<double> LHAPDFWrap::xfxpM(int nset, double x, double Q, double P2, int ip) {  
  vector<double> r(13);
  fevolvepdfpm(&nset, &x, &Q, &P2,&ip, &r[0]);
  return r;
}

double LHAPDFWrap::xfxpM(int nset, double x, double Q, double P2, int ip, int fl) {  
  vector<double> r(13);
  fevolvepdfpm(&nset, &x, &Q, &P2, &ip, &r[0]);
  return r[fl+6];
}

vector<double> LHAPDFWrap::xfxaM(int nset, double x, double Q, double a) {  
  vector<double> r(13);
  fevolvepdfam(&nset, &x, &Q, &a, &r[0]);
  return r;
}

double LHAPDFWrap::xfxaM(int nset, double x, double Q, double a, int fl) {  
  vector<double> r(13);
  fevolvepdfam(&nset, &x, &Q, &a, &r[0]);
  return r[fl+6];
}

vector<double> LHAPDFWrap::xfxphotonM(int nset, double x, double Q) {  
  vector<double> r(13);
  double mphoton;
  fevolvepdfphotonm(&nset, &x, &Q, &r[0], &mphoton );
  r.push_back(mphoton);
  return r;
}

double LHAPDFWrap::xfxphotonM(int nset, double x, double Q, int fl) {  
  vector<double> r(13);
  double mphoton;
  fevolvepdfphotonm(&nset, &x, &Q, &r[0], &mphoton);
  if ( fl == 7 )
    return mphoton;
  else
    return r[fl+6];
}

int LHAPDFWrap::numberPDFM(int nset) {
  int N; 
  fnumberpdfm(&nset, &N); 
  return N;
}

double LHAPDFWrap::alphasPDFM(int nset, double Q) {
  double a;
  falphaspdfm(&nset, &Q, &a);
  return a;
}

int LHAPDFWrap::getOrderPDFM(int nset) {
  int N;
  fgetorderpdfm(&nset, &N);
  return N;
}

int LHAPDFWrap::getOrderAlphaSM(int nset) {
  int N;
  fgetorderasm(&nset, &N);
  return N;
}

double LHAPDFWrap::getQMassM(int nset, int f) {
  double m;
  fgetqmassm(&nset, &f, &m);
  return m;
}

double LHAPDFWrap::getThresholdM(int nset, int f) {
  double m;
  fgetthresholdm(&nset, &f, &m);
  return m;
}

int LHAPDFWrap::getNfM(int nset) {
  int N; 
  fgetnfm(&nset, &N); 
  return N;
}

double LHAPDFWrap::getLam4M(int nset, int m){
  double l;
  fgetlam4m(&nset, &m, &l);
  return l;
}

double LHAPDFWrap::getLam5M(int nset, int m){
  double l;
  fgetlam5m(&nset, &m, &l);
  return l;
}

double LHAPDFWrap::getXminM(int nset,int m){
  double xmin;
  fgetxminm(&nset, &m, &xmin);
  return xmin;
}

double LHAPDFWrap::getXmaxM(int nset,int m){
  double xmax;
  fgetxmaxm(&nset, &m, &xmax);
  return xmax;
}

double LHAPDFWrap::getQ2minM(int nset,int m){
  double q2min;
  fgetq2minm(&nset, &m, &q2min);
  return q2min;
}

double LHAPDFWrap::getQ2maxM(int nset,int m){
  double q2max;
  fgetq2maxm(&nset, &m, &q2max);
  return q2max;
}

void LHAPDFWrap::extrapolate(){
  fextrapolate_();
}
