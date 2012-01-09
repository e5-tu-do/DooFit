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
 *  @version $Rev$  \sa
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
  
  
 protected:
  
 private:
  
  std::map< std::string, const RealDimension& > dims_real_; 
  std::map< std::string, const DiscreteDimension& > dims_discrete_;
  
  std::map< std::string, const StandardParameter& > params_std_;
  std::map< std::string, const UnblindUniformParameter& > params_bldunif_;
  std::map< std::string, const FormulaParameter& > params_formula_;
  
  //std::map< std::string, AbsResolution* > resols_;
  //std::map< std::string, AbsPdf* > pdfs_;
  
};

/** @struct RegistrationFailed
 *  @brief Registration of an object to the map fails
 */
struct RegistrationFailed: public virtual boost::exception, public virtual std::exception { 
  virtual const char* what() const throw() { return "Registration of object in Recipe failed."; }
};


} // namespace fitmodelbrewery
} // namespace builder


#endif // BUILDER_FITMODELBREWERY_RECIPE_RECIPE_H_
