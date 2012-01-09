#ifndef BUILDER_FITMODELBREWERY_RECIPE_DIMENSION_ABSDIMENSION_H_
#define BUILDER_FITMODELBREWERY_RECIPE_DIMENSION_ABSDIMENSION_H_

// from STL
#include <string>

namespace builder {
namespace fitmodelbrewery {

class AbsDimension {
 public:
  AbsDimension();
  virtual ~AbsDimension();
  
  
  virtual std::string GetName() const = 0;
  
  virtual void RegisterInRecipe() = 0;
  virtual void CheckForConsistency() = 0;
  
 protected:
  
 private:
  
};

} // namespace fitmodelbrewery
} // namespace builder


#endif // BUILDER_FITMODELBREWERY_RECIPE_DIMENSION_ABSDIMENSION_H_
