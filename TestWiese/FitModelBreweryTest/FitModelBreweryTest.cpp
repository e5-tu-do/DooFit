// from STL
#include <iostream>

// from project
#include "Builder/FitModelBrewery/Recipe/Recipe.h"

#include "Builder/FitModelBrewery/Recipe/Dimension/RealDimension.h"
#include "Builder/FitModelBrewery/Recipe/Dimension/DiscreteDimension.h"

#include "Builder/FitModelBrewery/Recipe/Category/BasicCategory.h"
#include "Builder/FitModelBrewery/Recipe/Category/SuperCategory.h"

#include "Builder/FitModelBrewery/Recipe/Parameter/StandardParameter.h"

using namespace std;
using namespace builder::fitmodelbrewery;

int main() {
  Recipe recipe;
  
  RealDimension varProptime;
  varProptime.set_name("kunz");
  varProptime.set_desc("kunz param");
  varProptime.RegisterInRecipe(recipe);
  
  DiscreteDimension varTag;
  varTag.set_name("varTag");
  varTag.set_desc("d");
  varTag.AddType("B0",1);
  varTag.AddType("Untagged",0);
  varTag.AddType("B0bar",-1);
  varTag.RegisterInRecipe(recipe);
  
  StandardParameter parMass;
  parMass.set_name("heinz");
  parMass.set_desc("heinz param");
  parMass.RegisterInRecipe(recipe);
    
  BasicCategory catTagged;
  catTagged.set_name("catTagged");
  catTagged.set_desc("Indicates if tagged.");
  catTagged.AddType("Tagged",1);
  catTagged.AddType("Untagged",0);
  catTagged.RegisterInRecipe(recipe);
  
  BasicCategory catTrigger;
  catTrigger.set_name("catTrigger");
  catTrigger.set_desc("Indicates if triggered by biased or unbiased.");
  catTrigger.AddType("Biased",1);
  catTrigger.AddType("Unbiased",0);
  catTrigger.RegisterInRecipe(recipe);
  
  SuperCategory catTrigTag;
  catTrigTag.set_name("catTrigTag");
  catTrigTag.set_desc("SuperCat");
  catTrigTag.AddBasicCategory("catTrigger");
  catTrigTag.AddBasicCategory("catTagged");
  catTrigTag.RegisterInRecipe(recipe);
  
  return 0;
}
