#include "doofit/Builder/numerobis/blueprint/elements/realvalelement.h"

// RooFit
#include "RooAbsReal.h"
#include "RooWorkspace.h"

namespace doofit {
namespace builder {
namespace numerobis {
namespace blueprint {
namespace elements {


RealValElement::RealValElement() 
    : roo_obj_(NULL)
{
  
}

RealValElement::~RealValElement() {
  
}

void RealValElement::AddToWorkspace(RooWorkspace* ws) {
  if (!ready()) {
    throw; 
  }
  
  RooAbsReal* roo_obj_temp = CreateTempRooObj();

  ws->import(*roo_obj_temp);
  delete roo_obj_temp;
  
  roo_obj_ = ws->function(id_abs().c_str());
}

} // namespace elements 
} // namespace blueprint 
} // namespace numerobis 
} // namespace builder 
} // namespace doofit