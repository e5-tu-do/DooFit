#ifndef BUILDER_FITMODELBREWERY_RECIPE_PARAMETER_ABSPARAMETER_H_
#define BUILDER_FITMODELBREWERY_RECIPE_PARAMETER_ABSPARAMETER_H_

// from STL
#include <string>

namespace builder {
namespace fitmodelbrewery {

// forward declarations in namespace
class Recipe;

class AbsParameter {
 public:
  AbsParameter( const Recipe& recipe );
  virtual ~AbsParameter();
  
  virtual std::string GetName() const = 0;

  
 protected:
  
 private:
  
};

} // namespace fitmodelbrewery
} // namespace builder


#endif // BUILDER_FITMODELBREWERY_RECIPE_PARAMETER_ABSPARAMETER_H_
