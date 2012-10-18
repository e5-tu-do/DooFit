#include "doofit/builder/FitModelBrewery/Recipe/Category/BasicCategory.h"

// from boost
#include <boost/foreach.hpp>

// from project
#include "doofit/builder/FitModelBrewery/Recipe/Recipe.h"

using namespace std;
using namespace boost;

namespace doofit {
namespace builder {
namespace fitmodelbrewery {

BasicCategory::BasicCategory() 
  : AbsCategory(),
    name_("BasicCategoryName"),
    desc_("BasicCategoryDesc"),
    map_types_()
{

}

BasicCategory::~BasicCategory() {

}

void BasicCategory::RegisterInRecipe(Recipe& recipe) {
  recipe.RegisterRecipeElement(*this);
}

void BasicCategory::AddType(const string& type_name, int type_value) {
  // Check if value already exists
  typedef pair<string,int> type_pair;
  BOOST_FOREACH( type_pair type , map_types_ ){
    if ( type.second == type_value ){
      throw ExcBasicCategoryTypeExists();
    }
  }
  // Try to add new type
  pair<map<string, int >::iterator,bool> ret;
  ret = map_types_.insert(pair<string,int>(type_name, type_value));
  // Check if key already exists in map
  if (ret.second == false){
    throw ExcBasicCategoryTypeExists();
  }
}


} // namespace fitmodelbrewery 
} // namespace builder
} // namespace doofit
