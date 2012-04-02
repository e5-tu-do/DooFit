#ifndef DOOFIT_BUILDER_NUMEROBIS_BLUEPRINT_PDFS_DIMENSION_H
#define DOOFIT_BUILDER_NUMEROBIS_BLUEPRINT_PDFS_DIMENSION_H

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
  
 protected:  
 private:
  std::string id_rel_;
  std::string id_abs_;
  
  /**
   *  @brief absolute ID of the DimReal element to use
   */
  std::string id_abs_dimelement_;
  
  /**
   *  @brief absolute ID(s) of PDF(s) to for this dimension
   */
  std::vector<std::string> ids_abs_pdfs_;
};

} // namespace pdfs 
} // namespace blueprint 
} // namespace numerobis 
} // namespace builder 
} // namespace doofit 

#endif // DOOFIT_BUILDER_NUMEROBIS_BLUEPRINT_PDFS_DIMENSION_H
