#ifndef DOOFIT_BUILDER_NUMEROBIS_BLUEPRINT_ELEMENTS_FACTORY_H
#define DOOFIT_BUILDER_NUMEROBIS_BLUEPRINT_ELEMENTS_FACTORY_H

// from STL
#include <string>

// from boost
#include <boost/ptr_container/ptr_map.hpp>

// from project
#include "doofit/Builder/numerobis/blueprint/elements/registrar.h"
#include "doofit/Builder/numerobis/blueprint/elements/parambasic.h"
#include "doofit/Builder/numerobis/blueprint/elements/dimreal.h"
#include "doofit/Builder/numerobis/blueprint/elements/formula.h"

namespace doofit {
namespace builder {
namespace numerobis {
namespace blueprint {
namespace elements {

// forward declarations
class Registrar;

/** @class Factory
 *  @brief The Elements factory
 *
 *  In this first iteration this factory will merely function as a wrapper for
 *  Elements' constructors.
 */  
class Factory {
 public:
  /**
   *  @brief Constructor for this Factory with the Registrar to use
   *
   *  @param registrar the Registrar for registering the Elements
   */
  Factory(Registrar& registrar);
  
  /**
   *  @brief Destructor
   */
  virtual ~Factory ();
  
  /**
   *  @brief Assemble function for ParamBasic
   *
   *  @param id_rel relative id of the object
   *  @param id_abs absolute id of the object
   *  @param desc description of the object
   *  @param val_init initial value
   *  @param val_min lower limit
   *  @param val_max upper limit
   *  @param unit unit as string (optional)
   */
  void AssembleParamBasic(const std::string& id_rel, const std::string& id_abs, const std::string& desc, double val_init, double val_min, double val_max, const std::string& unit="") {
    registrar_.Declare(new ParamBasic(id_rel, id_abs, desc, val_init, val_min, val_max, unit));
  }
  
  /**
   *  @brief Assemble function for ParamBasic
   *
   *  @param id_rel relative id of the object
   *  @param id_abs absolute id of the object
   *  @param desc description of the object
   *  @param val_init initial value
   *  @param unit unit as string (optional)
   */
  void AssembleParamBasic(const std::string& id_rel, const std::string& id_abs, const std::string& desc, double val_init, const std::string& unit="") {    
    registrar_.Declare(new ParamBasic(id_rel, id_abs, desc, val_init, unit));
  }
  
  /**
   *  @brief Assemble function for DimReal
   *
   *  @param id_rel relative id of the object
   *  @param id_abs absolute id of the object
   *  @param desc description of the object
   *  @param val_min lower limit
   *  @param val_max upper limit
   *  @param unit unit as string (optional)
   */
  void AssembleDimReal(const std::string& id_rel, const std::string& id_abs, const std::string& desc, double val_min, double val_max, const std::string& unit) {
    registrar_.Declare(new DimReal(id_rel, id_abs, desc, val_min, val_max, unit));
  }
  
  /**
   *  @brief Assemble function for Formula
   *
   *  @param id_rel relative id of the object
   *  @param id_abs absolute id of the object
   *  @param formula formula
   *  @param elements elements to use in the formula (in same order as formula)
   */
  void AssembleFormula(const std::string& id_rel, const std::string& id_abs, const std::string& formula, const std::vector<std::string>& elements) {
    registrar_.Declare(new Formula(id_rel, id_abs, formula, elements));
  }
  
 protected:
  
 private:
  /**
   *  @brief Brief description
   */
  Registrar& registrar_;
};

} // namespace elements 
} // namespace blueprint 
} // namespace numerobis 
} // namespace builder 
} // namespace doofit 

#endif // DOOFIT_BUILDER_NUMEROBIS_BLUEPRINT_ELEMENTS_FACTORY_H
