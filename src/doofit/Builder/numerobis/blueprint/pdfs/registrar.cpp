#include "doofit/Builder/numerobis/blueprint/pdfs/registrar.h"

// from STL
#include <string>
#include <map>

// from BOOST
#include <boost/foreach.hpp>

// from RooFit

// from project
#include "doofit/utils/MsgStream.h"
#include "doofit/Builder/numerobis/blueprint/elements/element.h"
#include "doofit/Builder/numerobis/blueprint/pdfs/pdf.h"

// forward decalaration
class RooWorkspace;

namespace doofit {
namespace builder {
namespace numerobis {
namespace blueprint {
namespace pdfs {


Registrar::Registrar()
    : pdfs_()
{
  
}

Registrar::~Registrar() {
  
}

void Registrar::Declare(Pdf* pdf) {
  std::string temp_key = pdf->id_abs();
  pdfs_.insert(temp_key, pdf);
}
  
void Registrar::Print() const {
  namespace du = doofit::utils;
  du::sinfo.Ruler();
  du::sinfo << "Registrar contents:" << du::endmsg;
  du::sinfo.increment_indent(2);
  for (boost::ptr_map<std::string, Pdf>::const_iterator it=pdfs_.begin();
       it!=pdfs_.end(); ++it) {
    du::sinfo << *(*it).second << du::endmsg;
  }
  du::sinfo.increment_indent(-2);
}
  
bool Registrar::CheckReady(const std::string& pdf_name) {
//  boost::ptr_map<std::string, Element>::iterator it_element = elements_.find(element_name);
//  if (it_element == elements_.end()) return false;
//  
//  Element* element = it_element->second;
//  
//  namespace du = doofit::utils;
//  //du::sdebug << "Checking ready for: " << element.id_abs() << du::endmsg;
//  
//  // Check if element is ready, if not, check if dependants are ready.
//  if (!element->ready()) {
//    const std::vector<std::string>& dep = element->dependants();
//    bool allready = true;
//    
//    boost::ptr_map<std::string, Element>::iterator it_el;
//    BOOST_FOREACH (std::string it_dep, dep) {
//      it_el = elements_.find(it_dep);
//      if (it_el == elements_.end()) {
//        allready = false;
//      } else {
//        allready &= CheckReady(it_el->second->id_abs());
//      }  
//    } 
//    if (allready) element->set_ready(true);
//  }
//  
//  return element->ready();
}

RooAbsArg* Registrar::Register(RooWorkspace* ws, const std::string& pdf_name) {
//  if (CheckReady(element_name)) {
//    Element* element = elements_.find(element_name)->second;
//    
//    const std::vector<std::string>& dep_ids = element->dependants();
//    
//    // Iterate over dependants and check if dependants exist in elements_. 
//    // If not, throw exception.
//    // If yes, register them and invoke AddToWorkspace
//    std::vector<RooAbsArg*> dep_elements;
//    boost::ptr_map<std::string, Element>::iterator it_el;
//    BOOST_FOREACH(std::string it_dep, dep_ids) {
//      it_el = elements_.find(it_dep);
//      if (it_el == elements_.end()) {
//        throw UnexpectedException();
//      } else {
//        Element* dep_element = it_el->second;
//        dep_elements.push_back(Register(ws, dep_element->id_abs()));
//      }
//    }
//    return element->AddToWorkspace(ws, dep_elements);
//  }
//  return NULL;
}

} // namespace pdfs 
} // namespace blueprint 
} // namespace numerobis 
} // namespace builder 
} // namespace doofit
