#include "DooFit/Builder/FitModelBrewery/Recipe/Parameter/ParameterFactory.h"

// from project
#include "DooFit/Builder/FitModelBrewery/Recipe/Recipe.h"
#include "DooFit/Builder/FitModelBrewery/Recipe/Parameter/AbsParameter.h"
#include "DooFit/Builder/FitModelBrewery/Recipe/Parameter/StandardParameter.h"

namespace DooFit {
namespace builder {
namespace fitmodelbrewery {

ParameterFactory::ParameterFactory(){
  
}

ParameterFactory::~ParameterFactory(){
  
}

static AbsParameter* createNewParameter(const std::string& parameter_type){
  if ("Standard" == parameter_type) {
    return new StandardParameter();
  }
  else {
    return NULL;
  }
}


} // namespace fitmodelbrewery 
} // namespace builder
} // namespace DooFit 