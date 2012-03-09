#include "doofit/Builder/numerobis/blueprint/elements/dimreal.h"

// from RooFit
#include "RooRealVar.h"

namespace doofit {
namespace builder {
namespace numerobis {
namespace blueprint {
namespace elements {


DimReal::DimReal(){
  
}

DimReal::~DimReal() {
  
}

RooAbsReal* DimReal::CreateTempRooObj() {
  RooRealVar* temp_realvar = new RooRealVar(id_abs().c_str(), desc().c_str(), val_min(), val_max(), unit().c_str());
  return temp_realvar;
}

} // namespace elements 
} // namespace blueprint 
} // namespace numerobis 
} // namespace builder 
} // namespace doofit