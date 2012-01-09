#include "Builder/FitModelBrewery/Recipe/Recipe.h"

// from project
#include "Builder/FitModelBrewery/Recipe/Dimension/DiscreteDimension.h"
#include "Builder/FitModelBrewery/Recipe/Dimension/RealDimension.h"

#include "Builder/FitModelBrewery/Recipe/Parameter/StandardParameter.h"
#include "Builder/FitModelBrewery/Recipe/Parameter/UnblindUniformParameter.h"
#include "Builder/FitModelBrewery/Recipe/Parameter/FormulaParameter.h"

namespace builder {
namespace fitmodelbrewery {

Recipe::Recipe() {
  
}

Recipe::~Recipe() {
  
}

// templating?
void Recipe::RegisterRecipeElement(const DiscreteDimension& dim_discrete) {
  // check if any element with the same name exists
  if (RecipeElementExists(dim_discrete.GetName())){
    throw ExcRecipeRegistrationFailed();
  }
}

void Recipe::RegisterRecipeElement(const RealDimension& dim_real) {
  // check if any element with the same name exists
  if (RecipeElementExists(dim_real.GetName())){
    throw ExcRecipeRegistrationFailed();
  }
}

void Recipe::RegisterRecipeElement(const StandardParameter& param_std) {
  // check if any element with the same name exists
  if (RecipeElementExists(param_std.GetName())){
    throw ExcRecipeRegistrationFailed();
  }
}
// 
// void Recipe::RegisterRecipeElement(const UnblindUniformParameter& param_bldunif) {
//   // check if any element with the same name exists
//   if (RecipeElementExists(param_bldunif.GetName())){
//     throw ExcRecipeRegistrationFailed();
//   }
// }
// 
// void Recipe::RegisterRecipeElement(const FormulaParameter& param_formula) {
//   // check if any element with the same name exists
//   if (RecipeElementExists(param_formula.GetName())){
//     throw ExcRecipeRegistrationFailed();
//   }
//}

bool Recipe::RecipeElementExists(const std::string& search_name) {
  if (dims_real_.find(search_name) != dims_real_.end()) return true;
  else if (dims_discrete_.find(search_name) != dims_discrete_.end()) return true;
  else if (params_std_.find(search_name) != params_std_.end()) return true;
  else if (params_bldunif_.find(search_name) != params_bldunif_.end()) return true;
  else if (params_formula_.find(search_name) != params_formula_.end()) return true;
  else return false;
}


} // namespace fitmodelbrewery
} // namespace builder
