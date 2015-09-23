EXTRA_DIST = pdf-plot.py pdf-ratio-plot.py

AM_FCFLAGS = $(FCFLAGS)
AM_LDFLAGS = $(top_builddir)/lib/libLHAPDF.la $(FCLIBS)

noinst_PROGRAMS = lhapdf-example1 lhapdf-example2 lhapdf-example3 lhapdf-example7 lhapdf-example8 lhapdf-example9 lhapdf-example10
lhapdf_example1_SOURCES = Example1.f
lhapdf_example2_SOURCES = Example2.f
lhapdf_example3_SOURCES = Example3.f
lhapdf_example7_SOURCES = Example7.f
lhapdf_example8_SOURCES = Example8.f
lhapdf_example9_SOURCES = Example9.f
lhapdf_example10_SOURCES = Example10.f

#TESTS_ENVIRONMENT = LHAPATH=$(top_srcdir)/PDFsets
TESTS = lhapdf-example1
# lhapdf-example2 lhapdf-example3

EXTRA_DIST += Example4.f Example5.f Example6.f
if ENABLE_LHAGLUE
noinst_PROGRAMS += lhapdf-example4 lhapdf-example5 lhapdf-example6
lhapdf_example4_SOURCES = Example4.f
lhapdf_example5_SOURCES = Example5.f
lhapdf_example6_SOURCES = Example6.f
TESTS += lhapdf-example4
endif

EXTRA_DIST += CCTest1.cc CCTest2.cc CCTest3.cc CCTest4.cc CCTest5.cc
if ENABLE_CCWRAP
noinst_PROGRAMS += lhapdf-cctest1 lhapdf-cctest2 lhapdf-cctest3 lhapdf-cctest4 lhapdf-cctest5
lhapdf_cctest1_SOURCES = CCTest1.cc
lhapdf_cctest2_SOURCES = CCTest2.cc
lhapdf_cctest3_SOURCES = CCTest3.cc
lhapdf_cctest4_SOURCES = CCTest4.cc
lhapdf_cctest5_SOURCES = CCTest5.cc
TESTS += lhapdf-cctest1
#lhapdf-cctest2 lhapdf-cctest3 lhapdf-cctest4 lhapdf-cctest5
endif

check-full:
	lhapdf-getdata CT10 MRST2004nlo.LHgrid MSTW2008{lo,nlo,nnlo}90cl Botje_100 Alekhin_100 NNPDF21_nnlo_nf5_100
	if test -x lhapdf-example1; then ./lhapdf-example1; fi
	if test -x lhapdf-example2; then ./lhapdf-example2; fi
	if test -x lhapdf-example3; then ./lhapdf-example3; fi
	if test -x lhapdf-example4; then ./lhapdf-example4; fi
	if test -x lhapdf-example5; then ./lhapdf-example5; fi
	if test -x lhapdf-example6; then ./lhapdf-example6; fi
	if test -x lhapdf-example7; then ./lhapdf-example7; fi
	if test -x lhapdf-example8; then ./lhapdf-example8; fi
	if test -x lhapdf-cctest1; then ./lhapdf-cctest1; fi
	if test -x lhapdf-cctest2; then ./lhapdf-cctest2; fi
	if test -x lhapdf-cctest3; then ./lhapdf-cctest3; fi
	if test -x lhapdf-cctest4; then ./lhapdf-cctest4; fi

clean-local:
	-rm -f *.large.grid
