#ifndef DOOFIT_BUILDER_NUMEROBIS_BLUEPRINT_PDFS_BASICPDF_H
#define DOOFIT_BUILDER_NUMEROBIS_BLUEPRINT_PDFS_BASICPDF_H

//from project
#include "pdf.h"

namespace doofit {
namespace builder {
namespace numerobis {
namespace blueprint {
namespace pdfs {

class BasicPdf : public Pdf {
 public:
  BasicPdf();
  virtual ~BasicPdf();
 protected:
  
 private:
  /* data */
};

} // namespace pdfs
} // namespace blueprint 
} // namespace numerobis 
} // namespace builder 
} // namespace doofit

#endif // DOOFIT_BUILDER_NUMEROBIS_BLUEPRINT_PDFS_BASICPDF_H