EXTRA_DIST = lhapdf-octave-example1.m wrapoctave.cc

pkgdata_DATA = lhapdf.oct

lhapdf.oct: wrapoctave.cc
	$(MKOCTFILE) -I$(top_srcdir)/include -v -g -o lhapdf.oct wrapoctave.cc -L$(top_builddir)/lib -L$(top_builddir)/lib/.libs -lLHAPDF

TESTS_ENVIRONMENT = LHAPATH=$(top_srcdir)/PDFsets $(OCTAVE)
TESTS = $(srcdir)/lhapdf-octave-example1.m
