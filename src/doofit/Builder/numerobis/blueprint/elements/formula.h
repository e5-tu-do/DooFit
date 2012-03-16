#ifndef DOOFIT_BUILDER_NUMEROBIS_BLUEPRINT_ElEMENTS_FORMULA_H
#define DOOFIT_BUILDER_NUMEROBIS_BLUEPRINT_ElEMENTS_FORMULA_H

// from STL
#include <string>
#include <vector>

// from project
#include "doofit/Builder/numerobis/blueprint/elements/realvalelement.h"


// forward declarations - RooFit
class RooFormulaVar;

namespace doofit {
namespace builder {
namespace numerobis {
namespace blueprint {
namespace elements {

// forward declarations - Numerobis...
class Registrar;
  
class Formula : public RealValElement {
 public:
  Formula();
  Formula(const std::string& id_rel, const std::string& id_abs, const std::string& formula, std::vector<std::string> elements);
  
  virtual ~Formula();
  
 protected:
  /** @brief Create instance of this element as RooFit object
   *  
   *  Initialize the appropriate RooFit object and return it as RooAbsReal 
   *  pointer.
   *
   *  @param dependants the dependants to be used for initialization
   *  @return the temporary initialized object to be put onto a workspace
   */
  RooAbsReal* CreateTempRooObj(std::vector<RooAbsArg*> dependants = std::vector<RooAbsArg*>());
  
 private:
  /**
   *  @brief Formula to use for calculating our value
   */
  std::string formula_;
  
  //RooFormulaVar* roo_obj_;
};


} // namespace elements
} // namespace blueprint
} // namespace numerobis
} // namespace builder
} // namespace doofit


#endif // DOOFIT_BUILDER_NUMEROBIS_BLUEPRINT_ElEMENTS_FORMULA_H