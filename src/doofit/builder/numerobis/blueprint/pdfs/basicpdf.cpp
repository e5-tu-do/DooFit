#include "basicpdf.h"

// from STL
#include <string>

namespace doofit {
namespace builder {
namespace numerobis {
namespace blueprint {
namespace pdfs {

BasicPdf::BasicPdf(const std::string& id_rel, const std::string& id_abs) 
  : Pdf(id_rel,id_abs)
{
  
}

BasicPdf::~BasicPdf() {}
  
} // namespace pdfs
} // namespace blueprint 
} // namespace numerobis 
} // namespace builder 
} // namespace doofit