EXTRA_DIST = lhapdf.cpp lhapdf.pyx clhapdf.pxd __init__.py

if WITH_CYTHON
lhapdf.cpp: lhapdf.pyx clhapdf.pxd
	cython $(srcdir)/lhapdf.pyx --cplus -2 -o lhapdf.cpp
else
lhapdf.cpp:
	@echo "Not (re)generating lhapdf.cpp since Cython is not installed"
endif



## fixes for out-of-source builds, especially "make distcheck"

FIXSOURCES = $(EXTRA_DIST)

fix-out-of-source: $(FIXSOURCES)
	for i in $^; do \
	  orig="$$i"; \
	  build="$$(basename $$i)"; \
	  if [ ! -e $${build} ]; then cp $${orig} $${build}; \
	  echo $${build} >> $@; fi; \
	done
	touch $@

if WITH_PYTHON

#AM_CPPFLAGS =
PYEXT_ENV = CC="$(CXX)" CXX="$(CXX)" CFLAGS="$(CFLAGS)" CXXFLAGS="$(CXXFLAGS)" CPPFLAGS="$(CPPFLAGS) -I$(srcdir)"

all-local: fix-out-of-source
	@test "${FORCE}" = 1 && touch lhapdf.pyx || true
	$(PYEXT_ENV) $(PYTHON) build.py

install-exec-local:
	$(MKDIR_P) $(DESTDIR)$(PYTHON_PATH)/ && \
	cp -r $(abs_builddir)/build/lhapdf $(DESTDIR)$(PYTHON_PATH)/

uninstall-local:
	rm -rf $(DESTDIR)$(PYTHON_PATH)/lhapdf; \
	rmdir $(DESTDIR)$(PYTHON_PATH) || true


clean-local: fix-out-of-source
	@rm -f $(builddir)/*.pyc
	@rm -rf $(builddir)/build
	@rm -rf dist
	if [ -e $< ]; then if [ -n "$$(cat $<)" ]; then rm -f $$(cat $<); fi; fi
	rm -f $<

distclean-local:
	@true

else

all-local: fix-out-of-source

clean-local: fix-out-of-source
	if [ -e $< ]; then if [ -n "$$(cat $<)" ]; then rm -f $$(cat $<); fi; fi
	rm -f $<

endif
