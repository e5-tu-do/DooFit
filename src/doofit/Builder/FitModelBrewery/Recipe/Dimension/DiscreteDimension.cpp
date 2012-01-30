#include "doofit/Builder/FitModelBrewery/Recipe/Dimension/DiscreteDimension.h"

// from boost
#include <boost/foreach.hpp>

// from project
#include "doofit/Builder/FitModelBrewery/Recipe/Recipe.h"

using namespace std;
using namespace boost;

namespace doofit {
namespace builder {
namespace fitmodelbrewery {

DiscreteDimension::DiscreteDimension() 
  : AbsDimension(),
    name_("DiscreteDimensionName"),
    desc_("DiscreteDimensionDesc"),
    map_types_()
{

}

DiscreteDimension::~DiscreteDimension() {

}

void DiscreteDimension::RegisterInRecipe(Recipe& recipe) {
  recipe.RegisterRecipeElement(*this);
}

void DiscreteDimension::AddType(string type_name, int type_value) {
  // Check if value already exists
  typedef pair<string,int> type_pair;
  BOOST_FOREACH( type_pair type , map_types_ ){
    if ( type.second == type_value ){
      throw ExcDiscreteDimensionTypeExists();
    }
  }
  
  // Try to add new type
  pair<map<string, int >::iterator,bool> ret;
  ret = map_types_.insert(pair<string,int>(type_name, type_value));
  // Check if key already exists in map
  if (ret.second == false){
    throw ExcDiscreteDimensionTypeExists();
  }
}


} // namespace fitmodelbrewery 
} // namespace builder
} // namespace doofit
