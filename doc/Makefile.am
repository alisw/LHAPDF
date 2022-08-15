EXTRA_DIST = mkpdflist pdfsets.dox

if WITH_DOXYGEN
doxy:
	@echo "Running Doxygen..."
	ln -sf $(top_srcdir)/TODO TODO.dox
	ln -sf $(top_srcdir)/DESIGN DESIGN.dox
	ln -sf $(top_srcdir)/CODINGSTYLE CODINGSTYLE.dox
	ln -sf $(top_srcdir)/CONFIGFLAGS CONFIGFLAGS.dox
	ln -sf $(top_srcdir)/INSTALL INSTALL.dox
	ln -sf $(top_srcdir)/migration/README MIGRATION.dox
	@$(DOXYGEN) || true
install-data-local: doxy
	@$(NORMAL_INSTALL)
	$(MKDIR_P) $(DESTDIR)$(docdir)/doxygen/search
	$(INSTALL_DATA) doxygen/*.* $(DESTDIR)$(docdir)/doxygen/
	$(INSTALL_DATA) doxygen/search/* $(DESTDIR)$(docdir)/doxygen/search/

uninstall-local:
	@$(NORMAL_UNINSTALL)
	@rm -rf $(DESTDIR)$(docdir)/doxygen
	@test ! -d $(DESTDIR)$(docdir) || rmdir --ignore-fail-on-non-empty $(DESTDIR)$(docdir)
endif
