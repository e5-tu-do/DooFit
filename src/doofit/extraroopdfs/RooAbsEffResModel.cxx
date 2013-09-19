#include <RooResolutionModel.h>
#include "RooAbsEffResModel.h"

namespace doofit {
namespace extraroopdfs {
//_____________________________________________________________________________
RooAbsEffResModel::RooAbsEffResModel(const char *name, const char *title, RooRealVar& __convVar) 
   : RooResolutionModel(name, title, __convVar)
{ }

//_____________________________________________________________________________
RooAbsEffResModel::RooAbsEffResModel(const RooAbsEffResModel& other, const char* name) 
  : RooResolutionModel(other, name)
{ }

RooAbsEffResModel::~RooAbsEffResModel()
{ }
}
}