#include "doofit/Builder/numerobis/blueprint/blueprint.h"

namespace doofit {
namespace builder {
namespace numerobis {
namespace blueprint {

Blueprint::Blueprint() 
    : reg_elems_()
    , fac_elems_(reg_elems_)
    , reg_pdfs_(reg_elems_)
{
  
}

Blueprint::~Blueprint() {
  
}

} // namespace blueprint 
} // namespace numerobis 
} // namespace builder 
} // namespace doofit 