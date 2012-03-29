#ifndef DOOFIT_BUILDER_NUMEROBIS_BLUEPRINT_BLUEPRINT_H
#define DOOFIT_BUILDER_NUMEROBIS_BLUEPRINT_BLUEPRINT_H

// from STL
#include <string>

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





} // namespace blueprint 
} // namespace numerobis 
} // namespace builder 
} // namespace doofit 


#endif // DOOFIT_BUILDER_NUMEROBIS_BLUEPRINT_BLUEPRINT_H