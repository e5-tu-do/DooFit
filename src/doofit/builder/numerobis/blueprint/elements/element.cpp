#include "doofit/builder/numerobis/blueprint/elements/element.h"

// from STL
#include <string>

// from RooFit
#include "RooWorkspace.h"

// from project
#include "doofit/builder/numerobis/blueprint/elements/registrar.h"

namespace doofit {
namespace builder {
namespace numerobis {
namespace blueprint {
namespace elements {


// Element::Element()
//     : id_rel_("NoRelID")
//     , id_abs_("NoAbsID")
//     , initialized_(false)
//     , ready_(false)
//     , onworkspace_(false)
// {
//   
// }
  
Element::Element(const std::string& id_rel, const std::string& id_abs)
 : id_rel_(id_rel)
 , id_abs_(id_abs)
 , initialized_(false)
 , ready_(false)
 , onworkspace_(false)
{
  
}

Element::~Element() {
  
}

RooAbsArg* Element::AddToWorkspace(RooWorkspace* ws, const std::vector<RooAbsArg*>& dependants) {
  if (!ready()) {
    throw; 
  }
  
  if (!onworkspace()){
    RooAbsArg* roo_obj_temp = CreateTempRooObj(dependants);

    ws->import(*roo_obj_temp);
    delete roo_obj_temp;
    
    set_onworkspace(true);
  }
  return GetRooObjFromWorkspace(ws);
}

} // namespace elements 
} // namespace blueprint 
} // namespace numerobis 
} // namespace builder 
} // namespace doofit
