#ifndef BUILDER_FITMODELBREWERY_RECIPE_DIMENSION_REALDIMENSION_H
#define BUILDER_FITMODELBREWERY_RECIPE_DIMENSION_REALDIMENSION_H

// from STL
#include <string>

// from project
#include "doofit/Builder/FitModelBrewery/Recipe/Dimension/AbsDimension.h"

namespace doofit {
namespace builder {
namespace fitmodelbrewery {

// forward declarations in namespace
class Recipe;

class RealDimension : public AbsDimension {
 public:
  RealDimension();
  virtual ~RealDimension();
  
  virtual std::string GetName() const { return name(); };
  virtual void RegisterInRecipe(Recipe& recipe);
  virtual void CheckForConsistency() const {};
  
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
  
  
  
 protected:
  
 private:
  std::string name_;
  std::string desc_;
  double val_min_;
  double val_max_;
  std::string unit_;
  
};

} // namespace fitmodelbrewery 
} // namespace builder
} // namespace doofit 

#endif // BUILDER_FITMODELBREWERY_RECIPE_DIMENSION_REALDIMENSION_H
