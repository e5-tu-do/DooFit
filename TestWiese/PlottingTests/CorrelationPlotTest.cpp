// from STL

// from ROOT
#include "TFile.h"

// from RooFit
#include "RooFitResult.h"

// from TMVA

// from BOOST
#include <boost/lexical_cast.hpp>
#include <boost/assign/std/vector.hpp>
using namespace boost::assign;

// from DooCore
#include "doocore/io/MsgStream.h"

// from DooFit
#include "doofit/plotting/correlations/CorrelationPlot.h"

// from DooSelection

// from CPToyMC

// from here


using namespace doocore::io;

int main(int argc, char *argv[]){
  sinfo << "Starting CorrelationPlotTest..." << endmsg;
  std::string file_name = argv[1];
  std::string fit_result_name = argv[2];

  sinfo << "Opening file " << file_name << " with fit result " << fit_result_name << endmsg;



  TFile   fit_result_file(file_name.c_str(),"READ");
  RooFitResult* fit_result = dynamic_cast<RooFitResult*>(fit_result_file.Get(fit_result_name.c_str()));
  if (fit_result){
    fit_result_file.Close();
  }
  else{
    serr << "fit result pointer empty" << endmsg;
    abort();
  }
  fit_result->Print();

  // selected plot pars
  std::vector<std::string> plot_pars;
  plot_pars += "parSigTimeCjpsiKS_blind";
  plot_pars += "parSigTimeSin2b_blind";
  plot_pars += "parSigTimeDeltaM";
  plot_pars += "parSigTimeTau";
  plot_pars += "parSigEtaDeltaP0_OS";
  plot_pars += "parSigEtaDeltaP0_SS";
  plot_pars += "parSigEtaDeltaP1_OS";
  plot_pars += "parSigEtaDeltaP1_SS";
  plot_pars += "parSigEtaDeltaProd_11";
  plot_pars += "parSigEtaDeltaProd_12";
  plot_pars += "parSigEtaP0_OS";
  plot_pars += "parSigEtaP0_SS";
  plot_pars += "parSigEtaP1_OS";
  plot_pars += "parSigEtaP1_SS";

  // plotting the correlation matrix
  doofit::plotting::correlations::CorrelationPlot corr_plot(*fit_result, plot_pars);
  corr_plot.Plot("");
}
