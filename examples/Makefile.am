AM_CPPFLAGS += -I$(top_srcdir)/include
AM_LDFLAGS += -L$(top_builddir)/src -L$(prefix)/lib
LIBS = -lLHAPDF

## Define example install dir
pkgexampledir = $(docdir)/examples

## Build compiled examples
noinst_PROGRAMS = testpdf testpdfset analyticpdf compatibility testpdfunc hessian2replicas reweight
testpdf_SOURCES = testpdf.cc
testpdfset_SOURCES = testpdfset.cc
analyticpdf_SOURCES = analyticpdf.cc
compatibility_SOURCES = compatibility.cc
testpdfunc_SOURCES = testpdfunc.cc
hessian2replicas_SOURCES = hessian2replicas.cc
reweight_SOURCES = reweight.cc

## Install compiled examples
dist_pkgexample_DATA = testpdf testpdfset analyticpdf compatibility testpdfunc hessian2replicas reweight

## Install example sources
## C++
dist_pkgexample_DATA += testpdf.cc testpdfset.cc analyticpdf.cc compatibility.cc testpdfunc.cc hessian2replicas.cc reweight.cc
## Python
dist_pkgexample_DATA += pythonexample.py testpdfunc.py
## Fortran: we don't build, since we didn't test for a compiler, but build like e.g.:
##   gfortran fexample1.f90 -o fexample1 -L/path/to/lhapdf/libdir -lLHAPDF -lstdc++
dist_pkgexample_DATA += fexample1.f90 fexample2.f90

uninstall-local:
	@$(NORMAL_UNINSTALL)
	@rm -rf $(DESTDIR)$(pkgexampledir)
	@test ! -d $(DESTDIR)$(docdir) || rmdir --ignore-fail-on-non-empty $(DESTDIR)$(docdir)
