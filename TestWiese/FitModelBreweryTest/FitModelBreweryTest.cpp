
#include "Builder/FitModelBrewery/Recipe/Recipe.h"

#include "Builder/FitModelBrewery/Recipe/Dimension/RealDimension.h"
#include "Builder/FitModelBrewery/Recipe/Parameter/StandardParameter.h"

using namespace builder::fitmodelbrewery;

int main() {
  Recipe recipe;
  StandardParameter parMass;
  parMass.set_name("heinz");
  RealDimension parProptime;
  parProptime.set_name("heinz");
  parMass.RegisterInRecipe(recipe);
  parProptime.RegisterInRecipe(recipe);
  return 0;
}
