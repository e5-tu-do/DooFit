#ifndef DOOFIT_BUILDER_NUMEROBIS_BLUEPRINT_ElEMENTS_REALVALELEMENT_H
#define DOOFIT_BUILDER_NUMEROBIS_BLUEPRINT_ElEMENTS_REALVALELEMENT_H

// from STL
#include <string>

// from project
#include "doofit/builder/numerobis/blueprint/elements/element.h"


// forward declarations - RooFit
class RooAbsReal;
class RooAbsArg;

namespace doofit {
namespace builder {
namespace numerobis {
namespace blueprint {
namespace elements {

class RealValElement : public Element {
 public:
  RealValElement(const std::string& id_rel, const std::string& id_abs);
  virtual ~RealValElement();
    
 protected:

};


} // namespace elements
} // namespace blueprint 
} // namespace numerobis 
} // namespace builder 
} // namespace doofit 


#endif // DOOFIT_BUILDER_NUMEROBIS_BLUEPRINT_ElEMENTS_REALVALELEMENT_H
