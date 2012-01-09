#ifndef BUILDER_FITMODELBREWERY_RECIPE_DIMENSION_DISCRETEDIMENSION_H
#define BUILDER_FITMODELBREWERY_RECIPE_DIMENSION_DISCRETEDIMENSION_H


// from project
#include "Builder/FitModelBrewery/Recipe/Dimension/AbsDimension.h"

namespace builder {
namespace fitmodelbrewery {

class DiscreteDimension : public AbsDimension {
 public:
  DiscreteDimension();
  virtual ~DiscreteDimension ();
  
  virtual std::string GetName() const;
  
  
 protected:
  
 private:
  /* data */
};

} // namespace fitmodelbrewery 
} // namespace builder 

#endif // BUILDER_FITMODELBREWERY_RECIPE_DIMENSION_DISCRETEDIMENSION_H
