#ifndef DOOFIT_BUILDER_NUMEROBIS_BLUEPRINT_ElEMENTS_ELEMENT_H
#define DOOFIT_BUILDER_NUMEROBIS_BLUEPRINT_ElEMENTS_ELEMENT_H

// from STL
#include <string>

// forward declarations

namespace doofit {
namespace builder {
namespace numerobis {
namespace blueprint {
namespace elements {

class Element {
 public:
  Element();
  virtual ~Element();
  
  /** Setter and getter for name_ */
  std::string name() const { return name_; }
  void set_name(const std::string& name) { name_ = name; } 
  
  
 protected:
  std::string name_;
  
};




} // namespace elements
} // namespace blueprint 
} // namespace numerobis 
} // namespace builder 
} // namespace doofit 


#endif // DOOFIT_BUILDER_NUMEROBIS_BLUEPRINT_ElEMENTS_ELEMENT_H