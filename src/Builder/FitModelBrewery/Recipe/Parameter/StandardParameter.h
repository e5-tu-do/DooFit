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

class StandardParameter : public AbsParameter {
 public:
  StandardParameter( const Recipe& recipe );
  ~StandardParameter();
  
  virtual std::string GetName() const{ return name_; }
  
  /** @brief Setters and Getters */
  std::string name() const { return name_; }
  void set_name(const std::string& name) { name_ = name; }
  
  std::string desc() const { return desc_; }
  void set_desc(const std::string& desc) { desc_ = desc; }
  
  double init_val() const { return init_val_; }
  void set_init_val(double init_val) { init_val_ = init_val; } 
  
  double min_val() const { return min_val_; }
  void set_min_val(double min_val) { min_val_ = min_val; } 
  
  double max_val() const { return max_val_; }
  void set_max_val(double max_val) { max_val_ = max_val; }

  std::string unit() const { return unit_; }
  void set_unit(const std::string& unit) { unit_ = unit; }

 private:
  std::string name_;
  std::string desc_;
  double      init_val_;
  double      min_val_;
  double      max_val_;
  std::string unit_;
};

} // namespace fitmodelbrewery 
} // namespace builder 

#endif // BUILDER_FITMODELBREWERY_RECIPE_PARAMETER_STANDARDPARAMETER_H
