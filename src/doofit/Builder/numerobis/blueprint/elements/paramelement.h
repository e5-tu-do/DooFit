#ifndef DOOFIT_BUILDER_NUMEROBIS_BLUEPRINT_ElEMENTS_PARAMELEMENT_H
#define DOOFIT_BUILDER_NUMEROBIS_BLUEPRINT_ElEMENTS_PARAMELEMENT_H

// from STL
#include <string>

// from project
#include "doofit/Builder/numerobis/blueprint/elements/realvalelement.h"


namespace doofit {
namespace builder {
namespace numerobis {
namespace blueprint {
namespace elements {

class ParamElement : public RealValElement {
 public:
  ParamElement();
  virtual ~ParamElement();
  
 protected:
  /** Getter for desc_ */
  std::string desc() const { return desc_; }
  /** Setter for desc_ */
  void set_desc(const std::string& desc) { desc_ = desc; } 
  
   
 private:
  std::string desc_;
  
};


} // namespace elements
} // namespace blueprint
} // namespace numerobis
} // namespace builder
} // namespace doofit



#endif // DOOFIT_BUILDER_NUMEROBIS_BLUEPRINT_ElEMENTS_PARAMELEMENT_H