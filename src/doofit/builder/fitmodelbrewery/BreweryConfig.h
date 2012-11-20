#ifndef BUILDER_FITMODELBREWERY_BREWERYCONFIG_H_
#define BUILDER_FITMODELBREWERY_BREWERYCONFIG_H_

// from STL

// from project
#include "doofit/config/AbsConfig.h"

namespace doofit {
namespace builder { 
namespace fitmodelbrewery {

class BreweryConfig : public config::AbsConfig {
 public:    
  BreweryConfig();
  BreweryConfig(const std::string& name);
  
  ~BreweryConfig();
  
 protected:
  /** @name Virtual functions
   *  Implementation of pure virtual functions.
   */
  ///@{
  /**
   *  \brief Define options for the derived class.
   *
   *  This function will define all options for the derived BreweryConfig class.
   * 
   *  @see config::AbsConfig::DefineOptions()
   */
  void DefineOptions();
  
  /**
   *  \brief Load program options into own member variables.
   *
   *  This function will store all options from the variable map into own 
   *  members.
   *  
   *  @see config::AbsConfig::LoadOptions()
   */
  void LoadOptions();
   
  /**
   *  \brief Print all options for this object.
   *
   *  Virtual function that will print all options for this Config object.
   */
  void PrintOptions() const;
  ///@}
 private:
  
};


} // namespace fitmodelbrewery
} // namespace builder
} // namespace doofit


#endif // BUILDER_FITMODELBREWERY_BREWERYCONFIG_H_