#ifndef DOOFIT_BUILDER_NUMEROBIS_BLUEPRINT_PDFS_GAUSSPDF_H
#define DOOFIT_BUILDER_NUMEROBIS_BLUEPRINT_PDFS_GAUSSPDF_H

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
  /* data */
};

} // namespace pdfs
} // namespace blueprint 
} // namespace numerobis 
} // namespace builder 
} // namespace doofit

#endif // DOOFIT_BUILDER_NUMEROBIS_BLUEPRINT_PDFS_GAUSSPDF_H