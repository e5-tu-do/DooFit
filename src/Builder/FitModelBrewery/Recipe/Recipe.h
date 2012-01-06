#ifndef BUILDER_FITMODELBREWERY_RECIPE_RECIPE_H_
#define BUILDER_FITMODELBREWERY_RECIPE_RECIPE_H_

// from STL
#include <map>
#include <string>

namespace builder {
namespace fitmodelbrewery {

// forward declarations
class AbsDimension;
class AbsParameter;
class AbsPdf;

class Recipe {
 public:
  Recipe();
  ~Recipe();
  
  
 protected:
  
 private:
  std::map< std::string, AbsParameter* > parameters_;
};

} // namespace fitmodelbrewery
} // namespace builder


#endif // BUILDER_FITMODELBREWERY_RECIPE_RECIPE_H_
