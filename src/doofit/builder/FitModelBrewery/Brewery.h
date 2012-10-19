/** @namespace doofit::builder::fitmodelbrewery
  * @brief This namespace encapsulates all classes and methods related to the
  * FitModelBrewery PDF building project
  * 
  * @see builder::fitmodelbrewery::Brewery
  */
#ifndef BUILDER_FITMODELBREWERY_BREWERY_H_

// from project
#include "doofit/Config/CommonConfig.h"
#include "doofit/builder/FitModelBrewery/BreweryConfig.h"


namespace doofit {
namespace builder { 
namespace fitmodelbrewery {

/** @class Brewery
 *  @brief The Brewery class organizes the creation of the fit model.
 *
 *  The Brewery class delegates the work of creating a fitmodel in terms of a       
 *  PDF. The task of reading in the user configuration information for the PDF
 *  is performed by the Scrivener. The Scrivener uses e.g. a text file with
 *  a property tree and translates it to a Recipe. The Recipe is a
 *  hierarchical structure that defines the properties of the PDFs,
 *  parameters, dimensions and their relationship. It can check itself for 
 *  consistency. After the Recipe is prepared the Brewer takes over and 
 *  creates the final product, e.g. a Roofit workspace containing all   
 *  elements of the fit model. 
 */
class Brewery {
 public:
  Brewery();
  Brewery(const Config::CommonConfig& cfg_com, const BreweryConfig& cfg_brw);
  ~Brewery();
 
 protected:
  
 private:
  
};


} // namespace fitmodelbrewery
} // namespace builder
} // namespace doofit


#endif // BUILDER_FITMODELBREWERY_BREWERY_H_
