#ifndef TOYSTUDYSTDCONFIG_h
#define TOYSTUDYSTDCONFIG_h

// STL
#include <vector>

// from project
#ifndef __CINT__
#include "DooFit/Config/AbsConfig.h"
#include "DooFit/Config/CommaSeparatedPair.h"
#else
// ROOT Cint hacks...
#include "../../Config/AbsConfig.h"
#include "../../Config/CommaSeparatedPair.h"
#endif /* __CINT __ */

// forward declarations

namespace DooFit {
namespace Toy {
  /** @class ToyStudyStdConfig
   *  @brief DooFit Config class for the ToyStudyStd class
   *
   *  This class is responsible for all ToyStudyStd related configuration options
   *  which are not covered by CommonConfig.
   */

  class ToyStudyStdConfig : public Config::AbsConfig {
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
    const Config::CommaSeparatedPair& store_result_filename_treename() const {return store_result_filename_treename_;}
    /**
     *  @brief Getter for file names and tree names to read fit result from
     *
     *  @return current value of read_results_filename_treename_
     */
    const std::vector<Config::CommaSeparatedPair>& read_results_filename_treename() const {return read_results_filename_treename_;}
    /**
     *  @brief Getter for plot directory for evaluation of fit results
     *
     *  @return current value of plot_directory_
     */
    const std::string& plot_directory() const {return plot_directory_;}
    ///@}

    /** @name Setter actual options
     *  Setter functions for actual options members.
     */
    ///@{
    /**
     *  @brief Setter for file name and tree name to store fit result to
     *
     *  @param result_file_tree new value for store_result_filename_treename_
     */
    void set_store_result_filename_treename(const Config::CommaSeparatedPair& result_file_tree) {store_result_filename_treename_ = result_file_tree;}
    /**
     *  @brief Setter for file names and tree names to read fit result from
     *
     *  @param result_file_tree new value for read_results_filename_treename_
     */
    void set_read_results_filename_treename(const std::vector<Config::CommaSeparatedPair>& results_file_tree) {read_results_filename_treename_ = results_file_tree;}
    /**
     *  @brief Setter for plot directory for evaluation of fit results
     *
     *  @param plot_directory new value for plot_directory_
     */
    void set_plot_directory(const std::string& plot_directory) {plot_directory_ = plot_directory;}
        
    /**
     *  @brief Adder for file names and tree names to read fit result from
     *
     *  @param result_file_tree new file and tree name to add to read_results_filename_treename_
     */
    void AddReadResultsFilenameTreename(const Config::CommaSeparatedPair& result_file_tree) {read_results_filename_treename_.push_back(result_file_tree);}
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
     *  @see Config::AbsConfig::DefineOptions()
     */
    void DefineOptions();
    
    /**
     *  \brief Load program options into own member variables.
     *
     *  This function will store all options from the variable map into own 
     *  members.
     *  
     *  @see Config::AbsConfig::LoadOptions()
     */
    void LoadOptions();
    
    /**
     *  \brief Print all options for this object.
     *
     *  Virtual function that will print all options for this Config object.
     */
    void PrintOptions() const;
    ///@}
    
   private:
    /** @name Actual options
     *  The actual options members.
     */
    ///@{
    /**
     *  @brief File name and tree name to store fit result to
     */
    Config::CommaSeparatedPair store_result_filename_treename_;
    /**
     *  @brief File names and tree names to read fit result from
     */
    std::vector<Config::CommaSeparatedPair> read_results_filename_treename_;
    /**
     *  @brief Plot directory for evaluation of fit results
     */
    std::string plot_directory_;
    ///@}
  };
} // namespace Toy
} // namespace DooFit

#endif // TOYSTUDYSTDCONFIG_h
