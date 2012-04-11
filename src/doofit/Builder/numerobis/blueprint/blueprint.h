#ifndef DOOFIT_BUILDER_NUMEROBIS_BLUEPRINT_BLUEPRINT_H
#define DOOFIT_BUILDER_NUMEROBIS_BLUEPRINT_BLUEPRINT_H

// from STL
#include <string>

// from boost
#include <boost/ptr_container/ptr_map.hpp>

// from project
#include "doofit/Builder/numerobis/blueprint/elements/registrar.h"
#include "doofit/Builder/numerobis/blueprint/elements/factory.h"
#include "doofit/Builder/numerobis/blueprint/pdfs/registrar.h"
#include "doofit/Builder/numerobis/blueprint/pdfs/factory.h"
#include "doofit/Builder/numerobis/blueprint/component.h"
#include "doofit/Builder/numerobis/blueprint/dimension.h"

// forward declarations
class RooWorkspace;

namespace doofit {
namespace builder {
namespace numerobis {
namespace blueprint {

class Blueprint {
 public:
  Blueprint();
  virtual ~Blueprint();
  
  /** 
   * @brief Getter for reg_elements_
   **/
  elements::Registrar& reg_elements() { return reg_elements_; }
  /** 
   * @brief Getter for fac_elements_
   **/
  elements::Factory& fac_elements() { return fac_elements_; }
  /** 
   * @brief Getter for reg_pdfs_
   **/
  pdfs::Registrar& reg_pdfs() { return reg_pdfs_; }
  /** 
   * @brief Getter for fac_pdfs_
   **/
  pdfs::Factory& fac_pdfs() { return fac_pdfs_; }

 private:
  elements::Registrar reg_elements_;
  elements::Factory   fac_elements_;
  
  pdfs::Registrar     reg_pdfs_;
  pdfs::Factory       fac_pdfs_;
  
  boost::ptr_map<std::string, Component> comps_;
  boost::ptr_map<std::string, Dimension> dimensions_;
  
  // std::string pdf_pre_;
  // std::string var_pre_;
  // std::string par_pre_;
  
  // map names
  
  // map dimensionsreal
  // map dimensionscat
  // map categories
  // map supercategories
  // map acceptances
  // map pdfs
  // map resols
  // map pdfswresol
  
  // map samepar
  
  /* data**/
  
  
};

/**
 *  @brief Convert a vector of RooAbsArgs to a RooArgList
 *
 *  @param vec the vector to convert
 *  @return the RooArgList based on the vector
**/
RooArgList VectorToArgList(std::vector<RooAbsArg*> vec);




} // namespace blueprint 
} // namespace numerobis 
} // namespace builder 
} // namespace doofit 


#endif // DOOFIT_BUILDER_NUMEROBIS_BLUEPRINT_BLUEPRINT_H