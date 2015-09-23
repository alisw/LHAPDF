EXTRA_DIST = Doxyfile

if ENABLE_CCWRAP


## C++ wrapper library to be combined into libLHAPDF
noinst_LTLIBRARIES = libLHAPDFCCWrap.la
libLHAPDFCCWrap_la_SOURCES = LHAPDFfw.f LHAPDF.cc
## This accidentally double-links the dummy main on Mac OS X
#libLHAPDFCCWrap_la_LIBADD = $(FCLIBS)

## Deprecated C++ wrapper header
if ENABLE_OLDCCWRAP
noinst_LTLIBRARIES += libLHAPDFOldWrap.la
libLHAPDFOldWrap_la_SOURCES = LHAPDFWrap.cc
## This accidentally double-links the dummy main on Mac OS X
#libLHAPDFOldWrap_la_LIBADD = $(FCLIBS)
endif

## Deal with the Doxygen stuff
if WITH_DOXYGEN
.PHONY = dox
dox:
	$(DOXYGEN) $(top_srcdir)/ccwrap/Doxyfile || true
all-local: dox
	@true
install-data-local:
	$(install_sh_DATA) doxy/html/* $(DESTDIR)$(pkgdatadir)/doc/html
mostlyclean-local:
	rm -rf $(top_builddir)/ccwrap/doxy
uninstall-local:
	rm -rf $(DESTDIR)$(pkgdatadir)/doc; \
	test ! -d $(DESTDIR)$(pkgdatadir) || rmdir --ignore-fail-on-non-empty $(DESTDIR)$(pkgdatadir)
endif


else


## Make sure that source is always bundled
EXTRA_DIST += \
  LHAPDFfw.f LHAPDF.cc LHAPDF.h LHAPDFfw.h \
  LHAPDFWrap.cc LHAPDFWrap.h


endif
