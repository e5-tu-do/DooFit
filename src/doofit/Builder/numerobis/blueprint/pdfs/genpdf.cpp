#include "genpdf.h"

// from STL
#include <string>

namespace doofit {
namespace builder {
namespace numerobis {
namespace blueprint {
namespace pdfs {

GenPdf::GenPdf() {
  
}

GenPdf::GenPdf(const std::string& id_rel, const std::string& id_abs) 
  : BasicPdf(id_rel, id_abs)
{
  
}
  
GenPdf::~GenPdf() {}
  
} // namespace pdfs
} // namespace blueprint 
} // namespace numerobis 
} // namespace builder 
} // namespace doofit