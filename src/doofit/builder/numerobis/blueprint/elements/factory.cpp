#include "doofit/builder/numerobis/blueprint/elements/factory.h"

// from project
#include "doofit/builder/numerobis/blueprint/elements/registrar.h"
#include "doofit/builder/numerobis/blueprint/elements/parambasic.h"
#include "doofit/builder/numerobis/blueprint/elements/formula.h"


namespace doofit {
namespace builder {
namespace numerobis {
namespace blueprint {
namespace elements {

Factory::Factory(Registrar& registrar) 
  : registrar_(registrar)
{}

Factory::~Factory () {}
  
} // namespace elements 
} // namespace numerobis 
} // namespace blueprint 
} // namespace builder 
} // namespace doofit 
