#include "doofit/Toy/ToyStudyStd/ToyStudyStdConfig.h"

// STL

// Boost

// ROOT

// RooFit

// from project
#include "doofit/utils//MsgStream.h"
#include "doofit/Config/CommaSeparatedPair.h"

using namespace std;
namespace po = boost::program_options;
using namespace doofit::utils;

namespace doofit {
namespace Toy {
  ToyStudyStdConfig::ToyStudyStdConfig() : 
  Config::AbsConfig("empty_name"),
  handle_asymmetric_errors_(true),
  fit_on_plot_window_(true)
  {
    swarn << "Usage of ToyStudyStdConfig::ToyStudyStdConfig() is not recommended!" <<endmsg;
  }
  
  ToyStudyStdConfig::ToyStudyStdConfig(const std::string& name) :
  Config::AbsConfig(name),
  handle_asymmetric_errors_(true)
  {
  }
  
  ToyStudyStdConfig::~ToyStudyStdConfig() {
  }
  
  void ToyStudyStdConfig::PrintOptions() const {
    if (store_result_filename_treename_.first().size() > 0) {
      scfg << "File and tree to save result to:   " << store_result_filename_treename_ << endmsg;
    }
    
    for (vector<Config::CommaSeparatedPair>::const_iterator it = read_results_filename_treename_.begin(); it != read_results_filename_treename_.end(); ++it) {
      scfg << "File and tree to read result from: " << *it << endmsg;
    }
    
    if (plot_directory_.size() > 0) {
      scfg << "Plot directory:                    " << plot_directory_ << endmsg;
    }
    
    scfg << "Using asymmetric errors for pulls: " << handle_asymmetric_errors_ << endmsg;
    scfg << "Fit parameters on plot window:     " << fit_on_plot_window_ << endmsg;
  }
  
  void ToyStudyStdConfig::DefineOptions() {
    po::options_description* generation = new po::options_description("Toy study options");
    
    generation->add_options()
    (GetOptionString("store_result_filename_treename").c_str(), po::value<Config::CommaSeparatedPair>(&store_result_filename_treename_),"File name and tree name to save fit results to (set as filename,treename)")
    (GetOptionString("read_results_filename_treename").c_str(), po::value<vector<Config::CommaSeparatedPair> >(&read_results_filename_treename_)->composing(), "File names and tree names to read fit results from (set as filename,treename)")
    (GetOptionString("plot_directory").c_str(), po::value<std::string>(&plot_directory_), "Plot directory for evaluation of fit results")
    (GetOptionString("handle_asymmetric_errors").c_str(), po::value<bool>(&handle_asymmetric_errors_)->default_value(true),"Set to false to not use asymmetric errors for pull calculation (c.f. CDF/ANAL/PUBLIC/5776). Default is true. If unsure, use asymmetric errors.")
    (GetOptionString("fit_on_plot_window").c_str(), po::value<bool>(&fit_on_plot_window_)->default_value(true),"Fit pulls and other distributions on the plotting window instead of the full dataset (default: true; use this to avoid influence of pull values of obviously failed fits).");
    
    descs_visible_.push_back(generation);
  }
  
  void ToyStudyStdConfig::LoadOptions() {
  }

} // namespace Toy
} // namespace doofit
