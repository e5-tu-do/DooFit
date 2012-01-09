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
  
  /** Setter and getter for name_ */
  std::string name() const { return name_; }
  void set_name(const std::string& name) { name_ = name; } 
  
  
 protected:
  std::string name_;
 private:
  
};

} // namespace fitmodelbrewery
} // namespace builder


#endif // BUILDER_FITMODELBREWERY_RECIPE_PARAMETER_ABSPARAMETER_H_
