// STL

// Boost

// ROOT

// RooFit
#include "RooWorkspace.h"

// from Project
#include "Config/CommonConfig.h"

#include "Builder/BuilderStd/BuilderStd.h"
#include "Builder/BuilderStd/BuilderStdConfig.h"

#include "Pdf2Ws/Pdf2WsStd/Pdf2WsStdMass.h"

int main(int argc, char *argv[]){
  // Test Builder
  CommonConfig cfg_com("common");
  cfg_com.InitializeOptions(argc, argv);
  
  BuilderStdConfig cfg_bld("builder");
  cfg_bld.InitializeOptions(cfg_com);
  
  cfg_com.CheckHelpFlagAndPrintHelp();
  
  BuilderStd bld(cfg_com,cfg_bld);
  

  
  
  RooWorkspace* ws = new RooWorkspace("ws");
  Pdf2WsStd::Mass::Gaussian(ws, "test", "test","mass","mittelwert", "abweichung");
  
  ws->Print("t");
  
  
  
  return 0;
}
