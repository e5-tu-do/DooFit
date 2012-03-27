#ifndef DOOFIT_BUILDER_NUMEROBIS_BLUEPRINT_ElEMENTS_PARAMBASIC_H
#define DOOFIT_BUILDER_NUMEROBIS_BLUEPRINT_ElEMENTS_PARAMBASIC_H

// from STL
#include <string>

// from project
#include "doofit/Builder/numerobis/blueprint/elements/paramelement.h"
#include "doofit/Builder/numerobis/blueprint/elements/realvalelement.h"


// forward declarations - RooFit
class RooRealVar;

namespace doofit {
namespace builder {
namespace numerobis {
namespace blueprint {
namespace elements {
  
class ParamBasic : public ParamElement {
 public:
  ParamBasic();
  ParamBasic(const std::string& id_rel, const std::string& id_abs, const std::string& desc, double val_init, double val_min, double val_max, const std::string& unit="");  
  ParamBasic(const std::string& id_rel, const std::string& id_abs, const std::string& desc, double val_init, const std::string& unit="");  

  virtual ~ParamBasic();

 protected:
  /** @brief Create instance of this element as RooFit object
   *  
   *  Initialize the appropriate RooFit object and return it as RooAbsReal 
   *  pointer.
   *
   *  @param dependants the dependants to be used for initialization
   *  @return the temporary initialized object to be put onto a workspace
   */
  RooAbsArg* CreateTempRooObj(const std::vector<RooAbsArg*>& dependants = std::vector<RooAbsArg*>());

  /** @brief Create instance of this element as RooFit object
   *  
   *  Initialize the appropriate RooFit object and return it as RooAbsArg 
   *  pointer.
   *
   *  @param dependants the dependants to be used for initialization
   *  @return the temporary initialized object to be put onto a workspace
   */
  RooAbsArg* GetRooObjFromWorkspace(RooWorkspace* ws);


  /** @brief Getter for val_init_ */
  double val_init() const { return val_init_; }
  /** @brief Setter for val_init_ */
  void set_val_init(double val_init) { val_init_ = val_init; }

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
  double val_init_;
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

#endif // DOOFIT_BUILDER_NUMEROBIS_BLUEPRINT_ElEMENTS_PARAMBASIC_H
