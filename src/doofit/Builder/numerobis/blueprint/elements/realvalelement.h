#ifndef DOOFIT_BUILDER_NUMEROBIS_BLUEPRINT_ElEMENTS_REALVALELEMENT_H
#define DOOFIT_BUILDER_NUMEROBIS_BLUEPRINT_ElEMENTS_REALVALELEMENT_H

// from STL
#include <string>

// from project
#include "doofit/Builder/numerobis/blueprint/elements/element.h"


// forward declarations - RooFit
class RooAbsReal;
class RooAbsArg;

namespace doofit {
namespace builder {
namespace numerobis {
namespace blueprint {
namespace elements {

class RealValElement : public Element {
 public:
  RealValElement();
  RealValElement(const std::string& id_rel, const std::string& id_abs);
  virtual ~RealValElement();
    
 protected:
  /** @brief Create instance of this element as RooFit object
   *  
   *  Initialize the appropriate RooFit object and return it as RooAbsReal 
   *  pointer.
   *
   *  @param dependants the dependants to be used for initialization
   *  @return the temporary initialized object to be put onto a workspace
   */
  virtual RooAbsArg* CreateTempRooObj(std::vector<RooAbsArg*> dependants = std::vector<RooAbsArg*>()) = 0;
  
  RooAbsReal* GetRooObj() { return roo_obj_; };
  
  RooAbsReal* roo_obj_;
};


} // namespace elements
} // namespace blueprint 
} // namespace numerobis 
} // namespace builder 
} // namespace doofit 


#endif // DOOFIT_BUILDER_NUMEROBIS_BLUEPRINT_ElEMENTS_REALVALELEMENT_H
