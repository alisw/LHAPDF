## Process this file with autoconf to produce a configure script.

AC_PREREQ(2.61)
AC_INIT([LHAPDF],[5.9.1],[lhapdf@projects.hepforge.org],[lhapdf])
if test "$prefix" = "$PWD"; then
  AC_MSG_ERROR([Installation into the build directory is not supported: use a different --prefix argument])
fi
AC_CONFIG_AUX_DIR([config])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE()
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])


## Library version flags (at least update the version comment with each new release version)
VERSIONFLAGS="-version-info 1:6:5" ## for 5.8.6
AC_SUBST(VERSIONFLAGS)
AC_DEFINE_UNQUOTED(LHAPDF_VERSION, "$PACKAGE_VERSION", "LHAPDF version string")

## Mac OS X compatibility
AC_CHECK_TOOL(SWVERS, sw_vers)
if test x$SWVERS != x; then
  PROD_NAME=$($SWVERS -productName | cut -f 2 -d:)
fi
AM_CONDITIONAL(WITH_OSX, [test "$PROD_NAME" == "Mac OS X"])
if test "$PROD_NAME" == "Mac OS X"; then
  MACOSX_DEPLOYMENT_TARGET=$($SWVERS -productVersion | cut -f 1,2 -d.)
  AC_MSG_NOTICE([MACOSX_DEPLOYMENT_TARGET = $MACOSX_DEPLOYMENT_TARGET])
  AM_CXXFLAGS="$AM_CXXFLAGS -Dunix"
  AM_CXXFLAGS="$AM_LDFLAGS -flat_namespace"
fi


## Checks and config headers
AC_LANG(C++)
AC_CONFIG_SRCDIR([src/LHpdflib.F])
AC_CONFIG_HEADERS([config/config.h include/LHAPDF/FortranWrappers.h include/LHAPDF/LHAPDFConfig.h])


## Set Fortran compiler behaviour
if test "x$FCFLAGS" == "x"; then
  FCFLAGS="-O2"
fi
## Try to respect users' Fortran compiler variables
if test "x$FC" == "x"; then
  if test "x$F77" == "x"; then
    FC="$GFORTRAN"
  else
    FC="$F77"
  fi
fi
#AC_LANG_PUSH(Fortran)
AC_PROG_FC
#AC_FC_SRCEXT(f90)
AC_FC_LIBRARY_LDFLAGS
AC_FC_WRAPPERS
AC_FC_FREEFORM
#AC_LANG_POP(Fortran)


## Checks for programs
AC_PROG_CXX
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_LN_S


## Enable binreloc
AM_BINRELOC


## Hack to make libtool use the FC Fortran compiler via F77, since
## libtool < 2.x doesn't understand --tag=FC
F77=$FC
AM_FFLAGS=$AM_FCFLAGS
FFLAGS="$FFLAGS $FCFLAGS"
AC_SUBST(FCFLAGS)
AC_SUBST(FFLAGS)
AC_SUBST(F77)
AC_SUBST(AM_FFLAGS)

AC_LIBTOOL_DLOPEN
AC_PROG_LIBTOOL

## Select pdf sets to include
AC_MSG_CHECKING([for pdf sets to include])

allowedsets=(mrst mrst06 mrst98 mrstqed cteq grv nnpdf mstw gjr h1 zeus hera alekhin botje fermi hkn pions photons user)

AC_ARG_ENABLE(pdfsets,
        AC_HELP_STRING([--enable-pdfsets=LIST],[comma-separated list of pdf sets to include.
Options are: mrst mrst06 mrst98 mrstqed cteq grv nnpdf mstw gjr h1 zeus hera alekhin botje fermi hkn pions photons user]),
        [],
        [enable_pdfsets=all]
        )
if test "x$enable_pdfsets" = "xyes" -o "x$enable_pdfsets" = "xall"; then
   all=yes
fi
AC_MSG_RESULT([$enable_pdfsets])

isneg=()
for ((i=0; i<${#allowedsets@<:@*@:>@}; i++)); do
    setname=${allowedsets@<:@i@:>@}
    isnegative=$( echo "$enable_pdfsets" | egrep -i -- "^-$setname\$|^-$setname,|,-$setname,|,-$setname\$" )

    if test "$isnegative"; then
	all=yes
	isneg@<:@i@:>@=neg
    fi
done

for ((i=0; i<${#allowedsets@<:@*@:>@}; i++)); do
    setname=${allowedsets@<:@i@:>@}
    if test "$setname"; then
        if test ! ${isneg@<:@i@:>@}; then
	    ispos=$( echo "$enable_pdfsets" | egrep -i -- "^$setname\$|^$setname,|,$setname,|,$setname\$" )
    	    if test "$all" -o "$ispos"; then
               declare $setname=yes
  	        fi
        fi
    fi
done

AM_CONDITIONAL(WANT_ALL,[test "$all" -a ${#isneg[*]} = 0])
AM_CONDITIONAL(WANT_MRST,[test "$mrst"])
AM_CONDITIONAL(WANT_MRST06,[test "$mrst06"])
AM_CONDITIONAL(WANT_MRST98,[test "$mrst98"])
AM_CONDITIONAL(WANT_MRSTQED,[test "$mrstqed"])
AM_CONDITIONAL(WANT_CTEQ,[test "$cteq"])
AM_CONDITIONAL(WANT_NNPDF,[test "$nnpdf"])
AM_CONDITIONAL(WANT_MSTW,[test "$mstw"])
AM_CONDITIONAL(WANT_GRV,[test "$grv"])
AM_CONDITIONAL(WANT_GJR,[test "$gjr"])
AM_CONDITIONAL(WANT_H1,[test "$h1"])
AM_CONDITIONAL(WANT_ZEUS,[test "$zeus"])
AM_CONDITIONAL(WANT_HERA,[test "$hera"])
AM_CONDITIONAL(WANT_ALEKHIN,[test "$alekhin"])
AM_CONDITIONAL(WANT_BOTJE,[test "$botje"])
AM_CONDITIONAL(WANT_FERMI,[test "$fermi"])
AM_CONDITIONAL(WANT_HKN,[test "$hkn"])
AM_CONDITIONAL(WANT_PIONS,[test "$pions"])
AM_CONDITIONAL(WANT_PHOTONS,[test "$photons"])
AM_CONDITIONAL(WANT_USER,[test "$user"])
AM_CONDITIONAL(WANT_QCDNUM,[test "$mrst" -o "$mrst98" -o "$zeus" -o "$hera" -o "$alekhin" -o "$botje" -o "$fermi" -o "$user"])

## Use lightweight version (no CTEQ/MRST Hessian sets)
AC_ARG_ENABLE([low-memory],
  [AC_HELP_STRING(--enable-low-memory,
  [use reduced CTEQ and MRST PDF sets (default=use full sets)])])
if test x$enable_low_memory == xyes; then
  AC_MSG_NOTICE(Using low memory mode: full Hessian error sets for CTEQ and MRST will not be held concurrently in memory)
  AC_DEFINE(LHAPDF_LOWMEM, "1", "Has LHAPDF been built in low memory mode?")
fi
AM_CONDITIONAL(ENABLE_LOWMEM, [test x$enable_low_memory == xyes])


## NMXSET - max number of concurrent PDF sets
## Normal default number of PDF sets is 3
default_max_num_pdfsets=3
AC_ARG_WITH([max-num-pdfsets],
  [AC_HELP_STRING(--with-max-num-pdfsets,
  [maximum number of PDF sets that can be held concurrently in memory - affects virtual memory overhead (default=3)])],
  [], [with_max_num_pdfsets=$default_max_num_pdfsets])
NMXSET=$with_max_num_pdfsets
if test ! $NMXSET -gt 0 2> /dev/null; then
  AC_MSG_ERROR([Invalid number of PDF sets allocated: $NMXSET])
else
  AC_MSG_NOTICE([Number of concurrent PDF set arrays allocated = $NMXSET])
fi
AC_DEFINE_UNQUOTED(LHAPDF_NMXSET, "$NMXSET", "Max number of concurrent PDF sets")
AC_SUBST(NMXSET)


## Enable debugging symbols.
AC_ARG_ENABLE([debug],
  [AC_HELP_STRING(--enable-debug, [build with debugging symbols (default=no)])],
  [], [enable_debug=no])
if test x$enable_debug == xyes; then
  [AM_FCFLAGS="$AM_FCFLAGS -g"]
fi


## Enable LHAGLUE compatibility fns for PDFLIB
AC_ARG_ENABLE([lhaglue],
  [AC_HELP_STRING(--disable-lhaglue, [build without PDFLIB compatibility routines])],
  [], [enable_lhaglue=yes])
if test x$enable_lhaglue == xyes; then
   AC_MSG_NOTICE(Building LHAGLUE wrappers)
else
   AC_MSG_NOTICE(Not building LHAGLUE wrappers)
fi
AM_CONDITIONAL(ENABLE_LHAGLUE, [test x$enable_lhaglue == xyes])


## Enable C++ wrapper
AC_ARG_ENABLE([ccwrap],
  [AC_HELP_STRING(--disable-ccwrap, [build without C++ wrapper])],
  [], [enable_ccwrap=yes])
if test x$enable_ccwrap == xyes; then
   AC_MSG_NOTICE(Building C++ wrapper)
else
   AC_MSG_NOTICE(Not building C++ wrapper)
fi
AM_CONDITIONAL(ENABLE_CCWRAP, [test x$enable_ccwrap == xyes])


## Backwards compatibility C++ wrapper
## TODO: remove completely
AC_ARG_ENABLE([old-ccwrap],
  [AC_HELP_STRING(--disable-old-ccwrap, [build without deprecated C++ wrapper])],
  [], [enable_old_ccwrap=no])
if test x$enable_ccwrap == xyes; then
   if test x$enable_old_ccwrap == xyes; then
      AC_MSG_NOTICE(Also building deprecated C++ wrapper library)
      AC_MSG_WARN(Use of libLHAPDFWrap C++ wrapper library is deprecated!)
   else
      AC_MSG_NOTICE(Not building deprecated C++ wrapper library... good for you.)
   fi
else
   ## If we're not building the new C++ wrapper, don't build the old one either.
   enable_old_ccwrap=no
fi
AM_CONDITIONAL(ENABLE_OLDCCWRAP, [test x$enable_old_ccwrap == xyes])


## Build Doxygen if possible
AC_ARG_ENABLE([doxygen],
  [AC_HELP_STRING(--disable-doxygen, [don't try to make Doxygen documentation])],
  [], [enable_doxygen=yes])
if test x$enable_doxygen == xyes; then
   AC_PATH_PROG(DOXYGEN, doxygen)
fi
AM_CONDITIONAL(WITH_DOXYGEN, [test x$DOXYGEN != x])


## Python extension
AC_ARG_ENABLE(pyext, [AC_HELP_STRING(--disable-pyext,
  [don't build Python module (default=build)])],
  [], [enable_pyext=yes])
if test x$enable_ccwrap != xyes; then
   AC_MSG_WARN([Can't build Python extension since the C++ wrapper is not being built])
   enable_pyext=no
fi
AZ_PYTHON_DEFAULT
## Basic Python checks
if test x$enable_pyext == xyes; then
  AZ_PYTHON_PATH
  AZ_PYTHON_VERSION_ENSURE([2.3])
  ## Test for Python header
  if test -x "$PYTHON"; then
    AC_MSG_CHECKING([for Python include path])
    python_incpath=`$PYTHON -c "import distutils.sysconfig; print distutils.sysconfig.get_python_inc();"`
    AC_MSG_RESULT([$python_incpath])
    python_header="$python_incpath/Python.h"
    if test -z "$python_incpath"; then
      AC_MSG_WARN([Can't build Python extension since include directory cannot be determined from distutils])
      enable_pyext=no
    elif test ! -e "$python_header"; then
      AC_MSG_WARN([Can't build Python extension since header file $python_header cannot be found])
      enable_pyext=no
    fi
  fi
fi
## SWIG version checks
if test x$enable_pyext == xyes -a ! -e pyext/lhapdf_wrap.cc; then
  AC_PATH_PROG(SWIG, swig)
  if test -x "$SWIG"; then
    swig_version=`$SWIG -version 2>&1 | grep Version | cut -f3 -d" "`
    swig_major=`echo $swig_version | cut -f1 -d.`
    swig_minor=`echo $swig_version | cut -f2 -d.`
    swig_micro=`echo $swig_version | cut -f3 -d.`
    let swig_version_num="1000*$swig_major+100*$swig_minor+$swig_micro"
    if test "$swig_version_num" -lt 1333; then
    #if test "$swig_major" -eq 1 -a "$swig_minor" -eq 3 -a "$swig_micro" -lt 33; then
      AC_MSG_WARN([Can't build Python extension since SWIG version is too old to build wrapper files: please use a tarball release])
      enable_pyext=no
    fi
  fi
fi
## Finish
AM_CONDITIONAL(ENABLE_PYEXT, [test x$enable_pyext == xyes])
if test x$enable_pyext == xyes; then
  AC_MSG_NOTICE([All Python build checks successful: 'lhapdf' Python extension will be built])
fi


## Build Octave interface if possible
AC_ARG_ENABLE([octave],
  [AC_HELP_STRING(--disable-octave, [don't try to make Octave interface])],
  [], [enable_octave=yes])
if test x$enable_octave == xyes; then
   AC_PATH_PROG(OCTAVE, octave)
   AC_PATH_PROG(MKOCTFILE, mkoctfile)
fi
AM_CONDITIONAL(WITH_OCTAVE, [test x$OCTAVE != x && test x$MKOCTFILE != x])


## Make a flag to detect if this build is on Windows
AC_MSG_CHECKING(if the target platform is Windows)
on_win32=no
if `echo "$target" | grep win32`; then on_win32=yes; fi
AC_MSG_RESULT([$on_win32])
AM_CONDITIONAL(USING_WIN32, [test x$on_win32 == xyes])


## Set final FCFLAGS, CXXFLAGS and CPPFLAGS
AM_CPPFLAGS="$AM_CPPFLAGS -I\$(top_srcdir)/include -I\$(top_builddir)/include"
AM_CPPFLAGS=["$AM_CPPFLAGS -DDEFAULTPREFIXPATH=\\\"$prefix\\\" -DDEFAULTLHAPATH=\\\"$datadir\\\" "]
AC_SUBST(AM_CPPFLAGS)
#
AC_CEDAR_CHECKCXXFLAG([-pedantic], [AM_CXXFLAGS="$AM_CXXFLAGS -pedantic "])
AC_CEDAR_CHECKCXXFLAG([-ansi], [AM_CXXFLAGS="$AM_CXXFLAGS -ansi "])
AC_CEDAR_CHECKCXXFLAG([-Wall], [AM_CXXFLAGS="$AM_CXXFLAGS -Wall "])
AC_SUBST(AM_CXXFLAGS)
#
AC_CEDAR_CHECKFCFLAG([-pedantic], [AM_FCFLAGS="$AM_FCFLAGS -pedantic "])
AC_CEDAR_CHECKFCFLAG([-Wall], [AM_FCFLAGS="$AM_FCFLAGS -Wall "])
AC_CEDAR_CHECKFCFLAG([-ffree-form], [AM_FCFLAGS="$AM_FCFLAGS -ffree-form "])
## For libtool F77 mode:
AM_FFLAGS=$AM_FCFLAGS
AC_SUBST(AM_FCFLAGS)
AC_SUBST(AM_FFLAGS)


## Output
AC_CONFIG_FILES([Makefile])
AC_CONFIG_FILES([src/Makefile src/parmsetup.inc])
AC_CONFIG_FILES([include/Makefile include/LHAPDF/Makefile])
AC_CONFIG_FILES([ccwrap/Makefile])
AC_CONFIG_FILES([lib/Makefile])
AC_CONFIG_FILES([examples/Makefile])
AC_CONFIG_FILES([pyext/Makefile pyext/setup.py])
AC_CONFIG_FILES([octave/Makefile])
AC_CONFIG_FILES([bin/Makefile bin/lhapdf-config])
AC_CONFIG_FILES([tests/Makefile])
AC_OUTPUT

## Message about getting PDF sets
pkgdatadir=`eval "echo $datarootdir/$PACKAGE_TARNAME"`
echo
echo "****************************************************************"
echo "IMPORTANT INFORMATION ABOUT PDF SETS"
echo
echo "Due to the increasing number and size of PDF data files, LHAPDF"
echo "no longer bundles PDF set data in the package tarball. The sets"
echo "are instead all stored online at"
echo "  http://www.hepforge.org/archive/lhapdf/pdfsets/"
echo "and you should install those that you wish to use into"
echo "  $pkgdatadir"
echo
echo "The lhapdf-getdata script helps to automate the set download"
echo "and installation process: before installation you will find"
echo "it in the bin directory. Alternatively, download the sets by"
echo "hand from the URL given above, and place them in the PDF sets"
echo "directory (also given above.)"
echo "****************************************************************"
echo
