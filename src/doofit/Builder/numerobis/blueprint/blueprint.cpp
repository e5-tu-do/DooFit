#include "doofit/Builder/numerobis/blueprint/blueprint.h"

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