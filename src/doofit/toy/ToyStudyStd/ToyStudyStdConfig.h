#ifndef TOYSTUDYSTDCONFIG_h
#define TOYSTUDYSTDCONFIG_h

// STL
#include <vector>
#include <string> 

// from project
#ifndef __CINT__
#include "doofit/config/AbsConfig.h"
#include "doofit/config/CommaSeparatedPair.h"
#else
// ROOT Cint hacks...
#include "../../Config/AbsConfig.h"
#include "../../Config/CommaSeparatedPair.h"
#endif /* __CINT __ */

// forward declarations

namespace doofit {
namespace toy {
  /** @class ToyStudyStdConfig
   *  @brief DooFit Config class for the ToyStudyStd class
   *
   *  This class is responsible for all ToyStudyStd related configuration options
   *  which are not covered by CommonConfig.
   *
   *  @author Florian Kruse
   */

  class ToyStudyStdConfig : public config::AbsConfig {
   public:
    /**
     *  @brief Default constructor for ToyStudyStdConfig
     *
     *  Unfortunately this constructor is needed for ROOT CINT and streaming to 
     *  ROOT files.
     */
    ToyStudyStdConfig();
    
    /**
     *  \brief Standard constructor for ToyStudyStdConfig
     *
     *  Sets all members to default values.
     *
     *  @param name Name of this Config object.
     */
    ToyStudyStdConfig(const std::string& name);
    
    /**
     *  @brief Destructor for ToyStudyStdConfig
     */
    ~ToyStudyStdConfig();
    
    /** @name Getter actual options
     *  Getter functions for actual options members.
     */
    ///@{
    /**
     *  @brief Getter for file name and tree name to store fit result to
     *
     *  @return current value of store_result_filename_treename_
     */
    const config::CommaSeparatedPair& store_result_filename_treename() const {return store_result_filename_treename_;}
    /**
     *  @brief Getter for fit result 1 branch name
     *
     *  @return current value of fit_result1_branch_name_
     */
    const std::string& fit_result1_branch_name() const {return fit_result1_branch_name_;}
    /**
     *  @brief Getter for fit result 2 branch name
     *
     *  @return current value of fit_result2_branch_name_
     */
    const std::string& fit_result2_branch_name() const {return fit_result2_branch_name_;}
    /**
     *  @brief Getter for file names and tree names to read fit result from
     *
     *  @return current value of read_results_filename_treename_
     */
    const std::vector<config::CommaSeparatedPair>& read_results_filename_treename() const {return read_results_filename_treename_;}
    /**
     *  @brief Getter for plot directory for evaluation of fit results
     *
     *  @return current value of plot_directory_
     */
    const std::string& plot_directory() const {return plot_directory_;}
    /**
     *  @brief Getter for for handling of asymmetric errors (c.f. CDF/ANAL/PUBLIC/5776)
     *
     *  @see ToyStudyStdConfig::set_handle_asymmetric_errors(bool)
     *  @return current value of handle_asymmetric_errors_
     */
    bool handle_asymmetric_errors() const {return handle_asymmetric_errors_;}
    /**
     *  @brief Getter for fitting and plotting pulls on quantile window instead of full range
     *
     *  @return current value of fit_plot_on_quantile_window_
     */
    bool fit_plot_on_quantile_window() const {return fit_plot_on_quantile_window_;}
    /**
     *  @brief Getter for neglecting all toy fits where at least one parameter is at/near limit
     *
     *  @return current value of neglect_parameters_at_limit_
     */
    bool neglect_parameters_at_limit() const {return neglect_parameters_at_limit_;}
    /**
     *  @brief Getter for neglecting all toy fits where at least one parameter has MINOS problems
     *
     *  @return current value of neglect_minos_problems_
     */
    bool neglect_minos_problems() const {return neglect_minos_problems_;}
    /**
     *  @brief Getter for file to read generation values of parameters from 
     *
     *  @return current value of parameter_genvalue_read_file_
     */
    const std::string& parameter_genvalue_read_file() const {return parameter_genvalue_read_file_;}
    
    /**
     *  @brief Getter for minimum acceptable covariance matrix quality for fit results
     *
     *  When evaluating toy fit results, a check for the covariance matrix
     *  quality is made berforehand. Only results with a minimum quality of
     *  min_acceptable_cov_matrix_quality will be accepted for evaluation.
     *  See RooFitResult::covQual() for reference.
     *
     *  @return current value of min_acceptable_cov_matrix_quality_
     */
    int min_acceptable_cov_matrix_quality() const  { return min_acceptable_cov_matrix_quality_; }
    ///@}

    /** @name Setter actual options
     *  Setter functions for actual options members.
     */
    ///@{
    /**
     *  @brief Setter for file name and tree name to store fit result to
     *
     *  @warning In its current implementation ToyStudyStd::StoreFitResult() 
     *           will lock the file before saving to allow multiple processes
     *           to save into one single file safely. However, this file locking 
     *           (although implemented as light-weight as possible) can render
     *           certain "high-performance parallel file systems" like FhGFS 
     *           unusable. This might lead to significant deadtimes while 
     *           waiting for a file lock when accessing the file from a certain
     *           amount of processes at the same time. In this case, one should
     *           revert to a simple NFS server or local disks for result files 
     *           which in general performs way better.
     *
     *  @param result_file_tree new value for store_result_filename_treename_
     */
    void set_store_result_filename_treename(const config::CommaSeparatedPair& result_file_tree) {store_result_filename_treename_ = result_file_tree;}
    /**
     *  @brief Setter for fit result 1 branch name
     *
     *  @param fit_result1_branch_name new value for fit_result1_branch_name_
     */
    void set_fit_result1_branch_name(const std::string& fit_result1_branch_name) {fit_result1_branch_name_ = fit_result1_branch_name;}
    /**
     *  @brief Setter for fit result 2 branch name
     *
     *  @param fit_result2_branch_name new value for fit_result2_branch_name_
     */
    void set_fit_result2_branch_name(const std::string& fit_result2_branch_name) {fit_result2_branch_name_ = fit_result2_branch_name;}
    /**
     *  @brief Setter for file name pattern and tree name to read fit result from
     *
     *  Instead of specifying each file to read in separately, a pattern for 
     *  file names can be specified. Each file matching this regular expression
     *  pattern will be included to the file list. The regular expression engine
     *  used is boost::regex.
     *
     *  @see set_read_results_filename_treename()
     *
     *  @param result_file_tree new value for store_result_filename_treename_ as comma-separated string "pattern,treename"
     */
    void set_read_results_filename_treename_pattern(const config::CommaSeparatedPair& read_results_filename_treename_pattern) {read_results_filename_treename_pattern_ = read_results_filename_treename_pattern;}
    /**
     *  @brief Setter for file names and tree names to read fit result from
     *
     *  Set the file names and tree names (inside these files) which are supposed
     *  to be read in for automated or manual analysis. Each file name, tree 
     *  name combination needs to be specified as a config::CommaSeparatedPair.
     *
     *  @param result_file_tree new value for read_results_filename_treename_
     */
    void set_read_results_filename_treename(const std::vector<config::CommaSeparatedPair>& results_file_tree) {read_results_filename_treename_ = results_file_tree;}
    /**
     *  @brief Setter for one file and tree to read fit result from
     *
     *  Set just one file name and tree name (inside this file) which is supposed
     *  to be read in for automated or manual analysis. The file name, tree
     *  name combination needs to be specified as a config::CommaSeparatedPair.
     *
     *  @param result_file_tree file and tree name to read
     */
    void set_read_results_filename_treename(config::CommaSeparatedPair results_file_tree) {
      read_results_filename_treename_ = std::vector<config::CommaSeparatedPair>();
      read_results_filename_treename_.push_back(results_file_tree);
    }
    /**
     *  @brief Setter for plot directory for evaluation of fit results
     *
     *  @param plot_directory new value for plot_directory_
     */
    void set_plot_directory(const std::string& plot_directory) {plot_directory_ = plot_directory;}
    /**
     *  @brief Setter for handling of asymmetric errors (c.f. CDF/ANAL/PUBLIC/5776)
     *
     *  In case of asymmetric errors for fitted parameters, pulls should 
     *  normally be calculated using these asymmetric errors to get standard 
     *  distributed pull distribution. Refer to CDF/ANAL/PUBLIC/5776 for 
     *  details. By default, ToyStudyStdConfig will use asymmetric errors 
     *  automatically if available. This option is only needed if the 
     *  symmetric errors of the parameters are to be used @a always.
     *
     *  @param handle_asymmetric_errors whether to handle asymmetric errors (true) or not
     */
    void set_handle_asymmetric_errors(bool handle_asymmetric_errors) {handle_asymmetric_errors_ = handle_asymmetric_errors;}
    /**
     *  @brief Setter for fitting and plotting pulls on quantile window instead of full range
     *
     *  @param fit_plot_on_quantile_window new value for fit_plot_on_quantile_window_
     */
    void set_fit_plot_on_quantile_window(bool& fit_plot_on_quantile_window) {fit_plot_on_quantile_window_ = fit_plot_on_quantile_window;}
    /**
     *  @brief Setter for neglecting all toy fits where at least one parameter is at/near limit
     *
     *  @param neglect_parameters_at_limit new value for neglect_parameters_at_limit_
     */
    void set_neglect_parameters_at_limit(bool& neglect_parameters_at_limit) {neglect_parameters_at_limit_ = neglect_parameters_at_limit;}
    /**
     *  @brief Setter for neglecting all toy fits where at least one parameter has MINOS problems
     *
     *  @param neglect_minos_problems new value for neglect_minos_problems_
     */
    void set_neglect_minos_problems(bool& neglect_minos_problems) {neglect_minos_problems_ = neglect_minos_problems;}
    /**
     *  @brief Setter for file to read generation values of parameters from 
     *
     *  To calculate a pull based on different values as generation values than
     *  the init values used in the fit, specifying a parameter file here gives
     *  the possibility to read in different parameter values than the 
     *  initialization values stored in the fit result. If not specified, the 
     *  init values of the fit results will be used
     *
     *  @param parameter_genvalue_read_file new value for parameter_genvalue_read_file_
     */
    void set_parameter_genvalue_read_file(const std::string& parameter_genvalue_read_file) {parameter_genvalue_read_file_ = parameter_genvalue_read_file;}
    
    /**
     *  @brief Setter for minimum acceptable covariance matrix quality for fit results
     *
     *  When evaluating toy fit results, a check for the covariance matrix 
     *  quality is made berforehand. Only results with a minimum quality of 
     *  min_acceptable_cov_matrix_quality will be accepted for evaluation. 
     *  See RooFitResult::covQual() for reference.
     *
     *  @param min_acceptable_cov_matrix_quality new value for min_acceptable_cov_matrix_quality_
     */
    void set_min_acceptable_cov_matrix_quality(int min_acceptable_cov_matrix_quality) {min_acceptable_cov_matrix_quality_ = min_acceptable_cov_matrix_quality;}
    
    /**
     *  @brief Adder for file names and tree names to read fit result from
     *
     *  @param result_file_tree new file and tree name to add to read_results_filename_treename_
     */
    void AddReadResultsFilenameTreename(const config::CommaSeparatedPair& result_file_tree) {read_results_filename_treename_.push_back(result_file_tree);}
    ///@}
    
   protected:
    /** @name Virtual functions
     *  Implementation of pure virtual functions.
     */
    ///@{
    /**
     *  \brief Define options for the derived class.
     *
     *  This function will define all options for the derived CommonConfig class.
     * 
     *  @see config::AbsConfig::DefineOptions()
     */
    void DefineOptions();
    
    /**
     *  \brief Load program options into own member variables.
     *
     *  This function will store all options from the variable map into own 
     *  members.
     *  
     *  @see config::AbsConfig::LoadOptions()
     */
    void LoadOptions();
    
    /**
     *  \brief Print all options for this object.
     *
     *  Virtual function that will print all options for this Config object.
     */
    void PrintOptions() const;
    ///@}
    
    /**
     *  @brief Parse read file pattern and extend read file/tree list
     *
     *  This function will parse read_results_filename_treename_pattern_ and 
     *  extend read_results_filename_treename_ accordingly.
     */
    void ParseReadFileTreePattern();
    
   private:
    /** @name Actual options
     *  The actual options members.
     */
    ///@{
    /**
     *  @brief File name and tree name to store fit result to
     *
     *  @see ToyStudyStdConfig::set_store_result_filename_treename()
     */
    config::CommaSeparatedPair store_result_filename_treename_;
    /**
     *  @brief Fit result 1 branch name
     *
     *  @see ToyStudyStdConfig::set_fit_result1_branch_name()
     */
    std::string fit_result1_branch_name_;
    /**
     *  @brief Fit result 2 branch name
     *
     *  @see ToyStudyStdConfig::set_fit_result2_branch_name()
     */
    std::string fit_result2_branch_name_;
    /**
     *  @brief File names and tree names to read fit result from
     */
    std::vector<config::CommaSeparatedPair> read_results_filename_treename_;
    /**
     *  @brief File name pattern and tree name to read fit result from
     *
     *  The list in read_results_filename_treename_ will be extended by all
     *  files matching this pattern.
     *
     *  @see ToyStudyStdConfig::set_read_results_filename_treename_pattern()
     */
    config::CommaSeparatedPair read_results_filename_treename_pattern_;
    /**
     *  @brief Plot directory for evaluation of fit results
     */
    std::string plot_directory_;
    /**
     *  @brief Handle asymmetric errors correctly (c.f. CDF/ANAL/PUBLIC/5776)
     */
    bool handle_asymmetric_errors_;
    /**
     *  @brief Fit and plot pulls on a sensible quantile based window instead of full range (neglect outliers)
     */
    bool fit_plot_on_quantile_window_;
    /**
     *  @brief Neglect all toy fits where at least one parameter is at/near limit
     */
    bool neglect_parameters_at_limit_;
    /**
     *  @brief Neglect all toy fits where at least one parameter has MINOS problems
     */
    bool neglect_minos_problems_;
    /**
     *  @brief File to read generation values of parameters from before pull calculation
     */
    std::string parameter_genvalue_read_file_;
    
    /**
     *  @brief Minimum acceptable covariance matrix quality for fit results
     */
    int min_acceptable_cov_matrix_quality_;
    ///@}
  };
} // namespace toy
} // namespace doofit

#endif // TOYSTUDYSTDCONFIG_h
