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
   *  \brief Constructor for usage with input from command line.
   *
   *  @param argc Number of arguments.
   *  @param argv Char array with arguments.
   */
  ConfigTest(int argc, char* argv[]);
  
  /**
   *  \brief Constructor for usage with string vector command options.
   */
  ConfigTest(const std::vector<std::string>& option_vector);
  
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
   */
  void DefineOptions();
  
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
