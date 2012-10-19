#include "doofit/builder/numerobis/blueprint/pdfs/factory.h"

// from project
#include "doofit/builder/numerobis/blueprint/pdfs/registrar.h"

namespace doofit {
namespace builder {
namespace numerobis {
namespace blueprint {
namespace pdfs {

Factory::Factory(Registrar& registrar) 
  : registrar_(registrar)
{}

Factory::~Factory () {}
  
} // namespace pdfs 
} // namespace numerobis 
} // namespace blueprint 
} // namespace builder 
} // namespace doofit 
