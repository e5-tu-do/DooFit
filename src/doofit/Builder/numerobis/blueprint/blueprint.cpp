#include "doofit/Builder/numerobis/blueprint/blueprint.h"

// from STL
#include <string>

// from boost
#include <boost/ptr_container/ptr_map.hpp>
#include <boost/foreach.hpp>

// from RooFit
#include "RooWorkspace.h"
#include "RooAbsReal.h"

// from project
#include "doofit/Builder/numerobis/blueprint/elements/registrar.h"
#include "doofit/Builder/numerobis/blueprint/elements/factory.h"
#include "doofit/Builder/numerobis/blueprint/pdfs/registrar.h"
#include "doofit/Builder/numerobis/blueprint/pdfs/factory.h"
#include "doofit/Builder/numerobis/blueprint/component.h"
#include "doofit/Builder/numerobis/blueprint/dimension.h"

#include "doofit/utils//MsgStream.h"

namespace doofit {
namespace builder {
namespace numerobis {
namespace blueprint {

using namespace doofit::utils;
  
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
  bool all_registered = true;
  for (boost::ptr_map<std::string, Dimension>::const_iterator it = dimensions_.begin();
       it != dimensions_.end(); ++it) {
    all_registered &= RegisterDimension(workspace, it->first);
  }
  return all_registered;
}
  
bool Blueprint::RegisterDimension(RooWorkspace* workspace, const std::string& id_abs_dim) {
  bool all_registered = true;
  boost::ptr_map<std::string, Dimension>::const_iterator it = dimensions_.find(id_abs_dim);
  if (it == dimensions_.end()) return false;

  all_registered &= static_cast<bool>(reg_elements_.Register(workspace, it->second->id_abs_dimelement()));
  const std::vector<std::string>& pdfs_ids = it->second->ids_abs_pdfs();
  BOOST_FOREACH(std::string it_pdf_id, pdfs_ids) {
    all_registered &= static_cast<bool>(reg_pdfs_.Register(workspace, it_pdf_id));
  }
  return all_registered;
}
  
bool Blueprint::RegisterComponents(RooWorkspace* workspace) {
  bool all_registered = true;
  for (boost::ptr_map<std::string, Component>::const_iterator it = comps_.begin();
       it != comps_.end(); ++it) {
    all_registered &= static_cast<bool>(RegisterComponent(workspace, it->first));
  }
    
  return all_registered;
}

RooAbsArg* Blueprint::RegisterComponent(RooWorkspace* workspace, const std::string& id_abs_comp) {
  bool all_registered = true;
  boost::ptr_map<std::string, Component>::iterator it = comps_.find(id_abs_comp);
  if (it == comps_.end()) return NULL;

  RooAbsReal* yield = NULL;
  if (it->second->is_extended()) {
    yield = dynamic_cast<RooAbsReal*>(reg_elements_.Register(workspace, it->second->id_abs_yield()));
    all_registered &= static_cast<bool>(yield);
  }
  const std::vector<std::string>& dims_ids = it->second->ids_abs_dimensions();
  std::vector<RooAbsArg*> pdfs_dimensions;
  BOOST_FOREACH(std::string it_dim_id, dims_ids) {
    all_registered &= RegisterDimension(workspace, it_dim_id);
    boost::ptr_map<std::string, Dimension>::iterator it = dimensions_.find(it_dim_id);
    if (it != dimensions_.end()) {
      pdfs_dimensions.push_back(reg_pdfs_.Register(workspace, it->second->PdfId()));
    }
  }
  
  if (all_registered) {
    Component* comp = it->second;
    
    comp->set_ready(true);
    return comp->AddToWorkspace(workspace, pdfs_dimensions, yield);
  } else {
    return NULL;
  }
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