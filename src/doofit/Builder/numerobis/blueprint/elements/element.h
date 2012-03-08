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
  
  /** Getter for id_ */
  std::string id() const { return id_; }
  /** Setter for id_ */
  void set_id(const std::string& id) { id_ = id; }
  
 protected:
  /** Getter for initialized_ */
  bool initialized() const { return initialized_; }
  /** Setter for initialized_ */
  void set_initialized(bool initialized) { initialized_ = initialized; } 
  
  /** Getter for onworkspace_ */
  bool onworkspace() const { return onworkspace_; }
  /** Setter for onworkspace_ */
  void set_onworkspace(bool onworkspace) { onworkspace_ = onworkspace; } 
  
  /** Getter for ready_ */
  bool ready() const { return ready_; }
  /** Setter for ready_ */
  void set_ready(bool ready) { ready_ = ready; } 
  
  
  
 private:
  std::string id_;
  
  bool initialized_;
  bool ready_;
  bool onworkspace_;
  
};


} // namespace elements
} // namespace blueprint
} // namespace numerobis
} // namespace builder
} // namespace doofit


#endif // DOOFIT_BUILDER_NUMEROBIS_BLUEPRINT_ElEMENTS_ELEMENT_H