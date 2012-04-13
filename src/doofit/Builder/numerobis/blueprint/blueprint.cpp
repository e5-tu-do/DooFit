#include "doofit/Builder/numerobis/blueprint/blueprint.h"

// from STL
#include <string>

// from boost
#include <boost/ptr_container/ptr_map.hpp>
#include <boost/foreach.hpp>

// from RooFit
#include "RooWorkspace.h"

// from project
#include "doofit/Builder/numerobis/blueprint/elements/registrar.h"
#include "doofit/Builder/numerobis/blueprint/elements/factory.h"
#include "doofit/Builder/numerobis/blueprint/pdfs/registrar.h"
#include "doofit/Builder/numerobis/blueprint/pdfs/factory.h"
#include "doofit/Builder/numerobis/blueprint/component.h"
#include "doofit/Builder/numerobis/blueprint/dimension.h"

namespace doofit {
namespace builder {
namespace numerobis {
namespace blueprint {

Blueprint::Blueprint() 
    : reg_elements_()
    , fac_elements_(reg_elements_)
    , reg_pdfs_(reg_elements_)
    , fac_pdfs_(reg_pdfs_)
{
  
}

Blueprint::~Blueprint() {
  
}

bool Blueprint::RegisterDimensions(RooWorkspace* workspace) {
  for (boost::ptr_map<std::string, Dimension>::const_iterator it = dimensions_.begin();
       it != dimensions_.end(); ++it) {
    reg_elements_.Register(workspace, it->second->id_abs_dimelement());
    const std::vector<std::string>& pdfs_ids = it->second->ids_abs_pdfs();
    BOOST_FOREACH(std::string it_pdf_id, pdfs_ids) {
      reg_pdfs_.Register(workspace, it_pdf_id);
    }
  }
  
  return false;
}
  
RooArgList VectorToArgList(std::vector<RooAbsArg*> vec) {
  RooArgList list; 
  for (std::vector<RooAbsArg*>::const_iterator it=vec.begin(); 
       it!=vec.end(); ++it) {
    list.add(**it, true);
  }
  return list;
}

} // namespace blueprint 
} // namespace numerobis 
} // namespace builder 
} // namespace doofit 