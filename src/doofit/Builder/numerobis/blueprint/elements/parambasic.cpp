#include "doofit/Builder/numerobis/blueprint/elements/parambasic.h"

// from RooFit
#include "RooRealVar.h"

namespace doofit {
namespace builder {
namespace numerobis {
namespace blueprint {
namespace elements {

ParamBasic::ParamBasic(){

}

ParamBasic::ParamBasic(const std::string& id_rel, const std::string& id_abs, 
                       const std::string& desc, double val_init, double val_min, 
                       double val_max, const std::string& unit) 
    : val_init_(val_init)
    , val_min_(val_min)
    , val_max_(val_max)
    , unit_(unit)
{
  set_id_rel(id_rel);
  set_id_abs(id_abs);
  set_desc(desc);
  
  set_initialized(true);
  set_ready(true);
}

ParamBasic::ParamBasic(const std::string& id_rel, const std::string& id_abs, 
                       const std::string& desc, double val_init, 
                       const std::string& unit) 
    : val_init_(val_init)
    , val_min_(val_init_)
    , val_max_(val_init_)
    , unit_(unit)
{
  set_id_rel(id_rel);
  set_id_abs(id_abs);
  set_desc(desc);
  
  set_initialized(true);
  set_ready(true);
}

ParamBasic::~ParamBasic() {

}

RooAbsReal* ParamBasic::CreateTempRooObj(std::vector<RooAbsArg*> dependants) {
  RooRealVar* temp_realvar = NULL;
  if (val_max_ == val_init_ && val_min_ == val_init_) {
    temp_realvar = new RooRealVar(id_abs().c_str(), desc().c_str(), val_init(), unit().c_str());
  } else {
    temp_realvar = new RooRealVar(id_abs().c_str(), desc().c_str(), val_init(), val_min(), val_max(), unit().c_str());
  }
  return temp_realvar;
}

} // namespace elements 
} // namespace blueprint 
} // namespace numerobis 
} // namespace builder 
} // namespace doofit