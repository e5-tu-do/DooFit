// STL
#include <vector>
#include <sstream>
#include <utility>

// Boost
#include <boost/thread.hpp>
#include <boost/date_time.hpp>
#include <boost/assign/std/vector.hpp>

// ROOT

// RooFit
#include "RooWorkspace.h"
#include "RooDataSet.h"
#include "RooArgSet.h"
#include "RooRealVar.h"
#include "RooCategory.h"
#include "RooGaussian.h"
#include "RooExtendPdf.h"
#include "RooAddPdf.h" 
#include "RooProdPdf.h"
#include "RooFitResult.h"
#include "RooExponential.h"
#include "RooDecay.h"

// from DooCore
#include "doocore/io/MsgStream.h"

// from Project
#include "doofit/config/CommonConfig.h"
#include "doofit/toy/ToyFactoryStd/ToyFactoryStd.h"
#include "doofit/toy/ToyFactoryStd/ToyFactoryStdConfig.h"
#include "doofit/toy/ToyStudyStd/ToyStudyStd.h"
#include "doofit/toy/ToyStudyStd/ToyStudyStdConfig.h"

#include "doofit/plotting/Plot/Plot.h"

#include "doofit/builder/EasyPdf/EasyPdf.h"

using namespace boost::assign;
using namespace ROOT;
using namespace RooFit;
using namespace doocore::io;
using namespace doofit;
using namespace doofit::builder;
using namespace doofit::toy;
using namespace doofit::plotting;

/**
 *  @brief EasyPdf demo function
 *
 *  This function demonstrates PDF building via EasyPdf
 *
 *  @param epdf an EasyPdf builder to create PDFs inside
 *  @return the built complete PDF
 */
RooAbsPdf& BuildPDF(EasyPdf& epdf) {
  epdf.Var("mass").setMin(5100);
  epdf.Var("mass").setMax(5500);
  epdf.Var("time").setMin(0);
  epdf.Var("time").setMax(20);
    
  epdf.Vars("mass,time", "observables");
  epdf.Set("observables").Print();
  
  epdf.DoubleGaussianScaled("pdf_sig_mass", epdf.Var("mass"), epdf.Var("par_sig_mass_mean"),
                            epdf.Var("par_sig_mass_sigma"), epdf.Var("par_sig_mass_sigma_scale"),
                            epdf.Var("par_sig_mass_frac"));
                              
  // Gaussian model for time
  epdf.GaussModel("pdf_model_time", epdf.Var("time"), epdf.Var("par_model_bias"), epdf.Var("par_model_sigma"));
  
  epdf.Product("pdf_sig_nonext",
               RooArgList(epdf.Pdf("pdf_sig_mass"),
                          epdf.Decay("pdf_sig_time", epdf.Var("time"), epdf.Var("par_sig_time_tau"),
                                     epdf.Model("pdf_model_time"))));
  epdf.Product("pdf_bkg_nonext",
               RooArgList(epdf.Exponential("pdf_bkg_mass", epdf.Var("mass"), epdf.Var("par_bkg_mass_exp")),
                          epdf.Decay("pdf_bkg_time", epdf.Var("time"), epdf.Var("par_bkg_time_tau"),
                                     epdf.Model("pdf_model_time"))));
  epdf.Product("pdf_pbkg_nonext",
               RooArgList(epdf.Gaussian("pdf_pbkg_mass", epdf.Var("mass"), epdf.Var("par_pbkg_mass_mean"),
                                        epdf.Var("par_pbkg_mass_sigma")),
                          epdf.Decay("pdf_pbkg_time", epdf.Var("time"), epdf.Var("par_pbkg_time_tau"),
                                     epdf.Model("pdf_model_time"))));
   
  epdf.Extend("pdf_sig", epdf.Pdf("pdf_sig_nonext"), epdf.Var("par_sig_yield"));
  epdf.Extend("pdf_bkg", epdf.Pdf("pdf_bkg_nonext"), epdf.Var("par_bkg_yield"));
  epdf.Extend("pdf_pbkg", epdf.Pdf("pdf_pbkg_nonext"), epdf.Var("par_pbkg_yield"));
  
  epdf.Add("pdf", RooArgSet(epdf.Pdf("pdf_sig"), epdf.Pdf("pdf_bkg"), epdf.Pdf("pdf_pbkg")));
    
  epdf.Var("tag2").setMin(-10);
  epdf.Var("tag2").setMax(+10);
  
  epdf.Cat("tag").defineType("b0",     +1);
  epdf.Cat("tag").defineType("b0_bar", -1);
  
  // set parameters and limits from file
  epdf.AllVars().writeToFile("parameters.new");
  epdf.AllVars().readFromFile("parameters.txt");
  
  return epdf.Pdf("pdf");
}

/**
 *  @brief doofit::plotting demo function
 *
 *  This function demonstrates plotting via doofit::plotting
 *
 *  @param pdf PDF to plot
 *  @param data dataset to plot
 *  @param cfg_plot PlotConfig with plotting configuration
 */
void PlotResults(const RooAbsPdf& pdf, const RooAbsData& data, const doofit::plotting::PlotConfig& cfg_plot) {
  RooArgSet* obs = pdf.getObservables(data);
  
  std::vector<std::string> components;
  components += "pdf_sig", "pdf_bkg", "pdf_pbkg";
  
  // iterate over all observables for automated plotting
  TIterator* iter = obs->createIterator();
  RooAbsArg* arg  = NULL;
  while ((arg = dynamic_cast<RooAbsArg*>(iter->Next()))) {
    RooRealVar* var = dynamic_cast<RooRealVar*>(arg);
    if (var != NULL) {
      Plot myplot(cfg_plot, *var, data, pdf, components);
      myplot.PlotItLogNoLogY();
    }
  }
}

/**
 *  @brief doofit::toy demo function
 *
 *  This function demonstrates toy generation and evaluation
 *
 *  @param argc number of command line arguments
 *  @param argv command line arguments
 */
void TestToys(int argc, char *argv[]) {
  using namespace RooFit;
  
  // initalize all needed Config objects
  config::CommonConfig cfg_com("common");
  cfg_com.InitializeOptions(argc, argv);
  
  ToyFactoryStdConfig cfg_tfac("toyfac");
  cfg_tfac.InitializeOptions(cfg_com);
  
  ToyStudyStdConfig cfg_tstudy("toystudy");
  cfg_tstudy.InitializeOptions(cfg_tfac);
  
  PlotConfig cfg_plot("cfg_plot");
  cfg_plot.InitializeOptions(cfg_tstudy);
  
  cfg_com.CheckHelpFlagAndPrintHelp();
  
  // build the PDF and set everything necessary
  RooWorkspace* ws = new RooWorkspace("ws");
  EasyPdf epdf(ws);
  RooAbsPdf& pdf = BuildPDF(epdf);
  ws->defineSet("argset_obs", epdf.Vars("mass,time,tag2,tag"));
  cfg_tfac.set_workspace(ws);

  // print configuration
  cfg_com.PrintAll();
  
  ws->Print("v");
  
  // toy sample generation
  ToyFactoryStd tfac(cfg_com, cfg_tfac);
  RooDataSet* data = NULL;
  ToyStudyStd tstudy(cfg_com, cfg_tstudy);

  // generate 10 toy samples, fit and store results
  for (int i=0; i<10; ++i) {
    data = tfac.Generate();
    RooFitResult* fit_result = pdf.fitTo(*data, NumCPU(8), Extended(true), Save(true), Verbose(false), Timer(true), Minimizer("Minuit2"));
    
    fit_result->Print("v");
    
    PlotResults(pdf, *data, cfg_plot);
    
    tstudy.StoreFitResult(fit_result);
    delete data;
  }
  tstudy.FinishFitResultSaving();
  
  // evaluate previously generated samples
  tstudy.ReadFitResults();
  tstudy.EvaluateFitResults();
  tstudy.PlotEvaluatedParameters();
  
  delete ws;
}

int main(int argc, char *argv[]) {
  TestToys(argc, argv);
}



