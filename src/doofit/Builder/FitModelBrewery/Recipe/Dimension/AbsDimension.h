/** @class AbsDimension
 *  @brief Abstract base class for dimension object
 *
 *  This class is the abstract base class for all dimension object. 
 *  
 *  © Copyright 2012  - Julian Wishahi. All Rights Reserved.
 *
 *  @author Julian Wishahi 
 *  @author $Author$
 *  @date 2012-01-10
 *  @date $Date$
 *  @version $Rev$
 */

#ifndef BUILDER_FITMODELBREWERY_RECIPE_DIMENSION_ABSDIMENSION_H_
#define BUILDER_FITMODELBREWERY_RECIPE_DIMENSION_ABSDIMENSION_H_

// from STL
#include <string>

namespace DooFit {
namespace builder {
namespace fitmodelbrewery {

// forward declarations in namespace
class Recipe;


class AbsDimension {
 public:
  AbsDimension();
  virtual ~AbsDimension();
  
  
  virtual std::string GetName() const = 0;
  virtual void RegisterInRecipe(Recipe& recipe) = 0;
  virtual void CheckForConsistency() const = 0;
    
 protected:
  
 private:
  
};

} // namespace fitmodelbrewery
} // namespace builder
} // namespace DooFit


#endif // BUILDER_FITMODELBREWERY_RECIPE_DIMENSION_ABSDIMENSION_H_
