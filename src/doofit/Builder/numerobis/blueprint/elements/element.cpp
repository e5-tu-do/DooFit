#include "doofit/Builder/numerobis/blueprint/elements/element.h"

namespace doofit {
namespace builder {
namespace numerobis {
namespace blueprint {
namespace elements {


Element::Element()
    : id_("NoID")
    , initialized_(false)
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