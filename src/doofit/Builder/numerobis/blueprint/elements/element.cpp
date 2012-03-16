#include "doofit/Builder/numerobis/blueprint/elements/element.h"

// from STL
#include <string>

// from project
#include "doofit/Builder/numerobis/blueprint/elements/registrar.h"

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


} // namespace elements 
} // namespace blueprint 
} // namespace numerobis 
} // namespace builder 
} // namespace doofit