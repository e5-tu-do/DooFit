#ifndef BUILDER_FITMODELBREWERY_RECIPE_CATEGORY_SUPERCATEGORY_H
#define BUILDER_FITMODELBREWERY_RECIPE_CATEGORY_SUPERCATEGORY_H

// from STL
#include <string>
#include <vector>

// from boost
#include <boost/exception/all.hpp>

// from project
#include "doofit/builder/fitmodelbrewery/Recipe/Category/AbsCategory.h"

namespace doofit {
namespace builder {
namespace fitmodelbrewery {

// forward declarations in namespace
class Recipe;

/** @class SuperCategory
 *  @brief This class represents category cominations of multiple BasicCategory objects.
 *
 *  This class does not check if the requiered BasicCategory objects which
 *  name identifiers are saved in vector_cats_basic_ when registering a copy of
 *  itself in the Recipe. This check is only performed when calling 
 *  CheckForConsistency().
 *  
 *  © Copyright 2012  - Julian Wishahi. All Rights Reserved.
 *
 *  @author Julian Wishahi 
 *  @author $Author$
 *  @date 2012-01-10
 *  @date $Date$
 *  @version $Rev$
 */

class SuperCategory : public AbsCategory {
 public:
  SuperCategory();
  virtual ~SuperCategory ();
  
  virtual std::string GetName() const { return name(); }
  virtual void RegisterInRecipe(Recipe& recipe);
  virtual void CheckForConsistency(){};
  
  /** Setter and getter for the name name_ */
  std::string name() const { return name_; }
  void set_name(const std::string& name) { name_ = name; }
  
  /** Setter and getter for the description desc_ */
  std::string desc() const { return desc_; }
  void set_desc(const std::string& desc) { desc_ = desc; } 
  
  /** Add name of BasicCategory as basis for SuperCategory. */
  void AddBasicCategory(const std::string& basic_cat_name);
  
 protected:
  
 private:
  std::string name_;
  std::string desc_;
  std::vector<std::string> vector_cats_basic_;
  
};

} // namespace fitmodelbrewery 
} // namespace builder
} // namespace doofit 

#endif // BUILDER_FITMODELBREWERY_RECIPE_CATEGORY_SUPERCATEGORY_H
