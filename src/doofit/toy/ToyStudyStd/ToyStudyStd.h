#ifndef TOYSTUDYSTD_h
#define TOYSTUDYSTD_h

// STL
#include <tuple>
#include <vector>
#include <map>

// BOOST
#include <boost/thread.hpp>

// ROOT

// from RooFit
#include "RooFitResult.h"
#include "RooArgSet.h"

// from project
#include "doofit/toy/ToyStudyStd/ToyStudyStdConfig.h"
#include "doocore/lutils/lutils.h"
#include "doocore/io/MsgStream.h"
#include "doofit/plotting/Plot/PlotConfig.h"

// forward declarations
class RooFitResult;
class RooDataSet;
class RooRealVar;
class TStopwatch;

namespace doofit {
  namespace config {
    class CommonConfig; 
  }
  namespace fitter { namespace easyfit {
    class EasyFitResult;
  }
}
namespace toy {
  
  // fit_result1, fit_result2, time_cpu1, time_real1, time_cpu2, time_real2, seed, run_id
  typedef std::tuple<const RooFitResult*,const RooFitResult*,double,double,double,double,int,int> FitResultContainer;

  // fit_result1, fit_result2
  typedef std::tuple<const doofit::fitter::easyfit::EasyFitResult*,const doofit::fitter::easyfit::EasyFitResult*> EasyFitResultContainer;
  
  /** @class ToyStudyStd
   *  @brief Standard toy study for DooFit to help conduct and evaluate mass toy fits
   *
   *  This class is designed to assist in both conducting mass toy fits as well 
   *  evaluating results from these fits. It's design is not to conduct the
   *  generation or actual fit itself. Other modules like ToyFactoryStd and
   *  fitter modules are well suited for that.
   *
   *  @warning In some cases, programs using ToyStudyStd might crash at the end, 
   *  just before or while saving the last fit result. This happens often when 
   *  the according PDF is on a RooWorkspace and the RooWorkspace can die 
   *  @a before the ToyStudyStd. The reason is a RooFit bug where the 
   *  RooFitResult becomes invalid if the RooWorkspace is deleted. Don't know 
   *  why this happens, does not make much sense to me. Solution: Use 
   *  @link FinishFitResultSaving() to enforce storing of all queued fit results
   *  near the end of your program.
   * 
   *  @author Florian Kruse
   */

  /**
   *  @brief Evaluate fit result quality
   *
   *  For a given fit result evaluate the fit quality (i.e. convergence, 
   *  covariance matrix quality and so on).
   *
   *  @param fit_result RooFitResult to use for evaluation
   *  @param min_acceptable_cov_matrix_quality minimal acceptable covariance quality
   *  @return true if fit result is okay, false if not
   */
  bool FitResultOkay(const RooFitResult& fit_result, int min_acceptable_cov_matrix_quality);

  /**
   *  @brief Check if fit result has (nearly) no varied parameters
   *
   *  For a given fit result evaluate the fit quality in terms of parameter
   *  variation. If more than 80% of non-fixed parameters have not been varied
   *  the fit result is to be neglected.
   *
   *  @param fit_result RooFitResult to use for evaluation
   *  @return true if fit result is problematic, false if not
   */
  bool FitResultNotVariedParameterSet(const RooFitResult& fit_result);

  /**
   *  @brief Check if fit result has no asymmetric errors for at least one parameter
   *
   *  For a given fit result evaluate the fit quality in terms of asymmetric 
   *  errors. If at least one parameter has no asymmetric errors, true is 
   *  returned.
   *
   *  @param fit_result RooFitResult to use for evaluation
   *  @return true if fit result is problematic, false if not
   */
  bool FitResultNoAsymmetricErrors(const RooFitResult& fit_result);

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
     *  @param cfg_plot PlotConfig for this specific toy study.
     */
    ToyStudyStd(const config::CommonConfig& cfg_com, const ToyStudyStdConfig& cfg_tstudy, const doofit::plotting::PlotConfig& cfg_plot);
    
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
     *  ToyStudyStdConfig::set_store_result_filename_treename(). The branch 
     *  names for the fit results need to be configured via 
     *  ToyStudyStdConfig::set_fit_result1_branch_name() and 
     *  ToyStudyStdConfig::set_fit_result2_branch_name() respectively (or via
     *  config files or command line parameters).
     *
     *  Fit results are stored temporarily in ToyStudyStd and written to the 
     *  tree/file deferredly in a separate thread so that no deadtime should 
     *  occur directly (see locking below).
     * 
     *  Writing the tree to the file is collision-safe, i.e. the file will be 
     *  locked before  write and if locked the saver thread will wait for the 
     *  file to be unlocked again. If the file will not be unlocked after some 
     *  time, the tree will be saved to an alternative file (not yet 
     *  implemented).
     *  
     *  Branch name(s) are specified via ToyStudyStdConfig.
     *
     *  @warning Passing pointers to this function, it is assumed that this 
     *        ToyStudyStd instance does take over ownership of the fit 
     *        result(s) (in contrast to previous versions). FitResults passed
     *        over will be deleted afterwards and are not guaranteed to live 
     *        on afterwards.
     *
     *
     *  @warning Calling of FinishFitResultSaving() might be 
     *        necessary to finish writing of fit results.
     *
     *
     *  @warning See warnings for doofit::toy::ToyStudyStdConfig::set_store_result_filename_treename(const config::CommaSeparatedPair&)
     *
     *  @see const config::CommaSeparatedPair& ToyStudyStdConfig::store_result_filename_treename
     *  @see ToyStudyStdConfig
     *  @see ToyStudyStdConfig::set_store_result_filename_treename()
     *  @see ToyStudyStdConfig::set_fit_result1_branch_name()
     *
     *  @todo Implement fit saving to alternative file after no lock possible
     *
     *  @param fit_result1 first RooFitResult to save
     *  @param fit_result2 second RooFitResult to save (optional)
     *  @param stopwatch1 first stop watch to save fit times (optional)
     *  @param stopwatch2 second stop watch to save fit times (optional)
     */
    void StoreFitResult(const RooFitResult* fit_result1, 
                        const RooFitResult* fit_result2 = NULL,
                        TStopwatch* stopwatch1 = NULL,
                        TStopwatch* stopwatch2 = NULL,
                        int seed = 0,
                        int run_id = 0);
    
    /**
     *  @brief End the save fit result worker thread and wait for it to save everything
     *  
     *  This function is mandatory when storing @a and reading fit results in 
     *  one ToyStudyStd instance. It can safely be ignored otherwise.
     */
    void FinishFitResultSaving() {
      if (accepting_fit_results_) {
        accepting_fit_results_ = false;
        fit_results_save_queue_.disable_queue();
        fitresult_save_worker_mutex_.unlock();
        fitresult_save_worker_.join();
      }
    }
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
     *  and stored internally in the toy factory. Branch names are handled
     *  via corresponding Config object.
     *
     *  This function will just invoke the reader worker thread which will load
     *  fit results in the background.
     */
    void ReadFitResults();
    /**
     *  @brief Get a read in fit result (pair)
     *
     *  While the fit result reader is reading in fit results in the background
     *  this function will get one of these fit results (or fit result pairs if
     *  two results are to be read in simultaneously). Null pointers are 
     *  returned if no more fit results are available.
     *
     *  Using this function, ToyStudyStd gives up ownership of these fit 
     *  results. The caller needs to release these via 
     *  ToyStudyStd::ReleaseFitResult()
     *
     *  @return tuple of fit results and respective fit times (CPU and real)
     */
    FitResultContainer GetFitResult();

    /**
     *  @brief Get next EasyFitResult (pair) in queue
     *
     *  On demand open files and trees containing EasyFitResults and return next
     *  EasyFitResultContainer in queue. 
     *
     *  Returned EasyFitResult references will be overwritten on next call. If 
     *  necessary, the caller has to copy the EasyFitResults themself.
     *
     *  @return tuple of fit results
     */
    EasyFitResultContainer GetEasyFitResult();

    unsigned long long NumberOfAvailableEasyFitResults();

    unsigned int NumberOfStoredResults() const { return fit_results_read_queue_.size(); }

    /**
     *  @brief Release a fit result (pair) for deletion
     *
     *  While the fit result reader is reading in fit results in the background
     *  this function will take one of these fit result (or fit result pairs if 
     *  two results are to be read in simultaneously) for deletion as apparently
     *  RooFit's constructor/destructor methods are not thread-safe.
     *
     *  @param fit_results tuple of fit results and respective fit times (CPU and real) to release
     */
    void ReleaseFitResult(FitResultContainer fit_results);

    /**
     *  @brief Purge previously released fit results
     */
    void PurgeReleasedFitResults();

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
     *  @brief Empty stored vector of fit results (deprecated?).
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
    
    void set_debug(bool debug) { debug_ = debug; } 

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
    RooArgSet BuildEvaluationArgSet(FitResultContainer fit_results);

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
     *  @brief Evaluate a RooFormulaVar based on a set of variables
     *
     *  This function evaluates a formula based on a collection of variables 
     *  (e.g. a RooArgSet). Every dependent of the formula that is in args is 
     *  set accordingly. Effectively, the formula can be evaluated for the given
     *  args.
     *
     *  @param args argument collection to use for setting formula's dependents
     *  @param formula the formula to apply args to
     */
    void EvaluateFormula(const RooAbsCollection& args, const RooFormulaVar& formula) const;
    
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
    
    /**
     *  @brief Worker function for reading of fit results
     *
     *  This function will take care of reading fit results from a file. 
     */
    void ReadFitResultWorker();
    
   private:
    
    /**
     *  @brief Lock the fitresult_save_worker_mutex_ to stop saver thread
     *
     *  In order to let the saver thread save fit results half-asynchronously 
     *  this function will lock the fitresult_save_worker_mutex_ to hold the 
     *  thread until the next fit results are handled over.
     */
    void LockSaveFitResultMutex() {
      // disabling this line enables full asynchrous deferred writing of fit results. 
      fitresult_save_worker_mutex_.lock();
    }
    
    /**
     *  \brief CommonConfig instance to use
     */
    const config::CommonConfig& config_common_;
    
    /**
     *  \brief ToyFactoryStdConfig instance to use
     */
    const ToyStudyStdConfig& config_toystudy_;

    /**
     *  \brief CommonConfig instance to use
     */
    const doofit::plotting::PlotConfig& config_plot_;

    /**
     *  \brief Container for read in and active fit results
     */
    std::vector<FitResultContainer> fit_results_;

    /**
     *  @brief Container for read in and active fit results with key as seed
     */
    std::multimap<int, FitResultContainer> fit_results_by_seed_;

    /**
     *  \brief Container for all ever read in fit results
     */
    std::vector<FitResultContainer> fit_results_bookkeep_;
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

    /** @name Save worker members
     *  Member objects for the save worker thread
     */
    ///@{
    /**
     *  @brief Thread for (half-)asynchronous saving of fit results
     */
    boost::thread fitresult_save_worker_;
    /**
     *  @brief Mutex for (half-)asynchronous saving of fit results
     */
    boost::mutex fitresult_save_worker_mutex_;
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
    doocore::lutils::concurrent_queue<FitResultContainer > fit_results_save_queue_;
    ///@}
    
    /** @name Reader worker members
     *  Member objects for the reader worker thread
     */
    ///@{
    /**
     *  @brief Thread for reading of fit results
     */
    boost::thread fitresult_reader_worker_;
    /**
     *  @brief State variable determining if this class is still reading fit results
     *
     *  This variable will be set to true upon construction and false upon 
     *  destruction to allow the reader thread to finish properly.
     * 
     */
    bool reading_fit_results_;
    /**
     *  @brief Thread-safe queue for fit results to read in
     */
    doocore::lutils::concurrent_queue<FitResultContainer > fit_results_read_queue_;
    /**
     *  @brief Thread-safe queue for fit results to delete
     */
    doocore::lutils::concurrent_queue<FitResultContainer > fit_results_release_queue_;
    ///@}

    /** @name EasyFitResult input support
     *  Member objects for input of EasyFitResult containers
     */
    ///@{
    /**
     *  @brief Files containing EasyFitResults
     */
    std::list<doofit::config::CommaSeparatedPair<std::string>> results_files_easyfit_;

    /**
     *  @brief Currently opened TFile + TTree for EasyFitResult input
     */
    std::pair<TFile*,TTree*> file_tree_easyfit_;

    /**
     *  @brief Position in currently opened TTree for EasyFitResult input
     */
    unsigned long long position_tree_easyfit_;

    /**
     *  @brief Number of available EasyFitResults
     */
    unsigned long long num_easyfit_results_;

    /**
     *  @brief Current EasyFitResult #1
     */
    doofit::fitter::easyfit::EasyFitResult* easyfit_result_0_;

    /**
     *  @brief Current EasyFitResult #2
     */
    doofit::fitter::easyfit::EasyFitResult* easyfit_result_1_;
    ///@}

    bool debug_;
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
} // namespace toy
} // namespace doofit


#endif // TOYSTUDYSTD_h
