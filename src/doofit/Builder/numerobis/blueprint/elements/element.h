#ifndef DOOFIT_BUILDER_NUMEROBIS_BLUEPRINT_ElEMENTS_ELEMENT_H
#define DOOFIT_BUILDER_NUMEROBIS_BLUEPRINT_ElEMENTS_ELEMENT_H

// from STL
#include <string>

// forward declarations - RooFit
class RooWorkspace;

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
  std::string id() const { return id_; }
  void set_id(const std::string& id) { id_ = id; } 
  
 protected:
  std::string id_;
  
  bool initialized_;
  bool onworkspace_;
  
};




} // namespace elements
} // namespace blueprint
} // namespace numerobis
} // namespace builder
} // namespace doofit


#endif // DOOFIT_BUILDER_NUMEROBIS_BLUEPRINT_ElEMENTS_ELEMENT_H