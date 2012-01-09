#ifndef BUILDER_FITMODELBREWERY_RECIPE_DIMENSION_REALDIMENSION_H
#define BUILDER_FITMODELBREWERY_RECIPE_DIMENSION_REALDIMENSION_H

// from project
#include "Builder/FitModelBrewery/Recipe/Dimension/AbsDimension.h"

namespace builder {
namespace fitmodelbrewery {

class RealDimension : public AbsDimension {
 public:
  RealDimension();
  virtual ~RealDimension();
  
  virtual std::string GetName() const;
  
  void RegisterInRecipe();
  void CheckForConsistency();
  
 protected:
  
 private:
  //std::string 
};

} // namespace fitmodelbrewery 
} // namespace builder 

#endif // BUILDER_FITMODELBREWERY_RECIPE_DIMENSION_REALDIMENSION_H
