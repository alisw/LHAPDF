#include <stdio.h> 
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "oct.h"
#include "octave.h"
#include "LHAPDF/LHAPDF.h"

/**
 * @mainpage An Octave wrapper for the LHAPDF library
 *
 * @section intro Introduction
 * The LHAPDF library provides an Octave wrapper for its 
 * Fortran subroutines. New users should browse this
 * documentation and take a look at the @c OctaveTest1.m 
 * which is an example of how the wrapper is used.
 *
 * @section credits Credits
 *
 * @li Originally by Philip Ilten.
 *
 * @example ./OctaveTest1.m
 * This is an example of a program using the Octave 
 * interface to LHAPDF.
 */

using namespace std;

// define the input cases
#define ERROR    0
#define NUCLEON  1
#define NUCLEAR  2
#define PION     3
#define PHOTON   4
#define CONSTANT 5
#define VARIABLE 6

// define types of PDF sets
#define TYPEPDF  7
#define TYPEGRID 8

// declare methods
int ReadArguments(octave_value_list args);
int ParseName(string *name);
bool FileExists(string name);
void InitializeLHAPDF(string name, int type, int subset);
octave_value_list Nucleon(octave_value_list args);
octave_value_list Nuclear(octave_value_list args);
octave_value_list Pion(octave_value_list args);
octave_value_list Photon(octave_value_list args);
octave_value_list Constant(octave_value_list args);
octave_value_list Variable(octave_value_list args);

// main octave argument
DEFUN_DLD(lhapdf, args, nargout,
  "-*- texinfo -*-\n\
@deftypefn {Loadable Function} {@var{z} =} lhapdf (@dots{},@var{name},@var{subset})\n\
Octave interface to the Les Houches Parton Density Function unified library. All information available through both the LHAPDF FORTRAN and C++ interfaces can be accessed through this wrapper. A variety of arguments are possible, depending upon the information being retrieved. However, all arguments must end with @var{NAME} and @var{SUBSET} specifying the PDF and subset of the PDF to use. @var{NAME} should end in a .LHgrid or .LHpdf postfix and should not include the path to the PDF set; the PDF set must be placed in the common PDFsets folder for the local installation of LHAPDF. See the LHAPDF manual for further details on how to use LHAPDF.\n\
@deftypefnx {Loadable Function} {@var{xfx} =} lhapdf (@var{x},@var{q},@var{f},@var{name},@var{subset})\n\
@deftypefnx {Loadable Function} {@var{xfa} =} lhapdf (@var{x},@var{q},@var{f},@var{a},@var{name},@var{subset})\n\
@deftypefnx {Loadable Function} {@var{xfxpion} =} lhapdf (@var{x},@var{q},@var{f},@var{p2},@var{ip},@var{name},@var{subset})\n\
@deftypefnx {Loadable Function} {@var{xfxphoton} =} lhapdf (@var{x},@var{q},@var{f},'photon',@var{name},@var{subset})\n\
Compute the parton distribution function xf(x) for the momentum fraction @var{x} at a momentum transfer @var{q} of parton @var{f} where:\n\
@ifinfo\n\
@example\n\
-----------------------------\n\
| g | d | u | s | c | b | t |\n\
| 0 | 1 | 2 | 3 | 4 | 5 | 6 |\n\
-----------------------------\n\
@end example\n\
@end ifinfo\n\
and anti-particles are specified by a negative. Additionally for the photon PDF, a value of 7 indicates the photon. All numerical arguments may be matrices but must have the same dimensions. The @var{SUBSET} must be a scalar.\n\
Specifying only @var{x}, @var{q}, @var{f} returns the nucleon momentum fraction @var{xfx}. Adding the parameter @var{a}, atomic mass, returns the nuclear momentum fraction @var{xfxa}, while adding the parameters @var{p2} and @var{ip} returns the structure function for the pion, @var{xfxpion}. Finally it is possible to return the photon structure function by specifying the string 'photon' after @var{x}, @var{q}, and @var{f}. Note that implementing a specific structure function for a non-supported PDF set will cause Octave to crash.\n\
@deftypefnx {Loadable Function} {@var{z} =} lhapdf (@var{property},@var{name},@var{subset})\n\
Return the value @var{property} for a subset of a PDF where @var{property} is one of the following strings.\n\
@table @samp\n\
@item pdfsubset\n\
Number of subsets available for a specific PDF.\n\
@item alphasorder\n\
Order of the strong coupling constant.\n\
@item pdforder\n\
Order of the PDF set.\n\
@item quarkflavors\n\
Available quark flavors.\n\
@item xmin\n\
Minimum available momentum fraction.\n\
@item xmax\n\
Maximum available momentum fraction.\n\
@item q2min\n\
Minimum available square of the momentum transfer.\n\
@item q2max\n\
Maximum available square of the momentum transfer.\n\
@item lambda4\n\
Returns Lambda 4.\n\
@item lambda5 \n\
Returns Lambda 5.\n\
@end table\n\
@deftypefnx {Loadable Function} {@var{z} =} lhapdf (@var{property},@var{y},@var{name},@var{subset})\n\
Return the value @var{property} for a subset of a PDF where @var{property} is one of the following strings evaluated at the corresponding @var{y}.\n\
@table @samp\n\
@item alphas\n\
Strong coupling constant at momentum transfer @var{y}.\n\
@item quarkmass\n\
Mass of a quark with flavor @var{y} used in the PDF set.\n\
@item quarkthreshold\n\
Threshold for a quark with flavor @var{y}.\n\
@end table\n\
@end deftypefn")
{
  switch(ReadArguments(args))
    {
    case ERROR :
      {
	octave_value_list output;
	return output;
      }
    case NUCLEON :
      {
	return Nucleon(args);
      }
    case NUCLEAR :
      {
	return Nuclear(args);
      }
    case PION :
      {
	return Pion(args);
      }
    case PHOTON :
      {
	return Photon(args);
      }
    case CONSTANT :
      {
	return Constant(args);
      }
    case VARIABLE :
      {
	return Variable(args);
      }
    }
}


// read the arguments and determine the case
int ReadArguments(octave_value_list args)
{
  // determine number of arguments
  int nargin = args.length();
  // return if too few or many arguments
  if (nargin < 3)
    {
      cout << "error: less than 3 arguments" << endl;
      return ERROR;
    }
  else if (nargin > 7)
    {
      cout << "error: more than 7 arguments" << endl;
      return ERROR;
    }
  // determine requested operation type
  // return if last two arguments are not string and integer
  if (!args(nargin-2).is_string())
    {
      cout << "error: second to last argument must be a string" << endl;
      return ERROR;
    }
  else if (!args(nargin-1).is_numeric_type())
    {
      cout << "error: last argument must be an integer" << endl;
      return ERROR;
    }
  // enter individual cases (hopefully we made it this far)
  if (nargin == 3)
    {
      // constant case
      if (args(0).is_string())
	{
	  return CONSTANT;
	}
      else
	{
	  cout << "error: first argument is not a string" << endl;
	  return ERROR;
	}
    }
  else if (nargin == 4)
    {
      // variable case
      if (args(0).is_string() && args(1).is_numeric_type())
	{
	  return VARIABLE;
	}
      else
	{
	  if (!args(0).is_string())
	    {
	      cout << "error: first argument is not a string" << endl;
	    }
	  if (!args(1).is_numeric_type())
	    {
	      cout << "error: second argument is not numeric" << endl;
	    }
	  return ERROR;
	}
    }
  else if (nargin == 5)
    {
      // nucleon case (xfx)
      if (args(0).is_numeric_type() && args(1).is_numeric_type() &&
	  args(2).is_numeric_type())
	{
	  return NUCLEON;
	}
      else
	{
	  if (!args(0).is_numeric_type())
	    {
	      cout << "error: first argument is not numeric" << endl;
	    }
	  if (!args(1).is_numeric_type())
	    {
	      cout << "error: second argument is not numeric" << endl;
	    }
	  if (!args(2).is_numeric_type())
	    {
	      cout << "error: third argument is not integer" << endl;
	    }
	  return ERROR;
	}
    }
  else if (nargin == 6)
    {
      // nuclear case (xfxa)
      if (args(0).is_numeric_type() && args(1).is_numeric_type() &&
	  args(2).is_numeric_type() && args(3).is_numeric_type())
	{
	  return NUCLEAR;
	}
      // photon case (xfxphoton)
      else if (args(0).is_numeric_type() && args(1).is_numeric_type() &&
	  args(2).is_numeric_type() && args(3).is_string())
	{
	  return PHOTON;
	}
      else
	{
	  if (!args(0).is_numeric_type())
	    {
	      cout << "error: first argument is not numeric" << endl;
	    }
	  if (!args(1).is_numeric_type())
	    {
	      cout << "error: second argument is not numeric" << endl;
	    }
	  if (!args(2).is_numeric_type() || !args(2).is_numeric_type())
	    {
	      cout << "error: third argument is not numeric or integer" << endl;
	    }
	  if (!args(3).is_numeric_type() || !args(3).is_string())
	    {
	      cout << "error: third argument is not integer or string" << endl;
	    }
	  return ERROR;
	}
    }
  else if (nargin == 7)
    {
      // pion case (xfxp)
      if (args(0).is_numeric_type() && args(1).is_numeric_type() &&
	  args(2).is_numeric_type() && args(3).is_numeric_type() &&
	  args(4).is_numeric_type())
	{
	  return PION;
	}
      else
	{
	  if (!args(0).is_numeric_type())
	    {
	      cout << "error: first argument is not numeric" << endl;
	    }
	  if (!args(1).is_numeric_type())
	    {
	      cout << "error: second argument is not numeric" << endl;
	    }
	  if (!args(2).is_numeric_type())
	    {
	      cout << "error: third argument is not numeric" << endl;
	    }
	  if (!args(3).is_numeric_type())
	    {
	      cout << "error: third argument is not integer" << endl;
	    }
	  if (!args(4).is_numeric_type())
	    {
	      cout << "error: fourth argument is not integer" << endl;
	    }
	  return ERROR;
	}
    }
}

// determine if the PDF file exists
bool FileExists(string name)
{
  fstream file;
  file.open(name.c_str(),ios::in);
  if( file.is_open() )
    {
      file.close();
      return true;
    }
  file.close();
  cout << "error: PDF file " << name << " not found" << endl;
  return false;
}

// parse the PDF set name and determine its type
int ParseName(string *name)
{
  // check to see if file exists
  string fullname = LHAPDF::pdfsetsPath()+"/"+(*name);
  if (!FileExists(fullname))
    {
      return ERROR;
    }
  // break apart the name
  string suffix = name->substr(name->find_last_of("."),name->size());
  (*name)       = name->substr(0,name->find_last_of("."));
  if (suffix == ".LHpdf")
    {
      return TYPEPDF;
    }
  else if (suffix == ".LHgrid")
    {
      return TYPEGRID;
    }
  else
    {
      cout << "error: PDF set filename does not end in '.LHpdf' or '.LHgrid'"
	   << endl;
      return ERROR;
    }
}

// initialize LHAPDF
void InitializeLHAPDF(string name, int type, int subset)
{
  switch(type)
    {
    case TYPEPDF :
      {
        LHAPDF::initPDFSet(name, LHAPDF::LHPDF, subset);
        break;
      }
    case TYPEGRID :
      {
        LHAPDF::initPDFSet(name, LHAPDF::LHGRID, subset);
        break;
      }
    }
}

// determine the nuclear momentum fraction
octave_value_list Nucleon(octave_value_list args)
{
  octave_value_list output;
  // initialize the variables
  Matrix arg1  = args(0).matrix_value();
  Matrix arg2  = args(1).matrix_value();
  Matrix arg3  = args(2).matrix_value();
  string name  = args(3).string_value();
  int subset   = args(4).int_value();
  int type     = ParseName(&name);
  if (type == ERROR)
    {
      return output;
    }
  // check that arguments are the same size
  if (arg1.rows() != arg2.rows() || arg1.columns() != arg2.columns() ||
      arg1.rows() != arg3.rows() || arg1.columns() != arg3.columns())
    {
      cout << "error: arguments X, Q, and F are not the same size" << endl;
      return output;
    }
  // initialize LHAPDF
  InitializeLHAPDF(name,type,subset);
  // determine safe ranges for X and Q
  double qMIN = sqrt(LHAPDF::getQ2min(subset)); 
  double qMAX = sqrt(LHAPDF::getQ2max(subset)); 
  double xMIN = sqrt(LHAPDF::getXmin(subset)); 
  double xMAX = sqrt(LHAPDF::getXmax(subset));
  Matrix xfx(arg1.rows(),arg1.columns());
  for (int i=0; i<arg1.rows(); i++)
    {
      for (int j=0; j<arg1.columns(); j++)
	{
	  double x = arg1(i,j);
	  double q = arg2(i,j);
	  int    f = static_cast<int>(arg3(i,j));
	  // ensure that X is within safe range
	  if (x < xMIN)
	    {
	      cout << "warning: setting X = " << x << " to X = " << xMIN 
		   << endl;
	      x = xMIN;
	    }
	  else if (x > xMAX)
	    {
	      cout << "warning: setting X = " << x << " to X = " << xMAX 
		   << endl;
	      x = xMAX;
	    }
	  // ensure that Q is within safe range
	  if (q < qMIN)
	    {
	      cout << "warning: setting Q = " << q << " to Q = " << qMIN 
		   << endl;
	      q = qMIN;
	    }
	  else if (q > qMAX)
	    {
	      cout << "warning: setting Q = " << q << " to Q = " << qMAX 
		   << endl;
	      q = qMAX;
	    }
	  // ensure that parton is between 0 and 6
	  if (fabs(f) > 6)
	    {
	      cout << "warning: setting F = " << f << " to F = 1" << endl;
	      f = 1;
	    }
	   xfx(i,j) = LHAPDF::xfx(x,q,f);
	}
    }
  return octave_value(xfx);
}

// determine the nulcear momentum fraction
octave_value_list Nuclear(octave_value_list args)
{
  octave_value_list output;
  // initialize the variables
  Matrix arg1  = args(0).matrix_value();
  Matrix arg2  = args(1).matrix_value();
  Matrix arg3  = args(2).matrix_value();
  Matrix arg4  = args(3).matrix_value();
  string name  = args(4).string_value();
  int subset   = args(5).int_value();
  int type     = ParseName(&name);
  if (type == ERROR)
    {
      return output;
    }
  // check that arguments are the same size
  if (arg1.rows() != arg2.rows() || arg1.columns() != arg2.columns() ||
      arg1.rows() != arg3.rows() || arg1.columns() != arg3.columns() ||
      arg1.rows() != arg4.rows() || arg1.columns() != arg4.columns())
    {
      cout << "error: arguments X, Q, A, and F are not the same size"
	   << endl;
      return output;
    }
  // initialize LHAPDF
  InitializeLHAPDF(name,type,subset);
  // determine safe ranges for X and Q
  double qMIN = sqrt(LHAPDF::getQ2min(subset)); 
  double qMAX = sqrt(LHAPDF::getQ2max(subset)); 
  double xMIN = sqrt(LHAPDF::getXmin(subset)); 
  double xMAX = sqrt(LHAPDF::getXmax(subset));
  Matrix xfxa(arg1.rows(),arg1.columns());
  for (int i=0; i<arg1.rows(); i++)
    {
      for (int j=0; j<arg1.columns(); j++)
	{
	  double x = arg1(i,j);
	  double q = arg2(i,j);
	  double a = arg3(i,j);
	  int    f = static_cast<int>(arg4(i,j));
	  // ensure that X is within safe range
	  if (x < xMIN)
	    {
	      cout << "warning: setting X = " << x << " to X = " << xMIN 
		   << endl;
	      x = xMIN;
	    }
	  else if (x > xMAX)
	    {
	      cout << "warning: setting X = " << x << " to X = " << xMAX 
		   << endl;
	      x = xMAX;
	    }
	  // ensure that Q is within safe range
	  if (q < qMIN)
	    {
	      cout << "warning: setting Q = " << q << " to Q = " << qMIN 
		   << endl;
	      q = qMIN;
	    }
	  else if (q > qMAX)
	    {
	      cout << "warning: setting Q = " << q << " to Q = " << qMAX 
		   << endl;
	      q = qMAX;
	    }
	  // ensure that parton is between 0 and 6
	  if (fabs(f) > 6)
	    {
	      cout << "warning: setting F = " << f << " to F = 1" << endl;
	      f = 1;
	    }
	  xfxa(i,j) = LHAPDF::xfxa(x,q,a,f);
	}
    }
  return octave_value(xfxa);
}

// determine the photon momentum fraction
octave_value_list Photon(octave_value_list args)
{
  octave_value_list output;
  // initialize the variables
  Matrix arg1  = args(0).matrix_value();
  Matrix arg2  = args(1).matrix_value();
  Matrix arg3  = args(2).matrix_value();
  string arg4  = args(3).string_value();
  string name  = args(4).string_value();
  int subset   = args(5).int_value();
  int type     = ParseName(&name);
  if (type == ERROR)
    {
      return output;
    }
  // check "photon" option is requested
  if (arg4 != "photon")
    {
      cout << "error: option '" << arg4 << "' not reconized" << endl;
      return output;
    }
  // check that arguments are the same size
  if (arg1.rows() != arg2.rows() || arg1.columns() != arg2.columns() ||
      arg1.rows() != arg3.rows() || arg1.columns() != arg3.columns())
    {
      cout << "error: arguments X, Q, and F are not the same size" << endl;
      return output;
    }
  // initialize LHAPDF
  InitializeLHAPDF(name,type,subset);
  // determine safe ranges for X and Q
  double qMIN = sqrt(LHAPDF::getQ2min(subset)); 
  double qMAX = sqrt(LHAPDF::getQ2max(subset)); 
  double xMIN = sqrt(LHAPDF::getXmin(subset)); 
  double xMAX = sqrt(LHAPDF::getXmax(subset));
  Matrix xfxphoton(arg1.rows(),arg1.columns());
  for (int i=0; i<arg1.rows(); i++)
    {
      for (int j=0; j<arg1.columns(); j++)
	{
	  double x = arg1(i,j);
	  double q = arg2(i,j);
	  int    f = static_cast<int>(arg3(i,j));
	  // ensure that X is within safe range
	  if (x < xMIN)
	    {
	      cout << "warning: setting X = " << x << " to X = " << xMIN 
		   << endl;
	      x = xMIN;
	    }
	  else if (x > xMAX)
	    {
	      cout << "warning: setting X = " << x << " to X = " << xMAX 
		   << endl;
	      x = xMAX;
	    }
	  // ensure that Q is within safe range
	  if (q < qMIN)
	    {
	      cout << "warning: setting Q = " << q << " to Q = " << qMIN 
		   << endl;
	      q = qMIN;
	    }
	  else if (q > qMAX)
	    {
	      cout << "warning: setting Q = " << q << " to Q = " << qMAX 
		   << endl;
	      q = qMAX;
	    }
	  // ensure that parton is between 0 and 6 or 7
	  if (fabs(f) > 6 && f != 7)
	    {
	      cout << "warning: setting F = " << f << " to F = 1" << endl;
	      f = 1;
	    }
	   xfxphoton(i,j) = LHAPDF::xfxphoton(x,q,f);
	}
    }
  return octave_value(xfxphoton);
}

// determine the pion momentum fraction
octave_value_list Pion(octave_value_list args)
{
  octave_value_list output;
  // initialize the variables
  Matrix arg1  = args(0).matrix_value();
  Matrix arg2  = args(1).matrix_value();
  Matrix arg3  = args(2).matrix_value();
  Matrix arg4  = args(3).matrix_value();
  Matrix arg5  = args(4).matrix_value();
  string name  = args(5).string_value();
  int subset   = args(6).int_value();
  int type     = ParseName(&name);
  if (type == ERROR)
    {
      return output;
    }
  // check that arguments are the same size
  if (arg1.rows() != arg2.rows() || arg1.columns() != arg2.columns() ||
      arg1.rows() != arg3.rows() || arg1.columns() != arg3.columns() ||
      arg1.rows() != arg4.rows() || arg1.columns() != arg4.columns() ||
      arg1.rows() != arg5.rows() || arg1.columns() != arg5.columns())
     {
      cout << "error: arguments X, Q, P2, IP, and F are not the same size"
	   << endl;
      return output;
    }
  // initialize LHAPDF
  InitializeLHAPDF(name,type,subset);
  // determine safe ranges for X and Q
  double qMIN = sqrt(LHAPDF::getQ2min(subset)); 
  double qMAX = sqrt(LHAPDF::getQ2max(subset)); 
  double xMIN = sqrt(LHAPDF::getXmin(subset)); 
  double xMAX = sqrt(LHAPDF::getXmax(subset));
  Matrix xfxpion(arg1.rows(),arg1.columns());
  for (int i=0; i<arg1.rows(); i++)
    {
      for (int j=0; j<arg1.columns(); j++)
	{
	  double x  = arg1(i,j);
	  double q  = arg2(i,j);
	  double p2 = arg3(i,j);
	  int    ip = static_cast<int>(arg4(i,j));
	  int    f  = static_cast<int>(arg5(i,j));
	  // ensure that X is within safe range
	  if (x < xMIN)
	    {
	      cout << "warning: setting X = " << x << " to X = " << xMIN 
		   << endl;
	      x = xMIN;
	    }
	  else if (x > xMAX)
	    {
	      cout << "warning: setting X = " << x << " to X = " << xMAX 
		   << endl;
	      x = xMAX;
	    }
	  // ensure that Q is within safe range
	  if (q < qMIN)
	    {
	      cout << "warning: setting Q = " << q << " to Q = " << qMIN 
		   << endl;
	      q = qMIN;
	    }
	  else if (q > qMAX)
	    {
	      cout << "warning: setting Q = " << q << " to Q = " << qMAX 
		   << endl;
	      q = qMAX;
	    }
	  // ensure that parton is between 0 and 6
	  if (fabs(f) > 6)
	    {
	      cout << "warning: setting F = " << f << " to F = 1" << endl;
	      f = 1;
	    }
	  xfxpion(i,j) = LHAPDF::xfxp(x,q,p2,ip,f);
	}
    }
  return octave_value(xfxpion);
}

// determine a constant of the PDF set
octave_value_list Constant(octave_value_list args)
{
  octave_value_list output;
  // initialize the variables
  string param = args(0).string_value();
  string name  = args(1).string_value();
  int subset   = args(2).int_value();
  int type     = ParseName(&name);
  if (type == ERROR)
    {
      return output;
    }
  // initialize LHAPDF
  InitializeLHAPDF(name,type,subset);
  // return the requested constant
  if (param == "pdfsubsets")
    {
      return octave_value(LHAPDF::numberPDF());
    }
  else if (param == "alphasorder")
    {
      return octave_value(LHAPDF::getOrderAlphaS());
    }
  else if (param == "pdforder")
    {
      return octave_value(LHAPDF::getOrderPDF());
    }
  else if (param == "quarkflavors")
    {
      return octave_value(LHAPDF::getNf());
    }
  else if (param == "xmin")
    {
      return octave_value(LHAPDF::getXmin(subset));
    }
  else if (param == "xmax")
    {
      return octave_value(LHAPDF::getXmax(subset));
    }
  else if (param == "q2min")
    {
      return octave_value(LHAPDF::getQ2min(subset));
    }
  else if (param == "q2max")
    {
      return octave_value(LHAPDF::getQ2max(subset));
    }
  else if (param == "lambda4")
    {
      return octave_value(LHAPDF::getLam4(subset));
    }
  else if (param == "lambda5")
    {
      return octave_value(LHAPDF::getLam5(subset));
    }
  else
    {
      cout << "error: parameter " << param << " not recognized" << endl;
      return output;
    }
}

// determine a variable of the PDF set
octave_value_list Variable(octave_value_list args)
{
  octave_value_list output;
  // initialize the variables
  string param = args(0).string_value();
  Matrix num   = args(1).matrix_value();
  string name  = args(2).string_value();
  int subset   = args(3).int_value();
  int type     = ParseName(&name);
  if (type == ERROR)
    {
      return output;
    }
  // initialize LHAPDF
  InitializeLHAPDF(name,type,subset);
  // determine safe ranges for X and Q
  double qMIN = sqrt(LHAPDF::getQ2min(subset)); 
  double qMAX = sqrt(LHAPDF::getQ2max(subset)); 
  double xMIN = sqrt(LHAPDF::getXmin(subset)); 
  double xMAX = sqrt(LHAPDF::getXmax(subset)); 
  // return the requested variable
  if (param == "alphas")
    {
      Matrix alphas(num.rows(),num.columns());
      // loop over flavor values
      for (int i=0; i<num.rows(); i++)
	{
	  for (int j=0; j<num.columns(); j++)
	    {
	      double q = num(i,j);
	      // ensure that Q is within safe range
	      if (q < qMIN)
		{
		  cout << "warning: setting Q = " << q << " to Q = " << qMIN 
		       << endl;
		  q = qMIN;
		}
	      else if (q > qMAX)
		{
		  cout << "warning: setting Q = " << q << " to Q = " << qMAX 
		       << endl;
		  q = qMAX;
		}
	      alphas(i,j) = LHAPDF::alphasPDF(q);
	    }
	}
      return octave_value(alphas);
    }
  else if (param == "quarkmass")
    {
      Matrix mass(num.rows(),num.columns());
      // loop over flavor values
      for (int i=0; i<num.rows(); i++)
	{
	  for (int j=0; j<num.columns(); j++)
	    {
	      int f = static_cast<int>(num(i,j));
	      // ensure that quark flavor is between 1 and 6
	      if (fabs(f) < 1 || fabs(f) > 6)
		{
		  cout << "warning: setting F = " << f << " to F = 1" << endl;
		  f = 1;
		}
	      mass(i,j) = LHAPDF::getQMass(f);
	    }
	}
      return octave_value(mass);
    }
  else if (param == "quarkthreshold")
    {
      Matrix threshold(num.rows(),num.columns());
      // loop over flavor values
      for (int i=0; i<num.rows(); i++)
	{
	  for (int j=0; j<num.columns(); j++)
	    {
	      int f = static_cast<int>(num(i,j));
	      // ensure that quark flavor is between 1 and 6
	      if (fabs(f) < 1 || fabs(f) > 6)
		{
		  cout << "warning: setting F = " << f << " to F = 1" << endl;
		  f = 1;
		}
	      threshold(i,j) = LHAPDF::getThreshold(f);
	    }
	}
      return octave_value(threshold);
    }
  else
    {
      cout << "error: parameter " << param << " not recognized" << endl;
      return output;
    }
}
