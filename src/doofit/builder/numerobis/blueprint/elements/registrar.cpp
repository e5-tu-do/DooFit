#include "doofit/builder/numerobis/blueprint/elements/registrar.h"

// from STL
#include <string>
#include <map>

// from BOOST
#include <boost/foreach.hpp>

// from RooFit
#include "RooArgList.h"

// from project
#include "doocore/io/MsgStream.h"
#include "doofit/builder/numerobis/blueprint/elements/element.h"
#include "doofit/builder/numerobis/blueprint/elements/dimreal.h"
#include "doofit/builder/numerobis/blueprint/elements/parambasic.h"
#include "doofit/builder/numerobis/blueprint/elements/formula.h"


// forward decalaration
class RooWorkspace;

namespace doofit {
namespace builder {
namespace numerobis {
namespace blueprint {
namespace elements {


Registrar::Registrar()
    : elements_()
    , dim_reals_()
    , param_basics_()
{
  
}

Registrar::~Registrar() {
  
}

void Registrar::Declare(DimReal* dim_real) {
  DeclareElement(dim_real);
  dim_reals_.insert(dim_real->id_abs());
}

void Registrar::Declare(ParamBasic* param_basic) {
  DeclareElement(param_basic);
  param_basics_.insert(param_basic->id_abs());
}

void Registrar::Declare(Formula* formula) {
  DeclareElement(formula);
  formulas_.insert(formula->id_abs());
}

void Registrar::DeclareElement(Element* element) {
  std::string temp_key = element->id_abs();
  elements_.insert(temp_key,  element);
}
  
void Registrar::Print() const {
  namespace du = doocore::io;
  du::sinfo.Ruler();
  du::sinfo << "Registrar contents:" << du::endmsg;
  du::sinfo.increment_indent(2);
  for (boost::ptr_map<std::string, Element>::const_iterator it=elements_.begin();
       it!=elements_.end(); ++it) {
    du::sinfo << *(*it).second << du::endmsg;
  }
  du::sinfo.increment_indent(-2);
}
  
bool Registrar::CheckReady(const std::string& element_name) {
  boost::ptr_map<std::string, Element>::iterator it_element = elements_.find(element_name);
  if (it_element == elements_.end()) return false;
  
  Element* element = it_element->second;
  
  namespace du = doocore::io;
  //du::sdebug << "Checking ready for: " << element.id_abs() << du::endmsg;
  
  // Check if element is ready, if not, check if dependants are ready.
  if (!element->ready()) {
    const std::vector<std::string>& dep = element->dependants();
    bool allready = true;
    
    BOOST_FOREACH (std::string it_dep, dep) {
      it_element = elements_.find(it_dep);
      if (it_element == elements_.end()) {
        allready = false;
      } else {
        allready &= CheckReady(it_element->second->id_abs());
      }  
    } 
    if (allready) element->set_ready(true);
  }
  
  return element->ready();
}

RooAbsArg* Registrar::Register(RooWorkspace* ws, const std::string& element_name) {
  if (CheckReady(element_name)) {
    Element* element = elements_.find(element_name)->second;
    
    const std::vector<std::string>& dep_ids = element->dependants();
    
    // Iterate over dependants and check if dependants exist in elements_. 
    // If not, throw exception.
    // If yes, register them and invoke AddToWorkspace
    std::vector<RooAbsArg*> dep_elements;
    boost::ptr_map<std::string, Element>::iterator it_el;
    BOOST_FOREACH(std::string it_dep, dep_ids) {
      it_el = elements_.find(it_dep);
      if (it_el == elements_.end()) {
        throw UnexpectedException();
      } else {
        Element* dep_element = it_el->second;
        dep_elements.push_back(Register(ws, dep_element->id_abs()));
      }
    }
    return element->AddToWorkspace(ws, dep_elements);
  }
  return NULL;
}
  
} // namespace elements 
} // namespace blueprint 
} // namespace numerobis 
} // namespace builder 
} // namespace doofit
