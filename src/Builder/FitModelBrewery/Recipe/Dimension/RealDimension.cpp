#include "Builder/FitModelBrewery/Recipe/Dimension/RealDimension.h"

// from project
#include "Builder/FitModelBrewery/Recipe/Recipe.h"


namespace builder {
namespace fitmodelbrewery {
  
RealDimension::RealDimension() {
  
}

RealDimension::~RealDimension() {
  
}

void RealDimension::RegisterInRecipe(Recipe& recipe) {
  recipe.RegisterRecipeElement(*this);
}

} // namespace fitmodelbrewery 
} // namespace builder 