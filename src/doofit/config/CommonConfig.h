/** \class doofit::config::CommonConfig
 *  \brief DooFit common and general Config class for all applications.
 *
 *  This common Config object is responsible for all options common to all 
 *  DooFit applications.
 *
 *  \see config::AbsConfig
 */


#ifndef COMMONCONFIG_h
#define COMMONCONFIG_h

// STL
#include <string>

// Boost

// ROOT

// RooFit

// from project
#ifndef __CINT__
#include "doofit/config/AbsConfig.h"
#else
// ROOT Cint hacks...
#include "AbsConfig.h"
#endif /* __CINT __ */

// forward declarations

namespace doofit {
namespace config {
class CommonConfig : public config::AbsConfig {
 public:
  /**
   *  @brief Default constructor for CommonConfig
   *
   *  Unfortunately this constructor is needed for ROOT CINT and streaming to 
   *  ROOT files.
   */
  CommonConfig();
  
  /**
   *  \brief Standard constructor.
   *
   *  Sets all memebers to default values.
   *
   *  @param name Name of this Config object.
   */
  CommonConfig(const std::string& name);
  
  /**
   *  \brief Destructor.
   */
  ~CommonConfig();
  
  /**
   *  @brief EasyConfig config file name
   *
   *  Get filename of optional additional EasyConfig file.
   *
   *  @return EasyConfig file name
   */
  std::string easyconfig_file() const { return easyconfig_file_; }
  
 protected:
  /** @name Virtual functions
   *  Implementation of pure virtual functions.
   */
  ///@{
  /**
   *  \brief Define options for the derived class.
   *
   *  This function will define all options for the derived CommonConfig class.
   * 
   *  @see Config::DefineOptions()
   */
  void DefineOptions();
  
  /**
   *  \brief Load program options into own member variables.
   *
   *  This function will store all options from the variable map into own 
   *  members.
   *  
   *  @see Config::LoadOptions()
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
  /** @name Actual options
   *  The actual options members.
   */
  ///@{
  /**
   *  @brief EasyConfig file name
   */
  std::string easyconfig_file_;
  ///@}
  
  /**
   *  @brief ClassDef statement for CINT dictionary generation
   */
  //ClassDef(CommonConfig,1);
};
} // namespace config
} // namespace doofit

#endif //COMMONCONFIG_h
