#ifndef BUILDER_FITMODELBREWERY_RECIPE_PDF_ABSDIMPDF_H_
#define BUILDER_FITMODELBREWERY_RECIPE_PDF_ABSDIMPDF_H_

// from STL
#include <string>

namespace builder {
namespace fitmodelbrewery {

// forward declarations in namespace
class Recipe;

class AbsDimPdf {
 public:
  explicit AbsDimPdf();
  virtual ~AbsDimPdf();
  
  virtual std::string GetName() const = 0;

  virtual void CheckForConsistency() const = 0;
  virtual void RegisterInRecipe(Recipe& recipe) = 0;

  
 protected:
  
 private:
  
};

} // namespace fitmodelbrewery
} // namespace builder


#endif // BUILDER_FITMODELBREWERY_RECIPE_PDF_ABSDIMPDF_H_
