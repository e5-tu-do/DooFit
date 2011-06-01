#include "TestWiese/ConfigTest/ConfigTest.h"

int main(int argc, char *argv[]) {
  /**
   * \todo When should config objects display help message and how should this
   *       be captured?
   * \todo How can printing of all set options be handled without having to 
   *       invoke all Config::Print() functions by hand?
   */
  
  ConfigTest config;
  config.InitializeOptions(argc, argv);
  
  ConfigTestSecond config2;
  config2.InitializeOptions(config.unrec_options());
  
  config.PrintHelp();
  
  config.Print();
  config2.Print();
  
  return 0;
}