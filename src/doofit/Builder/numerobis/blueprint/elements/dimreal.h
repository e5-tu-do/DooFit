#ifndef DOOFIT_BUILDER_NUMEROBIS_BLUEPRINT_ElEMENTS_DIMREAL_H
#define DOOFIT_BUILDER_NUMEROBIS_BLUEPRINT_ElEMENTS_DIMREAL_H

// from STL
#include <string>

// from project
#include "doofit/Builder/numerobis/blueprint/elements/dimelement.h"
#include "doofit/Builder/numerobis/blueprint/elements/realvalelement.h"


namespace doofit {
namespace builder {
namespace numerobis {
namespace blueprint {
namespace elements {

class DimReal : public DimElement, public RealValElement {
 public:
  DimReal();
  virtual ~DimReal();
  
 protected:
  
};


} // namespace elements
} // namespace blueprint
} // namespace numerobis
} // namespace builder
} // namespace doofit


#endif // DOOFIT_BUILDER_NUMEROBIS_BLUEPRINT_ElEMENTS_DIMREAL_H