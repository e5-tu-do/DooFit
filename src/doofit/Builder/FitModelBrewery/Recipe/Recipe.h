/** @class Recipe
 *  @brief The Recipe describes the relation of the pdf to be built.
 *
 *  The Recipe defines the relation of pdfs, parameters, etc.
 *  
 *  © Copyright 2012  - Julian Wishahi. All Rights Reserved.
 *
 *  @author Julian Wishahi 
 *  @author $Author$
 *  @date 2012-01-09
 *  @date $Date$
 *  @version $Rev$
 */

#ifndef BUILDER_FITMODELBREWERY_RECIPE_RECIPE_H_
#define BUILDER_FITMODELBREWERY_RECIPE_RECIPE_H_

// from STL
#include <map>
#include <string>

// from boost
#include <boost/exception/all.hpp>

/// from Builder/FitModelBrewery
#include "DooFit/Builder/FitModelBrewery/Recipe/Dimension/DiscreteDimension.h"
#include "DooFit/Builder/FitModelBrewery/Recipe/Dimension/RealDimension.h"

#include "DooFit/Builder/FitModelBrewery/Recipe/Category/BasicCategory.h"
#include "DooFit/Builder/FitModelBrewery/Recipe/Category/SuperCategory.h"

#include "DooFit/Builder/FitModelBrewery/Recipe/Parameter/StandardParameter.h"
#include "DooFit/Builder/FitModelBrewery/Recipe/Parameter/UnblindUniformParameter.h"
#include "DooFit/Builder/FitModelBrewery/Recipe/Parameter/FormulaParameter.h"

namespace DooFit {
namespace builder {
namespace fitmodelbrewery {

// forward declarations
class AbsPdf;

class Recipe {
 public:
  Recipe();
  ~Recipe();
  
  /** @brief Adds a copy of the DiscreteDimension object to map_dims_discrete_. */
  void RegisterRecipeElement(const DiscreteDimension& dim_discrete);
  /** @brief Adds a copy of the RealDimension object to map_dims_discrete_. */
  void RegisterRecipeElement(const RealDimension& dim_real);
  /** @brief Adds a copy of the RealDimension object to map_cats_basic_. */
  void RegisterRecipeElement(const BasicCategory& cat_basic);
  /** @brief Adds a copy of the RealDimension object to map_cats_super_. */
  void RegisterRecipeElement(const SuperCategory& cat_super);
  /** @brief Adds a copy of the StandardParameter object to params_std_. */
  void RegisterRecipeElement(const StandardParameter& param_std);
//  /** @brief Adds a copy of the UnblindUniformParameter object to params_bldunif_. */
//  void RegisterRecipeElement(const UnblindUniformParameter& param_bldunif);
//  /** @brief Adds a copy of the FormulaParameter object to params_formula_. */
//  void RegisterRecipeElement(const FormulaParameter& param_formula);
  
  
 protected:
  
 private:
  typedef std::map<std::string, const RealDimension> RealDimensionMap;
  typedef std::map<std::string, const DiscreteDimension> DiscreteDimensionMap;
  
  typedef std::map<std::string, const BasicCategory> BasicCategoryMap;
  typedef std::map<std::string, const SuperCategory> SuperCategoryMap;
  
  typedef std::map<std::string, const StandardParameter> StandardParameterMap;    
//  typedef std::map<std::string, const UnblindUniformParameter> UnblindUniformParameterMap;
//  typedef std::map<std::string, const FormulaParameter> FormulaParameterMap;     
  
  
  
  /** @brief Function that checks on all maps if an object of with name search_name exists. */
  bool RecipeElementExists(const std::string& search_name);
  
  RealDimensionMap            map_dims_real_; 
  DiscreteDimensionMap        map_dims_discrete_;
  
  BasicCategoryMap            map_cats_basic_;
  SuperCategoryMap            map_cats_super_;
  
  StandardParameterMap        map_params_std_;
//  UnblindUniformParameterMap  map_params_bldunif_;
//  FormulaParameterMap         map_params_formula_;
  
  //std::map< std::string, AbsResolution* > resols_;
  //std::map< std::string, AbsPdf* > pdfs_;
  
  bool is_ready;
  bool is_consistent;
  
  
};

/** @struct RegistrationFailed
 *  @brief Registration of an object to the map fails
 */
struct ExcRecipeRegistrationFailed: public virtual boost::exception, public virtual std::exception { 
  virtual const char* what() const throw() { return "Registration of object in Recipe failed."; }
};


} // namespace fitmodelbrewery
} // namespace builder
} // namespace DooFit


#endif // BUILDER_FITMODELBREWERY_RECIPE_RECIPE_H_