#ifndef DOOFIT_BUILDER_NUMEROBIS_BLUEPRINT_COMPONENT_H
#define DOOFIT_BUILDER_NUMEROBIS_BLUEPRINT_COMPONENT_H

// from STL
#include <string>
#include <vector>

// from project
#include "doofit/Builder/numerobis/blueprint/pdfs/pdf.h"

namespace doofit {
namespace builder {
namespace numerobis {
namespace blueprint {

/** @class Component
 *  @brief A component representing a component PDF (contains 1+ dimension(s))
 *
 *  Objects of this class represent a component in a fit. It will contain/link 
 *  to several Dimension objects and an extended PDF which combines the product
 *  of the corresponding dimension PDFs.
 **/  
class Component {
 public:
  /**
   *  @brief Constructor for Component with a vector of dimensions
   *
   *  Create a component depending on a vector of dimensions and a yield
   *  parameter.
   *
   *  @param id_rel this Component's relative ID
   *  @param id_abs this Component's absolute ID
   *  @param id_abs_yield absolute ID of the yield element
   *  @param id_abs_dimensions vector of absolute IDs of dimensions to use
   */
  Component(const std::string& id_rel, const std::string& id_abs, 
            const std::string& id_abs_yield, 
            const std::vector<std::string>& id_abs_dimensions);
  
  /**
   *  @brief Destructor.
   */
  virtual ~Component();
  
  /** @name Getter/Setters for IDs
   */
  ///@{
  /** 
   * @brief Getter for id_rel_
   **/
  std::string id_rel() const { return id_rel_; }
  /** 
   * @brief Setter for id_rel_
   **/
  void set_id_rel(const std::string& id_rel) { id_rel_ = id_rel; } 
  
  /** 
   * @brief Getter for id_abs_
   **/
  std::string id_abs() const { return id_abs_; }
  /** 
   * @brief Setter for id_abs_
   **/
  void set_id_abs(const std::string& id_abs) { id_abs_ = id_abs; } 
  ///@}
  
  /** @name Getter/Setters for readyness state
   */
  ///@{
  /** 
   * @brief Getter for initialized_
   **/
  bool initialized() const { return initialized_; }
  /** 
   * @brief Setter for initialized_
   **/
  void set_initialized(bool initialized) { initialized_ = initialized; } 
  
  /** 
   * @brief Getter for onworkspace_
   **/
  bool onworkspace() const { return onworkspace_; }
  /** 
   * @brief Setter for onworkspace_
   **/
  void set_onworkspace(bool onworkspace) { onworkspace_ = onworkspace; } 
  
  /** 
   * @brief Getter for ready_
   **/
  bool ready() const { return ready_; }
  /** 
   * @brief Setter for ready_
   **/
  void set_ready(bool ready) { ready_ = ready; }
  ///@}
  
  /** 
   * @brief Getter for id_abs_yield_
   **/
  const std::string& id_abs_yield() const { return id_abs_yield_; }
  /** 
   * @brief Setter for id_abs_yield_
   **/
  void set_id_abs_yield(const std::string& id_abs_yield) { id_abs_yield_ = id_abs_yield; }
  /** 
   * @brief Getter for ids_abs_dimensions_
   **/
  const std::vector<std::string>& ids_abs_dimensions() const { return ids_abs_dimensions_; }
  /** 
   * @brief Setter for ids_abs_dimensions_
   **/
  void set_ids_abs_dimensions(const std::vector<std::string>& ids_abs_dimensions) { ids_abs_dimensions_ = ids_abs_dimensions; }
  
  /** @brief Adds RooFit representation to RooWorkspace
   *
   *  This function will initialize the appropriate RooFit object and import 
   *  this to the supplied workspace. A vector of the dependants as RooAbsArg* 
   *  must be supplied in exactly the same order as the dependants vector.
   *
   *  @param ws the workspace to add the object to
   *  @param yield the yield to construct an extended PDF
   *  @param dependants the dependant PDFs to be used for initialization
   *  @return the RooAbsArg on the workspace
   **/
  RooAbsArg* AddToWorkspace(RooWorkspace* ws, RooAbsReal* yield,
                            const std::vector<RooAbsArg*>& dependants);
  
 protected:  
 private:
  std::string id_rel_;
  std::string id_abs_;
  
  bool initialized_;
  bool ready_;
  bool onworkspace_;
  
  /**
   *  @brief absolute ID of the yield parameter element to use
   */
  std::string id_abs_yield_;
  
  /**
   *  @brief absolute ID(s) of dimension(s) to use for this component
   */
  std::vector<std::string> ids_abs_dimensions_;
};

} // namespace blueprint 
} // namespace numerobis 
} // namespace builder 
} // namespace doofit 

#endif // DOOFIT_BUILDER_NUMEROBIS_BLUEPRINT_COMPONENT_H
