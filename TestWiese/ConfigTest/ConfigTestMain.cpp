#include "TestWiese/ConfigTest/ConfigTest.h"

int main(int argc, char *argv[]) {
  /**
   * \todo When should Config objects display help message and how should this
   *       be captured? I.e. if a "--help" option is supplied, only one Config
   *       object will have it defined as option in its variables map. At some 
   *       point in the code one should check if the help flag is set and invoke
   *       Config::PrintHelp(). I'd like this to be rather automatic without the
   *       need to invoke something like Config::CheckforHelpFlagAndPrintHelp() 
   *       in every binary or main().
   *       Solution: Capture manually.
   * \todo How can printing of all set options be handled without having to 
   *       invoke all Config::Print() functions by hand? Imagine having created 
   *       a bunch of derived Config objects and then you want to print/dump the
   *       complete set of options at different points in the code. It would be
   *       rather not nice to have to always use a block of Print() statements
   *       there.
   *       Solution: use static vector to store all config objects (incl. 
   *       checking for uniqueness by name) and have a Config::PrintAll() 
   *       function that can loop over it. Make it private so that no derived 
   *       class can tamper with it.
   */
  
  ConfigTest config;
  config.InitializeOptions(argc, argv);
  
  ConfigTestSecond config2;
  config2.InitializeOptions(config);
  
  config.PrintHelp();
  
  config.Print();
  config2.Print();
  
  return 0;
}