AC_DEFUN([BOOST_BUG_IN_FOREACH],
[
dnl
dnl see http://code-muse.com/2013/08/06/boostforeach-and-compiler-bugs
dnl
m4_pattern_allow([BOOST_FOREACH])
AC_MSG_CHECKING([if boost rvalue bug is present])
oldcppflags_boost="$CPPFLAGS"
CPPFLAGS="$CPPFLAGS $BOOST_CPPFLAGS"
AC_RUN_IFELSE([
  AC_LANG_PROGRAM([[
    #include <boost/shared_ptr.hpp>
    #include <boost/foreach.hpp>
    #include <vector>
    const std::vector<boost::shared_ptr<int> > f()
    {
      return std::vector<boost::shared_ptr<int> >(4,
        boost::shared_ptr<int>(new int(12)));
    }
  ]],
  [[int sum = 48;
    BOOST_FOREACH(const boost::shared_ptr<int> &pi, f())
    {
      sum -= *pi;
    }
    return sum;
  ]]
  )],
  [
  AC_MSG_RESULT([no])
  ],
  [
  AC_MSG_RESULT([yes])
  AC_MSG_ERROR([
***********************************************
*** Boost is too old for this compiler version. $CXX needs boost > 1_47.
*** See http://code-muse.com/2013/08/06/boostforeach-and-compiler-bugs
***********************************************])
  ]
)
CPPFLAGS="$oldcppflags_boost"
])
