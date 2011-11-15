/** \class BuilderS2bConfig
 *  \brief DooFit Config class for the BuilderS2b Pdf builder.
 *
 *  This Config object is responsible for all options of the BuilderS2b not
 *  covered by CommonConfig
 *  \see CommonConfig
 *  \see BuilderS2b
 */


#ifndef BUILDERS2BCONFIG_h
#define BUILDERS2BCONFIG_h

// STL

// Boost

// ROOT

// RooFit

// from project
#include "Config/AbsConfig.h"

// forward declarations

class BuilderS2bConfig : public Config::AbsConfig {
public:
  /**
   *  \brief Standard constructor.
   *
   *  Sets all memebers to default values.
   *
   *  @param name Name of this Config object.
   */
  BuilderS2bConfig(const std::string& name);
  
  /**
   *  \brief Destructor.
   */
  ~BuilderS2bConfig();
  
protected:
  /** @name Virtual functions
   *  Implementation of pure virtual functions.
   */
  ///@{
  /**
   *  \brief Define options for the derived class.
   *
   *  This function will define all options for the derived BuilderS2bConfig class.
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
  
  /** BAUSTELLE */
  void load(const std::string &filename);
  void save(const std::string &filename);
  
private:
  /** @name Actual options
   *  The actual options members.
   */
  ///@{
  ///@}
  
  std::vector<std::string> categories_;
  std::vector<std::string> components_;
  
  
};

#endif // BUILDERS2BCONFIG_h
