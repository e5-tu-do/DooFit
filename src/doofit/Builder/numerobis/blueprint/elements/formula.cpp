#include "doofit/Builder/numerobis/blueprint/elements/formula.h"

// from RooFit
#include "RooFormulaVar.h"
#include "RooWorkspace.h"

// from project
#include "doofit/Builder/numerobis/blueprint/elements/registrar.h"

namespace doofit {
namespace builder {
namespace numerobis {
namespace blueprint {
namespace elements {
  
Formula::Formula() {
  set_initialized(false);
  set_ready(false);  
}

Formula::Formula(const std::string& id_rel, const std::string& id_abs, 
                 const std::string& formula, std::vector<std::string> elements) 
: RealValElement(id_rel, id_abs)
, formula_(formula)
{
  set_initialized(true);
  set_ready(false);
  
  dependants_ = elements;
}


Formula::~Formula() {
  
}
  
RooAbsArg* Formula::CreateTempRooObj(std::vector<RooAbsArg*> dependants) {
  RooArgList dep_list = VectorToArgList(dependants);
  
  RooFormulaVar* temp_formulavar = NULL;
  temp_formulavar = new RooFormulaVar(id_abs().c_str(), formula_.c_str(), dep_list);
  
  return temp_formulavar;
}
  
RooAbsArg* Formula::GetRooObjFromWorkspace(RooWorkspace* ws) {
  roo_obj_ = ws->function(id_abs().c_str());
  return dynamic_cast<RooFormulaVar*>(roo_obj_);
}

  
} // namespace elements 
} // namespace blueprint 
} // namespace numerobis 
} // namespace builder 
} // namespace doofit
