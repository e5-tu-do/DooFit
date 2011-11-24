#ifndef TOYSTUDYSTDCONFIG_h
#define TOYSTUDYSTDCONFIG_h

// from project
#ifndef __CINT__
#include "Config/AbsConfig.h"
#include "Config/CommaSeparatedPair.h"
#else
// ROOT Cint hacks...
#include "../../Config/AbsConfig.h"
#include "../../Config/CommaSeparatedPair.h"
#endif /* __CINT __ */

// forward declarations

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
     *  @return current value of result_filename_treename_
     */
    const Config::CommaSeparatedPair& result_filename_treename() const {return result_filename_treename_;}
    ///@}

    /** @name Setter actual options
     *  Setter functions for actual options members.
     */
    ///@{
    /**
     *  @brief Setter for file name and tree name to store fit result to
     *
     *  @param result_file_tree new value for result_filename_treename_
     */
    void set_result_filename_treename(const Config::CommaSeparatedPair& result_file_tree) {result_filename_treename_ = result_file_tree;}    
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
    Config::CommaSeparatedPair result_filename_treename_;
    ///@}
  };
}

#endif // TOYSTUDYSTDCONFIG_h
