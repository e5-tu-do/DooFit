#include "gausspdf.h"

namespace doofit {
namespace builder {
namespace numerobis {
namespace blueprint {
namespace pdfs {

GaussPdf::GaussPdf() {
  set_initialized(false);
  set_ready(false);  
}

GaussPdf::GaussPdf(const std::string& id_rel, const std::string& id_abs, const std::string& id_abs_dimension, const std::string& id_abs_mean, const std::string& id_abs_sigma)
  : GenPdf(id_rel, id_abs)
  , id_abs_dimension_(id_abs_dimension)
  , id_abs_mean_(id_abs_mean)
  , id_abs_sigma_(id_abs_sigma)
{
  set_initialized(true);
  set_ready(false);
}
  
GaussPdf::~GaussPdf() {
  
}

} // namespace pdfs
} // namespace blueprint 
} // namespace numerobis 
} // namespace builder 
} // namespace doofit
