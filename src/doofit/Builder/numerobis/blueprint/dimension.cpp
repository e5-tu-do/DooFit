#include "doofit/Builder/numerobis/blueprint/dimension.h"

// from project
#include "doofit/Builder/numerobis/blueprint/pdfs/pdf.h"

namespace doofit {
namespace builder {
namespace numerobis {
namespace blueprint {

Dimension::Dimension(const std::string& id_rel, const std::string& id_abs, 
                     const std::string& id_abs_dimelement, 
                     const std::string& id_abs_pdf) 
  : id_rel_(id_rel)
  , id_abs_(id_abs)
  , id_abs_dimelement_(id_abs_dimelement)
  , ids_abs_pdfs_() 
{
  ids_abs_pdfs_.push_back(id_abs_pdf);
}
  
Dimension::~Dimension () {}
  
} // namespace numerobis 
} // namespace blueprint 
} // namespace builder 
} // namespace doofit 
