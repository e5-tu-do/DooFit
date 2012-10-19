#include "doofit/builder/numerobis/blueprint/elements/paramelement.h"

namespace doofit {
namespace builder {
namespace numerobis {
namespace blueprint {
namespace elements {


ParamElement::ParamElement(const std::string& id_rel, const std::string& id_abs)
    : RealValElement(id_rel, id_abs)
{
  
}

ParamElement::~ParamElement() {
  
}



} // namespace elements 
} // namespace blueprint 
} // namespace numerobis 
} // namespace builder 
} // namespace doofit
