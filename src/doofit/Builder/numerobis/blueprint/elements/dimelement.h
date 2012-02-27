#ifndef DOOFIT_BUILDER_NUMEROBIS_BLUEPRINT_ElEMENTS_DIMELEMENT_H
#define DOOFIT_BUILDER_NUMEROBIS_BLUEPRINT_ElEMENTS_DIMELEMENT_H

// from STL
#include <string>

// from project
#include "doofit/Builder/numerobis/blueprint/elements/element.h"


namespace doofit {
namespace builder {
namespace numerobis {
namespace blueprint {
namespace elements {

class DimElement : public virtual Element {
 public:
  DimElement();
  virtual ~DimElement();
  
 protected:
  
};


} // namespace elements
} // namespace blueprint
} // namespace numerobis
} // namespace builder
} // namespace doofit


#endif // DOOFIT_BUILDER_NUMEROBIS_BLUEPRINT_ElEMENTS_DIMELEMENT_H