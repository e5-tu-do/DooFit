#include "ConfigTest.h"
#include <exception>
#include <iostream>

int main(int argc, char *argv[]) {
  try {
    
    ConfigTest config("FirstConfig");
    config.InitializeOptions(argc, argv);
    
    ConfigTestSecond config2("SecondConfig");
    config2.InitializeOptions(config);
    
    config.CheckHelpFlagAndPrintHelp();
    
    Config::PrintAll();
    
  } catch (std::exception& e) {
    // in case of parsing problems give an error message and throw exception further
    std::cout << "ERROR: " << e.what() << std::endl;
    throw;
  }
  
  return 0;
}