#ifndef DOOFIT_TOOLS_SCANNING_PARAMETERSCANNERCONFIG_h
#define DOOFIT_TOOLS_SCANNING_PARAMETERSCANNERCONFIG_h

// STL

// BOOST

// ROOT

// from RooFit

// from project
#include "doofit/config/AbsConfig.h"

// forward declarations
namespace doofit { namespace builder {
  class EasyPdf;
}}
class RooArgSet;

namespace doofit {
namespace tools {
namespace scanning {

/** @class ParameterScannerConfig
 *  @brief DooFit Config class for the ParameterScanner class
 *
 *  This class is responsible for all ParameterScanner related configuration options
 *  which are not covered by CommonConfig.
 */
class ParameterScannerConfig : public config::AbsConfig {
 public:  
  /**
   *  @brief Standard constructor for ParameterScannerConfig
   *
   *  Sets all members to default values.
   *
   *  @param name Name of this Config object.
   */
  ParameterScannerConfig(const std::string& name);
  
  /**
   *  @brief Destructor for ParameterScannerConfig
   */
  ~ParameterScannerConfig();
    
  /**
   *  @brief Set scan parameters in EasyPdf container
   */
  void SetParameters(doofit::builder::EasyPdf& epdf) const;

  /**
   *  @brief Set scan parameters in RooArgSet container
   */
  void SetParameters(RooArgSet& argset) const;

  /** @name Getter functions
   *  Implementation of getter functions.
   */
  ///@{
  std::string name_parameter1() const { return name_parameter1_; }
  double value_parameter1() const { return value_parameter1_; }
  std::string name_parameter2() const { return name_parameter2_; }
  double value_parameter2() const { return value_parameter2_; }
  ///@}

  /** @name Setter functions
   *  Implementation of setter functions.
   */
  ///@{
  ParameterScannerConfig& set_name_parameter1(std::string name_parameter1)   { name_parameter1_ = name_parameter1; return *this; }
  ParameterScannerConfig& set_value_parameter1(double value_parameter1) { value_parameter1_ = value_parameter1; return *this; }
  ParameterScannerConfig& set_name_parameter2(std::string name_parameter2)   { name_parameter2_ = name_parameter2; return *this; }
  ParameterScannerConfig& set_value_parameter2(double value_parameter2) { value_parameter2_ = value_parameter2; return *this; }
  ///@}
    
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
  /**
   * Name of scan parameter 1
   */
  std::string name_parameter1_;

  /**
   * Value of scan parameter 1
   */
  double value_parameter1_;

  /**
   * Constness of scan parameter 1
   */
  bool const_parameter1_;

  /**
   * Name of scan parameter 2
   */
  std::string name_parameter2_;

  /**
   * Value of scan parameter 2
   */
  double value_parameter2_;

  /**
   * Constness of scan parameter 2
   */
  bool const_parameter2_;
};

} // namespace scanning
} // namespace tools
} // namespace doofit
  
#endif // DOOFIT_TOOLS_SCANNING_PARAMETERSCANNERCONFIG_h