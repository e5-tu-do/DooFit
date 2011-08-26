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

int main(){
  // Test Builder
  CommonConfig cfg_com("cfg_com");
  BuilderStdConfig cfg_bld("cfg_bld");
  BuilderStd bld(cfg_com,cfg_bld);
  
  
  RooWorkspace* ws = new RooWorkspace("ws");
  Pdf2WsStd::Mass::Gaussian(ws, "test", "test","mass","mittelwert", "abweichung");
  
  ws->Print("t");
  
  
  
  return 0;
}
