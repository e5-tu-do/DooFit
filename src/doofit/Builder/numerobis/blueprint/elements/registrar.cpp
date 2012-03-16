#include "doofit/Builder/numerobis/blueprint/elements/registrar.h"

// from STL
#include <string>
#include <map>

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


Registrar::Registrar(){
  
}

Registrar::~Registrar() {
  
}

void Registrar::Declare(Element &element) {
  elements_[element.id_abs()] = &element;
}
  
void Registrar::Print() const {
  namespace du = doofit::utils;
  du::sinfo.Ruler();
  du::sinfo << "Registrar contents:" << du::endmsg;
  du::sinfo.increment_indent(2);
  for (std::map<std::string, Element*>::const_iterator it=elements_.begin();
       it!=elements_.end(); ++it) {
    du::sinfo << *(*it).second << du::endmsg;
  }
  du::sinfo.increment_indent(-2);
}
  
bool Registrar::CheckReady(Element& element) {
  namespace du = doofit::utils;
  //du::sdebug << "Checking ready for: " << element.id_abs() << du::endmsg;
  
  if (!element.ready()) {
    const std::vector<std::string>& dep = element.dependants();
    bool allready = true;
    std::map<std::string, Element*>::iterator it_el;
    for (std::vector<std::string>::const_iterator it_dep=dep.begin(); it_dep!=dep.end(); ++it_dep) {
      //du::sdebug << "Checking ready for dependant: " << *it_dep << du::endmsg;
      it_el = elements_.find(*it_dep);
      if (it_el == elements_.end()) {
        allready = false;
      } else {
        allready &= CheckReady(*it_el->second);
      }
    }
    
    if (allready) element.set_ready(true);
  }
  
  return element.ready();
}

RooAbsArg* Registrar::Register(RooWorkspace* ws, Element &element) {
  if (CheckReady(element)) {
    const std::vector<std::string>& dep_ids = element.dependants();
    std::vector<RooAbsArg*> dep_elements;
    std::map<std::string, Element*>::iterator it_el;
    for (std::vector<std::string>::const_iterator it_dep=dep_ids.begin(); 
         it_dep!=dep_ids.end(); ++it_dep) {
      it_el = elements_.find(*it_dep);
      if (it_el == elements_.end()) {
        throw UnexpectedException();
      } else {
        Element& dep_element = *it_el->second;
        dep_elements.push_back(Register(ws, dep_element));
      }
    }
    
    return element.AddToWorkspace(ws, dep_elements);
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