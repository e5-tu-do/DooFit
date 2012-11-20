// STL
#include <vector>
#include <sstream>
#include <utility>

// Boost
#include <boost/thread.hpp>
#include <boost/date_time.hpp>
#include <boost/assign/std/vector.hpp>

// ROOT
#include "TCanvas.h"
#include "TFile.h"
#include "TStopwatch.h"
#include "TDirectory.h"
#include "TROOT.h"
#include "TObjectTable.h"
#include "TStopwatch.h"

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
#include "RooExponential.h"

// from DooCore
#include "doocore/io/MsgStream.h"

// from Project
#include "doofit/config/CommonConfig.h"

#include "doofit/plotting/Plot/Plot.h"

#include "doofit/builder/BuilderStd/BuilderStd.h"
#include "doofit/builder/BuilderStd/BuilderStdConfig.h"

#include "doofit/pdf2ws/pdf2wsstd/Pdf2WsStdMass.h"
#include "doofit/pdf2ws/pdf2wsstd/Pdf2WsStdCommonFuncs.h"

#include "doofit/toy/ToyFactoryStd/ToyFactoryStd.h"
#include "doofit/toy/ToyFactoryStd/ToyFactoryStdConfig.h"

#include "doofit/toy/ToyStudyStd/ToyStudyStd.h"
#include "doofit/toy/ToyStudyStd/ToyStudyStdConfig.h"

#include "doocore/io/MsgStream.h"

#include "doofit/builder/EasyPdf/EasyPdf.h"

using namespace ROOT;
using namespace RooFit;
using namespace doofit;
using namespace doocore::io;
using namespace doofit::builder;
using namespace boost::assign;

/// just a helper function to build a PDF
RooAbsPdf* BuildPDF(EasyPdf& epdf, RooWorkspace* ws) {
  epdf.Var("mass").setMin(5100);
  epdf.Var("mass").setMax(5500);
  epdf.Var("time").setMin(5100);
  epdf.Var("time").setMax(5500);
  
  epdf.Formula("massShift", "@0-100", RooArgList(epdf.Var("mass")));
  
  epdf.Vars("mass,time", "observables");
  epdf.Set("observables").Print();
  
  epdf.Add("test1",
          RooArgSet(epdf.Gaussian("test1_1", epdf.Formula("massShift"), epdf.Var("mean1"),
                                  epdf.Var("abweichung_1")),
                    epdf.Gaussian("test1_2", epdf.Var("mass"), epdf.Var("mean1"),
                                  epdf.Var("abweichung_2"))),
          RooArgSet(epdf.Var("dgauss_frac")));
  
  epdf.Product("pdf_prod1",
               RooArgList(epdf.Pdf("test1"),
                          epdf.Gaussian("time1", epdf.Var("time"), epdf.Var("mean_time1"),
                                        epdf.Var("sigma_time1"))));
  epdf.Product("pdf_prod2",
               RooArgList(epdf.Exponential("test2", epdf.Var("mass"), epdf.Var("c")),
                          epdf.Gaussian("time2", epdf.Var("time"), epdf.Var("mean_time2"),
                                        epdf.Var("sigma_time2"))));
  epdf.Product("pdf_prod3",
               RooArgList(epdf.Gaussian("test3", epdf.Var("mass"), epdf.Var("mean3"),
                                        epdf.Var("abweichung_bkg2")),
                          epdf.Gaussian("time3", epdf.Var("time"), epdf.Var("mean_time3"),
                                        epdf.Var("sigma_time3"))));
   
  epdf.Extend("pdf_extend1", epdf.Pdf("pdf_prod1"), epdf.Var("yield1"));
  epdf.Extend("pdf_extend2", epdf.Pdf("pdf_prod2"), epdf.Var("yield2"));
  epdf.Extend("pdf_extend3", epdf.Pdf("pdf_prod3"), epdf.Var("yield3"));
  
  epdf.Add("pdf_add", RooArgSet(epdf.Pdf("pdf_extend1"), epdf.Pdf("pdf_extend2"), epdf.Pdf("pdf_extend3")));
    
  epdf.Var("tag2").setMin(-10);
  epdf.Var("tag2").setMax(+10);
  
  RooArgSet argset_obs("argset_obs");
  argset_obs.add(epdf.Var("mass"));
  argset_obs.add(epdf.Var("time"));
  argset_obs.add(epdf.Var("tag2"));
  
  RooCategory* tag = new RooCategory("tag", "tag");
  tag->defineType("b0", 1);
  tag->defineType("b0_bar", -1);
  ws->import(*tag);
  delete tag;
  tag = ws->cat("tag");
  
  argset_obs.add(*tag);
  ws->defineSet("argset_obs",argset_obs);  
  
  // adding external constraints
  //RooGaussian* pdf_constr_mean2 = pdf2wsstd::mass::Gaussian(ws, "pdf_constr_mean2", "Gaussian constraint pdf for mean2", "mean2", "mean2_mu", "mean2_sigma");
  //ws->defineSet("constraint_pdfs", "pdf_constr_mean2");
  
  return ws->pdf("pdf_add");
}

/// just a helper function to plot a single fit
void PlotToyFit(RooWorkspace* ws, const RooAbsPdf* pdf) {
  TFile f("data.root","read");
  RooDataSet* data = (RooDataSet*)f.Get("dataset");
  
  RooRealVar* mass = (RooRealVar*)pdf2wsstd::commonfuncs::getVar(ws, "mass", "", 0, 0, 0, "");
  RooRealVar* time = (RooRealVar*)pdf2wsstd::commonfuncs::getVar(ws, "time", "", 0, 0, 0, "");
  
  RooPlot* mass_frame = mass->frame();
  RooPlot* time_frame = time->frame();
  RooPlot* tag2_frame = ws->var("tag2")->frame();
  
  data->plotOn(mass_frame);
  pdf->plotOn(mass_frame);
  
  data->plotOn(time_frame);
  pdf->plotOn(time_frame);
  
  data->plotOn(tag2_frame);
  
  TCanvas c1("c1", "c1", 800, 600);
  mass_frame->Draw();
  c1.SaveAs("mass.pdf");
  
  time_frame->Draw();
  c1.SaveAs("time.pdf");
  
  tag2_frame->Draw();
  c1.SaveAs("c2.pdf");
  
  Roo1DTable* table = data->table(RooArgSet(*ws->cat("tag")));
  table->Print();
  delete table;
  
  delete mass_frame;
  delete tag2_frame;
  
  delete data;
}

/// using Doofit's toy mechanisms
void TestToys(int argc, char *argv[]) {
  using namespace toy;
  using namespace RooFit;
  
  config::CommonConfig cfg_com("common");
  cfg_com.InitializeOptions(argc, argv);
  
  ToyFactoryStdConfig cfg_tfac("toyfac");
  cfg_tfac.InitializeOptions(cfg_com);
  
  ToyStudyStdConfig cfg_tstudy("toystudy");
  cfg_tstudy.InitializeOptions(cfg_tfac);
  
  using namespace doofit::plotting;
  PlotConfig cfg_plot("cfg_plot");
  cfg_plot.InitializeOptions(cfg_tstudy);
  
  cfg_com.CheckHelpFlagAndPrintHelp();
  
  RooWorkspace* ws = new RooWorkspace("ws");
  EasyPdf epdf(ws);
  RooAbsPdf* pdf = BuildPDF(epdf, ws);
//  TFile ws_file("ws.root", "read");
//  RooWorkspace* ws = (RooWorkspace*)ws_file.Get("ws");
    
  cfg_tfac.set_workspace(ws);
  //cfg_tfac.set_generation_pdf_workspace("pdfFull");
  //cfg_tfac.set_argset_generation_observables_workspace("argset_obs");
  
  //cfg_tfac.set_argset_constraining_pdfs(ws->set("constraint_pdfs"));
  
  ToyFactoryStd tfac(cfg_com, cfg_tfac);

  cfg_com.PrintAll();
  
  RooDataSet* data = NULL;
  ToyStudyStd tstudy(cfg_com, cfg_tstudy);

  for (int i=0; i<20; ++i) {
    data = tfac.Generate();
    //    delete data;
    //    gObjectTable->Print(); 
    //    utils::sdebug << i << utils::endmsg;
    //  }
    
//    pdf->getParameters(data)->readFromFile("generation.par");
//    ws->pdf("pdf_constr_mean2")->getParameters(data)->readFromFile("generation.par");
    RooFitResult* fit_result = pdf->fitTo(*data, NumCPU(2), Extended(true), Save(true), Verbose(false),Timer(true), Minimizer("Minuit2"), ExternalConstraints(*ws->set("constraint_pdfs")));
    
    Plot myplot(cfg_plot, *ws->var("mass"), *data, RooArgList(*pdf, *ws->pdf("test1"), *ws->pdf("test2"), *ws->pdf("test3")));
    myplot.PlotItLogNoLogY();
    
//    utils::setStyle();
//    RooPlot* plot_frame = ws->var("mass")->frame();
//    data->plotOn(plot_frame);
//    ws->pdf("pdf_add")->plotOn(plot_frame);
//    utils::PlotResiduals("mass", plot_frame, ws->var("mass"), ws->pdf("pdf_add"),"Plot/", true);
    
    //PlotToyFit(ws, pdf);

    sinfo << "fit no " << i << endmsg;
    tstudy.StoreFitResult(fit_result);
    delete data;

  }
  tstudy.FinishFitResultSaving();
    
  tstudy.ReadFitResults();
  tstudy.EvaluateFitResults();
  tstudy.PlotEvaluatedParameters();
  
  //PlotToyFit(ws, pdf);
  delete ws;
  
  //while (true) {}
}

int main(int argc, char *argv[]) {
  TestToys(argc, argv);
}



