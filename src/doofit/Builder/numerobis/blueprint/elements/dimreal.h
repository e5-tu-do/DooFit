#ifndef DOOFIT_BUILDER_NUMEROBIS_BLUEPRINT_ElEMENTS_DIMREAL_H
#define DOOFIT_BUILDER_NUMEROBIS_BLUEPRINT_ElEMENTS_DIMREAL_H

// from STL
#include <string>

// from project
#include "doofit/Builder/numerobis/blueprint/elements/dimelement.h"
#include "doofit/Builder/numerobis/blueprint/elements/realvalelement.h"


// forward declarations - RooFit
class RooRealVar;

namespace doofit {
namespace builder {
namespace numerobis {
namespace blueprint {
namespace elements {

class DimReal : public DimElement, public RealValElement {
 public:
  DimReal();
  DimReal(const std::string& id_rel, const std::string& id_abs, const std::string& desc, double val_min, double val_max, const std::string& unit);
  
  virtual ~DimReal();
  
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
  
  /** @brief Getter for val_min_ */
  double val_min() const { return val_min_; }
  /** @brief Setter for val_min_ */
  void set_val_min(double val_min) { val_min_ = val_min; } 
  
  /** @brief Getter for val_max_ */
  double val_max() const { return val_max_; }
  /** @brief Setter for val_max_ */
  void set_val_max(double val_max) { val_max_ = val_max; } 
  
  /** @brief Getter for unit_ */
  std::string unit() const { return unit_; }
  /** @brief Setter for unit_ */
  void set_unit(const std::string& unit) { unit_ = unit; }
  
  
  
  
 private:
  double val_min_;
  double val_max_;
  std::string unit_;
  
  //RooRealVar* roo_obj_;
};


} // namespace elements
} // namespace blueprint
} // namespace numerobis
} // namespace builder
} // namespace doofit


#endif // DOOFIT_BUILDER_NUMEROBIS_BLUEPRINT_ElEMENTS_DIMREAL_H
