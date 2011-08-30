 /**
   *  \file ConfigTestMain.cpp
   *
   *  \brief Test and demonstration file for Config system
   *
   *  This file implements a testing/demonstration use case of Config objects
   *  using ConfigTest and ConfigTestSecond implementation. 
   */

#include "ConfigTest.h"
#include "Config/CommonConfig.h"
#include "Pdf2Ws/Pdf2WsStd/Pdf2WsStdCommonFuncs.h"

#include "RooWorkspace.h"

#include <exception>
#include <iostream>

int main(int argc, char *argv[]) {
  try {
    
    // instantiate first config object
    CommonConfig config("CommonConfig");
    
    // initialize config object via argc and argv
    config.InitializeOptions(argc, argv);
    
    // instantiate second config object
    ConfigTestSecond config2("SecondConfig");
    
    // Initialize config object via previously initialized Config object.
    // See Config::InitializeOptions() why you should avoid initializing via 
    // argc and argv again.
    config2.InitializeOptions(config);
    
    // More or less mandatory step after initialization of all Config objects.
    // Check if the help flag is set and (if so) print a help message.
    config.CheckHelpFlagAndPrintHelp();
    
    // Print all set options for the user (optional).
    Config::PrintAll();
    
    RooWorkspace* ws = new RooWorkspace("ws", "ws");
    
    Pdf2WsStd::CommonFuncs::getVar(ws, "test_var", "test variable title", 10, 0, 20, "");
    
  } catch (std::exception& e) {
    // in case of parsing problems give an error message and throw exception further
    std::cout << "ERROR: " << e.what() << std::endl;
    throw;
  }
  
  return 0;
}