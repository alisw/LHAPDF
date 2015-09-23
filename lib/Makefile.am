## Make main LHAPDF library
lib_LTLIBRARIES = libLHAPDF.la
libLHAPDF_la_SOURCES = 
libLHAPDF_la_LDFLAGS = $(VERSIONFLAGS)
libLHAPDF_la_LIBADD = $(FCLIBS) $(top_builddir)/src/libLHAPDFCore.la
if ENABLE_CCWRAP
libLHAPDF_la_LIBADD += $(top_builddir)/ccwrap/libLHAPDFCCWrap.la
endif
## Hack to get linking done by CXX
libLHAPDF_la_LINK = $(LIBTOOL) --tag=CXX --mode=link $(CXX) $(AM_CXXFLAGS) $(CXXFLAGS) \
                    $(AM_LDFLAGS) $(LDFLAGS) -o $@


## Make deprecated compatibility C++ library
if ENABLE_OLDCCWRAP
lib_LTLIBRARIES += libLHAPDFWrap.la
libLHAPDFWrap_la_SOURCES = 
libLHAPDFWrap_la_LDFLAGS = -version-info 1:0:1
libLHAPDFWrap_la_LIBADD = $(FCLIBS) libLHAPDF.la $(top_builddir)/ccwrap/libLHAPDFOldWrap.la
endif
