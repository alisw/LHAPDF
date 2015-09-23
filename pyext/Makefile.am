EXTRA_DIST = lhapdf.i
# ez_setup.py

## Bundling SWIG-built sources with the tarball for SL4 compat
EXTRA_DIST += lhapdf_wrap.cc lhapdf.py
lhapdf_wrap.cc lhapdf.py: lhapdf.i
	swig -c++ -python -I$(HEPMCINCPATH) -I$(top_srcdir)/include -o lhapdf_wrap.cc $<


#if PYTHON_USE
if ENABLE_PYEXT

AM_CXXFLAGS += -Wno-long-long

all-local: lhapdf_wrap.cc
	$(PYTHON) setup.py build

install-exec-local:
	$(PYTHON) setup.py install --prefix=$(DESTDIR)$(prefix)

uninstall-local:
	rm -rf $(DESTDIR)$(libdir)/python*/site-packages/lhapdf*

clean-local:
	$(PYTHON) setup.py clean --all
	@rm -f $(top_builddir)/*.pyc

distclean-local:
	@rm -rf lhapdf.egg-info
	@rm -rf dist
	@rm -f lhapdf.py
	@rm -f lhapdf_wrap.cc

endif
