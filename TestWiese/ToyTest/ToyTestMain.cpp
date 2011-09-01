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

#include "ToyFactory/ToyFactoryStd/ToyFactoryStd.h"
#include "ToyFactory/ToyFactoryStd/ToyFactoryStdConfig.h"

int main() {
  CommonConfig cfg_com("cfg_com");
  BuilderStdConfig cfg_bld("cfg_bld");
  BuilderStd bld(cfg_com,cfg_bld);
  
  ToyFactoryStdConfig cfg_tfac("config_toyfactorystd");
  ToyFactoryStd tfac(cfg_com, cfg_tfac);
  
  RooWorkspace* ws = new RooWorkspace("ws");
  Pdf2WsStd::Mass::Gaussian(ws, "test", "Gaussian test pdf","mass","mittelwert", "abweichung");
  
  ws->Print("t");
  
  cfg_tfac.set_generation_pdf(ws->pdf("test"));
  
  cfg_com.PrintAll();
}