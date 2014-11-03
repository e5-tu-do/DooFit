// STL
#include <vector>
#include <sstream>
#include <utility>

// Boost
#include <boost/thread.hpp>
#include <boost/date_time.hpp>
#include <boost/assign/std/vector.hpp>

// ROOT
#include "Fit/Fitter.h"

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
#include "RooMinimizer.h"
#include "RooMinimizerFcn2.h"

// from DooCore
#include "doocore/io/MsgStream.h"
#include "doocore/io/EasyTuple.h"

// from DooFit
#include "doofit/config/CommonConfig.h"
#include "doofit/toy/ToyFactoryStd/ToyFactoryStd.h"
#include "doofit/toy/ToyFactoryStd/ToyFactoryStdConfig.h"
#include "doofit/toy/ToyStudyStd/ToyStudyStd.h"
#include "doofit/toy/ToyStudyStd/ToyStudyStdConfig.h"
#include "doofit/config/CommaSeparatedPair.h"

#include "doofit/plotting/Plot/Plot.h"

#include "doofit/builder/EasyPdf/EasyPdf.h"

using namespace boost::assign;
using namespace ROOT;
using namespace RooFit;
using namespace doocore::io;
using namespace doofit;
using namespace doofit::config;
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
  epdf.Var("mass").setMin(5300);
  epdf.Var("mass").setMax(5500);
  epdf.Var("time").setMin(0);
  epdf.Var("time").setMax(20);
    
  epdf.Vars("mass,time", "observables");
  epdf.Set("observables").Print();
  
  epdf.Gaussian("pdf_gauss", epdf.Var("mass"), epdf.Var("par_sig_mass_mean"), epdf.Var("par_sig_mass_sigma"));

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
  
  // define set of observables
  epdf.Vars("mass,time,tag2,tag", "argset_obs");
  
  // set parameters and limits from file
  epdf.AllVars().writeToFile("parameters.new");
  epdf.AllVars().readFromFile("parameters.txt");
  
  return epdf.Pdf("pdf_gauss");
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
void TestFitter(int argc, char *argv[]) {
  using namespace RooFit;
  using namespace doocore::io;
  
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
  EasyPdf epdf;
  RooAbsPdf& pdf = BuildPDF(epdf);

  epdf.Var("par_sig_mass_mean").setMin(5300);
  epdf.Var("par_sig_mass_mean").setMax(5450);
  epdf.Var("par_sig_mass_mean").setVal(5366);
  epdf.Var("par_sig_mass_mean").setConstant(false);

  epdf.Var("par_sig_mass_sigma").setMin(0);
  epdf.Var("par_sig_mass_sigma").setMax(20);
  epdf.Var("par_sig_mass_sigma").setVal(8);
  epdf.Var("par_sig_mass_sigma").setConstant(false);

  cfg_tfac.set_easypdf(&epdf);
  cfg_tfac.set_argset_generation_observables_workspace("observables");
  cfg_tfac.set_generation_pdf_workspace("pdf_gauss");
  cfg_tfac.set_expected_yield(10000);

  config::CommaSeparatedPair p;
  p = "bla.root,tree";
  cfg_tstudy.set_read_results_filename_treename(p);
  
  // print configuration
  cfg_com.PrintAll();
  
  // toy sample generation
  ToyFactoryStd tfac(cfg_com, cfg_tfac);
  RooDataSet* data = NULL;
  ToyStudyStd tstudy(cfg_com, cfg_tstudy);

  // generate 10 toy samples, fit and store results
  for (int i=0; i<1; ++i) {
    data = tfac.Generate();
    data->Print();

#ifndef __ROOFIT_NOROOMINIMIZER
    swarn << "__ROOFIT_NOROOMINIMIZER not defined!" << endmsg;
#endif
#ifdef __ROOFIT_NOROOMINIMIZER
    swarn << "__ROOFIT_NOROOMINIMIZER defined!" << endmsg;
#endif

    sinfo << "Fitting myself..." << endmsg;
    RooAbsReal* nll(pdf.createNLL(*data));
    RooMinimizer minimizer(*nll);

    ROOT::Fit::Fitter fitter;
    RooMinimizerFcn2 fcn(nll, &minimizer);
    
    fitter.Config().MinimizerOptions().SetTolerance(1.0);
    // default max number of calls
    fitter.Config().MinimizerOptions().SetMaxIterations(500*fcn.NDim());
    fitter.Config().MinimizerOptions().SetMaxFunctionCalls(500*fcn.NDim());

    // Shut up for now
    fitter.Config().MinimizerOptions().SetPrintLevel(0);

    // Use +0.5 for 1-sigma errors
    fitter.Config().MinimizerOptions().SetErrorDef(nll->defaultErrorLevel());


    fitter.Config().MinimizerOptions().SetPrintLevel(2);    

    fcn.Synchronize(fitter.Config().ParamsSettings(), false, false);
    fitter.Config().SetMinimizer("Minuit2", "minimize");

    RooAbsReal::setEvalErrorLoggingMode(RooAbsReal::CollectErrors) ;
    RooAbsReal::clearEvalErrorLog() ;

    sinfo << "Fitting myself: The magic begins." << endmsg;
    bool ret = fitter.FitFCN(fcn);

    sinfo << "Result: " << fitter.Result().Status() << endmsg;

    RooAbsReal::setEvalErrorLoggingMode(RooAbsReal::PrintErrors) ;

    // minimizer.setMinimizerType("Minuit2");
    // minimizer.minimize("Minuit2", "minimize");
    // sinfo << "Fitting myself: Now comes the Hessian!" << endmsg;
    // minimizer.hesse();
    sinfo << "Fitting myself: done!" << endmsg;

    RooFitResult* fit_result = pdf.fitTo(*data, NumCPU(8), Extended(false), Save(true), Verbose(false), Timer(true), Minimizer("Minuit2","minimize"));
    
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
}

int main(int argc, char *argv[]) {
  TestFitter(argc, argv);
}
