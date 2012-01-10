#ifndef BUILDER_FITMODELBREWERY_RECIPE_CATEGORY_ABSCATEGORY_H
#define BUILDER_FITMODELBREWERY_RECIPE_CATEGORY_ABSCATEGORY_H

// from STL
#include <string>

namespace builder {
namespace fitmodelbrewery {

// forward declarations in namespace
class Recipe;

class AbsCategory {
 public:
  AbsCategory();
  virtual ~AbsCategory ();
  
  virtual std::string GetName() const = 0;
  virtual void RegisterInRecipe(Recipe& recipe) = 0;
  virtual void CheckForConsistency() = 0;
  
 protected:
  
 private:
  /* data */
};

} // namespace fitmodelbrewery 
} // namespace builder 

#endif // BUILDER_FITMODELBREWERY_RECIPE_CATEGORY_ABSCATEGORY_H
