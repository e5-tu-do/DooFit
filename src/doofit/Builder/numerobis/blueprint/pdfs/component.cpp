#include "doofit/Builder/numerobis/blueprint/pdfs/component.h"

// from project
#include "doofit/Builder/numerobis/blueprint/pdfs/pdf.h"

namespace doofit {
namespace builder {
namespace numerobis {
namespace blueprint {
namespace pdfs {

Component::Component(const std::string& id_rel, const std::string& id_abs) 
  : id_rel_(id_rel)
  , id_abs_(id_abs)
{
}
  
Component::~Component () {}
  
} // namespace pdfs 
} // namespace numerobis 
} // namespace blueprint 
} // namespace builder 
} // namespace doofit 
