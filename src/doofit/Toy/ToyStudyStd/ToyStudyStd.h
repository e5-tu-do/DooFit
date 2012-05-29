#ifndef TOYSTUDYSTD_h
#define TOYSTUDYSTD_h

// STL

// BOOST
#include <boost/thread.hpp>

// ROOT

// from RooFit
#include "RooArgSet.h"
#include "RooFitResult.h"

// from project
#include "doofit/Toy/ToyStudyStd/ToyStudyStdConfig.h"
#include "doofit/utils/utils.h"

// forward declarations
class RooFitResult;
class RooDataSet;
class RooRealVar;

namespace doofit {
  namespace Config {
    class CommonConfig; 
  }
  
namespace Toy {
  /** @class ToyStudyStd
   *  @brief Standard toy study for DooFit to help conduct and evaluate mass toy fits
   *
   *  This class is designed to assist in both conducting mass toy fits as well 
   *  evaluating results from these fits. It's design is not to conduct the 
   *  generation or actual fit itself. Other modules like ToyFactoryStd and the 
   *  fitter module ... are well suited for that.
   * 
   */

  class ToyStudyStd {
   public:
    /**
     *  @brief Default constructor for ToyStudyStd
     *
     *  This is the default constructor for ToyStudyStd involving general 
     *  configuration @a cfg_com and specific configuration @a cfg_tstudy.
     *
     *  @param cfg_com CommonConfig for the general configuration.
     *  @param cfg_tstudy ToyStudyStdConfig for this specific toy study.
     */
    ToyStudyStd(const Config::CommonConfig& cfg_com, const ToyStudyStdConfig& cfg_tstudy);
    
    /**
     *  @brief Destructor for ToyStudyStd
     *
     *  Detailed description.
     *
     */
    ~ToyStudyStd();
    
    /** @name Fit helper functions
     *  Helper functions to conduct fits and save results
     */
    ///@{
    /**
     *  @brief Store one or more fit results into TTree
     *
     *  This function will save the given fit result(s) to a TTree in a ROOT 
     *  file. 
     *  The latter can be configured via 
     *  ToyStudyStdConfig::set_store_result_filename_treename(). Writing the 
     *  tree to the file is collision-safe, i.e. the file will be locked before 
     *  write and if locked the function will wait for the file to be unlocked 
     *  again. If the file will not be unlocked after some time, the tree will 
     *  be saved to an alternative file.
     *  
     *  If the branch name(s) are specified, it is assumed that in case the 
     *  file already exists, the tree and given branches also exist in the 
     *  accordingly.
     *
     *  @see const Config::CommaSeparatedPair& ToyStudyStdConfig::store_result_filename_treename
     *  @see ToyStudyStdConfig
     *  @see ToyStudyStdConfig::set_store_result_filename_treename()
     *
     *  @warning See warnings for doofit::Toy::ToyStudyStdConfig::set_store_result_filename_treename(const Config::CommaSeparatedPair&)
     *
     *  @todo Implement fit saving to alternative file after no lock possible
     *
     *  @param fit_result1 first RooFitResult to save
     *  @param branch_name1 name of branch for first fit result (optional)
     *  @param fit_result2 second RooFitResult to save (optional)
     *  @param branch_name2 name of branch for second fit result (optional)
     */
    void StoreFitResult(const RooFitResult* fit_result1, 
                        const std::string& branch_name1="fit_results",
                        const RooFitResult* fit_result2=NULL, 
                        const std::string& branch_name2="fit_results2") const;
    ///@}

    /** @name Evaluation functions
     *  Functions to evaluate fit results and produce pulls etc.
     */
    ///@{
    /**
     *  @brief Read fit results from TTrees
     *
     *  A list of fit result files as defined in 
     *  ToyStudyStdConfig::set_read_results_filename_treename() will be read in
     *  and stored internally in the toy factory. 
     *
     *  @param branch_name name of branch for fit results (optional)
     */
    void ReadFitResults(const std::string& branch_name="fit_results");
    /**
     *  @brief Evaluate read in fit results
     *
     *  Stored fit results are evaluated and pulls and other relevant values are
     *  being determined.
     */
    void EvaluateFitResults();
    /**
     *  @brief Generate plots for all evaluated parameters
     *
     *  After evaluation pulls and other relevant parameters are being plotted 
     *  to the plot directory specified in ToyStudyStdConfig::plot_directory_
     */
    void PlotEvaluatedParameters();
    ///@}
    
    /** @name Helper functions
     *  Helper functions for not yet implemented usage
     */
    ///@{
    /**
     *  @brief Return vector of read in fit results.
     *
     *  This function will return a vector of currently read in fit results. The
     *  results are constant pointers which indicates that the caller must not
     *  delete these. All ever read in fit results will be deleted upon 
     *  destruction of this toy study. Emptying the stored result vector via
     *  EmptyResults() will not destroy the fit results, it will just prepare 
     *  the toy study to read in new results.
     *
     *  @return Currently stored fit results
     */
    const std::vector<const RooFitResult*> GetFitResults() const;
    
    /**
     *  @brief Empty stored vector of fit results.
     *
     *  Too prevent mistakes and misuse, the ToyStudyStd will not read in new 
     *  fit results if fit results are already read in. This function will empty
     *  the vector of stored fit results to be able to read in new results.  
     *  Previously accessed fit results via GetFitResults() are still valid and
     *  will only be deleted upon destruction of this ToyStudyStd.
     */
    void EmptyResults() { fit_results_.clear(); }
    ///@}
    
    RooDataSet* evaluated_values() const { return evaluated_values_; }
    
   protected:
    /**
     *  @brief Build an argument set with evaluated parameters
     *
     *  For a given fit result this helper function will generate a RooArgSet 
     *  with all values extracted from this result (parameter values, pulls 
     *  etc.).
     *
     *  @param fit_result RooFitResult to use for evaluation
     *  @return argument set with all evaluated parameters
     *
     *  @todo enhance this to be able to work with asymmetric errors if available
     */
    RooArgSet BuildEvaluationArgSet(const RooFitResult& fit_result);
    /**
     *  @brief Evaluate fit result quality
     *
     *  For a given fit result evaluate the fit quality (i.e. convergence, 
     *  covariance matrix quality and so on).
     *
     *  @param fit_result RooFitResult to use for evaluation
     *  @return true if fit result is okay, false if not
     */
    bool FitResultOkay(const RooFitResult& fit_result) const;
    /**
     *  @brief Copy an existing RooRealVar into a new RooRealVar
     *
     *  This function creates a new RooRealVar based on a given RooRealVar and
     *  copys (for toy studies) relevant information into the new RooRealVar. 
     *
     *  Huh, sounds exactly what the copy constructor 
     *  RooRealVar::RooRealVar(const RooRealVar& other) is meant for, you might
     *  think. You are right. Unfortunately this copy constructor has an odd bug
     *  in which every subsequent call takes longer and longer. As we need to 
     *  copy thousands of RooRealVars, this is not applicable here.
     *
     *  @param other RooRealVar to copy
     *  @param new_name if not empty, a different name can be supplied here
     *  @param new_title if not empty, a different title can be supplied here
     *  @return the copied RooRealVar
     */
    RooRealVar& CopyRooRealVar(const RooRealVar& other, const std::string& new_name="", const std::string& new_title="") const;
    /**
     *  @brief Print an overview of how many pulls are beyond expected range
     *
     *  This function will basically print how often each parameter pull was 
     *  outside the expected range (-5..+5). 
     */
    void PrintUnexpectedPullOverview() const;
    
    /**
     *  @brief Handle Unix signals
     *
     *  In order to increase safety when saving fit results, this function 
     *  handles UNIX/POSIX signals to avoid file corruption.
     *
     *  @param param the signal parameter
     */
    static void HandleSignal(int param);
    
    /**
     *  @brief Worker function for asynchronous saving of fit results
     *
     *  This function will take care of saving fit results into a file 
     *  asynchrously when a file lock could be acquired.
     *
     */
    void SaveFitResultWorker();
    
   private:
    /**
     *  \brief CommonConfig instance to use
     */
    const Config::CommonConfig& config_common_;
    /**
     *  \brief ToyFactoryStdConfig instance to use
     */
    const ToyStudyStdConfig& config_toystudy_;
    /**
     *  \brief Container for read in and active fit results
     */
    std::vector<RooFitResult*> fit_results_;
    /**
     *  \brief Container for all ever read in fit results
     */
    std::vector<RooFitResult*> fit_results_bookkeep_;
    /**
     *  \brief RooDataSet for all evaluated parameters, their pulls and so on
     */
    RooDataSet* evaluated_values_;
    /**
     *  \brief Counter for unexpectedly large pulls
     *
     *  This map holds counters for all pulls larger +-5 to check how many 
     *  (value) pull values for each parameter (key) are outside the -5..+5 
     *  range.
     */
    std::map<std::string, int> unexpected_pulls_counter_;
    /**
     *  \brief Static variable for ToyFactoryStd::HandleSignal(int)
     *
     *  This variable will be set to true by ToyFactoryStd::HandleSignal(int) in
     *  case of a signal to abort the program. ToyStudyStd::StoreFitResult() 
     *  will try to end gracefully afterwards.
     */
    static bool abort_save_;
    
    /**
     *  @brief Thread for asynchronous saving of fit results
     */
    boost::thread fitresult_save_worker_;
    /**
     *  @brief State variable determining if this class is still accepting fit results
     *
     *  This variable will be set to true upon construction and false upon 
     *  destruction to allow the saving thread to finish properly.
     * 
     */
    bool accepting_fit_results_;
    /**
     *  @brief Thread-safe queue for fit results to save
     */
    doofit::utils::concurrent_ptr_queue<RooFitResult> fit_results_save_queue_;
  };
  
  /** \struct ExceptionCannotStoreFitResult
   *  \brief Exception for problem in saving fit results
   */
  struct ExceptionCannotStoreFitResult: public virtual boost::exception, public virtual std::exception { 
    virtual const char* what() const throw() { return "Cannot save fit result."; }
  };
  
  /** \struct ExceptionCannotReadFitResult
   *  \brief Exception for problem in reading fit results
   */
  struct ExceptionCannotReadFitResult: public virtual boost::exception, public virtual std::exception { 
    virtual const char* what() const throw() { return "Cannot read fit results."; }
  };
  
  /** \struct ExceptionCannotEvaluateFitResults
   *  \brief Exception for problem in evaluating fit results
   */
  struct ExceptionCannotEvaluateFitResults: public virtual boost::exception, public virtual std::exception { 
    virtual const char* what() const throw() { return "Cannot evaluate fit results."; }
  };
} // namespace Toy
} // namespace doofit


#endif // TOYSTUDYSTD_h
