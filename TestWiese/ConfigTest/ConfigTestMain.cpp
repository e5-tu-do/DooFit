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
   */
  
  ConfigTest config("FirstConfig");
  config.InitializeOptions(argc, argv);
  
  ConfigTestSecond config2("SecondConfig");
  config2.InitializeOptions(config);
  
  config.CheckHelpFlagAndPrintHelp();
  
  config.PrintAll();
  
  return 0;
}