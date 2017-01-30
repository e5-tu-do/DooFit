#include "doofit/toy/ToyStudyStd/ToyStudyStdConfig.h"

// STL
#include <map>
#include <utility>

// Boost
#include "boost/filesystem.hpp"
#include "boost/regex.hpp"

// ROOT

// RooFit
#include "RooFormulaVar.h"

// from project
#include "doocore/io/MsgStream.h"
#include "doofit/config/CommaSeparatedPair.h"
#include "doofit/config/CommaSeparatedList.h"

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
  plot_symmetric_around_mean_(false),
  plot_on_full_range_(false),
  neglect_parameters_at_limit_(false),
  neglect_minos_problems_(false),
  min_acceptable_cov_matrix_quality_(3),
  plot_parameter_vs_error_correlation_(false),
  evaluate_reference_toys_(false),
  reference_toys_id_(0),
  num_toys_read_(-1)
  {
    swarn << "Usage of ToyStudyStdConfig::ToyStudyStdConfig() is not recommended!" <<endmsg;
  }
  
  ToyStudyStdConfig::ToyStudyStdConfig(const std::string& name) :
  config::AbsConfig(name),
  handle_asymmetric_errors_(true),
  min_acceptable_cov_matrix_quality_(3),
  plot_parameter_vs_error_correlation_(false),
  evaluate_reference_toys_(false),
  reference_toys_id_(0),
  num_toys_read_(-1)
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
    if (store_converted_result_filename_treename_.first().size() > 0) {
      scfg << "File and tree to save converted result to: " << store_result_filename_treename_ << endmsg;
    }
    
    for (vector<config::CommaSeparatedPair<std::string>>::const_iterator it = read_results_filename_treename_.begin(); it != read_results_filename_treename_.end(); ++it) {
      scfg << "File and tree to read result from: " << *it << endmsg;
    }
    
    if (plot_directory_.size() > 0) {
      scfg << "Plot directory:                    " << plot_directory_ << endmsg;
    }
    
    scfg << "Using asymmetric errors for pulls: " << handle_asymmetric_errors_ << endmsg;
    scfg << "Fit/plot parameters on quantile window: " << fit_plot_on_quantile_window_ << endmsg;
    scfg << "Plot parameters centered around mean: " << plot_symmetric_around_mean_ << endmsg;
    scfg << "Plot parameters on full range: " << plot_on_full_range_ << endmsg;
    scfg << "Neglect parameters at limit:       " << neglect_parameters_at_limit_ << endmsg;
    scfg << "Neglect MINOS problems:            " << neglect_minos_problems_ << endmsg;
    for (int i = 0; i<minos_parameters_.size(); i++) {
      scfg << "MINOS check for parameters:      " << minos_parameters_[i] << endmsg;
    }
    if (parameter_genvalue_read_file().size()>0) {
      scfg << "Read generation values from:       " << parameter_genvalue_read_file() << endmsg;
    }
    if (parameter_fitbias_read_file().size()>0) {
      scfg << "Read fit biases from:       " << parameter_fitbias_read_file() << endmsg;
    }
    scfg << "Plotting of parameter vs error:    " << plot_parameter_vs_error_correlation_ << endmsg;

    scfg << "Perform reference toy analysis:    " << evaluate_reference_toys() << endmsg;
    if (evaluate_reference_toys()) {
      scfg << "Reference toy base id to evaluate: " << reference_toys_id() << endmsg;
    }

    if (num_toys_read() >= 0) {
      scfg << "Number of toys to read in:         " << num_toys_read() << endmsg;
    } else {
      scfg << "Number of toys to read in:         " << "all" << endmsg;
    }
  }
  
  void ToyStudyStdConfig::DefineOptions() {
    po::options_description* generation = new po::options_description("Toy study options");
    
    generation->add_options()
    (GetOptionString("store_result_filename_treename").c_str(), po::value<config::CommaSeparatedPair<std::string>>(&store_result_filename_treename_),"File name and tree name to save fit results to (set as filename,treename)")
    (GetOptionString("store_converted_result_filename_treename").c_str(), po::value<config::CommaSeparatedPair<std::string>>(&store_converted_result_filename_treename_),"File name and tree name to save converted fit results to (set as filename,treename)")
    (GetOptionString("fit_result1_branch_name").c_str(), po::value<std::string>(&fit_result1_branch_name_)->default_value("fit_results"),"Fit result 1 branch name in tree")
    (GetOptionString("fit_result2_branch_name").c_str(), po::value<std::string>(&fit_result2_branch_name_)->default_value("fit_results2"),"Fit result 2 branch name in tree")
    (GetOptionString("read_results_filename_treename").c_str(), po::value<vector<config::CommaSeparatedPair<std::string>> >(&read_results_filename_treename_)->composing(), "File names and tree names to read fit results from (set as filename,treename)")
    (GetOptionString("read_results_filename_treename_pattern").c_str(), po::value<config::CommaSeparatedPair<std::string>>(&read_results_filename_treename_pattern_),"File name pattern and tree name to read fit result from (set as regexfilenamepattern,treename)")
    (GetOptionString("plot_directory").c_str(), po::value<std::string>(&plot_directory_), "Plot directory for evaluation of fit results")
    (GetOptionString("handle_asymmetric_errors").c_str(), po::value<bool>(&handle_asymmetric_errors_)->default_value(true),"Set to false to not use asymmetric errors for pull calculation (c.f. CDF/ANAL/PUBLIC/5776). Default is true. If unsure, use asymmetric errors.")
    (GetOptionString("fit_plot_on_quantile_window").c_str(), po::value<bool>(&fit_plot_on_quantile_window_)->default_value(true),"Fit and plot pulls and other distributions on a sensible quantile based window instead of the full dataset (default: true; use this to avoid influence of pull values of obviously failed fits and other outliers).")
    (GetOptionString("plot_symmetric_around_mean").c_str(), po::value<bool>(&plot_symmetric_around_mean_)->default_value(false),"Plot pulls and other distributions centered around mean of Gaussian fit (default: false;)")
    (GetOptionString("plot_on_full_range").c_str(), po::value<bool>(&plot_on_full_range_)->default_value(false),"Plot pulls and other distributions on full dataset (default: false; use this to investigate outliers).")
    (GetOptionString("neglect_parameters_at_limit").c_str(), po::value<bool>(&neglect_parameters_at_limit_)->default_value(false),"Neglect any toy fit where at least one parameter is near the defined limits (default: false).")
    (GetOptionString("neglect_minos_problems").c_str(), po::value<bool>(&neglect_minos_problems_)->default_value(false),"Neglect any toy fit where at least one parameter has MINOS problems (default: false; only applies, if MINOS was run).")
    (GetOptionString("parameter_genvalue_read_file").c_str(), po::value<std::string>(&parameter_genvalue_read_file_),"Read in generation values from this file instead of using the init values in the fit results (default: empty, i.e. use init values).")
    (GetOptionString("parameter_fitbias_read_file").c_str(), po::value<std::string>(&parameter_fitbias_read_file_),"Read in fit biases from this file (default: empty, i.e. assume nominal fitter is unbiased).")
    (GetOptionString("min_acceptable_cov_matrix_quality").c_str(), po::value<int>(&min_acceptable_cov_matrix_quality_),"Minimum acceptable covariance matrix quality for fit results.")
    (GetOptionString("plot_parameter_vs_error_correlation").c_str(), po::value<bool>(&plot_parameter_vs_error_correlation_)->default_value(false),"Plot correlation scatter plots of values of parameters vs. their errors (default: false)")
    (GetOptionString("evaluate_reference_toys").c_str(), po::value<bool>(&evaluate_reference_toys_)->default_value(false),"Perform reference toy analysis of each two toys with identical random seeds (default: false)")
    (GetOptionString("reference_toys_id").c_str(), po::value<int>(&reference_toys_id_)->default_value(0),"Reference toy analysis run id to regard as study to evaluate.")
    (GetOptionString("num_toys_read").c_str(), po::value<int>(&num_toys_read_)->default_value(-1),"Number of toys to read in (default: -1 meaning all)")
    (GetOptionString("minos_parameters").c_str(), po::value<config::CommaSeparatedList<std::string>>(&minos_parameters_)->composing(),"List of parameters that MINOS was run on (considered when checking for MINOS problems)");
    
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
          config::CommaSeparatedPair<std::string> result_file_tree;
          result_file_tree.Parse(result_file_tree_str);
          AddReadResultsFilenameTreename(result_file_tree);
        }
      }
    }
  }

  void ToyStudyStdConfig::AddAdditionalVariable(const RooFormulaVar* formula_var, const RooFormulaVar* formula_err, std::string title) {
    std::tuple<const RooFormulaVar*,const RooFormulaVar*, std::string> tpl(formula_var, formula_err, title);

    additional_variables_.emplace(formula_var->GetName(), tpl);
  }

} // namespace toy
} // namespace doofit
