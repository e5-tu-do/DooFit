#ifndef DOOFIT_BUILDER_NUMEROBIS_BLUEPRINT_PDFS_GENPDF_H
#define DOOFIT_BUILDER_NUMEROBIS_BLUEPRINT_PDFS_GENPDF_H

// from STL
#include <string>

//from project
#include "basicpdf.h"

namespace doofit {
namespace builder {
namespace numerobis {
namespace blueprint {
namespace pdfs {

class GenPdf : public BasicPdf {
 public:
  GenPdf(const std::string& id_rel, const std::string& id_abs);
  virtual ~GenPdf();
 protected:
  
 private:
  /* data**/
};

} // namespace pdfs
} // namespace blueprint 
} // namespace numerobis 
} // namespace builder 
} // namespace doofit

#endif // DOOFIT_BUILDER_NUMEROBIS_BLUEPRINT_PDFS_GENPDF_H