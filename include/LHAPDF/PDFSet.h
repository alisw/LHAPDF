// -*- C++ -*-
//
// This file is part of LHAPDF
// Copyright (C) 2012-2022 The LHAPDF collaboration (see AUTHORS for details)
//
#pragma once
#ifndef LHAPDF_PDFSet_H
#define LHAPDF_PDFSet_H

#include "LHAPDF/Info.h"
#include "LHAPDF/Factories.h"
#include "LHAPDF/Version.h"
#include "LHAPDF/Config.h"
#include "LHAPDF/Utils.h"

namespace LHAPDF {

  /// CL percentage for a Gaussian 1-sigma
  const double CL1SIGMA = 100*erf(1/sqrt(2));


  // Forward declaration
  class PDF;


  /// @defgroup uncertainties Calculating PDF uncertainties
  ///
  /// See the PDFSet class and its PDFSet::uncertainty functions for usage.
  /// @{


  /// @brief Structure for storage of uncertainty info calculated over a PDF error set
  ///
  /// Used by the PDFSet::uncertainty functions.
  struct PDFUncertainty {
    using ErrPairs = std::vector<std::pair<double,double>>;

    /// Constructor
    PDFUncertainty(double cent=0, double eplus=0, double eminus=0, double esymm=0, double scalefactor=1,
                   double eplus_pdf=0, double eminus_pdf=0, double esymm_pdf=0,
                   double eplus_par=0, double eminus_par=0, double esymm_par=0)
      : central(cent), errplus(eplus), errminus(eminus), errsymm(esymm), scale(scalefactor),
        errplus_pdf(eplus_pdf), errminus_pdf(eminus_pdf), errsymm_pdf(esymm_pdf),
        errplus_par(eplus_par), errminus_par(eminus_par), errsymm_par(esymm_par)
    {    }

    /// Variables for the central value, +ve, -ve & symmetrised errors, and a CL scalefactor
    double central, errplus, errminus, errsymm, scale;

    /// Variables for separate PDF and parameter variation errors with combined sets
    double errplus_pdf, errminus_pdf, errsymm_pdf;
    double errplus_par, errminus_par, errsymm_par;
    double err_par; ///< @deprecated Remove redundant err_par; use errsymm_par

    /// Full error-breakdown of all quadrature uncertainty components, as (+,-) pairs
    ErrPairs errparts;
  };



  /// @brief Structure encoding the structure of the PDF error-set
  struct PDFErrInfo {
    using EnvPart = std::pair<std::string, size_t>;
    using EnvParts = std::vector<EnvPart>;
    using QuadParts = std::vector<EnvParts>;

    /// Constructor
    PDFErrInfo(QuadParts parts, double cl, const std::string& errtypestr="")
      : qparts(parts), conflevel(cl), errtype(errtypestr)
    {    }

    /// Default constructor (for STL, Cython, etc.)
    PDFErrInfo() {}

    /// Error-set quadrature parts
    QuadParts qparts;

    /// Default confidence-level
    double conflevel;

    /// Error-type annotation
    std::string errtype;

    /// Calculated name of a quadrature part
    std::string coreType() const { return qpartName(0); }

    /// Calculated name of a quadrature part
    std::string qpartName(size_t iq) const;
    /// Calculated names of all quadrature parts
    std::vector<std::string> qpartNames() const;

    /// Number of core-set members
    size_t nmemCore() const;
    /// Number of par-set members
    size_t nmemPar() const;

  };

  /// @}



  /// Class for PDF-set metadata and manipulation
  class PDFSet : public Info {
  public:

    /// Default constructor (for container compatibility)
    /// @todo Remove?
    PDFSet() { }

    /// Constructor from a set name
    /// @todo Remove?
    PDFSet(const std::string& setname);


    /// @name PDF set metadata specialisations
    /// @{

    /// @brief PDF set name
    ///
    /// @note _Not_ taken from the .info metadata file.
    std::string name() const {
      return _setname;
    }

    /// Description of the set
    std::string description() const {
      return get_entry("SetDesc");
    }

    /// First LHAPDF global index in this PDF set
    int lhapdfID() const {
      return get_entry_as<int>("SetIndex", -1);
    }

    /// Version of this PDF set's data files
    int dataversion() const {
      return get_entry_as<int>("DataVersion", -1);
    }

    /// Get the type of PDF errors in this set (replicas, symmhessian, hessian, custom, etc.)
    std::string errorType() const {
      return to_lower(get_entry("ErrorType", "UNKNOWN"));
    }

    /// Get the structured decomposition of the error-type string
    PDFErrInfo errorInfo() const;

    /// @brief Get the confidence level of the Hessian eigenvectors, in percent.
    ///
    /// If not defined, assume 1-sigma = erf(1/sqrt(2)) =~ 68.268949% by default,
    /// unless this is a replica set for which return -1.
    double errorConfLevel() const;

    /// Number of members in this set
    // int numMembers() const {
    //   return get_entry_as<int>("NumMembers");
    // }
    size_t size() const {
      return get_entry_as<unsigned int>("NumMembers");
    }

    /// Number of error members in this set
    ///
    /// @note Equal to size()-1
    size_t errSize() const {
      return size()-1;
    }

    /// @}


    /// Summary printout
    void print(std::ostream& os=std::cout, int verbosity=1) const;


    /// @name Creating PDF members
    /// @{

    /// Make the nth PDF member in this set, returned by pointer
    ///
    /// @note As with the mkPDF factory method, the PDF pointer returned by this
    /// method is heap allocated and its memory management is now the
    /// responsibility of the caller.
    PDF* mkPDF(int member) const {
      return LHAPDF::mkPDF(name(), member);
    }


    /// Make all the PDFs in this set, filling a supplied vector with PDF pointers
    ///
    /// This version may be preferred in many circumstances, since it can avoid
    /// the overhead of creating a new temporary vector.
    ///
    /// A vector of *smart* pointers can be used, for any smart pointer type which
    /// supports construction from a raw pointer argument, e.g. unique_ptr<PDF>(PDF*).
    ///
    /// @note The supplied vector will be cleared before filling!
    ///
    /// @note As with the mkPDF method and factory function, the PDF pointers
    /// returned by this method are heap allocated and their memory management
    /// is now the responsibility of the caller, either manually for raw pointers
    /// or automatically if smart pointers are used.
    ///
    /// @note Use an *appropriate* smart pointer, of course! This depends in
    /// detail on how you will use the PDF objects (do you want shared or unique
    /// pointers?), but they also need to be compatible with storage in STL
    /// containers, e.g. std::unique_ptr or std::shared_ptr but *not* the
    /// deprecated std::auto_ptr.
    //
    /// @todo Needs to be implemented in the header since the arg type is templated.
    template <typename PTR>
    void mkPDFs(std::vector<PTR>& pdfs) const {
      const int v = verbosity();
      if (v > 0) {
        std::cout << "LHAPDF " << version() << " loading all " << size() << " PDFs in set " << name() << std::endl;
        this->print(std::cout, v);
        if (this->has_key("Note")) std::cout << get_entry("Note") << std::endl;
      }
      pdfs.clear();
      pdfs.reserve(size());
      if (v < 2) setVerbosity(0); //< Disable every-member printout unless verbosity level is high
      for (size_t i = 0; i < size(); ++i) {
        /// @todo Need to use an std::move here, or write differently, for unique_ptr to work?
        pdfs.push_back( PTR(mkPDF(i)) );
      }
      setVerbosity(v);
    }

    /// Make all the PDFs in this set, returning as a vector of PDF pointers
    ///
    /// @note As with the mkPDF method and factory function, the PDF pointers
    /// returned by this method are heap allocated and their memory management
    /// is now the responsibility of the caller.
    std::vector<PDF*> mkPDFs() const {
      std::vector<PDF*> rtn;
      mkPDFs(rtn);
      return rtn;
    }

    /// @todo Use the following with default function template args if C++11 is being used
    // template <typename PTR=PDF*>
    template <typename PTR>
    std::vector<PTR> mkPDFs() const {
      std::vector<PTR> rtn;
      mkPDFs(rtn);
      return rtn;
    }

    /// @}


    /// @todo Add AlphaS getter for set-level alphaS?


    /// @name Generic metadata cascading mechanism
    /// @{

    /// Can this Info object return a value for the given key? (it may be defined non-locally)
    bool has_key(const std::string& key) const {
      return has_key_local(key) || getConfig().has_key(key);
    }

    /// Retrieve a metadata string by key name
    const std::string& get_entry(const std::string& key) const {
      if (has_key_local(key)) return get_entry_local(key); //< value is defined locally
      return getConfig().get_entry(key); //< fall back to the global config
    }

    /// Retrieve a metadata string by key name, with a fallback
    const std::string& get_entry(const std::string& key, const std::string& fallback) const {
      return Info::get_entry(key, fallback);
    }

    /// @}


    /// @name PDF set uncertainty functions
    ///
    /// See the @ref uncertainties group for more details
    /// @{

    /// @brief Calculate the central value and PDF uncertainty on an observable.
    ///
    /// @warning The @c values vector corresponds to the members of this PDF set and must be ordered accordingly.
    ///
    /// In the Hessian approach, the central value is the best-fit
    /// "values[0]" and the uncertainty is given by either the symmetric or
    /// asymmetric formula using eigenvector PDF sets.
    ///
    /// If the PDF set is given in the form of replicas, by default, the central value is
    /// given by the mean and is not necessarily "values[0]" for quantities with a non-linear
    /// dependence on PDFs, while the uncertainty is given by the standard deviation.
    ///
    /// Optional argument @c cl is used to rescale uncertainties to a
    /// particular confidence level (in percent); a negative number will rescale to the
    /// default CL for this set.
    ///
    /// @note If @c cl is omitted, automatically rescale to normal 1-sigma ~ 68.268949% uncertainties.
    ///
    /// If the PDF set is given in the form of replicas, then optional argument
    /// @c alternative equal to true (default: false) will construct a confidence
    /// interval from the probability distribution of replicas, with the central
    /// value given by the median.
    ///
    /// For a combined set, a breakdown of the separate PDF and parameter
    /// variation uncertainties is available.  The parameter variation
    /// uncertainties are computed from the last 2*n members of the set, with n
    /// the number of parameters.
    ///
    /// See the @ref uncertainties group for more details
    ///
    /// @todo Add option to restrict replica mean & stddev calculation to a central CI set?
    PDFUncertainty uncertainty(const std::vector<double>& values,
                               double cl=CL1SIGMA, bool alternative=false) const {
      PDFUncertainty rtn;
      uncertainty(rtn, values, cl, alternative);
      return rtn;
    }


    // // Trick to ensure no calls with implicit type conversion
    // template <typename T1, typename T2>
    // void uncertainty(const std::vector<double>& values, T1, T2) const = delete;

    // /// Alternative form allowing the alternative computation with default CL
    // PDFUncertainty uncertainty(const std::vector<double>& values,
    //                            bool alternative, double cl=CL1SIGMA) const {
    //   return uncertainty(values, cl, alternative);
    // }


    /// @brief Calculate the PDF uncertainty on an observable (as above), with efficient no-copy return to the @c rtn argument.
    ///
    /// @warning The @c values vector corresponds to the members of this PDF set and must be ordered accordingly.
    ///
    /// @todo For real efficiency, the chaining of these functions should be the other way around
    ///
    /// See the @ref uncertainties group for more details
    void uncertainty(PDFUncertainty& rtn,
                     const std::vector<double>& values,
                     double cl=CL1SIGMA, bool alternative=false) const;

    // // Trick to ensure no calls with implicit type conversion
    // template <typename T1, typename T2>
    // void uncertainty(PDFUncertainty& rtn, const std::vector<double>& values, T1, T2) const = delete;

    // /// Alternative form allowing the alternative computation with default CL
    // void uncertainty(PDFUncertainty& rtn,
    //                  const std::vector<double>& values,
    //                  bool alternative, double cl=CL1SIGMA) const {
    //   uncertainty(rtn, values, cl, alternative);
    // }


    /// @brief Calculate PDF uncertainties on multiple observables at once.
    ///
    /// The @c observables_values nested vector is a list of variation-value
    /// lists, with the first (outer) index corresponding to the M observables,
    /// e.g. a set of differential-observable bins, and the inner index the N PDF
    /// members.
    ///
    /// The return value is an M-element vector of PDFUncertainty summaray
    /// structs, one per observable.
    ///
    /// @warning The inner @c _values vector corresponds to the members of this
    /// PDF set and must be ordered accordingly.
    std::vector<PDFUncertainty> uncertainties(const std::vector<std::vector<double>>& observables_values,
                                              double cl=CL1SIGMA, bool alternative=false) const {
      std::vector<PDFUncertainty> rtn;
      uncertainties(rtn, observables_values, cl, alternative);
      return rtn;
    }


    /// @brief Calculate multiple PDF uncertainties (as above), with efficient no-copy return to the @c rtn argument.
    ///
    /// @warning The inner @c _values vector corresponds to the members of this
    /// PDF set and must be ordered accordingly.
    void uncertainties(std::vector<PDFUncertainty>& rtn,
                       const std::vector<std::vector<double>>& observables_values,
                       double cl=CL1SIGMA, bool alternative=false) const;


    /// @brief Calculate the PDF correlation between @c valuesA and @c valuesB using appropriate formulae for this set.
    ///
    /// The correlation can vary between -1 and +1 where values close to {-1,0,+1} mean that the two
    /// quantities A and B are {anticorrelated,uncorrelated,correlated}, respectively.
    ///
    /// For a combined set, the parameter variations are not included in the calculation of the correlation.
    ///
    /// See the @ref uncertainties group for more details
    double correlation(const std::vector<double>& valuesA, const std::vector<double>& valuesB) const;

    /// @brief Generate a random value from Hessian @c values and Gaussian random numbers.
    ///
    /// @note This routine is intended for advanced users!
    ///
    /// See Section 6 of G. Watt and R.S. Thorne, JHEP 1208 (2012) 052 [arXiv:1205.4024 [hep-ph]].
    ///
    /// Pass a vector @c values containing a value for each member of the Hessian PDF set.
    /// Pass a vector @c randoms containing neigen random numbers, where neigen is the number of distinct eigenvectors.
    ///
    /// Option @c symmetrise equal to true will symmetrise the random values (in the case of an asymmetric Hessian set)
    /// using a corrected Eq. (6.5) of arXiv:1205.4024v2, so that the average tends to the best-fit for a large number of replicas.
    ///
    /// Option @c symmetrise equal to false will use Eq. (6.4) of arXiv:1205.4024v2 (for an asymmetric Hessian set),
    /// then the average differs from the best-fit.  Option @c symmetrise has no effect for a symmetric Hessian set.
    ///
    /// Random values generated in this way can subsequently be used for applications such as Bayesian reweighting
    /// or combining predictions from different groups (as an alternative to taking the envelope).
    /// See, for example, supplementary material at http://mstwpdf.hepforge.org/random/.
    ///
    /// Use of this routine with a non-Hessian PDF set will throw a UserError.
    ///
    /// For a combined set, the parameter variations are not included in the generation of the random value.
    ///
    /// See the @ref uncertainties group for more details
    double randomValueFromHessian(const std::vector<double>& values, const std::vector<double>& randoms, bool symmetrise=true) const;


    /// Check that the type of each member matches the ErrorType of the set.
    ///
    /// @todo We need to make the signature clearer -- what is the arg? Why not
    ///   automatically check the members? Why not a plural name? Why not on PDF?
    ///   "Hiding" the name for now with the leading underscore.
    void _checkPdfType(const std::vector<string>& pdftypes) const;

    /// @}


  private:

    /// Name of this set
    std::string _setname;

    /// Cached PDF error-info breakdown struct
    mutable PDFErrInfo _errinfo;

  };


}
#endif
