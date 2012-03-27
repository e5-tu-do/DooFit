#include "doofit/Builder/numerobis/blueprint/elements/realvalelement.h"

// from STL
#include <string>

// RooFit
#include "RooAbsReal.h"

// from project
#include "doofit/Builder/numerobis/blueprint/elements/element.h"

namespace doofit {
namespace builder {
namespace numerobis {
namespace blueprint {
namespace elements {

  
RealValElement::RealValElement(const std::string& id_rel, const std::string& id_abs) 
    : Element(id_rel, id_abs)
{
  
}

RealValElement::~RealValElement() {
  
}


} // namespace elements 
} // namespace blueprint 
} // namespace numerobis 
} // namespace builder 
} // namespace doofit
