#ifndef BUILDER_FITMODELBREWERY_RECIPE_PARAMETER_STANDARDPARAMETER_H
#define BUILDER_FITMODELBREWERY_RECIPE_PARAMETER_STANDARDPARAMETER_H

// from STL
#include <string>

// from project
#include "Builder/FitModelBrewery/Recipe/Parameter/AbsParameter.h"

namespace builder {
namespace fitmodelbrewery {

//forward declarations in namespace
class Recipe;

/** @class StandardParameter
 *  @brief This class represents the standard parameter type to be used in pdfs.        
 *  
 *  © Copyright 2012  - Julian Wishahi. All Rights Reserved.
 *
 *  @author Julian Wishahi 
 *  @author $Author$
 *  @date 2012-01-10
 *  @date $Date$
 *  @version $Rev$
 */
class StandardParameter : public AbsParameter {
 public:
  explicit StandardParameter();
  ~StandardParameter();
  
  virtual std::string GetName() const{ return name(); }
  
  virtual void RegisterInRecipe(Recipe& recipe);
  virtual void CheckForConsistency() const;
  
  /** Setter and getter for name_ */
  std::string name() const { return name_; }
  void set_name(const std::string& name) { name_ = name; } 
  
  /** Setter and getter for desc_ */
  std::string desc() const { return desc_; }
  void set_desc(const std::string& desc) { desc_ = desc; } 
  
  /** Setter and getter for val_min_ */
  double val_min() const { return val_min_; }
  void set_val_min(const double val_min) { val_min_ = val_min; } 
  
  /** Setter and getter for val_max_ */
  double val_max() const { return val_max_; }
  void set_val_max(const double val_max) { val_max_ = val_max; } 
  
  /** Setter and getter for unit_ */
  std::string unit() const { return unit_; }
  void set_unit(const std::string& unit) { unit_ = unit; }
  
  
 private:
  std::string name_;
  std::string desc_;
  double      val_init_;
  double      val_min_;
  double      val_max_;
  std::string unit_;
};

} // namespace fitmodelbrewery 
} // namespace builder 

#endif // BUILDER_FITMODELBREWERY_RECIPE_PARAMETER_STANDARDPARAMETER_H
