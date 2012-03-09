#ifndef DOOFIT_BUILDER_NUMEROBIS_BLUEPRINT_ElEMENTS_REALVALELEMENT_H
#define DOOFIT_BUILDER_NUMEROBIS_BLUEPRINT_ElEMENTS_REALVALELEMENT_H

// from STL
#include <string>

// from project
#include "doofit/Builder/numerobis/blueprint/elements/element.h"


// forward declarations - RooFit
class RooAbsReal;

namespace doofit {
namespace builder {
namespace numerobis {
namespace blueprint {
namespace elements {

class RealValElement : public Element {
 public:
  RealValElement();
  virtual ~RealValElement();
  
  /** @brief Adds RooFit representation to RooWorkspace
   *
   *  Function à la Template Method. Calls other abstract functions:
   *  @li CreateTempRooObj()
   */
  void AddToWorkspace(RooWorkspace* ws);
  
 protected:
  /** @brief 
   *  
   */
  virtual RooAbsReal* CreateTempRooObj() = 0;
  
  RooAbsReal* GetRooObj() { return roo_obj_; };
  
  RooAbsReal* roo_obj_;
};


} // namespace elements
} // namespace blueprint 
} // namespace numerobis 
} // namespace builder 
} // namespace doofit 


#endif // DOOFIT_BUILDER_NUMEROBIS_BLUEPRINT_ElEMENTS_REALVALELEMENT_H