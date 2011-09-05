// STL

// Boost

// ROOT
#include "TCanvas.h"

// RooFit
#include "RooWorkspace.h"
#include "RooDataSet.h"
#include "RooArgSet.h"
#include "RooRealVar.h"
#include "RooPlot.h"

// from Project
#include "Config/CommonConfig.h"

#include "Builder/BuilderStd/BuilderStd.h"
#include "Builder/BuilderStd/BuilderStdConfig.h"

#include "Pdf2Ws/Pdf2WsStd/Pdf2WsStdMass.h"
#include "Pdf2Ws/Pdf2WsStd/Pdf2WsStdCommonFuncs.h"

#include "ToyFactory/ToyFactoryStd/ToyFactoryStd.h"
#include "ToyFactory/ToyFactoryStd/ToyFactoryStdConfig.h"

#include "Utils/MsgStream.h"

int main(int argc, char *argv[]) {
  CommonConfig cfg_com("cfg_com");
  
  cfg_com.InitializeOptions(argc, argv);
  
  BuilderStdConfig cfg_bld("cfg_bld");
  
  cfg_bld.InitializeOptions(cfg_com);
  
  ToyFactoryStdConfig cfg_tfac("config_toyfactorystd");
  
  cfg_bld.InitializeOptions(cfg_tfac);
  
  cfg_com.CheckHelpFlagAndPrintHelp();
  
  BuilderStd bld(cfg_com,cfg_bld);
  
  ToyFactoryStd tfac(cfg_com, cfg_tfac);
  
  RooWorkspace* ws = new RooWorkspace("ws");
  Pdf2WsStd::Mass::Gaussian(ws, "test", "Gaussian test pdf","mass","mittelwert", "abweichung");
  
  ws->Print("t");
  
  cfg_tfac.set_generation_pdf(ws->pdf("test"));
  cfg_tfac.set_expected_yield(100000);
  RooArgSet argset_obs("argset_obs");
  argset_obs.add(*(Pdf2WsStd::CommonFuncs::getVar(ws, "mass", "", 0, 0, 0, "")));
  
  cfg_tfac.set_argset_generation_observables(&argset_obs);
  
  cfg_com.PrintAll();
  
  RooDataSet* data = tfac.Generate();
 
  RooRealVar* mass = (RooRealVar*)Pdf2WsStd::CommonFuncs::getVar(ws, "mass", "", 0, 0, 0, "");
  RooPlot* mass_frame = mass->frame();
  
  data->plotOn(mass_frame);
  ws->pdf("test")->plotOn(mass_frame);
  
  TCanvas c1("c1", "c1", 800, 600);
  mass_frame->Draw();
  
  c1.SaveAs("c1.pdf");
}