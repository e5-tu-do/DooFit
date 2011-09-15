// STL
#include <vector>

// Boost

// ROOT
#include "TCanvas.h"
#include "TFile.h"

// RooFit
#include "RooWorkspace.h"
#include "RooDataSet.h"
#include "RooArgSet.h"
#include "RooRealVar.h"
#include "RooPlot.h"
#include "RooGaussian.h"
#include "RooExtendPdf.h"
#include "RooAddPdf.h" 
#include "RooProdPdf.h"

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
  CommonConfig cfg_com("common");
  cfg_com.InitializeOptions(argc, argv);
  
  BuilderStdConfig cfg_bld("builder");
  cfg_bld.InitializeOptions(cfg_com);
  
  ToyFactoryStdConfig cfg_tfac("toy");
  cfg_tfac.InitializeOptions(cfg_bld);
  
  {
    ToyFactoryStdConfig cfg_tfac_die("toy_die");
    cfg_tfac_die.InitializeOptions(cfg_bld);
  }
  
  cfg_com.CheckHelpFlagAndPrintHelp();
  
  BuilderStd bld(cfg_com, cfg_bld);
    
  RooWorkspace* ws = new RooWorkspace("ws");
  
  Pdf2WsStd::CommonFuncs::getVar(ws, "mean1", "mean1", 5200, 5000, 5700, "MeV/c^{2}");
  Pdf2WsStd::CommonFuncs::getVar(ws, "mean2", "mean2", 5300, 5000, 5700, "MeV/c^{2}");
  Pdf2WsStd::CommonFuncs::getVar(ws, "mean3", "mean3", 5400, 5000, 5700, "MeV/c^{2}");
  
  RooGaussian* pdf1 = Pdf2WsStd::Mass::Gaussian(ws, "test1", "Gaussian test pdf #1","mass","mean1", "abweichung");
  RooGaussian* pdf2 = Pdf2WsStd::Mass::Gaussian(ws, "test2", "Gaussian test pdf #2","mass","mean2", "abweichung_bkg");
  RooGaussian* pdf3 = Pdf2WsStd::Mass::Gaussian(ws, "test3", "Gaussian test pdf #3","mass","mean3", "abweichung_bkg2");
  
  RooGaussian* time1 = Pdf2WsStd::Mass::Gaussian(ws, "time1", "Gaussian test pdf #1 (time)","time","mean_time1", "sigma_time1");
  RooGaussian* time2 = Pdf2WsStd::Mass::Gaussian(ws, "time2", "Gaussian test pdf #2 (time)","time","mean_time2", "sigma_time2");
  RooGaussian* time3 = Pdf2WsStd::Mass::Gaussian(ws, "time3", "Gaussian test pdf #3 (time)","time","mean_time3", "sigma_time3");
  
  ws->Print("t");
    
  RooProdPdf pdf_prod1("pdf_prod1", "pdf_prod1", RooArgList(*pdf1, *time1));
  RooProdPdf pdf_prod2("pdf_prod2", "pdf_prod2", RooArgList(*pdf2, *time2));
  RooProdPdf pdf_prod3("pdf_prod3", "pdf_prod3", RooArgList(*pdf3, *time3));
  
  RooRealVar yield1("yield1", "pdf yield", 10000, 0, 10000);
  RooExtendPdf pdf_extend1("pdf_extend1", "extended pdf #1", pdf_prod1, yield1);
  
  RooRealVar yield2("yield2", "pdf yield", 50000, 0, 10000);
  RooExtendPdf pdf_extend2("pdf_extend2", "extended pdf #2", pdf_prod2, yield2);
  
  RooRealVar yield3("yield3", "pdf yield", 5000, 0, 10000);
  RooExtendPdf pdf_extend3("pdf_extend3", "extended pdf #3", pdf_prod3, yield3);
  
  RooRealVar coeff1("coeff1", "coeff1", 0.1, 0.0, 1.0);
  RooRealVar coeff2("coeff2", "coeff2", 0.1, 0.0, 1.0);
  RooAddPdf pdf_add("pdf_add", "added pdf", RooArgSet(pdf_extend1, pdf_extend2, pdf_extend3));
  //RooAddPdf pdf_add("pdf_add", "added pdf", RooArgList(*pdf1, *pdf2, *pdf3), RooArgList(coeff1, coeff2));
  //RooAddPdf pdf_add("pdf_add", "added pdf", *pdf1, *pdf2, coeff1);
  
  cfg_tfac.set_generation_pdf(pdf2);
  RooArgSet argset_obs("argset_obs");
  argset_obs.add(*(Pdf2WsStd::CommonFuncs::getVar(ws, "mass", "", 0, 0, 0, "")));
  argset_obs.add(*(Pdf2WsStd::CommonFuncs::getVar(ws, "time", "", 0, 0, 0, "")));
  argset_obs.add(*(Pdf2WsStd::CommonFuncs::getVar(ws, "varTag", "tag of B meson", 0, -1, 1, "")));
  
  cfg_tfac.set_argset_generation_observables(&argset_obs);
  
  ToyFactoryStd tfac(cfg_com, cfg_tfac);
  
  cfg_com.PrintAll();
  
  RooDataSet* data = tfac.Generate();
 
  RooRealVar* mass = (RooRealVar*)Pdf2WsStd::CommonFuncs::getVar(ws, "mass", "", 0, 0, 0, "");
  RooPlot* mass_frame = mass->frame();
  
  data->plotOn(mass_frame);
  pdf2->plotOn(mass_frame);
  
  TCanvas c1("c1", "c1", 800, 600);
  mass_frame->Draw();
  
  c1.SaveAs("c1.pdf");
}