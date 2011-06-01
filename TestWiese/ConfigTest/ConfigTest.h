/** \class ConfigTest
 *  \brief DooFit Config test class.
 *
 *  This is a test config class implementation to test the abstract Config base
 *  class.
 */

#ifndef CONFIGTEST_h
#define CONFIGTEST_h

// STL
#include <string>
#include <vector>

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
   */
  ConfigTest();
  
  /**
   *  \brief Destructor.
   */
  ~ConfigTest();
  
  /**
   *  \brief Print all options.
   *
   *  Virtual function that will print all options for this config object.
   */
  void Print();
  
  /**
   *  \brief Getter for ConfigTest::my_test_switch_
   */
  bool my_test_switch() const {return my_test_switch_;}
  /**
   *  \brief Getter for ConfigTest::my_test_int_
   */
  int my_test_int() const {return my_test_int_;}
  
  /**
   *  \brief Setter for ConfigTest::my_test_switch_
   */
  void set_my_test_switch(bool b) {my_test_switch_ = b;}
  /**
   *  \brief Setter for ConfigTest::my_test_int_
   */
  void set_my_test_int(int i) {my_test_int_ = i;}

 protected:
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
  
 private:
  /**
   *  \brief Test switch for ConfigTest class.
   */
  bool my_test_switch_;

  /**
   *  \brief Test integer for ConfigTest class.
   */
  int my_test_int_;

};

#endif //CONFIGTEST_h
