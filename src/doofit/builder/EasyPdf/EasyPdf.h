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

// forward declarations
class RooRealVar;
class RooAbsCategoryLValue;
class RooAbsPdf;
class RooGaussian;
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
  
  /** @name Variable access and creation
   *  Functions to access and/or create variables (real, formula, categories)
   */
  ///@{
  /**
   *  @brief Add or access real based variable (RooRealVar or RooFormulaVar)
   *
   *  Request a RooAbsReal by a specified name. If the variable/formula does not 
   *  yet exist in this EasyPdf pool of variables or formulas, it is created as 
   *  a RooRealVar and returned.
   *  Otherwise it will be returned from the pool. Variables take preference 
   *  over formulas in case of duplicate entries.
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
  
  /** @name Variable existency check
   *  Functions to test existence of variables
   */
  ///@{
  /**
   *  @brief Check if real based variable exists (RooRealVar or RooFormulaVar)
   *
   *  Check if a RooAbsReal exists by a specified name. If the variable/formula 
   *  does exist in this EasyPdf pool of variables or formulas, true is returned,
   *  otherwise false.
   *
   *  @param name name of the RooAbsReal
   *  @return whether the variable already exists
   */
  bool RealExists(const std::string& name);
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
   *  @param fraction fraction between Gaussians
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
   *  @param fraction1 fraction between first and second Gaussian
   *  @param fraction2 fraction between second and third Gaussian
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
   *  @param fraction1 fraction between first and second Gaussian
   *  @param fraction2 fraction between second and third Gaussian
   *  @param fraction3 fraction between third and fourth Gaussian
   *  @return the appropriate PDF
   */
  RooAddModel& QuadGaussModel(const std::string& name, RooRealVar& x, RooAbsReal& mean, RooAbsReal& sigma1, RooAbsReal& sigma2, RooAbsReal& sigma3, RooAbsReal& sigma4, RooAbsReal& fraction1, RooAbsReal& fraction2, RooAbsReal& fraction3);
  
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
   *  with sigma2 = scale*sigma as automatically generated formula. If the name
   *  of sigma2 is not supplied, it will be generated automatically.
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
  
 protected:
  
 private:
  /**
   *  @brief Container for all generated RooRealVars
   */
  std::map<std::string,RooRealVar*> vars_;
  
  /**
   *  @brief Container for all generated RooCategories
   */
  std::map<std::string,RooCategory*> cats_;
  
  /**
   *  @brief Container for all generated RooFormulaVars
   */
  std::map<std::string,RooFormulaVar*> formulas_;
  
  /**
   *  @brief Container for all generated RooAbsPdfs
   */
  std::map<std::string,RooAbsPdf*> pdfs_;
  
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
  virtual const char* what() const throw() { return "PDF with same name already existing"; }
};

/** \struct ObjectNotExistingException
 *  \brief Exception for PDF with given name not existing
 */
struct ObjectNotExistingException: public virtual boost::exception, public virtual std::exception { 
  virtual const char* what() const throw() { return "PDF with supplied name not existing"; }
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

} // namespace builder
} // namespace doofit

#endif // DOOFIT_BUILDER_EASYPDF_EASYPDF_h
