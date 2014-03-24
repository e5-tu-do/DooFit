#ifndef DOOFIT_BUILDER_EASYPDF_EASYPDF_h
#define DOOFIT_BUILDER_EASYPDF_EASYPDF_h

// from STL
#include <map>
#include <string>

// from Boost
#include <boost/exception/all.hpp>

// from RooFit
#include "RooWorkspace.h"
#include "RooGlobalFunc.h"
#include "RooArgSet.h"
#include "RooArgList.h"

// from DooCore
#include "doocore/io/MsgStream.h"

// forward declarations
class RooRealVar;
class RooAbsCategoryLValue;
class RooAbsPdf;
class RooGaussian;
class RooCBShape;
class RooExponential;
class RooArgList;
class RooProdPdf;
class RooExtendPdf;
class RooAbsReal;
class RooAddPdf;
class RooFormulaVar;
class RooCategory;
class RooGaussModel;
class RooAddModel;
class RooDecay;
class RooResolutionModel;
class RooEffProd;
class RooBDecay;
class RooSimultaneous;
class RooSuperCategory;
class RooKeysPdf;
class RooHistPdf;
class RooAbsHiddenReal;
class RooUnblindUniform;
class RooLognormal;
class RooGaussEfficiencyModel;
class RooAbsGaussModelEfficiency;
class RooEffResAddModel;
class RooAbsBinning;
class RooBinning;

/** @class doofit::builder::EasyPdf
 *  @brief Easy PDF and variable building without the clutter
 *
 *  This simple class is designed to make PDF and variable building less 
 *  painful. It is not intended for complex automated PDF building but rather to
 *  simply construct and access single PDFs and variables with automated 
 *  bookkeeping (and therefore also deletion upon EasyPdf destruction of all
 *  generated objects).
 * 
 *  Automatic workspace support is available if a workspace is supplied upon 
 *  construction. In this case, all objects are automatically added to the 
 *  workspace after construction. Only the workspace versions of these objects 
 *  are referenced and used. Also, in workspace mode EasyPdf will @a not delete 
 *  the objects on EasyPdf destruction.
 *
 *  @section usage Usage
 *
 *  Usage is shown via this example:
 *
 * @code
 * #include "doofit/builder/EasyPdf/EasyPdf.h"
 * int main() {
 *   EasyPdf epdf;
 *
 *   // generate a Gaussian PDF, the PDF and all variables are generated ad-hoc
 *   epdf.Gaussian("pdf_gauss", epdf.Var("mass"), epdf.Var("mean"), epdf.Var("sigma"));
 *
 *   // set range for mass variable 
 *   // (keep in mind, this can also be done in a handy way by using the 
 *   // RooArgSet::readFromFile() function)
 *   epdf.Var("mass").setMin(5100);
 *   epdf.Var("mass").setMax(5500);
 *   
 *   // add an exponential PDF
 *   epdf.Exponential("pdf_expo", epdf.Var("time"), epdf.Var("c"));
 *   
 *   // multiply the PDFs
 *   epdf.Product("pdf_prod", RooArgList(epdf.Pdf("pdf_gauss"), epdf.Pdf("pdf_expo")));
 *   
 *   // most of the above could also be written like this in practically one line
 *   epdf.Product("pdf_prod2",
 *                RooArgList(epdf.Gaussian("pdf_gauss2", epdf.Var("mass"), epdf.Var("mean"), epdf.Var("sigma")),
 *                epdf.Exponential("pdf_expo2", epdf.Var("time"), epdf.Var("c"))));
 * }
 * @endcode
 *
 *  The main goal of EasyPdf is to keep PDF/variable construction as sinmple as 
 *  possible. Therefore, variable and PDF generation and access are mostly 
 *  indistinguishable. Objects are created as needed and stored via their name.
 *
 *  @note About const-correctness: You may notice that nearly all functions are
 *        non-const. This is due to the reason that RooFit breaks 
 *        const-correctness all the time. For example, you cannot fit on a const
 *        RooAbsPdf&.
 *
 * @author Florian Kruse
 */

namespace doofit {
namespace builder {
class EasyPdf {
 public:
  /**
   *  @brief Default constructor for EasyPdf
   *
   *  If a RooWorkspace is supplied, all variables will be imported to this 
   *  RooWorkspace and not deleted upon EasyPdf destruction.
   *
   *  @param ws optional RooWorkspace to use
   */
  EasyPdf(RooWorkspace* ws = NULL);
  
  /**
   *  @brief Destructor for EasyPdf
   */
  ~EasyPdf();
  
  /**
   *  @brief Clean up all internally stored objects
   *
   *  Upon destruction or sometimes on users' demand it is necessary to delete
   *  all internally stored objects. This function takes care of that. In
   *  workspace mode, the user has to take care of emptying the workspace.
   */
  void PurgeAllObjects();
  
  /** @name Variable access and creation
   *  Functions to access and/or create variables (real, formula, categories)
   */
  ///@{
  /**
   *  @brief Add or access real based variable (RooRealVar, RooFormulaVar, RooAbsHiddenReal (blinded params))
   *
   *  Request a RooAbsReal by a specified name. If the variable/formula does not 
   *  yet exist in this EasyPdf pool of variables or formulas, it is created as 
   *  a RooRealVar and returned.
   *  Otherwise it will be returned from the pool. Variables take preference 
   *  over formulas taking preference over hidden reals (blinded parameters) 
   *  in case of duplicate entries.
   *
   *  @param name name of the RooAbsReal
   *  @return the appropriate RooAbsReal
   */
  RooAbsReal& Real(const std::string& name);
  
  /**
   *  @brief Add or access RooRealVar
   *
   *  Request a RooRealVar by a specified name. If the variable does not yet 
   *  exist in this EasyPdf pool of variables, it is created and returned. 
   *  Otherwise it will be returned from the pool.
   *
   *  @param name name of the RooRealVar
   *  @return the appropriate RooRealVar
   */
  RooRealVar& Var(const std::string& name);
  
  /**
   *  @brief Add or access RooCategory
   *
   *  Request a RooCategory by a specified name. If the category does not yet
   *  exist in this EasyPdf pool of categories, it is created and returned.
   *  Otherwise it will be returned from the pool.
   *
   *  @param name name of the RooCategory
   *  @return the appropriate RooCategory
   */
  RooCategory& Cat(const std::string& name);
  
  /**
   *  @brief Add and access RooFormulaVar
   *
   *  Request a RooFormulaVar by a specified name. If the formula does not yet
   *  exist in this EasyPdf pool of formulas, it is created and returned.
   *  Otherwise an exception ObjectExistsException is thrown.
   *
   *  @param name name of the RooFormulaVar
   *  @param formula the formula to use when creating a new formula
   *  @param dependents the dependents in the formula
   *  @return the appropriate RooFormulaVar
   */
  RooFormulaVar& Formula(const std::string& name, const std::string& formula,
                         const RooArgList& dependents);
  
  /**
   *  @brief Add and access a RooUnblindUniform
   *
   *  Request a RooUnblindUniform by a specified name. If the parameter does not
   *  yet exist in this EasyPdf pool of hidden reals, it is created and 
   *  returned. Otherwise an exception ObjectExistsException is thrown.
   *
   *  @param name name of the RooUnblindUniform
   *  @param blind_string the blinding string to use
   *  @param scale the blinding scale to use
   *  @param blind_value the parameter to blind
   *  @return the appropriate RooUnblindUniform
   */
  RooUnblindUniform& UnblindUniform(const std::string& name, const std::string& blind_string,
                                    double scale, RooAbsReal& blind_value);
  
  /**
   *  @brief Add and access RooSuperCategory
   *
   *  Request a RooSuperCategory by a specified name. If the category does not 
   *  yet exist in this EasyPdf pool of suoper categories, it is created and 
   *  returned.
   *  Otherwise an exception ObjectExistsException is thrown.
   *
   *  @param name name of the RooSuperCategory
   *  @param input_categories argset of categories to combine into super category
   *  @return the appropriate RooSuperCategory
   */
  RooSuperCategory& SuperCat(const std::string& name, const RooArgSet& input_categories);
  
  /**
   *  @brief Access RooFormulaVar
   *
   *  Request a RooFormulaVar by a specified name. If the formula does exist in 
   *  this EasyPdf pool of formulas, it is returned.
   *  Otherwise an exception ObjectNotExistingException is thrown.
   *
   *  @param name name of the RooFormulaVar
   *  @return the appropriate RooFormulaVar
   */
  RooFormulaVar& Formula(const std::string& name);
  
  /**
   *  @brief Access RooAbsHiddenReal (i.e. blinded parameters)
   *
   *  Request a RooAbsHiddenReal by a specified name. If the hidden real does 
   *  exist in this EasyPdf pool of hidden reals, it is returned.
   *  Otherwise an exception ObjectNotExistingException is thrown.
   *
   *  @param name name of the RooAbsHiddenReal
   *  @return the appropriate RooAbsHiddenReal
   */
  RooAbsHiddenReal& HiddenReal(const std::string& name);
  
  /**
   *  @brief Add or access RooSuperCategory
   *
   *  Request a RooSuperCategory by a specified name. If the category does exist
   *  in this EasyPdf pool of suoper categories, it is returned.
   *  Otherwise an exception ObjectNotExistingException is thrown.
   *
   *  @param name name of the RooSuperCategory
   *  @return the appropriate RooSuperCategory
   */
  RooSuperCategory& SuperCat(const std::string& name);
  
  /**
   *  @brief Add or access a fraction based on recursive fractions
   *
   *  This helper function defines a regular fraction as RooFormulaVar based on
   *  a supplied list of recursive fractions (as RooArgList). In this list the 
   *  fractions are ordered as fraction1, recursive_fraction2, 
   *  recursive_fraction3 and so on. The number of supplied fractions is handled
   *  automatically.
   *
   *  @param name of the fraction to define
   *  @param recursive_fractions list of recursive fractions
   *  @return the fraction as formula to use in added PDFs.
   */
  RooFormulaVar& RecursiveFraction(const std::string& name, const RooArgList& recursive_fractions);
  ///@}

  /** @name Item title and unit manipulation
   *  Functions to conveniently set titles and units of items
   */
  ///@{
  /**
   *  @brief Set item titles based on EasyConfig file
   *
   *  Based on a supplied EasyConfig file (see doocore::config::EasyConfig), 
   *  this function will set titles of all items that can be matched via name
   *  in the EasyConfig file. It will search for the section "easypdf_titles" in 
   *  the config file. Items are matched by name like this:
   *
   *  @code
   *  easypdf_titles
   *  {
   *    variable_name "fancy variable title for plots et al."
   *  }
   *  @endcode
   *
   *  @param config_file config file that will be parsed
   */
  void SetTitles(const std::string& config_file);
  
  /**
   *  @brief Set variable units based on EasyConfig file
   *
   *  Based on a supplied EasyConfig file (see doocore::config::EasyConfig),
   *  this function will set units of all variables that can be matched via name
   *  in the EasyConfig file. It will search for the section "easypdf_units" in
   *  the config file. Items are matched by name like this:
   *
   *  @code
   *  easypdf_units
   *  {
   *    variable_name "MeV/c^{2}"
   *  }
   *  @endcode
   *
   *  @param config_file config file that will be parsed
   */
  void SetUnits(const std::string& config_file);
  ///@}
  
  /** @name Variable collection access
   *  Functions to access and/or create collections of variables (real, formula, categories)
   */
  ///@{
  /**
   *  @brief Add and/or access RooRealVars, RooCategories and RooFormulaVars as RooArgSet
   *
   *  Request a set of RooRealVars, RooCategories or RooFormulaVars as a 
   *  comma-separated list of specified names.
   *  If the variables, categories or formulas do not yet exist in this EasyPdf 
   *  pool of variables, they are created and returned. Otherwise they will be 
   *  returned from the pool. If a variable, category or formula exist
   *  under the supplied name, the variable will be returned.
   *  This check is performed individually for each variable.
   *
   *  A name for the set can be defined via define_set_name for usage of 
   *  Set(const std::string&) afterwards. In workspace mode it will also define
   *  a corresponding set on the RooWorkspace.
   *
   *  @param names names of the variables as comma separated string
   *  @param define_set_name (optional) define an internal set with the given name
   *  @return the appropriate RooRealVars as RooArgSet
   */
  RooArgSet Vars(const std::string& names, const std::string define_set_name="");
  
  /**
   *  @brief Access previously defined set as RooArgSet
   *
   *  Request a set of RooRealVars, RooCategories or RooFormulaVars that was 
   *  previously defined via Vars() via its name. If the set was not defined, 
   *  an ObjectNotExistingException is thrown.
   *
   *  @param set_name name of the previously defined set
   *  @return the appropriate RooRealVars as RooArgSet
   */
  RooArgSet Set(const std::string& set_name);
  
  /**
   *  @brief Add and/or access RooRealVars, RooCategories and RooFormulaVars as RooArgList
   *
   *  Request a set of RooRealVars, RooCategories or RooFormulaVars as a
   *  comma-separated list of specified names.
   *  If the variables, categories or formulas do not yet exist in this EasyPdf
   *  pool of variables, they are created and returned. Otherwise they will be
   *  returned from the pool. If a variable, category or formula exist
   *  under the supplied name, the variable will be returned.
   *  This check is performed individually for each variable
   *
   *  @param names names of the variables as comma separated string
   *  @return the appropriate RooRealVars as RooArgList
   */
  RooArgList VarList(const std::string& names);
  
  /**
   *  @brief Access all RooRealVars and RooFormulaVars as RooArgSet
   *
   *  Request a set of all RooRealVars and RooFormulaVars.
   *
   *  @return the appropriate RooRealVars as RooArgSet
   */
  RooArgSet AllVars();
  ///@}
  
  /** @name Object existency check
   *  Functions to test existence of variables
   */
  ///@{
  /**
   *  @brief Check if real based variable exists (RooRealVar or RooFormulaVar, RooAbsHiddenReal (blinded params))
   *
   *  Check if a RooAbsReal exists by a specified name. If the variable/formula 
   *  does exist in this EasyPdf pool of variables or formulas, true is returned,
   *  otherwise false.
   *
   *  @param name name of the RooAbsReal
   *  @return whether the variable already exists
   */
  bool RealExists(const std::string& name);
  
  /**
   *  @brief Check if PDF exists
   *
   *  Check if a PDF exists by a specified name. If it does exist in this 
   *  EasyPdf pool of variables or formulas, true is returned, otherwise false.
   *
   *  @param name name of the RooAbsPdf
   *  @return whether the variable already exists
   */
  bool PdfExists(const std::string& name);
  
  /**
   *  @brief Check if binning exists
   *
   *  Check if a binning exists by a specified name. If it does exist in this
   *  EasyPdf pool of binnings, true is returned, otherwise false.
   *
   *  @param name name of the binning
   *  @return whether the binning already exists
   */
  bool BinningExists(const std::string& name);
  
  /**
   *  @brief Check if defined set exists
   *
   *  Check if a named set exists by a specified name. If it does exist in this
   *  EasyPdf pool of sets, true is returned, otherwise false.
   *
   *  @param set_name name of the defined set
   *  @return whether the set already exists
   */
  bool SetExists(const std::string& set_name) const;

  ///@}
  
  /** @name Basic PDFs
   *  PDF definitions of trivial PDFs
   */
  ///@{
  /**
   *  @brief Add and access a Gaussian PDF
   *
   *  Request a RooGaussian by a specified name. If the PDF does not yet
   *  exist in this EasyPdf pool of PDFs, it is created and returned.
   *  Otherwise an exception ObjectExistsException is thrown.
   *
   *  @param name name of the PDF
   *  @param x x variable
   *  @param mean mean variable
   *  @param sigma sigma variable
   *  @return the appropriate PDF
   */
  RooGaussian& Gaussian(const std::string& name, RooAbsReal& x, RooAbsReal& mean, RooAbsReal& sigma);
  
  /**
   *  @brief Add and access a Crystal Ball Shape PDF
   *
   *  Request a RooCBShape by a specified name. If the PDF does not yet
   *  exist in this EasyPdf pool of PDFs, it is created and returned.
   *  Otherwise an exception ObjectExistsException is thrown.
   *
   *  @param name name of the PDF
   *  @param x x variable
   *  @param mean mean variable
   *  @param sigma sigma variable
   *  @param alpha alpha variable
   *  @param n n variable
   *  @return the appropriate PDF
   */
  RooCBShape& CBShape(const std::string& name, RooAbsReal& x, RooAbsReal& mean, RooAbsReal& sigma, RooAbsReal& alpha, RooAbsReal& n);
  
  /**
   *  @brief Add and access an Exponential PDF
   *
   *  Request a RooExponential by a specified name. If the PDF does not yet
   *  exist in this EasyPdf pool of PDFs, it is created and returned.
   *  Otherwise an exception ObjectExistsException is thrown.
   *
   *  @param name name of the PDF
   *  @param x x variable
   *  @param e exponent variable
   *  @return the appropriate PDF
   */
  RooExponential& Exponential(const std::string& name, RooAbsReal& x, RooAbsReal& e);
  
  /**
   *  @brief Add and access an decay PDF
   *
   *  Request a RooDecay by a specified name. If the PDF does not yet
   *  exist in this EasyPdf pool of PDFs, it is created and returned.
   *  Otherwise an exception ObjectExistsException is thrown.
   *
   *  @param name name of the PDF
   *  @param t t variable
   *  @param tau lifetime of decay
   *  @param model the resolution model to use
   *  @return the appropriate PDF
   */
  RooDecay& Decay(const std::string& name, RooRealVar& t, RooAbsReal& tau, const RooResolutionModel& model);
  
  /**
   *  @brief Add and access a lognormal PDF
   *
   *  Request a RooLognormal by a specified name. If the PDF does not yet
   *  exist in this EasyPdf pool of PDFs, it is created and returned.
   *  Otherwise an exception ObjectExistsException is thrown.
   *
   *  @param name name of the PDF
   *  @param x x variable
   *  @param m1 median of the first distribution
   *  @param k1 first shape parameter
   *  @param m2 median of the second distribution
   *  @param k2 second shape parameter
   *  @return the appropriate PDF
   */
  RooLognormal& Lognormal(const std::string& name, RooAbsReal& x, RooAbsReal& m, RooAbsReal& k);
  
  /**
   *  @brief Add and access a double lognormal PDF
   *
   *  Request a double RooLognormal by a specified name. If the PDF does not yet
   *  exist in this EasyPdf pool of PDFs, it is created and returned.
   *  Otherwise an exception ObjectExistsException is thrown.
   *
   *  @param name name of the PDF
   *  @param x x variable
   *  @param m median of the distribution
   *  @param k shape parameter
   *  @param fraction fraction of first Gaussian
   *  @return the appropriate PDF
   */
  RooAddPdf& DoubleLognormal(const std::string& name, RooAbsReal& x, RooAbsReal& m1, RooAbsReal& k1, RooAbsReal& m2, RooAbsReal& k2, RooAbsReal& fraction);

  /**
   *  @brief Add and access a simultaneous PDF
   *
   *  Request a RooSimultaneous by a specified name. If the PDF does not yet
   *  exist in this EasyPdf pool of PDFs, it is created and returned.
   *  Otherwise an exception ObjectExistsException is thrown.
   *
   *  @param name name of the PDF
   *  @param category index category for the simultaneous PDF
   *  @return the appropriate PDF
   */
  RooSimultaneous& Simultaneous(const std::string& name, RooAbsCategoryLValue& category);
  
  /**
   *  @brief Add and access a simultaneous PDF
   *
   *  Request a RooSimultaneous by a specified name. If the PDF does not yet
   *  exist in this EasyPdf pool of PDFs, it is created and returned.
   *  Otherwise an exception ObjectExistsException is thrown.
   *
   *  @param name name of the PDF
   *  @param pdfs full list of client PDFs, each with the associated index label
   *  @param category index category for the simultaneous PDF
   *  @return the appropriate PDF
   */
  RooSimultaneous& Simultaneous(const std::string& name, std::map<std::string,RooAbsPdf*> pdfs, RooAbsCategoryLValue& category);
  ///@}
  
  /** @name PDF combinations/modifications
   *  Definitions of PDFs using and/or modifying other PDFs
   */
  ///@{
  /**
   *  @brief Add and access an product PDF
   *
   *  Request a RooProdPdf by a specified name. If the PDF does not yet
   *  exist in this EasyPdf pool of PDFs, it is created and returned.
   *  Otherwise an exception ObjectExistsException is thrown.
   *
   *  @param name name of the PDF
   *  @param pdfs RooArgList of factor PDFs
   *  @return the appropriate PDF
   */
  RooProdPdf& Product(const std::string& name, const RooArgList& pdfs);
  
  /**
   *  @brief Add and access an product PDF with RooCmdArg support
   *
   *  Request a RooProdPdf by a specified name. If the PDF does not yet
   *  exist in this EasyPdf pool of PDFs, it is created and returned.
   *  Otherwise an exception ObjectExistsException is thrown.
   *
   *  @param name name of the PDF
   *  @param pdfs RooArgList of factor PDFs
   *  @param arg1 RooCmdArg
   *  @param arg2 RooCmdArg
   *  @param arg3 RooCmdArg
   *  @param arg4 RooCmdArg
   *  @param arg5 RooCmdArg
   *  @param arg6 RooCmdArg
   *  @param arg7 RooCmdArg
   *  @param arg8 RooCmdArg
   *  @return the appropriate PDF
   */
  RooProdPdf& Product(const std::string& name, const RooArgList& pdfs, const RooCmdArg& arg1, const RooCmdArg& arg2 = RooCmdArg(), const RooCmdArg& arg3 = RooCmdArg(), const RooCmdArg& arg4 = RooCmdArg(), const RooCmdArg& arg5 = RooCmdArg(), const RooCmdArg& arg6 = RooCmdArg(), const RooCmdArg& arg7 = RooCmdArg(), const RooCmdArg& arg8 = RooCmdArg());
  
  /**
   *  @brief Add and access an extended PDF
   *
   *  Request a RooExtendPdf by a specified name. If the PDF does not yet
   *  exist in this EasyPdf pool of PDFs, it is created and returned.
   *  Otherwise an exception ObjectExistsException is thrown.
   *
   *  @param name name of the PDF
   *  @param pdf PDF to make extended
   *  @param yield yield argument for PDF
   *  @return the appropriate PDF
   */
  RooExtendPdf& Extend(const std::string& name, const RooAbsPdf& pdf, const RooAbsReal& yield);
  
  /**
   *  @brief Add and access an added PDF for extended PDFs
   *
   *  Request a RooAddPdf by a specified name. If the PDF does not yet
   *  exist in this EasyPdf pool of PDFs, it is created and returned.
   *  Otherwise an exception ObjectExistsException is thrown.
   *
   *  @param name name of the PDF
   *  @param pdfs RooArgList of extended PDFs to add
   *  @return the appropriate PDF
   */
  RooAddPdf& Add(const std::string& name, const RooArgList& pdfs);
  
  /**
   *  @brief Add and access an added PDF for PDFs with supplied yields or coefficients
   *
   *  Request a RooAddPdf by a specified name. If the PDF does not yet
   *  exist in this EasyPdf pool of PDFs, it is created and returned.
   *  Otherwise an exception ObjectExistsException is thrown.
   *
   *  @param name name of the PDF
   *  @param pdfs RooArgList of PDFs to add
   *  @param coefs RooArgList of coefficients/yields to use
   *  @return the appropriate PDF
   */
  RooAddPdf& Add(const std::string& name, const RooArgList& pdfs, const RooArgList& coefs);
  
  /**
   *  @brief Combine PDF and efficiency to a RooEffProd
   *
   *  Request a RooEffProd by a specified name. If the PDF does not yet
   *  exist in this EasyPdf pool of PDFs, it is created and returned.
   *  Otherwise an exception ObjectExistsException is thrown.
   *
   *  @param name name of the PDF
   *  @param pdf PDF to combine into efficiency product
   *  @param efficiency efficiency function
   *  @return the appropriate PDF
   */
  RooEffProd& EffProd(const std::string& name, RooAbsPdf& pdf, RooAbsReal& efficiency);
  ///@}

  /** @name Resolution PDFs
   *  PDF definitions of resolution models
   */
  ///@{
  /**
   *  @brief Add and access a RooGaussModel
   *
   *  Request a RooGaussModel by a specified name. If the PDF does not yet
   *  exist in this EasyPdf pool of PDFs, it is created and returned.
   *  Otherwise an exception ObjectExistsException is thrown.
   *
   *  @param name name of the PDF
   *  @param x the x variable
   *  @param mean mean or bias of resolution
   *  @param sigma width of resolution
   *  @return the appropriate PDF
   */
  RooGaussModel& GaussModel(const std::string& name, RooRealVar& x, RooAbsReal& mean, RooAbsReal& sigma);
  
  /**
   *  @brief Add and access a double RooGaussModel
   *
   *  Request a double RooGaussModel by a specified name. If the PDF does not 
   *  yet exist in this EasyPdf pool of PDFs, it is created and returned.
   *  Otherwise an exception ObjectExistsException is thrown.
   *
   *  @param name name of the PDF
   *  @param x the x variable
   *  @param mean mean or bias of resolution
   *  @param sigma1 width of first Gaussian
   *  @param sigma2 width of second Gaussian
   *  @param fraction fraction of first Gaussian
   *  @return the appropriate PDF
   */
  RooAddModel& DoubleGaussModel(const std::string& name, RooRealVar& x, RooAbsReal& mean, RooAbsReal& sigma1, RooAbsReal& sigma2, RooAbsReal& fraction);
  
  /**
   *  @brief Add and access a triple RooGaussModel
   *
   *  Request a triple RooGaussModel by a specified name. If the PDF does not
   *  yet exist in this EasyPdf pool of PDFs, it is created and returned.
   *  Otherwise an exception ObjectExistsException is thrown.
   *
   *  @param name name of the PDF
   *  @param x the x variable
   *  @param mean mean or bias of resolution
   *  @param sigma1 width of first Gaussian
   *  @param sigma2 width of second Gaussian
   *  @param sigma3 width of third Gaussian
   *  @param fraction1 fraction of first Gaussian
   *  @param fraction2 fraction of second Gaussian
   *  @return the appropriate PDF
   */
  RooAddModel& TripleGaussModel(const std::string& name, RooRealVar& x, RooAbsReal& mean, RooAbsReal& sigma1, RooAbsReal& sigma2, RooAbsReal& sigma3, RooAbsReal& fraction1, RooAbsReal& fraction2);
  
  /**
   *  @brief Add and access a quadruple RooGaussModel
   *
   *  Request a quadruple RooGaussModel by a specified name. If the PDF does not
   *  yet exist in this EasyPdf pool of PDFs, it is created and returned.
   *  Otherwise an exception ObjectExistsException is thrown.
   *
   *  @param name name of the PDF
   *  @param x the x variable
   *  @param mean mean or bias of resolution
   *  @param sigma1 width of first Gaussian
   *  @param sigma2 width of second Gaussian
   *  @param sigma3 width of third Gaussian
   *  @param sigma4 width of fourth Gaussian
   *  @param fraction1 fraction of first Gaussian
   *  @param fraction2 fraction of second Gaussian
   *  @param fraction3 fraction of third Gaussian
   *  @return the appropriate PDF
   */
  RooAddModel& QuadGaussModel(const std::string& name, RooRealVar& x, RooAbsReal& mean, RooAbsReal& sigma1, RooAbsReal& sigma2, RooAbsReal& sigma3, RooAbsReal& sigma4, RooAbsReal& fraction1, RooAbsReal& fraction2, RooAbsReal& fraction3);
  
  /**
   *  @brief Add and access a double RooGaussModel with scaled parameters
   *
   *  Request a double RooGaussModel by a specified name. If the PDF does not
   *  yet exist in this EasyPdf pool of PDFs, it is created and returned.
   *  Otherwise an exception ObjectExistsException is thrown.
   *
   *  The resolution model is modelled as
   *
   *  P(x) = fraction * GaussModel(x,mean,sigma) + (1-fraction) * GaussModel(x,mean,sigma2)
   *
   *  with sigma2 = scale*sigma as automatically generated formula. If the name
   *  of sigma2 is not supplied, it will be generated automatically.
   *
   *  @param name name of the PDF
   *  @param x the x variable
   *  @param mean mean or bias of resolution
   *  @param sigma width of first Gaussian
   *  @param scale variable for scale between sigma and sigma2
   *  @param fraction fraction of first Gaussian
   *  @param sigma2_name name of sigma2 formula
   *  @return the appropriate PDF
   */
  RooAddModel& DoubleGaussModelScaled(const std::string& name, RooRealVar& x, RooAbsReal& mean, RooAbsReal& sigma, RooAbsReal& scale, RooAbsReal& fraction, std::string sigma2_name);
  
  /**
   *  @brief Add and access a triple RooGaussModel with scaled parameters
   *
   *  Request a triple RooGaussModel by a specified name. If the PDF does not
   *  yet exist in this EasyPdf pool of PDFs, it is created and returned.
   *  Otherwise an exception ObjectExistsException is thrown.
   *
   *  The resolution model is modelled as
   *
   *  P(x) = fraction1 * GaussModel(x,mean,sigma)
   *       + fraction2 * GaussModel(x,mean,sigma2)
   *       + fraction3 * GaussModel(x,mean,sigma3)
   *
   *  with sigma2 = scale2*sigma, sigma3 = scale3*sigma2,
   *  fraction2 = (1-fraction)*frac_rec2 and
   *  fraction3 = (1-fraction)*(1-frac_rec2) as automatically generated formulae
   *  for scaled widths and recursive fractions. Names of these will be
   *  generated automatically based on the model name.
   *
   *  @param name name of the PDF
   *  @param x the x variable
   *  @param mean mean or bias of resolution
   *  @param sigma width of first GaussModel
   *  @param scale2 variable for scale between sigma and sigma2
   *  @param scale3 variable for scale between sigma2 and sigma3
   *  @param fraction1 fraction of first GaussModel
   *  @param frac_rec2 recursive fraction of second GaussModel
   *  @return the appropriate PDF
   */
  RooAddModel& TripleGaussModelScaled(const std::string& name, RooRealVar& x, RooAbsReal& mean, RooAbsReal& sigma, RooAbsReal& scale2, RooAbsReal& scale3, RooAbsReal& fraction1, RooAbsReal& frac_rec2);
  
  /**
   *  @brief Add and access a quadruple RooGaussModel with scaled parameters
   *
   *  Request a quadruple RooGaussModel by a specified name. If the PDF does not
   *  yet exist in this EasyPdf pool of PDFs, it is created and returned.
   *  Otherwise an exception ObjectExistsException is thrown.
   *
   *  The resolution model is modelled as
   *
   *  P(x) = fraction1 * GaussModel(x,mean,sigma)
   *       + fraction2 * GaussModel(x,mean,sigma2)
   *       + fraction3 * GaussModel(x,mean,sigma3)
   *       + fraction4 * GaussModel(x,mean,sigma4)
   *
   *  with sigma2 = scale2*sigma, sigma3 = scale3*sigma2, sigma4 = scale4*sigma3,
   *  fraction2 = (1-fraction)*frac_rec2 and
   *  fraction3 = (1-fraction)*(1-frac_rec2)*frac_rec3 and
   *  fraction4 = (1-fraction)*(1-frac_rec2)*(1-frac_rec3)
   *  as automatically generated formulae for scaled widths and recursive 
   *  fractions. Names of these will be generated automatically based on the 
   *  model name.
   *
   *  @param name name of the PDF
   *  @param x the x variable
   *  @param mean mean or bias of resolution
   *  @param sigma width of first GaussModel
   *  @param scale2 variable for scale between sigma and sigma2
   *  @param scale3 variable for scale between sigma2 and sigma3
   *  @param scale4 variable for scale between sigma3 and sigma4
   *  @param fraction1 fraction of first GaussModel
   *  @param frac_rec2 recursive fraction of second GaussModel
   *  @param frac_rec3 recursive fraction of third GaussModel
   *  @return the appropriate PDF
   */
  RooAddModel& QuadGaussModelScaled(const std::string& name, RooRealVar& x, RooAbsReal& mean, RooAbsReal& sigma, RooAbsReal& scale2, RooAbsReal& scale3, RooAbsReal& scale4, RooAbsReal& fraction1, RooAbsReal& frac_rec2, RooAbsReal& frac_rec3);
  
  /**
   *  @brief Add and access a quintuple RooGaussModel with scaled parameters
   *
   *  Request a quadruple RooGaussModel by a specified name. If the PDF does not
   *  yet exist in this EasyPdf pool of PDFs, it is created and returned.
   *  Otherwise an exception ObjectExistsException is thrown.
   *
   *  The resolution model is modelled as
   *
   *  P(x) = fraction1 * GaussModel(x,mean,sigma)
   *       + fraction2 * GaussModel(x,mean,sigma2)
   *       + fraction3 * GaussModel(x,mean,sigma3)
   *       + fraction4 * GaussModel(x,mean,sigma4)
   *       + fraction5 * GaussModel(x,mean,sigma5)
   *
   *  with sigma2 = scale2*sigma, sigma3 = scale3*sigma2, sigma4 = scale4*sigma3,
   *  sigma5 = scale5*sigma4, fraction2 = (1-fraction)*frac_rec2 and
   *  fraction3 = (1-fraction)*(1-frac_rec2)*frac_rec3,
   *  fraction4 = (1-fraction)*(1-frac_rec2)*(1-frac_rec3)*frac_rec4 and
   *  fraction5 = (1-fraction)*(1-frac_rec2)*(1-frac_rec3)*(1-frac_rec4)
   *  as automatically generated formulae for scaled widths and recursive
   *  fractions. Names of these will be generated automatically based on the
   *  model name.
   *
   *  @param name name of the PDF
   *  @param x the x variable
   *  @param mean mean or bias of resolution
   *  @param sigma width of first GaussModel
   *  @param scale2 variable for scale between sigma and sigma2
   *  @param scale3 variable for scale between sigma2 and sigma3
   *  @param scale4 variable for scale between sigma3 and sigma4
   *  @param scale5 variable for scale between sigma4 and sigma5
   *  @param fraction1 fraction of first GaussModel
   *  @param frac_rec2 recursive fraction of second GaussModel
   *  @param frac_rec3 recursive fraction of third GaussModel
   *  @param frac_rec4 recursive fraction of fourth GaussModel
   *  @return the appropriate PDF
   */
  RooAddModel& QuinGaussModelScaled(const std::string& name, RooRealVar& x, RooAbsReal& mean, RooAbsReal& sigma, RooAbsReal& scale2, RooAbsReal& scale3, RooAbsReal& scale4, RooAbsReal& scale5, RooAbsReal& fraction1, RooAbsReal& frac_rec2, RooAbsReal& frac_rec3, RooAbsReal& frac_rec4);
  
  /**
   *  @brief Add and access a per-event RooGaussModel
   *
   *  Request a RooGaussModel by a specified name. If the PDF does not yet
   *  exist in this EasyPdf pool of PDFs, it is created and returned.
   *  Otherwise an exception ObjectExistsException is thrown.
   *
   *  @param name name of the PDF
   *  @param x the x variable
   *  @param mean mean or bias of resolution
   *  @param error the per-event resolution estimate
   *  @param scale_error scale factor for error
   *  @param scale_mean scale factor for mean
   *  @return the appropriate PDF
   */
  RooGaussModel& GaussModelPerEvent(const std::string& name, RooRealVar& x, RooAbsReal& mean, RooAbsReal& scale_error, RooAbsReal& scale_mean, RooAbsReal& error);
  
  /**
   *  @brief Add and access a double per-event RooGaussModel
   *
   *  Request a RooGaussModel by a specified name. If the PDF does not yet
   *  exist in this EasyPdf pool of PDFs, it is created and returned.
   *  Otherwise an exception ObjectExistsException is thrown.
   *
   *  @param name name of the PDF
   *  @param x the x variable
   *  @param mean mean or bias of resolution
   *  @param error the per-event resolution estimate
   *  @param scale_error1 scale first factor for error
   *  @param scale_error2 scale second factor for error
   *  @param scale_mean scale factor for mean
   *  @param fraction fraction of first Gaussian
   *  @return the appropriate PDF
   */
  RooAddModel& DoubleGaussModelPerEvent(const std::string& name, RooRealVar& x, RooAbsReal& mean, RooAbsReal& scale_error1, RooAbsReal& scale_error2, RooAbsReal& scale_mean, RooAbsReal& error, RooAbsReal& fraction);

  /**
   *  @brief Add and access a triple per-event RooGaussModel
   *
   *  Request a RooGaussModel by a specified name. If the PDF does not yet
   *  exist in this EasyPdf pool of PDFs, it is created and returned.
   *  Otherwise an exception ObjectExistsException is thrown.
   *
   *  See doofit::builder::EasyPdf::TripleGaussModelScaled for definition of (recursive) fractions.
   *
   *  @param name name of the PDF
   *  @param x the x variable
   *  @param mean mean or bias of resolution
   *  @param error the per-event resolution estimate
   *  @param scale_error1 first scale factor for error
   *  @param scale_error2 second scale factor for error
   *  @param scale_error3 third scale factor for error
   *  @param scale_mean scale factor for mean
   *  @param fraction1 fraction of first Gaussian
   *  @param frac_rec2 recursive fraction of second GaussModel
   *  @return the appropriate PDF
   */
  RooAddModel& TripleGaussModelPerEvent(const std::string& name, RooRealVar& x, RooAbsReal& mean, RooAbsReal& scale_error1, RooAbsReal& scale_error2, RooAbsReal& scale_error3, RooAbsReal& scale_mean, RooAbsReal& error, RooAbsReal& fraction1, RooAbsReal& frac_rec2);
  
  /**
   *  @brief Add and access an added resolution PDF with supplied coefficients
   *
   *  Request a RooAddModel by a specified name. If the PDF does not yet
   *  exist in this EasyPdf pool of PDFs, it is created and returned.
   *  Otherwise an exception ObjectExistsException is thrown.
   *
   *  @param name name of the PDF
   *  @param pdfs RooArgList of PDFs to add
   *  @param coefs RooArgList of coefficients to use
   *  @return the appropriate PDF
   */
  RooAddModel& AddModel(const std::string& name, const RooArgList& pdfs, const RooArgList& coefs);
  ///@}
  
  /** @name Gaussian efficiency resolution PDFs
   *  PDF definitions of models based on RooGaussEfficiencyModel
   */
  ///@{
  /**
   *  @brief Add and access a RooGaussEfficiencyModel
   *
   *  Request a RooGaussEfficiencyModel by a specified name. If the PDF does not
   *  yet exist in this EasyPdf pool of PDFs, it is created and returned.
   *  Otherwise an exception ObjectExistsException is thrown.
   *
   *  @param name name of the PDF
   *  @param x the x variable
   *  @param eff the efficiency to use
   *  @param mean mean or bias of resolution
   *  @param sigma width of resolution
   *  @return the appropriate PDF
   */
  RooGaussEfficiencyModel& GaussEfficiencyModel(const std::string& name, RooRealVar& x, RooAbsGaussModelEfficiency &eff, RooAbsReal& mean, RooAbsReal& sigma);
  
  /**
   *  @brief Add and access a double RooGaussEfficiencyModel
   *
   *  Request a double RooGaussEfficiencyModel by a specified name. If the PDF 
   *  does not yet exist in this EasyPdf pool of PDFs, it is created and 
   *  returned. Otherwise an exception ObjectExistsException is thrown.
   *
   *  @param name name of the PDF
   *  @param x the x variable
   *  @param eff the efficiency to use
   *  @param mean mean or bias of resolution
   *  @param sigma1 width of first Gaussian
   *  @param sigma2 width of second Gaussian
   *  @param fraction fraction of first Gaussian
   *  @return the appropriate PDF
   */
  RooEffResAddModel& DoubleGaussEfficiencyModel(const std::string& name, RooRealVar& x, RooAbsGaussModelEfficiency &eff, RooAbsReal& mean, RooAbsReal& sigma1, RooAbsReal& sigma2, RooAbsReal& fraction);
  
  /**
   *  @brief Add and access a per-event RooGaussEfficiencyModel
   *
   *  Request a RooGaussEfficiencyModel by a specified name. If the PDF does not
   *  yet exist in this EasyPdf pool of PDFs, it is created and returned.
   *  Otherwise an exception ObjectExistsException is thrown.
   *
   *  @param name name of the PDF
   *  @param x the x variable
   *  @param eff the efficiency to use
   *  @param mean mean or bias of resolution
   *  @param error the per-event resolution estimate
   *  @param scale_error scale factor for error
   *  @return the appropriate PDF
   */
  RooGaussEfficiencyModel& GaussEfficiencyModelPerEvent(const std::string& name, RooRealVar& x, RooAbsGaussModelEfficiency &eff, RooAbsReal& mean, RooAbsReal& error, RooAbsReal &scale_error);
  
  /**
   *  @brief Add and access a double per-event RooGaussEfficiencyModel
   *
   *  Request a double RooGaussEfficiencyModel by a specified name. If the PDF 
   *  does not yet exist in this EasyPdf pool of PDFs, it is created and 
   *  returned. Otherwise an exception ObjectExistsException is thrown.
   *
   *  @param name name of the PDF
   *  @param x the x variable
   *  @param eff the efficiency to use
   *  @param mean mean or bias of resolution
   *  @param error the per-event resolution estimate
   *  @param scale_error1 scale first factor for error
   *  @param scale_error2 scale second factor for error
   *  @param fraction fraction of first Gaussian
   *  @return the appropriate PDF
   */
  RooEffResAddModel& DoubleGaussEfficiencyModelPerEvent(const std::string& name, RooRealVar& x, RooAbsGaussModelEfficiency &eff, RooAbsReal& mean, RooAbsReal& error, RooAbsReal& scale_error1, RooAbsReal& scale_error2, RooAbsReal& fraction);

  /**
   *  @brief Add and access an added efficiency resolution PDF with supplied coefficients
   *
   *  Request a RooEffResAddModel by a specified name. If the PDF does not yet
   *  exist in this EasyPdf pool of PDFs, it is created and returned.
   *  Otherwise an exception ObjectExistsException is thrown.
   *
   *  @param name name of the PDF
   *  @param pdfs RooArgList of PDFs to add
   *  @param coefs RooArgList of coefficients to use
   *  @return the appropriate PDF
   */
  RooEffResAddModel& EffResAddModel(const std::string& name, const RooArgList& pdfs, const RooArgList& coefs);
  ///@}
  

  /** @name Acceptance functions 
   *  RooFormulaVar definitions of Acceptance functions
   */
  ///@{
  /**
   *  @brief Add and access an Atan acceptance with 2 slope parameters
   */
  RooFormulaVar& AtanAcceptance(const std::string& name, RooRealVar& x, RooAbsReal& slope_a, RooAbsReal& slope_b);

  /**
   *  @brief Add and access an Atan acceptance with 2 slope parameters and beta effect
   */
  RooFormulaVar& AtanAcceptanceIncludingBeta(const std::string& name, RooRealVar& x, RooAbsReal& slope_a, RooAbsReal& slope_b, RooAbsReal& beta);
  ///@}


  /** @name Higher level PDFs
   *  PDF definitions of non-trivial PDFs
   */
  ///@{
  /**
   *  @brief Add and access a double Gaussian PDF with scaled second width
   *
   *  Request a double Gaussian as RooAddPdf by a specified name. If the PDF 
   *  does not yet exist in this EasyPdf pool of PDFs, it is created and 
   *  returned. Otherwise an exception ObjectExistsException is thrown.
   *
   *  The PDF is modelled as
   *
   *  P(x) = fraction * Gaussian(x,mean,sigma) + (1-fraction) * Gaussian(x,mean,sigma2)
   *
   *  with sigma2 = scale*sigma as automatically generated formula. If the name 
   *  of sigma2 is not supplied, it will be generated automatically.
   *
   *  @param name name of the PDF
   *  @param x x variable
   *  @param mean mean variable
   *  @param sigma sigma variable
   *  @param scale variable for scale between sigma and sigma2
   *  @param fraction fraction between two Gaussians
   *  @param sigma2_name name of sigma2 formula
   *  @return the appropriate PDF
   */
  RooAddPdf& DoubleGaussianScaled(const std::string& name, RooAbsReal& x, RooAbsReal& mean, RooAbsReal& sigma, RooAbsReal& scale, RooAbsReal& fraction, std::string sigma2_name="");
  
  /**
   *  @brief Add and access a triple Gaussian PDF with scaled widths and recursive fractions
   *
   *  Request a triple Gaussian as RooAddPdf by a specified name. If the PDF
   *  does not yet exist in this EasyPdf pool of PDFs, it is created and
   *  returned. Otherwise an exception ObjectExistsException is thrown.
   *
   *  The PDF is modelled as
   *
   *  P(x) = fraction1 * Gaussian(x,mean,sigma) + fraction2 * Gaussian(x,mean,sigma2)
   *         + fraction3 * Gaussian(x,mean,sigma3)
   *
   *  with sigma2 = scale2*sigma, sigma3 = scale3*sigma2, 
   *  fraction2 = (1-fraction)*frac_rec2 and 
   *  fraction3 = (1-fraction)*(1-frac_rec2) as automatically generated formulae
   *  for scaled widths and recursive fractions. Names of these will be 
   *  generated automatically based on the PDF name.
   *
   *  @param name name of the PDF
   *  @param x x variable
   *  @param mean mean variable
   *  @param sigma sigma variable
   *  @param scale variable for scale between sigma and sigma2
   *  @param fraction fraction between two Gaussians
   *  @param sigma2_name name of sigma2 formula
   *  @return the appropriate PDF
   */
  RooAddPdf& TripleGaussianScaled(const std::string& name, RooAbsReal& x, RooAbsReal& mean, RooAbsReal& sigma, RooAbsReal& scale2, RooAbsReal& scale3, RooAbsReal& fraction1, RooAbsReal& frac_rec2);
  
  /**
   *  @brief Add and access a double Decay PDF
   *
   *  Request a double Decay as RooAddPdf by a specified name. If the PDF
   *  does not yet exist in this EasyPdf pool of PDFs, it is created and
   *  returned. Otherwise an exception ObjectExistsException is thrown.
   *
   *  The PDF is modelled as
   *
   *  P(x) = fraction * Decay(t,tau1) + (1-fraction) * Decay(t,tau2)
   *
   *  @param name name of the PDF
   *  @param t t variable
   *  @param tau1 lifetime 1
   *  @param tau2 lifetime 2
   *  @param fraction fraction between two Decays
   *  @param model the resolution model to use
   *  @return the appropriate PDF
   */
  RooAddPdf& DoubleDecay(const std::string& name, RooRealVar& t, RooAbsReal& tau1, RooAbsReal& tau2, RooAbsReal& fraction, const RooResolutionModel& model);
  
  /**
   *  @brief Add and access a triple Decay PDF
   *
   *  Request a triple Decay as RooAddPdf by a specified name. If the PDF
   *  does not yet exist in this EasyPdf pool of PDFs, it is created and
   *  returned. Otherwise an exception ObjectExistsException is thrown.
   *
   *  The PDF is modelled as
   *
   *  P(x) = fraction1 * Decay(t,tau1) + (1-fraction1) * frac_rec2 * Decay(t,tau2) + (1 - fraction1 - ((1-fraction1) * frac_rec2)) * Decay(t,tau3)
   *
   *  @param name name of the PDF
   *  @param t t variable
   *  @param tau1 lifetime 1
   *  @param tau2 lifetime 2
   *  @param tau3 lifetime 3
   *  @param fraction1 fraction of first Decay
   *  @param frac_rec2 recursive fraction of second Decay
   *  @param model the resolution model to use
   *  @return the appropriate PDF
   */
  RooAddPdf& TripleDecay(const std::string& name, RooRealVar& t, RooAbsReal& tau1, RooAbsReal& tau2, RooAbsReal& tau3, RooAbsReal& fraction1, RooAbsReal& frac_rec2, const RooResolutionModel& model);

  /**
   *  @brief Add and access a RooBDecay PDF
   *
   *  Request a RooBDecay by a specified name. If the PDF does not yet exist in 
   *  this EasyPdf pool of PDFs, it is created and returned. Otherwise an 
   *  exception ObjectExistsException is thrown.
   *
   *  @param name name of the PDF
   *  @param t t variable
   *  @param tau lifetime
   *  @param dgamma delta gamma
   *  @param coef_cosh coefficient of cosh term
   *  @param coef_sinh coefficient of sinh term
   *  @param coef_cos coefficient of cos term
   *  @param coef_sin coefficient of sin term
   *  @param dm delta m
   *  @param model the resolution model to use
   *  @return the appropriate PDF
   */
  RooBDecay& BDecay(const std::string& name, RooRealVar& t, RooAbsReal& tau, RooAbsReal& dgamma, RooAbsReal& coef_cosh, RooAbsReal& coef_sinh, RooAbsReal& coef_cos, RooAbsReal& coef_sin, RooAbsReal& dm, const RooResolutionModel& model);
  
  /**
   *  @brief Add and access a RooKeysPdf from a file
   *
   *  Request a RooKeysPdf by a specified name. If the PDF does not yet exist in
   *  this EasyPdf pool of PDFs, it is created and returned. Otherwise an
   *  exception ObjectExistsException is thrown.   
   *
   *  @param name name of the PDF (after cloning, see below)
   *  @param file_name name of TFile containing the keys PDF
   *  @param ws_name name of workspace in TFile containing the keys PDF
   *  @param pdf_name_on_ws name of the keys PDF in the workspace
   *  @return the appropriate PDF
   */
  RooKeysPdf& KeysPdf(const std::string& name, const std::string& file_name, const std::string& ws_name, const std::string& pdf_name_on_ws);
  
  /**
   *  @brief Add and access a RooHistPdf from a RooDataHist in a file
   *
   *  Request a RooHistPdf by a specified name. If the PDF does not yet exist in
   *  this EasyPdf pool of PDFs, it is created and returned. Otherwise an
   *  exception ObjectExistsException is thrown.
   *
   *  @param name name of the PDF
   *  @param vars set of variables to use
   *  @param file_name name of TFile containing the RooDataHist to use for creation
   *  @param hist_name name of RooDataHist in TFile containing the histogrammed data
   *  @return the appropriate PDF
   */
  RooHistPdf& HistPdf(const std::string& name, const RooArgSet& vars, const std::string& file_name, const std::string& hist_name);
  ///@}
  
  /**
   *  @brief Add and access a given PDF
   *
   *  Templated function to add a given PDF pointer to the internal store (and
   *  workspace if necessary) and return this PDF afterwards.
   *  If PDF with same name already exists, an exception ObjectExistsException is 
   *  thrown.
   *
   *  @param pdf the PDF to add
   *  @return the added PDF
   */
  template <class PdfType>
  PdfType& AddPdfToStore(PdfType* pdf);

  /**
   *  @brief Add and access a given RooAbsReal
   *
   *  Templated function to add a given RooAbsReal pointer to the internal store
   *  (and workspace if necessary) and return this RooAbsReal afterwards.
   *  If a RooAbsReal with same name already exists, an exception 
   *  ObjectExistsException is thrown.
   *
   *  @param real the RooAbsReal to add
   *  @return the added real
   */
  template <class RealType>
  RealType& AddRealToStore(RealType* real);
  
  /**
   *  @brief Add and access a given RooAbsBinning
   *
   *  Templated function to add a given binning pointer to the internal store
   *  and return it afterwards. If a RooAbsBinning with same name already 
   *  exists, an exception ObjectExistsException is thrown.
   *
   *  @param binning the RooAbsBinning to add
   *  @return the added binning
   */
  template <class BinningType>
  BinningType& AddBinningToStore(BinningType* binning);
  
  /** @name PDF access
   *  Access to already defined PDFs
   */
  ///@{
  /**
   *  @brief Access an existing PDF
   *
   *  Request a RooAbsPdf by a specified name. If the PDF does exist in this 
   *  EasyPdf pool of PDFs, it is returned.
   *  Otherwise an exception ObjectNotExistingException is thrown.
   *
   *  @param name name of the PDF
   *  @return the appropriate PDF
   */
  RooAbsPdf& Pdf(const std::string& name);
  
  /**
   *  @brief Access an existing model PDF
   *
   *  Request a RooResolutionModel by a specified name. If the PDF does exist in this
   *  EasyPdf pool of PDFs, it is returned.
   *  Otherwise an exception ObjectNotExistingException is thrown.
   *
   *  @param name name of the PDF
   *  @return the appropriate PDF
   */
  RooResolutionModel& Model(const std::string& name);
  ///@}
  
  /** @name Binning creation and access
   *  Create different RooAbsBinnings and access them
   */
  ///@{
  /**
   *  @brief Add or access RooAbsBinning
   *
   *  Request a RooAbsBinning by a specified name. If the binning does not yet
   *  exist in this EasyPdf pool of binnings, a new RooBinning with min=-inf and
   *  max=+inf is created and returned. Otherwise the previously defined 
   *  RooAbsBinning will be returned from the pool.
   *
   *  @param name name of the binning
   *  @return the binning as RooAbsBinning (cast may be necessary)
   */
  RooAbsBinning& Binning(const std::string& name);
  
  /**
   *  @brief Add and access RooBinning based on vector of bin boundaries
   *
   *  Add a new RooBinning with given name and boundaries as specified in a
   *  vector. The binning minimum and maximum will be set to the first and last
   *  entry of the vector. After creation, the binning is returned.
   *
   *  If a RooAbsBinning with same name already exists, an exception
   *  ObjectExistsException is thrown.
   *
   *  @param name name of the binning
   *  @param boundaries vector with boundaries
   *  @return the binning as RooBinning
   */
  RooBinning& Binning(const std::string& name, std::vector<double> boundaries);
  ///@}
  
 protected:
  
 private:
  /**
   *  @brief Private copy constructor
   *
   *  Unless fixed, no copy of EasyPdf shall ever be made.
   *
   *  Copying EasyPdf containers is highly non-trivial and therefore not 
   *  supported by now.
   *
   *  @param other EasyPdf object to copy
   */
  EasyPdf(const EasyPdf& other) {}
  
  /**
   *  @brief Container for all generated RooRealVars
   */
  std::map<std::string,RooRealVar*> vars_;
  
  /**
   *  @brief Container for all generated RooCategories
   */
  std::map<std::string,RooCategory*> cats_;

  /**
   *  @brief Container for all generated RooSuperCategories
   */
  std::map<std::string,RooSuperCategory*> supercats_;
  
  /**
   *  @brief Container for all generated RooFormulaVars
   */
  std::map<std::string,RooFormulaVar*> formulas_;
  
  /**
   *  @brief Container for all generated RooAbsHiddenReal
   */
  std::map<std::string,RooAbsHiddenReal*> hidden_reals_;
  
  /**
   *  @brief Container for all manually added RooAbsReal
   */
  std::map<std::string,RooAbsReal*> external_reals_;
  
  /**
   *  @brief Container for all generated RooAbsPdfs
   */
  std::map<std::string,RooAbsPdf*> pdfs_;
  
  /**
   *  @brief Container for all cloned RooDataHists
   */
  std::vector<RooDataHist*> hists_;
  
  /**
   *  @brief Container for all generated RooAbsBinning
   */
  std::map<std::string,RooAbsBinning*> binnings_;
  
  /**
   *  @brief RooWorkspace for all objects
   */
  RooWorkspace* ws_;
  
  /**
   *  @brief Defined named sets
   */
  std::map<std::string,std::string> variable_sets_;
};

/** \struct ObjectExistsException
 *  \brief Exception for objects with same name already existing
 */
struct ObjectExistsException: public virtual boost::exception, public virtual std::exception { 
  virtual const char* what() const throw() { return "Object with same name already existing"; }
};

/** \struct ObjectNotExistingException
 *  \brief Exception for PDF with given name not existing
 */
struct ObjectNotExistingException: public virtual boost::exception, public virtual std::exception { 
  virtual const char* what() const throw() { return "Object with supplied name not existing"; }
};

template <class PdfType>
PdfType& EasyPdf::AddPdfToStore(PdfType* pdf) {
  std::string name = pdf->GetName();
  if (pdfs_.count(name) == 1) {
    throw ObjectExistsException();
  } else {
    if (ws_ == NULL) {
      pdfs_[name] = pdf;
    } else {
      ws_->import(*pdf, RooFit::Silence());
      delete pdf;
      pdfs_[name] = pdf = dynamic_cast<PdfType*>(ws_->pdf(name.c_str()));
    }
    return *pdf;
  }
}
  
template <class RealType>
RealType& EasyPdf::AddRealToStore(RealType* real) {
  std::string name = real->GetName();
  if (external_reals_.count(name) == 1) {
    throw ObjectExistsException();
  } else {
    if (ws_ == NULL) {
      external_reals_[name] = real;
    } else {
      ws_->import(*real, RooFit::Silence());
      delete real;
      external_reals_[name] = real = dynamic_cast<RealType*>(ws_->function(name.c_str()));
    }
    return *real;
  }
}

template <class BinningType>
BinningType& EasyPdf::AddBinningToStore(BinningType* binning) {
  std::string name = binning->GetName();
  if (binnings_.count(name) == 1) {
    throw ObjectExistsException();
  } else {
    if (ws_ == NULL) {
      binnings_[name] = binning;
    } else {
      doocore::io::swarn << "EasyPdf::AddBinningToStore(...): Cannot import binning to RooWorkspace, will store internally." << doocore::io::endmsg;
      binnings_[name] = binning;
    }
    return *binning;
  }
}

  
} // namespace builder
} // namespace doofit

#endif // DOOFIT_BUILDER_EASYPDF_EASYPDF_h
