#ifndef DOOFIT_BUILDER_NUMEROBIS_BLUEPRINT_PDFS_GAUSSPDF_H
#define DOOFIT_BUILDER_NUMEROBIS_BLUEPRINT_PDFS_GAUSSPDF_H

// from STL
#include <string>

//from project
#include "genpdf.h"

namespace doofit {
namespace builder {
namespace numerobis {
namespace blueprint {
namespace pdfs {

class GaussPdf : public GenPdf {
 public:
  GaussPdf();
  virtual ~GaussPdf();
 protected:
  
 private:
  /**
   *  @brief absolute ID of the associated dimension
   */
  std::string id_abs_dimension_;
  /**
   *  @brief absolute ID of the associated mean
   */
  std::string id_abs_mean_;
  /**
   *  @brief absolute ID of the associated sigma
   */
  std::string id_abs_sigma_;  
};

} // namespace pdfs
} // namespace blueprint 
} // namespace numerobis 
} // namespace builder 
} // namespace doofit

#endif // DOOFIT_BUILDER_NUMEROBIS_BLUEPRINT_PDFS_GAUSSPDF_H