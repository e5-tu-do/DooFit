// STL
#include <vector>
#include <sstream>

// Boost

// ROOT
#include "TCanvas.h"
#include "TFile.h"
#include "TStopwatch.h"
#include "TDirectory.h"
#include "TROOT.h"
#include "TObjectTable.h"

// RooFit
#include "RooWorkspace.h"
#include "RooDataSet.h"
#include "RooArgSet.h"
#include "RooRealVar.h"
#include "RooCategory.h"
#include "RooPlot.h"
#include "RooGaussian.h"
#include "RooExtendPdf.h"
#include "RooAddPdf.h" 
#include "RooProdPdf.h"
#include "Roo1DTable.h"
#include "RooFitResult.h"

// from Project
#include "Config/CommonConfig.h"

#include "Builder/BuilderStd/BuilderStd.h"
#include "Builder/BuilderStd/BuilderStdConfig.h"

#include "Pdf2Ws/Pdf2WsStd/Pdf2WsStdMass.h"
#include "Pdf2Ws/Pdf2WsStd/Pdf2WsStdCommonFuncs.h"

#include "Toy/ToyFactoryStd/ToyFactoryStd.h"
#include "Toy/ToyFactoryStd/ToyFactoryStdConfig.h"

#include "Toy/ToyStudyStd/ToyStudyStd.h"
#include "Toy/ToyStudyStd/ToyStudyStdConfig.h"

#include "Utils/MsgStream.h"

void leak() {
}

void TestToys(int argc, char *argv[]) {
  using namespace Toy;
  using namespace RooFit;
  
  CommonConfig cfg_com("common");
  cfg_com.InitializeOptions(argc, argv);
  
  BuilderStdConfig cfg_bld("builder");
  cfg_bld.InitializeOptions(cfg_com);
  
  ToyFactoryStdConfig cfg_tfac("toyfac");
  cfg_tfac.InitializeOptions(cfg_bld);
  
  ToyStudyStdConfig cfg_tstudy("toystudy");
  cfg_tstudy.InitializeOptions(cfg_tfac);
  
  cfg_com.CheckHelpFlagAndPrintHelp();
  
  //BuilderStd bld(cfg_com, cfg_bld);
  
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
  
  RooProdPdf pdf_prod1("pdf_prod1", "pdf_prod1", RooArgList(*pdf1, *time1));
  RooProdPdf pdf_prod2("pdf_prod2", "pdf_prod2", RooArgList(*pdf2, *time2));
  RooProdPdf pdf_prod3("pdf_prod3", "pdf_prod3", RooArgList(*pdf3, *time3));
  
  RooRealVar yield1("yield1", "pdf yield", 10000, 0, 1000000);
  RooExtendPdf pdf_extend1("pdf_extend1", "extended pdf #1", pdf_prod1, yield1);
  
  RooRealVar yield2("yield2", "pdf yield", 50000, 0, 1000000);
  RooExtendPdf pdf_extend2("pdf_extend2", "extended pdf #2", pdf_prod2, yield2);
  
  RooRealVar yield3("yield3", "pdf yield", 5000, 0, 1000000);
  RooExtendPdf pdf_extend3("pdf_extend3", "extended pdf #3", pdf_prod3, yield3);
  
  RooRealVar coeff1("coeff1", "coeff1", 0.1, 0.0, 1.0);
  RooRealVar coeff2("coeff2", "coeff2", 0.1, 0.0, 1.0);
  RooAddPdf pdf_add("pdf_add", "added pdf", RooArgSet(pdf_extend1, pdf_extend2, pdf_extend3));
  //RooAddPdf pdf_add("pdf_add", "added pdf", RooArgList(*pdf1, *pdf2, *pdf3), RooArgList(coeff1, coeff2));
  //RooAddPdf pdf_add("pdf_add", "added pdf", *pdf1, *pdf2, coeff1);
  
  ws->import(pdf_add);
  
  RooArgSet argset_obs("argset_obs");
  argset_obs.add(*(Pdf2WsStd::CommonFuncs::getVar(ws, "mass", "", 0, 0, 0, "")));
  argset_obs.add(*(Pdf2WsStd::CommonFuncs::getVar(ws, "time", "", 0, 0, 0, "")));
  argset_obs.add(*(Pdf2WsStd::CommonFuncs::getVar(ws, "tag2", "tag of B meson", 0, -10, 10, "")));
  RooRealVar* tag2 = (RooRealVar*)Pdf2WsStd::CommonFuncs::getVar(ws, "tag2", "", 0, 0, 0, "");
  
  RooCategory* tag = new RooCategory("tag", "tag");
  tag->defineType("b0", 1);
  tag->defineType("b0_bar", -1);
  ws->import(*tag);
  delete tag;
  tag = ws->cat("tag");
  
  argset_obs.add(*tag);
  ws->defineSet("argset_obs",argset_obs);  
  
  ws->Print("t");
  
//  TFile wsfile("ws.root", "recreate");
//  ws->Write("ws");
//  wsfile.Close();
    
//  cfg_tfac.set_workspace(ws);
//  cfg_tfac.set_generation_pdf_workspace("pdf_add");
//  cfg_tfac.set_argset_generation_observables_workspace("argset_obs");
  
  ToyFactoryStd tfac(cfg_com, cfg_tfac);
  
  cfg_com.PrintAll();
  
  RooDataSet* data = tfac.Generate();
  
  RooFitResult* fit_result = ws->pdf("pdf_add")->fitTo(*data, NumCPU(2), Extended(true), Save(true), Strategy(2), Minos(true), Hesse(true), Verbose(false),Timer(true));
  
  delete data;
  
  TFile f("data.root","read");
  data = (RooDataSet*)f.Get("dataset");
  
  RooRealVar* mass = (RooRealVar*)Pdf2WsStd::CommonFuncs::getVar(ws, "mass", "", 0, 0, 0, "");
  
  RooPlot* mass_frame = mass->frame();
  RooPlot* tag2_frame = tag2->frame();
  
  data->plotOn(mass_frame);
  ws->pdf("pdf_add")->plotOn(mass_frame);
  
  data->plotOn(tag2_frame);
  
  TCanvas c1("c1", "c1", 800, 600);
  mass_frame->Draw();
  c1.SaveAs("c1.pdf");
  
  tag2_frame->Draw();
  c1.SaveAs("c2.pdf");
  
  Roo1DTable* table = data->table(RooArgSet(*tag));
  table->Print();
  delete table;
  
  delete mass_frame;
  delete tag2_frame;
  
  delete data;
  delete ws;
  //gROOT = NULL;
}

int main(int argc, char *argv[]) {
//  // some performance test code
//  int i_max = 30;
//  int j_max = 1000000;
//  
//  TStopwatch sw;
//  sw.Start();
//  RooRealVar* var0 = new RooRealVar("var0","", 0, -10, 10);
//  RooArgSet argset0(*var0);
//  RooDataSet * data_test = new RooDataSet("data", "data", argset0);
//  for (int j=0; j<j_max; ++j) {
//    var0->setVal(5);
//    data_test->add(argset0);
//  }
//  
//  for (int i=1; i<i_max; ++i) {
//    ostringstream s;
//    s << "var" << i;
//    RooRealVar* var = new RooRealVar(s.str().c_str(),"", 0, -10, 10);
//    RooArgSet argset(*var);
//    
//    RooDataSet * sub_data = new RooDataSet("data", "data", argset);
//    
//    for (int j=0; j<j_max; ++j) {
//      var->setVal(5);
//      sub_data->add(argset);
//    }
//    
//    sdebug << "merge" << endmsg;
//    data_test->merge(sub_data);
//    delete sub_data;
//  }
//  data_test->Print();
//  sw.Stop();
//  
//  sdebug << "C: " << sw.CpuTime() << ", R: " << sw.RealTime() << endmsg;
//  
//  delete data_test;
//  sw.Reset();
//  sw.Start();
//
//  RooArgSet argset;
//  for (int i=0; i<i_max; ++i) {
//    ostringstream s;
//    s << "var" << i;
//    RooRealVar* var = new RooRealVar(s.str().c_str(),"", 0, -10, 10);
//    argset.add(*var);
//  }
//  
//  data_test = new RooDataSet("data", "data", argset);
//  
//  for (int j=0; j<j_max; ++j) {
//    for (int i=0; i<i_max; ++i) {
//      ostringstream s;
//      s << "var" << i;
//      RooRealVar* var = (RooRealVar*)argset.find(s.str().c_str());
//      var->setVal(5);
//    }
//  
//    data_test->add(argset);
//  }
//  data_test->Print();
//  sw.Stop();
//  
//  sdebug << "C: " << sw.CpuTime() << ", R: " << sw.RealTime() << endmsg;
  
  TestToys(argc, argv);
  
  gDirectory->Print();
  gObjectTable->Print();
  
  gDirectory = NULL;
  gObjectTable = NULL;
  
  for (int i=0; i<100000; ++i) {
    for (int i=0; i<10000; ++i) {}
  }
}