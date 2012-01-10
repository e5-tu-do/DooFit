#ifndef BUILDER_FITMODELBREWERY_RECIPE_DIMENSION_DISCRETEDIMENSION_H
#define BUILDER_FITMODELBREWERY_RECIPE_DIMENSION_DISCRETEDIMENSION_H

// from STL
#include <map>
#include <string>

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
  std::string name_;
  std::map<std::string,int> map_types_;
};

} // namespace fitmodelbrewery 
} // namespace builder 

#endif // BUILDER_FITMODELBREWERY_RECIPE_DIMENSION_DISCRETEDIMENSION_H
