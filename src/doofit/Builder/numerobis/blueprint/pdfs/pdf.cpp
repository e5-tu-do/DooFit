#include "pdf.h"

// from STL
#include <string>

// from RooFit
#include "RooWorkspace.h"
#include "RooAbsPdf.h"

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

Pdf::~Pdf() {
  
}

RooAbsPdf* Pdf::AddToWorkspace(RooWorkspace* ws, const std::map<std::string,RooAbsArg*>& dependants) {
  if (!ready()) {
    throw; 
  }
  
  if (!onworkspace()){
    RooAbsPdf* roo_obj_temp = CreateTempRooObj(dependants);

    ws->import(*roo_obj_temp);
    delete roo_obj_temp;
    
    set_onworkspace(true);
  }
  return GetRooObjFromWorkspace(ws);
}

} // namespace pdfs 
} // namespace blueprint 
} // namespace numerobis 
} // namespace builder 
} // namespace doofit 
