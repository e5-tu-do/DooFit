#include "doofit/builder/numerobis/blueprint/pdfs/registrar.h"

// from STL
#include <string>
#include <map>

// from BOOST
#include <boost/foreach.hpp>

// from RooFit
#include "RooAbsPdf.h"

// from project
#include "doocore/io/MsgStream.h"
#include "doofit/builder/numerobis/blueprint/elements/element.h"
#include "doofit/builder/numerobis/blueprint/elements/registrar.h"
#include "doofit/builder/numerobis/blueprint/pdfs/pdf.h"

// forward decalaration
class RooWorkspace;

namespace doofit {
namespace builder {
namespace numerobis {
namespace blueprint {
namespace pdfs {


Registrar::Registrar(doofit::builder::numerobis::blueprint::elements::Registrar& element_registrar)
  : pdfs_()
  , element_registrar_(element_registrar)
{
  
}

Registrar::~Registrar() {
  
}

void Registrar::Declare(Pdf* pdf) {
  // Add a consistency check, if name is already 
  std::string temp_key = pdf->id_abs();
  pdfs_.insert(temp_key, pdf);
}
  
void Registrar::Print() const {
  namespace du = doocore::io;
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
  boost::ptr_map<std::string, Pdf>::iterator it_pdf = pdfs_.find(pdf_name);
  if (it_pdf == pdfs_.end()) return false;
  
  Pdf* pdf = it_pdf->second;
  
  namespace du = doocore::io;
  namespace el = doofit::builder::numerobis::blueprint::elements;
  //du::sdebug << "Checking ready for: " << pdf.id_abs() << du::endmsg;
  
  // Check if PDF is ready, if not, check if dependants are ready.
  if (!pdf->ready()) {
    const std::map<std::string, std::string>& dep = pdf->dependants();
    bool allready = true;
  
    for (std::map<std::string, std::string>::const_iterator it_dep=dep.begin();
         it_dep != dep.end(); ++it_dep) {
      // first check if the dependant is available as element
      // if not, try as pdf
      if (!element_registrar_.CheckReady(it_dep->second)) {
        it_pdf = pdfs_.find(it_dep->second);
        if (it_pdf == pdfs_.end()) {
          allready = false;
        } else {
          allready &= CheckReady(it_pdf->second->id_abs());
        }
      }
    }
    if (allready) pdf->set_ready(true);
  }
  return pdf->ready();  
}

RooAbsArg* Registrar::Register(RooWorkspace* ws, const std::string& pdf_name) {
  if (CheckReady(pdf_name)) {
    Pdf* pdf = pdfs_.find(pdf_name)->second;
    const std::map<std::string, std::string>& dep = pdf->dependants();
 
    // Iterate over dependants and check if dependants exist in elements 
    // or in pdfs_. 
    // If not, throw exception.
    // If yes, register them and invoke AddToWorkspace
    std::map<std::string, RooAbsArg*> dep_rooobj;
    for (std::map<std::string, std::string>::const_iterator it_dep=dep.begin();
         it_dep != dep.end(); ++it_dep) {
      if (element_registrar_.CheckReady(it_dep->second)) {
        // dependant is element
        dep_rooobj[it_dep->first] = element_registrar_.Register(ws, it_dep->second);
      } else if (CheckReady(it_dep->second)) {
        // dependant is PDF
        dep_rooobj[it_dep->first] = Register(ws, it_dep->second);
      } else {
        throw UnexpectedException();
      }
    }
    return pdf->AddToWorkspace(ws, dep_rooobj);
  }
  return NULL;
}

} // namespace pdfs 
} // namespace blueprint 
} // namespace numerobis 
} // namespace builder 
} // namespace doofit
