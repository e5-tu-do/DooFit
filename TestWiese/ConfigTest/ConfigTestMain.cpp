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
#include "Pdf2Ws/Pdf2WsStd/Pdf2WsStdMass.h"

#include "RooWorkspace.h"
#include "RooRealVar.h"
#include "RooAbsReal.h"
#include "TH1D.h"

#include <exception>
#include <iostream>

int main(int argc, char *argv[]) {
  try {
    
    // instantiate first config object
    ConfigTest config("FirstConfig");
    
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
    
  } catch (std::exception& e) {
    // in case of parsing problems give an error message and throw exception further
    std::cout << "ERROR: " << e.what() << std::endl;
    throw;
  }
  
  return 0;
}