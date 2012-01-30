#ifndef BUILDER_FITMODELBREWERY_RECIPE_PARAMETER_PARAMETERFACTORY_H
#define BUILDER_FITMODELBREWERY_RECIPE_PARAMETER_PARAMETERFACTORY_H

// from STL
#include <string>


namespace doofit {
namespace builder {
namespace fitmodelbrewery {

// forward declarations in namespace
class AbsParameter;
class Recipe;

/** @class ParameterFactory
 *  @brief Factory class for creation of AbsParameter objects
 *
 *  This class provides a factory for AbsParameter objects. The 
 *  central CreateParameter function takes a type definition to
 *  match it to the known parameter types. It returns a pointer
 *  to a new instance of the matching parameter class.
 */

class ParameterFactory {
 public:
  ParameterFactory();
  ~ParameterFactory();
  
  /** @brief Returns pointer to derived class of @AbsParameter.
   *
   *  The type of the derived class to be returned is extracted by   
   *  interpreting the provided string.
   *  Supported type keywords are:
   *    - "Standard" for @StandardParameter
   *    - "IsEqual" for @IsEqualParameter
   *    - "UnblindUniform" for @UnblindUniformParameter
   *    - "Formula" for @FormulaParameter
   */
  static AbsParameter* createNewParameter(const std::string& parameter_type);
  
 private:
  /* data */
};

} // namespace fitmodelbrewery 
} // namespace builder
} // namespace doofit 


#endif // BUILDER_FITMODELBREWERY_RECIPE_PARAMETER_PARAMETERFACTORY_H
