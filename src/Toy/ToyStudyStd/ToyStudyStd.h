#ifndef TOYSTUDYSTD_h
#define TOYSTUDYSTD_h

// STL

// ROOT

// from RooFit

// from project
#include "Toy/ToyStudyStd/ToyStudyStdConfig.h"
#include "Config/CommonConfig.h"

// forward declarations
class RooFitResult;

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
    ToyStudyStd(const CommonConfig& cfg_com, const ToyStudyStdConfig& cfg_tstudy);
    
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
     *  @brief Store a fit result into TTree
     *
     *  This function will save the given fit result to a TTree in a ROOT file. 
     *  The latter can be configured via 
     *  ToyStudyStdConfig::set_store_result_filename_treename(). Writing the 
     *  tree to the file is collision-safe, i.e. the file will be locked before 
     *  write and if locked the function will wait for the file to be unlocked 
     *  again. If the file will not be unlocked after some time, the tree will 
     *  be saved to an alternative file.
     *
     *  @todo Implement fit saving to alternative file after no lock possible
     *
     *  @param fit_result RooFitResult to save
     */
    void StoreFitResult(const RooFitResult* fit_result) const;
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
     */
    void ReadFitResults();
    ///@}
    
   protected:
    
   private:
    /**
     *  \brief CommonConfig instance to use
     */
    const CommonConfig& config_common_;
    /**
     *  \brief ToyFactoryStdConfig instance to use
     */
    const ToyStudyStdConfig& config_toystudy_;
    /**
     *  \brief Container for read in fit results
     */
    std::vector<RooFitResult*> fit_results_;
  };
  
  /** \struct ExceptionCannotStoreFitResult
   *  \brief Exception for problem in saving fit results
   */
  struct ExceptionCannotStoreFitResult: public virtual boost::exception, public virtual std::exception { 
    virtual const char* what() const throw() { return "Cannot save fit result."; }
  };
}


#endif // TOYSTUDYSTD_h
