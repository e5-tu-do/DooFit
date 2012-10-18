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

// forward declarations
class RooRealVar;
class RooAbsPdf;
class RooGaussian;
class RooExponential;
class RooArgList;
class RooProdPdf;
class RooExtendPdf;
class RooAbsReal;
class RooAddPdf;

/** @namespace doofit::builder
 *  @brief DooFit PDF building namespace
 */

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
 *   epdf.Product("pdf_prod", epdf.Pdf("pdf_gauss"), epdf.Pdf("pdf_expo"));
 *   
 *   // most of the above could also be written like this in practically one line
 *   epdf.Product("pdf_prod2",
 *                epdf.Gaussian("pdf_gauss2", epdf.Var("mass"), epdf.Var("mean"), epdf.Var("sigma")),
 *                epdf.Exponential("pdf_expo2", epdf.Var("time"), epdf.Var("c")));
 * }
 * @endcode
 *
 *  The main goal of EasyPdf is to keep PDF/variable construction as sinmple as 
 *  possible. Therefore, variable and PDF generation and access are mostly 
 *  indistinguishable. Objects are created as needed and stored via their name.
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
   *  @brief Add and access a Gaussian PDF
   *
   *  Request a RooGaussian by a specified name. If the PDF does not yet
   *  exist in this EasyPdf pool of PDFs, it is created and returned.
   *  Otherwise an exception PdfExistsException is thrown.
   *
   *  @param name name of the PDF
   *  @param x x variable
   *  @param mean mean variable
   *  @param sigma sigma variable
   *  @return the appropriate PDF
   */
  RooGaussian& Gaussian(const std::string& name, RooRealVar& x, RooRealVar& mean, RooRealVar& sigma);
  
  /**
   *  @brief Add and access an Exponential PDF
   *
   *  Request a RooExponential by a specified name. If the PDF does not yet
   *  exist in this EasyPdf pool of PDFs, it is created and returned.
   *  Otherwise an exception PdfExistsException is thrown.
   *
   *  @param name name of the PDF
   *  @param x x variable
   *  @param e exponent variable
   *  @return the appropriate PDF
   */
  RooExponential& Exponential(const std::string& name, RooRealVar& x, RooRealVar& e);
  
  /**
   *  @brief Add and access an product PDF
   *
   *  Request a RooProdPdf by a specified name. If the PDF does not yet
   *  exist in this EasyPdf pool of PDFs, it is created and returned.
   *  Otherwise an exception PdfExistsException is thrown.
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
   *  Otherwise an exception PdfExistsException is thrown.
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
   *  Otherwise an exception PdfExistsException is thrown.
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
   *  Otherwise an exception PdfExistsException is thrown.
   *
   *  @param name name of the PDF
   *  @param pdfs RooArgList of extended PDFs to add
   *  @param coefs RooArgList of coefficients/yields to use
   *  @return the appropriate PDF
   */
  RooAddPdf& Add(const std::string& name, const RooArgList& pdfs, const RooArgList& coefs);
  
  /**
   *  @brief Add and access a given PDF
   *
   *  Templated function to add a given PDF pointer to the internal store (and
   *  workspace if necessary) and return this PDF afterwards.
   *  If PDF with same name already exists, an exception PdfExistsException is 
   *  thrown.
   *
   *  @param pdf the PDF to add
   *  @return the added PDF
   */
  template <class PdfType>
  PdfType& AddPdfToStore(PdfType* pdf);

  /**
   *  @brief Access an existing PDF
   *
   *  Request a RooAbsPdf by a specified name. If the PDF does exist in this 
   *  EasyPdf pool of PDFs, it is returned.
   *  Otherwise an exception PdfNotExistingException is thrown.
   *
   *  @param name name of the PDF
   *  @return the appropriate PDF
   */
  RooAbsPdf& Pdf(const std::string& name);
  
 protected:
  
 private:
  /**
   *  @brief Container for all generated RooRealVars
   */
  std::map<std::string,RooRealVar*> vars_;
  
  /**
   *  @brief Container for all generated RooAbsPdfs
   */
  std::map<std::string,RooAbsPdf*> pdfs_;
  
  /**
   *  @brief RooWorkspace for all objects
   */
  RooWorkspace* ws_;
};

/** \struct PdfExistsException
 *  \brief Exception for PDF with same name already existing
 */
struct PdfExistsException: public virtual boost::exception, public virtual std::exception { 
  virtual const char* what() const throw() { return "PDF with same name already existing"; }
};

/** \struct PdfNotExistingException
 *  \brief Exception for PDF with given name not existing
 */
struct PdfNotExistingException: public virtual boost::exception, public virtual std::exception { 
  virtual const char* what() const throw() { return "PDF with supplied name not existing"; }
};

template <class PdfType>
PdfType& EasyPdf::AddPdfToStore(PdfType* pdf) {
  std::string name = pdf->GetName();
  if (pdfs_.count(name) == 1) {
    throw PdfExistsException();
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
