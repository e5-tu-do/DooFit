#ifndef BUILDER_FITMODELBREWERY_RECIPE_DIMENSION_DISCRETEDIMENSION_H
#define BUILDER_FITMODELBREWERY_RECIPE_DIMENSION_DISCRETEDIMENSION_H

// from STL
#include <map>
#include <string>

// from boost
#include <boost/exception/all.hpp>

// from project
#include "doofit/Builder/FitModelBrewery/Recipe/Dimension/AbsDimension.h"

namespace doofit {
namespace builder {
namespace fitmodelbrewery {

// forward declarations in namespace
class Recipe;

/** @class DiscreteDimension
 *  @brief This class represents dimensions with discrete integer values.
 *  
 *  © Copyright 2012  - Julian Wishahi. All Rights Reserved.
 *
 *  @author Julian Wishahi 
 *  @author $Author$
 *  @date 2012-01-10
 *  @date $Date$
 *  @version $Rev$
 */

class DiscreteDimension : public AbsDimension {
 public:
  DiscreteDimension();
  virtual ~DiscreteDimension ();
  
  virtual std::string GetName() const { return name(); }
  virtual void RegisterInRecipe(Recipe& recipe);
  virtual void CheckForConsistency() const {};
  
  
  
  /** Setter and getter for the name name_ */
  std::string name() const { return name_; }
  void set_name(const std::string& name) { name_ = name; }
  
  /** Setter and getter for the description desc_ */
  std::string desc() const { return desc_; }
  void set_desc(const std::string& desc) { desc_ = desc; } 
  
  /** Add a new type to the discrete dimension map. */
  void AddType(std::string type_name, int type_value);
  
 protected:
  
 private:
  std::string name_;
  std::string desc_;
  std::map<std::string,int> map_types_;
  
};

/** @struct RegistrationFailed
 *  @brief Registration of an object to the map fails
 */
struct ExcDiscreteDimensionTypeExists: public virtual boost::exception, public virtual std::exception { 
  virtual const char* what() const throw() { return "Tried to add a type to a DiscreteDimension that was defined before."; }
};


} // namespace fitmodelbrewery 
} // namespace builder
} // namespace doofit 

#endif // BUILDER_FITMODELBREWERY_RECIPE_DIMENSION_DISCRETEDIMENSION_H
