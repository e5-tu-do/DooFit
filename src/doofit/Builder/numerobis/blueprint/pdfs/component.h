#ifndef DOOFIT_BUILDER_NUMEROBIS_BLUEPRINT_PDFS_COMPONENT_H
#define DOOFIT_BUILDER_NUMEROBIS_BLUEPRINT_PDFS_COMPONENT_H

// from STL
#include <string>
#include <vector>

// from project
#include "doofit/Builder/numerobis/blueprint/pdfs/pdf.h"

namespace doofit {
namespace builder {
namespace numerobis {
namespace blueprint {
namespace pdfs {

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
   *  @brief Constructor for Component with one Dimension
   *
   *  Detailed description.
   *
   *  @param id_rel this Component's relative ID
   *  @param id_abs this Component's absolute ID
   */
  Component(const std::string& id_rel, const std::string& id_abs);
  
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
  
 protected:  
 private:
  std::string id_rel_;
  std::string id_abs_;
};

} // namespace pdfs 
} // namespace blueprint 
} // namespace numerobis 
} // namespace builder 
} // namespace doofit 

#endif // DOOFIT_BUILDER_NUMEROBIS_BLUEPRINT_PDFS_COMPONENT_H
