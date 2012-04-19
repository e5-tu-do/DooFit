#include "doofit/Builder/numerobis/blueprint/component.h"

// from STL
#include <string>
#include <vector>

// from RooFit
#include "RooProdPdf.h"
#include "RooExtendPdf.h"
#include "RooWorkspace.h"

// from project
#include "doofit/Builder/numerobis/blueprint/pdfs/pdf.h"
#include "doofit/Builder/numerobis/blueprint/blueprint.h"

namespace doofit {
namespace builder {
namespace numerobis {
namespace blueprint {

Component::Component(const std::string& id_rel, const std::string& id_abs, 
                     const std::vector<std::string>& id_abs_dimensions,
                     const std::string& id_abs_yield) 
  : id_rel_(id_rel)
  , id_abs_(id_abs)
  , initialized_(false)
  , ready_(false)
  , onworkspace_(false)
  , is_extended_(true)
  , id_abs_yield_(id_abs_yield)
  , ids_abs_dimensions_(id_abs_dimensions)
{
  if (id_abs_yield_ == "") {
    is_extended_ = false;
  }
}
  
Component::~Component () {}

RooAbsArg* Component::AddToWorkspace(RooWorkspace* ws, 
                                     const std::vector<RooAbsArg*>& dependants,
                                     RooAbsReal* yield) {
  if (!ready()) {
    throw; 
  }
  
  if (!onworkspace()){
    RooArgList dep_list = VectorToArgList(dependants);
    
    if (is_extended()) {
      std::string id_pdf_prod = id_abs() + "_prod";
      RooProdPdf pdf_prod(id_pdf_prod.c_str(), id_pdf_prod.c_str(), dep_list);
      RooExtendPdf* pdf_extend = new RooExtendPdf(id_abs().c_str(), id_abs().c_str(),
                                                  pdf_prod, *yield);
      
      ws->import(*pdf_extend);
      delete pdf_extend;
    } else {
      RooProdPdf* pdf_prod = new RooProdPdf(id_abs().c_str(), id_abs().c_str(), dep_list);
      
      ws->import(*pdf_prod);
    }
    
    set_onworkspace(true);
  }
  return ws->pdf(id_abs().c_str());
}

} // namespace numerobis 
} // namespace blueprint 
} // namespace builder 
} // namespace doofit 
