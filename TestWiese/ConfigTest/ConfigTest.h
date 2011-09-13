/** \class ConfigTest
 *  \brief DooFit Config test class.
 *
 *  This is a test config class implementation to test the abstract Config base
 *  class.
 */

/** \class ConfigTestSecond
 *  \brief DooFit Config second test class.
 *
 *  This is a second test config class implementation to test the abstract 
 *  Config base class, especially passing unrecognized options from first test
 *  class.
 */

#ifndef CONFIGTEST_h
#define CONFIGTEST_h

// STL
#include <string>
#include <vector>
#include <map>

// Boost
#include <boost/program_options.hpp>

// ROOT

// RooFit

// from project
#include "Config/Config.h"

// forward declarations

class ConfigTest : public Config {
 public:
  /**
   *  \brief Standard constructor.
   *
   *  Sets all memebers to default values.
   *
   *  @param name Name of this Config object.
   */
  ConfigTest(const std::string& name);
  
  /**
   *  \brief Destructor.
   */
  ~ConfigTest();
  
  /** @name Getter actual options
   *  Getter functions for actual options members.
   */
  ///@{
  /**
   *  \brief Getter for ConfigTest::my_test_switch_
   */
  bool my_test_switch() const {return my_test_switch_;}
  /**
   *  \brief Getter for ConfigTest::my_test_int_
   */
  int my_test_int() const {return my_test_int_;}
  ///@}

  /** @name Setter actual options
   *  Setter functions for actual options members.
   */
  ///@{
  /**
   *  \brief Setter for ConfigTest::my_test_switch_
   */
  void set_my_test_switch(bool b) {my_test_switch_ = b;}
  /**
   *  \brief Setter for ConfigTest::my_test_int_
   */
  void set_my_test_int(int i) {my_test_int_ = i;}
  ///@}

 protected:
  /** @name Virtual functions
   *  Implementation of pure virtual functions.
   */
  ///@{
  /**
   *  \brief Define options for the derived class.
   *
   *  This function will define all options for the derived ConfigTest class.
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
   *  \brief Test switch for ConfigTest class.
   */
  bool my_test_switch_;

  /**
   *  \brief Test integer for ConfigTest class.
   */
  int my_test_int_;
  ///@}

};

class ConfigTestSecond : public Config {
public:
  /**
   *  \brief Standard constructor.
   *
   *  Sets all memebers to default values.
   *
   *  @param name Name of this Config object.
   */
  ConfigTestSecond(const std::string& name);
  
  /**
   *  \brief Destructor.
   */
  ~ConfigTestSecond();
  
  /** @name Getter actual options
   *  Getter functions for actual options members.
   */
  ///@{
  /**
   *  \brief Getter for ConfigTestSecond::my_test_switch_
   */
  bool my_test_switch() const {return my_test_switch_;}
  /**
   *  \brief Getter for ConfigTestSecond::my_test_int_
   */
  int my_test_int() const {return my_test_int_;}
  ///@}
  
  /** @name Setter actual options
   *  Setter functions for actual options members.
   */
  ///@{
  /**
   *  \brief Setter for ConfigTestSecond::my_test_switch_
   */
  void set_my_test_switch(bool b) {my_test_switch_ = b;}
  /**
   *  \brief Setter for ConfigTestSecond::my_test_int_
   */
  void set_my_test_int(int i) {my_test_int_ = i;}
  ///@}
  
protected:
  /** @name Virtual functions
   *  Implementation of pure virtual functions.
   */
  ///@{
  /**
   *  \brief Define options for the derived class.
   *
   *  This function will define all options for the derived ConfigTestSecond class.
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
   *  Virtual function that will print all options for this config object.
   */
  void PrintOptions() const;
  ///@}
  
private:
  /** @name Actual options
   *  The actual options members.
   */
  ///@{ 
  /**
   *  \brief Test switch for ConfigTestSecond class.
   */
  bool my_test_switch_;
  
  /**
   *  \brief Test integer for ConfigTestSecond class.
   */
  int my_test_int_;
  
  /**
   *  \brief Test string vector for ConfigTestSecond class.
   */
  std::vector<std::string> my_test_vector_;
  ///@}
  
};

/** \class ConfigTestAbstractType
 *  \brief DooFit Config test abstract data type
 *
 *  This is an abstract data type which should be read in via 
 *  boost::program_options. It's supposed to have a string representation (for 
 *  command line and config file) and should take care of parsing this string
 *  representation. Handling inside a Config class should be transparent. Maybe 
 *  a base class for abstract string-parsing types will arise from this.
 */
class ConfigTestAbstractType {
 public:
  /**
   *  @brief Default constructor for ConfigTestAbstractType
   */
  ConfigTestAbstractType() {}
  
  /**
   *  @brief Parse a given string
   *
   *  The given string @a str is parsed. If parsing is not successful, an 
   *  exception will be thrown. The string is supposed to be formatted like 
   *  this:
   *
   *  @verbatim
   *  simple,key,value,key,value,...,key,value
   *  @endverbatim
   *
   *  @see ConfigTestAbstractType::simple_
   *  @see ConfigTestAbstractType::map_
   *
   *  @param str string to parse
   */
  void Parse(std::string str);
  
 protected:
 private:
  /**
   *  @brief simple member to be parsed into
   * 
   *  Just a simple string, like a name or something.
   *
   *  @see ConfigTestAbstractType::Parse(std::string)
   */
  std::string simple_;
  
  /**
   *  @brief abstract member to be parsed into
   * 
   *  This map contains string key value pairs which are supposed to be filled
   *  via a string.
   *
   *  @see ConfigTestAbstractType::Parse(std::string)
   */
  std::map<std::string,std::string> map_;
};

#endif //CONFIGTEST_h
