#ifndef DOOFIT_BUILDER_NUMEROBIS_BLUEPRINT_ElEMENTS_ELEMENT_H
#define DOOFIT_BUILDER_NUMEROBIS_BLUEPRINT_ElEMENTS_ELEMENT_H

// from STL
#include <string>
#include <vector>

// from project
#include "doofit/utils//MsgStream.h"

// forward declarations - RooFit
class RooWorkspace;
class RooAbsArg;

namespace doofit {
namespace builder {
namespace numerobis {
namespace blueprint {
namespace elements {
  
/** @namespace doofit::builder::numerobis::blueprint::elements
 *
 *  @brief Elements namespace in Numerobis/Blueprint
 *
 *  All elements which go into PDFs are grouped into this workspace as well as 
 *  the approprate Registrar and Factory acting as interface to outside code.
 */

// forward declarations - Numerobis...
class Registrar;
  
/**
 *  @class doofit::builder::numerobis::blueprint::elements::Element
 *  @brief The elements mother class for all PDF elements
 *
 *  This abstract class is base class for all PDF elements used in 
 *  doofit::builder::numerobis::blueprint. It carries relative and absolute IDs
 *  as well as different states of readyness. Also the AddToWorkspace() 
 *  function is supplied to add the RooFit object to a RooWorkspace.
 */
class Element {
 public:
  //Element();
  Element(const std::string& id_rel, const std::string& id_abs);
  virtual ~Element();
  
  /** Getter for id_rel_**/
  std::string id_rel() const { return id_rel_; }
  /** Setter for id_rel_**/
  void set_id_rel(const std::string& id_rel) { id_rel_ = id_rel; } 
  
  /** Getter for id_abs_**/
  std::string id_abs() const { return id_abs_; }
  /** Setter for id_abs_**/
  void set_id_abs(const std::string& id_abs) { id_abs_ = id_abs; } 
  
  /** @brief Adds RooFit representation to RooWorkspace
   *
   *  This function will initialize the appropriate RooFit object and import 
   *  this to the supplied workspace. A vector of the dependants as RooAbsArg* 
   *  must be supplied in exactly the same order as the dependants vector.
   *
   *  @param ws the workspace to add the object to
   *  @param dependants the dependants to be used for initialization
   *  @return the RooAbsArg on the workspace
  **/
  RooAbsArg* AddToWorkspace(RooWorkspace* ws, const std::vector<RooAbsArg*>& dependants);
  
  /**
   *  @brief Get list of dependants
   *
   *  This function will return the list of dependants as 
   *  std::vector<std::string> with absolute IDs as vector elements
   *
   *  @return the dependants of this element
  **/
  const std::vector<std::string>& dependants() const { return dependants_; }
  
  /** Getter for initialized_**/
  bool initialized() const { return initialized_; }
  /** Setter for initialized_**/
  void set_initialized(bool initialized) { initialized_ = initialized; } 
  
  /** Getter for onworkspace_**/
  bool onworkspace() const { return onworkspace_; }
  /** Setter for onworkspace_**/
  void set_onworkspace(bool onworkspace) { onworkspace_ = onworkspace; } 
  
  /** Getter for ready_**/
  bool ready() const { return ready_; }
  /** Setter for ready_**/
  void set_ready(bool ready) { ready_ = ready; }
  
 protected:
  /** @brief Create instance of this element as RooFit object
   *  
   *  Initialize the appropriate RooFit object and return it as RooAbsArg 
   *  pointer.
   *
   *  @param dependants the dependants to be used for initialization
   *  @return the temporary initialized object to be put onto a workspace
  **/
  virtual RooAbsArg* CreateTempRooObj(const std::vector<RooAbsArg*>& dependants = std::vector<RooAbsArg*>()) = 0;
  
  /** @brief Create instance of this element as RooFit object
   *  
   *  Initialize the appropriate RooFit object and return it as RooAbsArg 
   *  pointer.
   *
   *  @param dependants the dependants to be used for initialization
   *  @return the temporary initialized object to be put onto a workspace
  **/
  virtual RooAbsArg* GetRooObjFromWorkspace(RooWorkspace* ws) = 0;
   
  /**
   *  @brief List of dependants
  **/
  std::vector<std::string> dependants_;
  
 private:
  std::string id_rel_;
  std::string id_abs_;
  
  bool initialized_;
  bool ready_;
  bool onworkspace_;
};

} // namespace elements
} // namespace blueprint
} // namespace numerobis
} // namespace builder
} // namespace doofit

namespace doofit {
namespace utils {
/**
 *  @brief Function to output Numerobis elements directly and nicely into MsgStreams
 *
 *  This function just prints all information in a Element nicely.
**/
inline MsgStream& operator<<(MsgStream& lhs, const doofit::builder::numerobis::blueprint::elements::Element& element) {
  lhs.stream() << "Element: " << element.id_abs();
  
  if (element.initialized()) {
    lhs.stream() << " initialized";
  }
  if (element.ready()) {
    lhs.stream() << " ready";
  }
  if (element.onworkspace()) {
    lhs.stream() << " on workspace";
  }
  
  return lhs;
}
} // namespace utils
} // namespace doofit

#endif // DOOFIT_BUILDER_NUMEROBIS_BLUEPRINT_ElEMENTS_ELEMENT_H
