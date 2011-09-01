/** \class ToyFactoryStdConfig
 *  \brief DooFit config class for the ToyFactoryStd class.
 *
 *  This class is responsible for all ToyFactoryStd related configuration options
 *  which are not covered by CommonConfig.
 */

#ifndef TOYFACTORYSTDCONFIG_h
#define TOYFACTORYSTDCONFIG_h

// STL

// Boost

// ROOT

// RooFit

// from project
#include "Config/Config.h"

// forward declarations

class ToyFactoryStdConfig : public Config {
 public:
  /**
   *  \brief Default constructor for ToyFactoryStdConfig
   *
   *  Sets all memebers to default values.
   *
   *  @param name Name of this Config object.
   */
  ToyFactoryStdConfig(const std::string& name);
  
  /**
   *  \brief Destructor for ToyFactoryStdConfig
   */
  ~ToyFactoryStdConfig();
  
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
  ///@}
};


#endif // TOYFACTORYSTDCONFIG_h
