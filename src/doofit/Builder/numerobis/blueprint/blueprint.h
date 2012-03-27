#ifndef DOOFIT_BUILDER_NUMEROBIS_BLUEPRINT_BLUEPRINT_H
#define DOOFIT_BUILDER_NUMEROBIS_BLUEPRINT_BLUEPRINT_H

// from STL
#include <string>

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
  
  void AddToWorkspace();
  
 private:
  std::string pdf_pre_;
  std::string var_pre_;
  std::string par_pre_;
  
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