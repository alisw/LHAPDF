// -*- C++ -*-
//
// This file is part of LHAPDF
// Copyright (C) 2012-2014 The LHAPDF collaboration (see AUTHORS for details)
//
#pragma once
#ifndef LHAPDF_Utils_H
#define LHAPDF_Utils_H

// STL includes
#include <cassert>
#include <vector>
#include <map>
#include <string>
#include <algorithm>
#include <memory>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <limits>
#include <cmath>
// Boost includes
#include "boost/shared_ptr.hpp"
#include "boost/lexical_cast.hpp"
#include "boost/foreach.hpp"
#include "boost/algorithm/string.hpp"
#include "boost/range.hpp"
// System includes
#include "sys/stat.h"


/// Namespace for all LHAPDF functions and classes
namespace LHAPDF {


  // Allow implicit use of the std and boost namespaces within namespace LHAPDF
  using namespace std;
  using namespace boost;


  /// Smart pointer abstraction between C++98 and C++11
  #if __cplusplus <= 199711L
  #define unique_ptr auto_ptr
  #endif


  /// @name String handling utility functions
  //@{

  /// Make a string representation of @a val
  template <typename T>
  inline std::string to_str(const T& val) {
    return lexical_cast<string>(val);
  }

  /// Make a string representation of a vector @a vec
  template <typename T>
  inline std::string to_str(const std::vector<T>& vec) {
    vector<string> svec; svec.reserve(vec.size());
    BOOST_FOREACH (const T& t, vec) svec.push_back( to_str(t) );
    return join(svec, ",");
  }

  /// Format an integer @a val as a zero-padded string of length @a nchars
  inline std::string to_str_zeropad(int val, size_t nchars=4) {
    stringstream ss;
    ss << setfill('0') << setw(nchars) << val;
    return ss.str();
  }

  /// Does a string @a s contain the @a sub substring?
  inline bool contains(const std::string& s, const std::string& sub) {
    return s.find(sub) != string::npos;
  }

  /// Does a string @a s start with the @a sub substring?
  inline bool startswith(const std::string& s, const std::string& sub) {
    return s.find(sub) == 0;
  }

  /// Does a string @a s end with the @a sub substring?
  inline bool endswith(const std::string& s, const std::string& sub) {
    return s.find(sub) == s.length()-sub.length();
  }

  /// How many times does a string @a s contain the character @a c?
  inline size_t countchar(const std::string& s, const char c) {
    return std::count(s.begin(), s.end(), c);
  }

  //@}


  /// @name Generic path functions in the LHAPDF namespace
  //@{

  /// Check if a path @a p (either file or dir) exists
  inline bool path_exists(const std::string& p) {
    struct stat st;
    return (stat(p.c_str(), &st) == 0);
  }

  /// Check if a file @a p exists
  inline bool file_exists(const std::string& p) {
    struct stat st;
    return (stat(p.c_str(), &st) == 0 && S_ISREG(st.st_mode));
  }

  /// Check if a dir @a p exists
  inline bool dir_exists(const std::string& p) {
    struct stat st;
    return (stat(p.c_str(), &st) == 0 && S_ISDIR(st.st_mode));
  }

  /// Operator for joining strings @a a and @a b with filesystem separators
  inline std::string operator / (const std::string& a, const std::string& b) {
    string rtn = a + "/" + b;
    while (contains(rtn, "//"))
      replace_first(rtn, "//", "/");
    return rtn;
  }

  /// Get the basename (i.e. terminal file name) from a path @a p
  inline std::string basename(const std::string& p) {
    if (!contains(p, "/")) return p;
    return p.substr(p.rfind("/")+1);
  }

  /// Get the dirname (i.e. path to the penultimate directory) from a path @a p
  inline std::string dirname(const std::string& p) {
    if (!contains(p, "/")) return "";
    return p.substr(0, p.rfind("/"));
  }

  /// Get the stem (i.e. part without a file extension) from a filename @a f
  inline std::string file_stem(const std::string& f) {
    if (!contains(f, ".")) return f;
    return f.substr(0, f.rfind("."));
  }

  /// Get the file extension from a filename @a f
  inline std::string file_extn(const std::string& f) {
    if (!contains(f, ".")) return "";
    return f.substr(f.rfind(".")+1);
  }

  /// @todo Add an abspath(p) function

  //@}


  /// @name Math functions in the LHAPDF namespace
  //@{

  /// Convenience function for squaring (of any type)
  template <typename N>
  inline N sqr(const N& x) { return x*x; }

  /// Get the sign of a number
  template <typename N>
  inline int sgn(N val) { return (N(0) < val) - (val < N(0)); }

  /// Check if a number is in a range (closed-open)
  inline int in_range(double x, double low, double high) { return x >= low && x < high; }

  /// Check if a number is in a range (closed-closed)
  inline int in_closed_range(double x, double low, double high) { return x >= low && x <= high; }

  /// Check if a number is in a range (open-open)
  inline int in_open_range(double x, double low, double high) { return x > low && x < high; }

  /// @todo Add iszero() & equals(,) functions?

  //@}


  /// @name Container handling helpers
  //@{

  /// Does the vector<T> @a container contain @a item?
  template <typename T>
  inline bool contains(const std::vector<T>& container, const T& item) {
    return find(container.begin(), container.end(), item) != container.end();
  }

  // /// Does the set<T> @a container contain @a item?
  // template <typename T>
  // inline bool contains(const std::set<T>& container, const T& item) {
  //   return container.find(item) != container.end();
  // }

  /// Does the map<K,T> @a container have a key K @a key?
  template <typename K, typename T>
  inline bool has_key(const std::map<K,T>& container, const K& key) {
    return container.find(key) != container.end();
  }

  // /// @name Implementation of generic begin/end container identification by traits
  // /// taken from http://stackoverflow.com/a/9407420/91808 . Needs C++11 (or maybe just C++0x).
  // //@{

  // #include <type_traits>
  // //#include "boost/type_traits.hpp"

  // template<typename T>
  // struct has_const_iterator {
  // private:
  //   typedef char                      yes;
  //   typedef struct { char array[2]; } no;
  //   template<typename C> static yes test(typename C::const_iterator*);
  //   template<typename C> static no  test(...);
  // public:
  //   static const bool value = sizeof(test<T>(0)) == sizeof(yes);
  //   typedef T type;
  // };

  // template <typename T>
  // struct has_begin_end {
  //   template<typename C> static char (&f(typename std::enable_if<
  //     std::is_same<decltype(static_cast<typename C::const_iterator (C::*)() const>(&C::begin)),
  //     typename C::const_iterator(C::*)() const>::value, void>::type*))[1];

  //   template<typename C> static char (&f(...))[2];

  //   template<typename C> static char (&g(typename std::enable_if<
  //     std::is_same<decltype(static_cast<typename C::const_iterator (C::*)() const>(&C::end)),
  //     typename C::const_iterator(C::*)() const>::value, void>::type*))[1];

  //   template<typename C> static char (&g(...))[2];

  //   static bool const beg_value = sizeof(f<T>(0)) == 1;
  //   static bool const end_value = sizeof(g<T>(0)) == 1;
  // };

  // template<typename T>
  // struct is_container
  //   : std::integral_constant<bool, has_const_iterator<T>::value &&
  //                            has_begin_end<T>::beg_value && has_begin_end<T>::end_value>
  // { };

  //@}

}
#endif
