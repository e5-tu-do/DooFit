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

// from Boost
#include <boost/exception/all.hpp>

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

class AbsPdf;

class Recipe {
 public:
  Recipe();
  ~Recipe();
  
  /** @brief Adds a copy of the DiscreteDimension object to dims_discrete_. */
  void RegisterRecipeElement(const DiscreteDimension& dim_discrete);
  /** @brief Adds a copy of the RealDimension object to dims_discrete_. */
  void RegisterRecipeElement(const RealDimension& dim_real);
  /** @brief Adds a copy of the StandardParameter object to params_std_. */
  void RegisterRecipeElement(const StandardParameter& param_std);
  /** @brief Adds a copy of the UnblindUniformParameter object to params_bldunif_. */
  void RegisterRecipeElement(const UnblindUniformParameter& param_bldunif);
  /** @brief Adds a copy of the FormulaParameter object to params_formula_. */
  void RegisterRecipeElement(const FormulaParameter& param_formula);
  
  
 protected:
  
 private:
  typedef std::map< std::string, const RealDimension& > RealDimensionMap;
  typedef std::map< std::string, const DiscreteDimension& > DiscreteDimensionMap;
  typedef std::map< std::string, const StandardParameter& > StandardParameterMap;    
  typedef std::map< std::string, const UnblindUniformParameter& > UnblindUniformParameterMap;
  typedef std::map< std::string, const FormulaParameter& > FormulaParameterMap;     
  
  
  
  /** @brief Function that checks on all maps if an object of with name search_name exists. */
  bool RecipeElementExists(const std::string& search_name);
  
  RealDimensionMap            dims_real_; 
  DiscreteDimensionMap        dims_discrete_;
  StandardParameterMap        params_std_;
  UnblindUniformParameterMap  params_bldunif_;
  FormulaParameterMap         params_formula_;
  
  //std::map< std::string, AbsResolution* > resols_;
  //std::map< std::string, AbsPdf* > pdfs_;
};

/** @struct RegistrationFailed
 *  @brief Registration of an object to the map fails
 */
struct ExcRecipeRegistrationFailed: public virtual boost::exception, public virtual std::exception { 
  virtual const char* what() const throw() { return "Registration of object in Recipe failed."; }
};


} // namespace fitmodelbrewery
} // namespace builder


#endif // BUILDER_FITMODELBREWERY_RECIPE_RECIPE_H_
