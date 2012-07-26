#include "doofit/Builder/FitModelBrewery/Recipe/Recipe.h"

// from STL
#include <utility>

// from project
/// from utils
#include "doofit/utils//MsgStream.h"



namespace doofit {
namespace builder {
namespace fitmodelbrewery {

  using namespace std;
  
Recipe::Recipe() 
    : map_dims_real_(),
      map_dims_discrete_(),
      map_cats_basic_(),
      map_cats_super_(),    
      map_params_std_(),
      //map_params_bldunif_(),
      //map_params_formula_(),
      is_ready(false),
      is_consistent(false)
{
  
}

Recipe::~Recipe() {
  
}

// templating?
void Recipe::RegisterRecipeElement(const RealDimension& dim_real) {
  // check if any element with the same name exists
  if (RecipeElementExists(dim_real.GetName())){
    throw ExcRecipeRegistrationFailed();
  }
  else {
    // name is unique and element can be added to associated map
    pair<string,RealDimension> dim_real_entry(dim_real.GetName(), RealDimension(dim_real));
    map_dims_real_.insert(dim_real_entry);
  }
}

void Recipe::RegisterRecipeElement(const DiscreteDimension& dim_discrete) {
  // check if any element with the same name exists
  if (RecipeElementExists(dim_discrete.GetName())){
    throw ExcRecipeRegistrationFailed();
  }
  else {
    // name is unique and element can be added to associated map
    pair<string,DiscreteDimension> dim_discrete_entry(dim_discrete.GetName(), DiscreteDimension(dim_discrete));
    map_dims_discrete_.insert(dim_discrete_entry);
  }
}

void Recipe::RegisterRecipeElement(const BasicCategory& cat_basic) {
  // check if any element with the same name exists
  if (RecipeElementExists(cat_basic.GetName())){
    throw ExcRecipeRegistrationFailed();
  }
  else {
    // name is unique and element can be added to associated map
    pair<string,BasicCategory> cat_basic_entry(cat_basic.GetName(), BasicCategory(cat_basic));
    map_cats_basic_.insert(cat_basic_entry);
  }
}

void Recipe::RegisterRecipeElement(const SuperCategory& cat_super) {
  // check if any element with the same name exists
  if (RecipeElementExists(cat_super.GetName())){
    throw ExcRecipeRegistrationFailed();
  }
  else {
    // name is unique and element can be added to associated map
    pair<string,SuperCategory> cat_super_entry(cat_super.GetName(), SuperCategory(cat_super));
    map_cats_super_.insert(cat_super_entry);
  }
}

void Recipe::RegisterRecipeElement(const StandardParameter& param_std) {
  // check if any element with the same name exists
  if (RecipeElementExists(param_std.GetName())){
    throw ExcRecipeRegistrationFailed();
  }
  else {
    // name is unique and element can be added to associated map
    pair<string,StandardParameter> param_std_entry(param_std.GetName(), StandardParameter(param_std));
    map_params_std_.insert(param_std_entry);
  }
}

bool Recipe::RecipeElementExists(const std::string& search_name) {
  // check if name of recipe element is unique
  if (map_dims_real_.find(search_name) != map_dims_real_.end()) return true;
  else if (map_dims_discrete_.find(search_name) != map_dims_discrete_.end()) return true;
  else if (map_cats_basic_.find(search_name) != map_cats_basic_.end()) return true;
  else if (map_cats_super_.find(search_name) != map_cats_super_.end()) return true;
  else if (map_params_std_.find(search_name) != map_params_std_.end()) return true;
//  else if (map_params_bldunif_.find(search_name) != map_params_bldunif_.end()) return true;
//  else if (map_params_formula_.find(search_name) != map_params_formula_.end()) return true;
  else return false;
}


} // namespace fitmodelbrewery
} // namespace builder
} // namespace doofit
