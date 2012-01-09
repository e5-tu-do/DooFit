#ifndef BUILDER_FITMODELBREWERY_RECIPE_RECIPE_H_
#define BUILDER_FITMODELBREWERY_RECIPE_RECIPE_H_

// from STL
#include <map>
#include <string>

namespace builder {
namespace fitmodelbrewery {

// forward declarations
class AbsDimension;
class DiscreteDimension;
class RealDimension;

class AbsParameter;
class StandardParameter;
class UnblindUniformParameter;
class FormulaParameter;

class IsEqualParameter;

class AbsPdf;

class Recipe {
 public:
  Recipe();
  ~Recipe();
  
  
 protected:
  
 private:
  std::map< std::string, RealDimension* > dims_real_;
    
  std::map< std::string, DiscreteDimension* > dims_discrete_;
  
  std::map< std::string, StandardParameter* > params_std_;
  std::map< std::string, UnblindUniformParameter* > params_bldunif_;
  std::map< std::string, FormulaParameter* > params_formula_;
  std::map< std::string, IsEqualParameter* > params_isequal_;
  
  //std::map< std::string, AbsResolution* > resols_;
  //std::map< std::string, AbsPdf* > pdfs_;
  
};

} // namespace fitmodelbrewery
} // namespace builder


#endif // BUILDER_FITMODELBREWERY_RECIPE_RECIPE_H_
