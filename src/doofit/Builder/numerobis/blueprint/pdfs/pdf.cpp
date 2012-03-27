#include "pdf.h"

namespace doofit {
namespace builder {
namespace numerobis {
namespace blueprint {
namespace pdfs {

Pdf::Pdf() 
    : id_rel_("NoRelID")
    , id_abs_("NoAbslID")
    , initialized_(false)
    , ready_(false)
    , onworkspace_(false)
{ }

Pdf::Pdf(const std::string& id_rel, const std::string& id_abs)
    : id_rel_(id_rel)
    , id_abs_(id_abs)
    , initialized_(false)
    , ready_(false)
    , onworkspace_(false)
{ }


} // namespace pdfs 
} // namespace blueprint 
} // namespace numerobis 
} // namespace builder 
} // namespace doofit 
