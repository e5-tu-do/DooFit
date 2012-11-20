#include "doofit/toy/ToyStudyStd/ToyStudyStdConfig.h"

// STL

// Boost
#include "boost/filesystem.hpp"
#include "boost/regex.hpp"

// ROOT

// RooFit

// from project
#include "doocore/io/MsgStream.h"
#include "doofit/config/CommaSeparatedPair.h"

using namespace std;
namespace po = boost::program_options;
using namespace doocore::io;

namespace doofit {
namespace toy {
  ToyStudyStdConfig::ToyStudyStdConfig() : 
  config::AbsConfig("empty_name"),
  fit_result1_branch_name_("fit_results"),
  fit_result2_branch_name_("fit_results2"),
  handle_asymmetric_errors_(true),
  fit_plot_on_quantile_window_(true),
  neglect_parameters_at_limit_(false),
  neglect_minos_problems_(false)
  {
    swarn << "Usage of ToyStudyStdConfig::ToyStudyStdConfig() is not recommended!" <<endmsg;
  }
  
  ToyStudyStdConfig::ToyStudyStdConfig(const std::string& name) :
  config::AbsConfig(name),
  handle_asymmetric_errors_(true)
  {
  }
  
  ToyStudyStdConfig::~ToyStudyStdConfig() {
  }
  
  void ToyStudyStdConfig::PrintOptions() const {
    if (store_result_filename_treename_.first().size() > 0) {
      scfg << "File and tree to save result to:   " << store_result_filename_treename_ << endmsg;
      scfg << "Branch name fit result 1:          " << fit_result1_branch_name_ << endmsg;
      scfg << "Branch name fit result 2:          " << fit_result2_branch_name_ << endmsg;
    }
    
    for (vector<config::CommaSeparatedPair>::const_iterator it = read_results_filename_treename_.begin(); it != read_results_filename_treename_.end(); ++it) {
      scfg << "File and tree to read result from: " << *it << endmsg;
    }
    
    if (plot_directory_.size() > 0) {
      scfg << "Plot directory:                    " << plot_directory_ << endmsg;
    }
    
    scfg << "Using asymmetric errors for pulls: " << handle_asymmetric_errors_ << endmsg;
    scfg << "Fit/plot parameters on quantile window: " << fit_plot_on_quantile_window_ << endmsg;
    scfg << "Neglect parameters at limit:       " << neglect_parameters_at_limit_ << endmsg;
    scfg << "Neglect MINOS problems:            " << neglect_minos_problems_ << endmsg;
    if (parameter_genvalue_read_file().size()>0) {
      scfg << "Read generation values from:       " << parameter_genvalue_read_file() << endmsg;
    }
  }
  
  void ToyStudyStdConfig::DefineOptions() {
    po::options_description* generation = new po::options_description("Toy study options");
    
    generation->add_options()
    (GetOptionString("store_result_filename_treename").c_str(), po::value<config::CommaSeparatedPair>(&store_result_filename_treename_),"File name and tree name to save fit results to (set as filename,treename)")
    (GetOptionString("fit_result1_branch_name").c_str(), po::value<std::string>(&fit_result1_branch_name_)->default_value("fit_results"),"Fit result 1 branch name in tree")
    (GetOptionString("fit_result2_branch_name").c_str(), po::value<std::string>(&fit_result2_branch_name_)->default_value("fit_results2"),"Fit result 2 branch name in tree")
    (GetOptionString("read_results_filename_treename").c_str(), po::value<vector<config::CommaSeparatedPair> >(&read_results_filename_treename_)->composing(), "File names and tree names to read fit results from (set as filename,treename)")
    (GetOptionString("read_results_filename_treename_pattern").c_str(), po::value<config::CommaSeparatedPair>(&read_results_filename_treename_pattern_),"File name pattern and tree name to read fit result from (set as regexfilenamepattern,treename)")
    (GetOptionString("plot_directory").c_str(), po::value<std::string>(&plot_directory_), "Plot directory for evaluation of fit results")
    (GetOptionString("handle_asymmetric_errors").c_str(), po::value<bool>(&handle_asymmetric_errors_)->default_value(true),"Set to false to not use asymmetric errors for pull calculation (c.f. CDF/ANAL/PUBLIC/5776). Default is true. If unsure, use asymmetric errors.")
    (GetOptionString("fit_plot_on_quantile_window").c_str(), po::value<bool>(&fit_plot_on_quantile_window_)->default_value(true),"Fit and plot pulls and other distributions on a sensible quantile based window instead of the full dataset (default: true; use this to avoid influence of pull values of obviously failed fits and other outliers).")
    (GetOptionString("neglect_parameters_at_limit").c_str(), po::value<bool>(&neglect_parameters_at_limit_)->default_value(false),"Neglect any toy fit where at least one parameter is near the defined limits (default: false).")
    (GetOptionString("neglect_minos_problems").c_str(), po::value<bool>(&neglect_minos_problems_)->default_value(false),"Neglect any toy fit where at least one parameter has MINOS problems (default: false; only applies, if MINOS was run).")
    (GetOptionString("parameter_genvalue_read_file").c_str(), po::value<std::string>(&parameter_genvalue_read_file_),"Read in generation values from this file instead of using the init values in the fit results (default: empty, i.e. use init values).");
    
    descs_visible_.push_back(generation);
  }
  
  void ToyStudyStdConfig::LoadOptions() {
    ParseReadFileTreePattern();
  }
  
  void ToyStudyStdConfig::ParseReadFileTreePattern() {
    namespace fs = boost::filesystem;
    
    std::string filename_pattern      = read_results_filename_treename_pattern_.first();
    std::string treename              = read_results_filename_treename_pattern_.second();
    
    if (filename_pattern.size() > 0) {
      fs::path filename_pattern_path = fs::absolute(filename_pattern);
      fs::path parent_path           = filename_pattern_path.parent_path();
      fs::path filename_path         = filename_pattern_path.filename();
      
      const boost::regex filename_pattern_regex(filename_path.string());
      std::vector<std::string> all_matching_files;
      
      if (fs::is_directory(parent_path)) {
        boost::filesystem::directory_iterator end_itr; // Default ctor yields past-the-end
        for( boost::filesystem::directory_iterator i(parent_path); i != end_itr; ++i ) {
          
          // Skip if not a file
          if(!fs::is_regular_file(i->status())) continue;
          
          boost::smatch what;
          // Skip if no match
          if(!boost::regex_match(i->path().filename().string(), what, filename_pattern_regex)) continue;
          
          std::string result_file_tree_str = i->path().string() + "," + treename;
          config::CommaSeparatedPair result_file_tree;
          result_file_tree.Parse(result_file_tree_str);
          AddReadResultsFilenameTreename(result_file_tree);
        }
      }
    }
  }

} // namespace toy
} // namespace doofit
