#include "doofit/Builder/numerobis/blueprint/elements/registrar.h"

// from STL
#include <string>
#include <map>

// from BOOST
#include <boost/foreach.hpp>

// from RooFit
#include "RooArgList.h"

// from project
#include "doofit/utils//MsgStream.h"
#include "doofit/Builder/numerobis/blueprint/elements/element.h"

// forward decalaration
class RooWorkspace;

namespace doofit {
namespace builder {
namespace numerobis {
namespace blueprint {
namespace elements {


Registrar::Registrar()
    : elements_()
{
  
}

Registrar::~Registrar() {
  
}

void Registrar::Declare(Element* const element) {
  std::string temp_key = element->id_abs();
  elements_.insert(temp_key,  element);
}
  
void Registrar::Print() const {
  namespace du = doofit::utils;
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
  
  namespace du = doofit::utils;
  //du::sdebug << "Checking ready for: " << element.id_abs() << du::endmsg;
  
  // Check if element is ready, if not, check if dependants are ready.
  if (!element->ready()) {
    const std::vector<std::string>& dep = element->dependants();
    bool allready = true;
    
    boost::ptr_map<std::string, Element>::iterator it_el;
    BOOST_FOREACH (std::string it_dep, dep) {
      it_el = elements_.find(it_dep);
      if (it_el == elements_.end()) {
        allready = false;
      } else {
        allready &= CheckReady(it_el->second->id_abs());
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
  
RooArgList VectorToArgList(std::vector<RooAbsArg*> vec) {
  RooArgList list; 
  for (std::vector<RooAbsArg*>::const_iterator it=vec.begin(); 
       it!=vec.end(); ++it) {
    list.add(**it, true);
  }
  return list;
}
  
} // namespace elements 
} // namespace blueprint 
} // namespace numerobis 
} // namespace builder 
} // namespace doofit
