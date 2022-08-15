ACLOCAL_AMFLAGS = -I m4

SUBDIRS = src include wrappers tests bin examples doc
#DIST_SUBDIRS = $(SUBDIRS) examples doc

dist_noinst_SCRIPTS = mkindex plotpdfs testpdfs testlhapdf
dist_noinst_DATA = CODINGSTYLE CONFIGFLAGS DATARELEASE DESIGN
dist_pkgdata_DATA = lhapdf.conf pdfsets.index

pkgconfigdir = $(libdir)/pkgconfig
dist_pkgconfig_DATA = lhapdf.pc

tests:
	(cd tests && $(MAKE))

examples:
	(cd examples && $(MAKE))

if WITH_DOXYGEN
doxy:
	(cd doc && $(MAKE) doxy)
doxy-upload: doxy
	rsync -r doc/doxygen/ login.hepforge.org:lhapdf/public_html/
endif

## Clean out git
#dist-hook:
#	rm -rf `find $(distdir) -name ".git"`

# Upload tarball
dist-upload: dist
	rsync $(DIST_ARCHIVES) login.hepforge.org:lhapdf/downloads/

## Remove the unused & misleading share/LHAPDF/sets directory on both install and uninstall!
# TODO: Remove this hook entirely after a couple of versions
install-data-hook:
	test -e $(pkgdatadir)/sets && rmdir $(pkgdatadir)/sets || true
uninstall-hook:
	test -e $(pkgdatadir)/sets && rmdir $(pkgdatadir)/sets || true
