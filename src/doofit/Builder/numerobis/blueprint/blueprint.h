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
  
  
  
 private:
  elements::Registrar reg_elems_;
  elements::Factory   fac_elems_;
  
  pdfs::Registrar     reg_pdfs_;
  pdfs::Factory       fac_pdfs_;
  
  //boost::ptr_map<std::string, pdfs::Component> comps_;
  //boost::ptr_map<std::string, pdfs::Dimension> dimensions_;
  
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