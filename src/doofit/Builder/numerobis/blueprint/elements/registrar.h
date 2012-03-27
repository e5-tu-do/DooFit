#ifndef DOOFIT_BUILDER_NUMEROBIS_BLUEPRINT_ELEMENTS_REGISTRAR_H
#define DOOFIT_BUILDER_NUMEROBIS_BLUEPRINT_ELEMENTS_REGISTRAR_H

// from STL
#include <string>

// from boost
#include <boost/exception/all.hpp>
#include <boost/ptr_container/ptr_map.hpp>

// from RooFit
#include "RooArgList.h"

// from project


// forward declarations
class RooWorkspace;
class RooAbsArg;

namespace doofit {
namespace builder {
namespace numerobis {
namespace blueprint {
namespace elements {

// forward decalarations
class Element;  
  
class Registrar {
 public:
  Registrar();
  virtual ~Registrar();

  /**
   *  @brief Declare a new element in this registrar
   *
   *  The element will be added to the elements_ container to be available for
   *  linking with other elements.
   *
   *  @param element the element to add
  **/
  void Declare(Element* element);
  
  /**
   *  @brief Print all registered elements
  **/
  void Print() const;
  
  /**
   *  @brief Register the element (i.e. instanciate and put on workspace)
   *
   *  This function will (if ready) create the element and its dependants and 
   *  put all of them on the supplied workspace.
   *
   *  @param ws the RooWorkspace to put all elements onto
   *  @param element_name name of the element to register
  **/
  RooAbsArg* Register(RooWorkspace* ws, const std::string& element_name);
  
 private:
  /**
   *  @brief Check ready state for an element
   *
   *  This function will check for one element if its dependants list is in 
   *  ready state. If so, this element will be set to ready.
   *
   *  @param element_name name of the element to check
   *  @return the ready state of element after check
  **/
  bool CheckReady(const std::string& element_name);
   
  /**
   *  @brief Map of all registered elements so far
  **/
  boost::ptr_map<std::string, Element> elements_;
};

/**
 *  @brief Convert a vector of RooAbsArgs to a RooArgList
 *
 *  @param vec the vector to convert
 *  @return the RooArgList based on the vector
**/
RooArgList VectorToArgList(std::vector<RooAbsArg*> vec);
  
/** \struct UnexpectedException
 *  \brief Unexpected exception that should not even occur
**/
struct UnexpectedException: public virtual boost::exception, public virtual std::exception { 
  virtual const char* what() const throw() { return "Unexpected exception"; }
};
 
} // namespace elements 
} // namespace blueprint 
} // namespace numerobis 
} // namespace builder 
} // namespace doofit 

#endif // DOOFIT_BUILDER_NUMEROBIS_BLUEPRINT_ELEMENTS_REGISTRAR_H
