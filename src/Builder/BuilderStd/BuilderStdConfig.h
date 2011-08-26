/** \class BuilderStdConfig
 *  \brief DooFit Config class for the BuilderStd Pdf builder.
 *
 *  This Config object is responsible for all options of the BuilderStd not
 *  covered by CommonConfig
 *  \see CommonConfig
 *  \see BuilderStd
 */


#ifndef BUILDERSTDCONFIG_h
#define BUILDERSTDCONFIG_h

// STL

// Boost

// ROOT

// RooFit

// from project
#include "Config/Config.h"

// forward declarations

class BuilderStdConfig : public Config {
 public:
  /**
   *  \brief Standard constructor.
   *
   *  Sets all memebers to default values.
   *
   *  @param name Name of this Config object.
   */
  BuilderStdConfig(const std::string& name);
  
  /**
   *  \brief Destructor.
   */
  ~BuilderStdConfig();
  
 protected:
  /** @name Virtual functions
   *  Implementation of pure virtual functions.
   */
  ///@{
  /**
   *  \brief Define options for the derived class.
   *
   *  This function will define all options for the derived BuilderStdConfig class.
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

#endif // BUILDERSTDCONFIG_h
