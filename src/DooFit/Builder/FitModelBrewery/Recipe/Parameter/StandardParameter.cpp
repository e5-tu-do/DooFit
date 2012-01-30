#include "Builder/FitModelBrewery/Recipe/Parameter/StandardParameter.h"

// from project
#include "Builder/FitModelBrewery/Recipe/Recipe.h"

namespace builder {
namespace fitmodelbrewery {
  
StandardParameter::StandardParameter() 
    : AbsParameter(),
      name_("StandardParameterName"),
      desc_("StandardParameterDesc"),
      val_init_(0.),
      val_min_(0.),
      val_max_(1.),
      unit_("StandardParameterUnit")
{
  
}

StandardParameter::~StandardParameter() {
  
}
 
void StandardParameter::RegisterInRecipe(Recipe& recipe) {
  recipe.RegisterRecipeElement(*this);
}

void StandardParameter::CheckForConsistency() const {
  
}
  
} // namespace fitmodelbrewery 
} // namespace builder 