#include "Builder/FitModelBrewery/Recipe/Dimension/RealDimension.h"

// from project
#include "Builder/FitModelBrewery/Recipe/Recipe.h"


namespace builder {
namespace fitmodelbrewery {
  
RealDimension::RealDimension() 
  : AbsDimension(),
    name_("StandardDimensionName"),
    desc_("StandardDimensionDesc"),
    val_min_(0.),
    val_max_(1.),
    unit_("StandardDimensionUnit")
{
  
}

RealDimension::~RealDimension() {
  
}

void RealDimension::RegisterInRecipe(Recipe& recipe) {
  recipe.RegisterRecipeElement(*this);
}

} // namespace fitmodelbrewery 
} // namespace builder 