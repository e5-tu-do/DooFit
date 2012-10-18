#ifndef DOOFIT_BUILDER_NUMEROBIS_BLUEPRINT_PDFS_REGISTRAR_H
#define DOOFIT_BUILDER_NUMEROBIS_BLUEPRINT_PDFS_REGISTRAR_H

// from STL
#include <string>

// from boost
#include <boost/exception/all.hpp>
#include <boost/ptr_container/ptr_map.hpp>

// from RooFit

// from project

// forward declarations
class RooWorkspace;
class RooAbsArg;

namespace doofit {
namespace builder {
namespace numerobis {
namespace blueprint {
  
namespace elements {
class Element;
class Registrar;
} // namespace pdfs 

namespace pdfs {

// forward decalarations
class Pdf;  
  
class Registrar {
 public:
  /**
   *  @brief Constructor for PDF registrar with element registrar
   *
   *  @param element_registrar the element registrar to use
  **/
  Registrar(doofit::builder::numerobis::blueprint::elements::Registrar& element_registrar);
  virtual ~Registrar();

  /**
   *  @brief Declare a new PDF in this registrar
   *
   *  The element will be added to the pdfs_ container to be available for
   *  linking with other PDFs.
   *
   *  @param pdf the PDF to add
  **/
  void Declare(Pdf* pdf);
  
  /**
   *  @brief Print all registered elements
  **/
  void Print() const;
  
  /**
   *  @brief Register the PDF (i.e. instanciate and put on workspace)
   *
   *  This function will (if ready) create the PDF and its dependants and 
   *  put all of them on the supplied workspace.
   *
   *  @param ws the RooWorkspace to put all elements onto
   *  @param pdf_name name of the PDF to register
  **/
  RooAbsArg* Register(RooWorkspace* ws, const std::string& pdf_name);
  
  /**
   *  @brief Check ready state for a PDF
   *
   *  This function will check for one PDF if its dependants list is in 
   *  ready state. If so, this element will be set to ready.
   *
   *  @param pdf_name name of the PDF to check
   *  @return the ready state of PDF after check
  **/
  bool CheckReady(const std::string& pdf_name);
  
 private:
  /**
   *  @brief Map of all registered elements so far
  **/
  boost::ptr_map<std::string, Pdf> pdfs_;
  
  /**
   *  @brief Element registrar to be used for accessing elements
  **/
  doofit::builder::numerobis::blueprint::elements::Registrar& element_registrar_;
};
  
/** \struct UnexpectedException
 *  \brief Unexpected exception that should not even occur
**/
struct UnexpectedException: public virtual boost::exception, public virtual std::exception { 
  virtual const char* what() const throw() { return "Unexpected exception"; }
};
 
} // namespace pdfs 
} // namespace blueprint 
} // namespace numerobis 
} // namespace builder 
} // namespace doofit 

#endif // DOOFIT_BUILDER_NUMEROBIS_BLUEPRINT_PDFS_REGISTRAR_H
