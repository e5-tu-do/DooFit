#include "doofit/Builder/numerobis/blueprint/elements/element.h"

namespace doofit {
namespace builder {
namespace numerobis {
namespace blueprint {
namespace elements {


Element::Element()
    : id_rel_("NoRelID")
    , id_abs_("NoAbsID")
    , initialized_(false)
    , ready_(false)
    , onworkspace_(false)
{
  
}

Element::~Element() {
  
}





} // namespace elements 
} // namespace blueprint 
} // namespace numerobis 
} // namespace builder 
} // namespace doofit