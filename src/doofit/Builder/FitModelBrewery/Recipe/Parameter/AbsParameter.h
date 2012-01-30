#ifndef BUILDER_FITMODELBREWERY_RECIPE_PARAMETER_ABSPARAMETER_H_
#define BUILDER_FITMODELBREWERY_RECIPE_PARAMETER_ABSPARAMETER_H_

// from STL
#include <string>

namespace doofit {
namespace builder {
namespace fitmodelbrewery {

// forward declarations in namespace
class Recipe;

class AbsParameter {
 public:
  explicit AbsParameter();
  virtual ~AbsParameter();
  
  virtual std::string GetName() const = 0;

  virtual void CheckForConsistency() const = 0;
  virtual void RegisterInRecipe(Recipe& recipe) = 0;

  
 protected:
  
 private:
  
};

} // namespace fitmodelbrewery
} // namespace builder
} // namespace doofit


#endif // BUILDER_FITMODELBREWERY_RECIPE_PARAMETER_ABSPARAMETER_H_
