%module lhapdf

%include "std_string.i"
%include "std_vector.i"
//%include "std_except.i"

%{
  #define SWIG_FILE_WITH_INIT
  #include "LHAPDF/LHAPDF.h"
  #include <cstddef>
%}

namespace LHAPDF {
  class PDFSetInfo;
  %ignore SetType;
  %ignore initPDFSet(const std::string& name, SetType type, int member=0);
  %ignore initPDFSet(int nset, const std::string& name, SetType type, int member=0);
}
%template(PDFSetInfoList) std::vector<LHAPDF::PDFSetInfo>;
%template(FloatList) std::vector<double>;

//%catches(std::runtime_error) LHAPDF;
%include "LHAPDF/LHAPDF.h"

%extend LHAPDF::PDFSetInfo {
  std::string __str__() {
    return $self->toString();
  }
}
