dist_noinst_SCRIPTS = lhapdf-regdump lhapdf-regtest
EXTRA_DIST = lhapdf-ccregdump.cc

if ENABLE_CCWRAP
noinst_PROGRAMS = lhapdf-ccregdump
lhapdf_ccregdump_SOURCES = lhapdf-ccregdump.cc
lhapdf_ccregdump_LDFLAGS = -L$(top_builddir)/lib
lhapdf_ccregdump_LDADD = -lm -lLHAPDF
endif
