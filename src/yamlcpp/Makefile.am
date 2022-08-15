## How to make our mangled yaml-cpp from the copied-in yaml-cpp tarball contents
# for i in `find ./ -type f -name "*.h" -o -name "*.cpp"`; do sed -e 's/namespace YAML/namespace LHAPDF_YAML/' -e 's/YAML::/LHAPDF_YAML::/g' -i "$i"; done

EXTRA_DIST = yaml-cpp
noinst_LTLIBRARIES = liblhapdf-yaml-cpp.la
liblhapdf_yaml_cpp_la_CPPFLAGS = $(AM_CPPFLAGS) -I$(srcdir)
liblhapdf_yaml_cpp_la_SOURCES = \
  binary.cpp          emitterstate.cpp  node.cpp             parser.cpp      simplekey.cpp \
  convert.cpp         emitterutils.cpp  node_data.cpp        regex_yaml.cpp  singledocparser.cpp \
  directives.cpp      exceptions.cpp    nodeevents.cpp       scanner.cpp     stream.cpp \
  emit.cpp            exp.cpp           null.cpp             scanscalar.cpp  tag.cpp \
  emitfromevents.cpp  memory.cpp        ostream_wrapper.cpp  scantag.cpp     depthguard.cpp \
  emitter.cpp         nodebuilder.cpp   parse.cpp            scantoken.cpp
dist_noinst_HEADERS = \
  collectionstack.h  exp.h          ptr_vector.h  scanscalar.h       streamcharsource.h \
  directives.h       indentation.h  regeximpl.h   scantag.h          stream.h \
  emitterstate.h     nodebuilder.h  regex_yaml.h  setting.h          stringsource.h \
  emitterutils.h     nodeevents.h   scanner.h     singledocparser.h  tag.h \
  token.h
