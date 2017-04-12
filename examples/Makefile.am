noinst_PROGRAMS = testpdf testpdfset analyticpdf compatibility testpdfunc hessian2replicas reweight
EXTRA_DIST = pythonexample.py testpdfunc.py fexample1.f

AM_CPPFLAGS += -I$(top_srcdir)/include -I$(prefix)/include $(BOOST_CPPFLAGS)
AM_LDFLAGS += -L$(top_builddir)/src -L$(prefix)/lib
LIBS = -lLHAPDF

testpdf_SOURCES = testpdf.cc
testpdfset_SOURCES = testpdfset.cc
analyticpdf_SOURCES = analyticpdf.cc
compatibility_SOURCES = compatibility.cc
testpdfunc_SOURCES = testpdfunc.cc
hessian2replicas_SOURCES = hessian2replicas.cc
reweight_SOURCES = reweight.cc
