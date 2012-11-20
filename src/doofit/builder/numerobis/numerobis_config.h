/** @class NumerobisConfig
 *  @brief brief description of the class
 *
 *  detailed description of the class
 *  
 *  © Copyright 2012  - Julian Wishahi. All Rights Reserved.
 *
 *  @author Julian Wishahi 
 *  @author $Author$
 *  @date 2012-02-08
 *  @date $Date$
 *  @version $Rev$
**/
#ifndef DOOFIT_BUILDER_NUMEROBIS_NUMEROBIS_CONFIG_H
#define DOOFIT_BUILDER_NUMEROBIS_NUMEROBIS_CONFIG_H

// from STL
#include <string>

// from project
#include "doofit/config/AbsConfig.h"

namespace doofit {
namespace builder {
namespace numerobis {

class NumerobisConfig : public config::AbsConfig {
 public:
  NumerobisConfig(const std::string& name);
  virtual ~NumerobisConfig();
 

  std::string pdf_config_file() const { return pdf_config_file_; }
 
 protected:
 
 private:
  /** @name Virtual functions
   *  Implementation of pure virtual functions.
  **/
  ///@{
  /**
   *  \brief Define options for the derived class.
   *
   *  This function will define all options for the derived CommonConfig class.
   * 
   *  @see config::AbsConfig::DefineOptions()
  **/
  void DefineOptions();
  
  /**
   *  \brief Load program options into own member variables.
   *
   *  This function will store all options from the variable map into own 
   *  members.
   *  
   *  @see config::AbsConfig::LoadOptions()
  **/
  void LoadOptions();
  
  /**
   *  \brief Print all options for this object.
   *
   *  Virtual function that will print all options for this Config object.
  **/
  void PrintOptions() const;
  ///@}
  
  std::string pdf_config_file_;
  
};

} // namespace numerobis 
} // namespace builder 
} // namespace doofit 

#endif // DOOFIT_BUILDER_NUMEROBIS_NUMEROBIS_CONFIG_H