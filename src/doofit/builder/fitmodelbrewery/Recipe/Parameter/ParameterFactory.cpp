#include "doofit/builder/fitmodelbrewery/Recipe/Parameter/ParameterFactory.h"

// from project
#include "doofit/builder/fitmodelbrewery/Recipe/Recipe.h"
#include "doofit/builder/fitmodelbrewery/Recipe/Parameter/AbsParameter.h"
#include "doofit/builder/fitmodelbrewery/Recipe/Parameter/StandardParameter.h"

namespace doofit {
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
} // namespace doofit 