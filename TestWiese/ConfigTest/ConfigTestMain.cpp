#include "TestWiese/ConfigTest/ConfigTest.h"

int main(int argc, char *argv[]) {
  
  ConfigTest config;
  
  config.InitializeOptions(argc, argv);
  config.Print();
  
  return 0;
}