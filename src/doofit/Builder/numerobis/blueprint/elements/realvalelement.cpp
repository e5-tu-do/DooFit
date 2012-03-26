#include "doofit/Builder/numerobis/blueprint/elements/realvalelement.h"

// from STL
#include <string>

// RooFit
#include "RooAbsReal.h"
#include "RooWorkspace.h"

// from project
#include "doofit/Builder/numerobis/blueprint/elements/element.h"

namespace doofit {
namespace builder {
namespace numerobis {
namespace blueprint {
namespace elements {


RealValElement::RealValElement() 
    : roo_obj_(NULL)
{
  
}
  
RealValElement::RealValElement(const std::string& id_rel, const std::string& id_abs) 
    : Element(id_rel, id_abs)
    , roo_obj_(NULL)
{
  
}

RealValElement::~RealValElement() {
  
}

RooAbsArg* RealValElement::AddToWorkspace(RooWorkspace* ws, std::vector<RooAbsArg*> dependants) {
  if (!ready()) {
    throw; 
  }
  
  if (!onworkspace()){
    RooAbsReal* roo_obj_temp = CreateTempRooObj(dependants);

    ws->import(*roo_obj_temp);
    delete roo_obj_temp;
  
    roo_obj_ = ws->function(id_abs().c_str());
    set_onworkspace(true);
  }
  return dynamic_cast<RooAbsArg*>(roo_obj_);
}

} // namespace elements 
} // namespace blueprint 
} // namespace numerobis 
} // namespace builder 
} // namespace doofit
