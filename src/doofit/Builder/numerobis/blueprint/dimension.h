#ifndef DOOFIT_BUILDER_NUMEROBIS_BLUEPRINT_DIMENSION_H
#define DOOFIT_BUILDER_NUMEROBIS_BLUEPRINT_DIMENSION_H

// from STL
#include <string>
#include <vector>

// from project
#include "doofit/Builder/numerobis/blueprint/pdfs/pdf.h"

namespace doofit {
namespace builder {
namespace numerobis {
namespace blueprint {

/** @class Dimension
 *  @brief A dimension representing a dimension PDF (contains 1+ PDF(s))
 *
 *  Objects of this class represent a dimension (i.e. DimReal) and 
 *  corresponding PDF(s).
**/  
class Dimension {
 public:
  /**
   *  @brief Constructor for dimension with one PDF
   *
   *  Detailed description.
   *
   *  @param id_rel this Dimension's relative ID
   *  @param id_abs this Dimension's absolute ID
   *  @param id_abs_dimelement the DimReal element's ID to use
   *  @param id_abs_pdf corresponding Pdf for this dimension
   */
  Dimension(const std::string& id_rel, const std::string& id_abs, const std::string& id_abs_dimelement, const std::string& id_abs_pdf);
  
  /**
   *  @brief Destructor.
   */
  virtual ~Dimension();
  
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
   * @brief Getter for id_abs_dimelement_
   **/
  std::string id_abs_dimelement() const { return id_abs_dimelement_; }
  /** 
   * @brief Setter for id_abs_dimelement_
   **/
  void set_id_abs_dimelement(const std::string& id_abs_dimelement) { id_abs_dimelement_ = id_abs_dimelement; }
  
  /** 
   * @brief Getter for ids_abs_pdfs_
   **/
  const std::vector<std::string>& ids_abs_pdfs() const { return ids_abs_pdfs_; }
  /** 
   * @brief Setter for ids_abs_pdfs_
   **/
  void set_ids_abs_pdfs(const std::vector<std::string>& ids_abs_pdfs) { ids_abs_pdfs_ = ids_abs_pdfs; }
  
  /** 
   * @brief Getter for ids_abs_pdfs_ for one dependant Pdf
   **/
  const std::string& ids_abs_pdf() const { return ids_abs_pdfs_.front(); }
  /** 
   * @brief Setter for ids_abs_pdfs_ for one dependant Pdf
   **/
  void set_ids_abs_pdf(const std::string& ids_abs_pdf) { ids_abs_pdfs_.clear();
    ids_abs_pdfs_.push_back(ids_abs_pdf); }
  
 protected:  
 private:
  std::string id_rel_;
  std::string id_abs_;
  
  bool initialized_;
  bool ready_;
  bool onworkspace_;
  
  /**
   *  @brief absolute ID of the DimReal element to use
   */
  std::string id_abs_dimelement_;
  
  /**
   *  @brief absolute ID(s) of PDF(s) to use for this dimension
   */
  std::vector<std::string> ids_abs_pdfs_;
};

} // namespace blueprint 
} // namespace numerobis 
} // namespace builder 
} // namespace doofit 

#endif // DOOFIT_BUILDER_NUMEROBIS_BLUEPRINT_DIMENSION_H
