#include "DooFit/Builder/FitModelBrewery/Recipe/Category/SuperCategory.h"

// from project
#include "DooFit/Builder/FitModelBrewery/Recipe/Recipe.h"

using namespace std;
using namespace boost;

namespace DooFit {
namespace builder {
namespace fitmodelbrewery {

SuperCategory::SuperCategory() 
  : AbsCategory(),
    name_("SuperCategoryName"),
    desc_("SuperCategoryDesc"),
    vector_cats_basic_()
{

}

SuperCategory::~SuperCategory() {

}

void SuperCategory::RegisterInRecipe(Recipe& recipe) {
  recipe.RegisterRecipeElement(*this);
}

void SuperCategory::AddBasicCategory(const std::string& basic_cat_name) {
  vector_cats_basic_.push_back(basic_cat_name);
}


} // namespace fitmodelbrewery 
} // namespace builder
} // namespace DooFit
