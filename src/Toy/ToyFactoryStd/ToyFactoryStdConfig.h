#ifndef TOYFACTORYSTDCONFIG_h
#define TOYFACTORYSTDCONFIG_h

// STL
#include <map>
#include <utility>
#include <vector>

// Boost

// ROOT

// RooFit
// forward declaration not enough as rootcint dictionary will fail compiling if 
// not included
#include "RooAbsPdf.h"
#include "RooArgSet.h"
#include "RooWorkspace.h"

// from project
#ifndef __CINT__
#include "Config/AbsConfig.h"
#include "Config/DiscreteProbabilityDistribution.h"
#include "Config/CommaSeparatedPair.h"
#else
// ROOT Cint hacks...
#include "../../Config/AbsConfig.h"
#include "../../Config/DiscreteProbabilityDistribution.h"
#include "../../Config/CommaSeparatedPair.h"
#endif /* __CINT __ */

// forward declarations
class TFile;

namespace Toy {
  /** \class ToyFactoryStdConfig
   *  \brief DooFit Config class for the ToyFactoryStd class.
   *
   *  This class is responsible for all ToyFactoryStd related configuration options
   *  which are not covered by CommonConfig.
   */
  
  class ToyFactoryStdConfig : public Config::AbsConfig {
  public:
    /**
     *  @brief Default constructor for ToyFactoryStdConfig
     *
     *  Unfortunately this constructor is needed for ROOT CINT and streaming to 
     *  ROOT files.
     */
    ToyFactoryStdConfig();
    
    /**
     *  \brief Standard constructor for ToyFactoryStdConfig
     *
     *  Sets all memebers to default values.
     *
     *  @param name Name of this Config object.
     */
    ToyFactoryStdConfig(const std::string& name);
    
    /**
     *  \brief Destructor for ToyFactoryStdConfig
     */
    ~ToyFactoryStdConfig();
    
    /** @name Getter actual options
     *  Getter functions for actual options members.
     */
    ///@{
    /**
     *  @brief Getter for workspace to use
     */
    RooWorkspace* workspace() const;
    /**
     *  \brief Getter for RooAbsPdf* to use for dataset generation
     *
     *  You might ask, why this needs non-const RooAbsPdf*. Refer to 
     *  ToyFactoryStdConfig::set_generation_pdf(RooAbsPdf*) for details on that.
     */
    RooAbsPdf* generation_pdf() const;
    /**
     *  \brief Getter for expected yield to use for dataset generation
     */
    double expected_yield() const {return expected_yield_;}
    /**
     *  \brief Getter for RooArgSet* with all observables to generate directly
     */
    const RooArgSet* argset_generation_observables() const; 
    /**
     *  \brief Getter for random seed
     *
     *  \see ToyFactoryStdConfig::set_random_seed(int)
     */
    int random_seed() const {return random_seed_;}
    /**
     *  @brief Getter for discrete probability distributions
     *
     *  @see discrete_probabilities_
     *  @see Config::DiscreteProbabilityDistribution
     *  @see ToyFactoryStdConfig::set_discrete_probabilities(const std::vector<Config::DiscreteProbabilityDistribution>&)
     *  @see ToyFactoryStdConfig::AddDiscreteProbability(const Config::DiscreteProbabilityDistribution&)
     */
    const std::vector<Config::DiscreteProbabilityDistribution>& discrete_probabilities() const {return discrete_probabilities_;}
    /**
     *  @brief Getter for proto dataset sections
     *
     *  @see @ref proto-sets
     *  @see proto_sections_
     *  @see Config::CommaSeparatedPair
     *  @see ToyFactoryStdConfig::set_proto_sections(const std::vector<Config::CommaSeparatedPair>&)
     *  @see ToyFactoryStdConfig::AddProtoSections(const Config::CommaSeparatedPair&)
     */
    const std::vector<Config::CommaSeparatedPair>& proto_sections() const {return proto_sections_;}
    /**
     *  @brief Getter for fixed data set size (in contrast to poisson smearing)
     *
     *  @see ToyFactoryStd::set_dataset_size_fixed(bool)
     *  @return current value of dataset_size_fixed_
     */
    bool dataset_size_fixed() const {return dataset_size_fixed_;}
    /**
     *  @brief Getter for for file to load workspace from (and name of workspace in file)
     *
     *  @see ToyFactoryStd::set_workspace_filename_name(Config::CommaSeparatedPair)
     *  @return current value of workspace_filename_name_
     */
    const Config::CommaSeparatedPair& workspace_filename_name() const {return workspace_filename_name_;}
    /**
     *  @brief Getter for file name to save dataset to (and name of dataset on file)
     *
     *  @see ToyFactoryStd::set_dataset_filename_name(Config::CommaSeparatedPair)
     *  @return current value of dataset_filename_name_
     */
    const Config::CommaSeparatedPair& dataset_filename_name() const {return dataset_filename_name_;}
    /**
     *  @brief Getter for file to read parameters from before generation
     *
     *  @return current filename to read from
     */
    const string& parameter_read_file() const {return parameter_read_file_;}
    /**
     *  @brief Getter for file to save parameters to after generation
     *
     *  @return current filename to save to
     */
    const string& parameter_save_file() const {return parameter_save_file_;}
    ///@}
    
    /** @name Setter actual options
     *  Setter functions for actual options members.
     */
    ///@{
    /**
     *  \brief Setter for RooAbsPdf* to use for dataset generation
     *
     *  Oops, you might notice, we're using a RooAbsPdf* here instead of a const
     *  RooAbsPdf*. This is actually a real problem as const correctness in C++ 
     *  can be an extreme pain if someone screws it up outside of your code. 
     *  
     *  Let me explain. In the end, we just need a RooAbsPdf pointer to invoke 
     *  RooAbsPdf::generate() on this. The generation should not change the PDF 
     *  itself and therefore all of the myriads of RooAbsPdf::generate() functions
     *  should be const. However, only half are and the one needed here is not.
     *  I don't know why, but it's implications are that we need a non-const 
     *  RooAbsPdf* for generation. However, Config objects are to be handled as 
     *  const inside their respective modules. A const object however cannot 
     *  return non-const members and this little missing const on 
     *  RooAbsPdf::generate() could therefore break everything for us. Fortunately
     *  we can declare the associated RooAbsPdf* member to be mutable which is far
     *  from being nice, but unfortunately it's the only way to fix this.
     */
    void set_generation_pdf(RooAbsPdf* generation_pdf) {generation_pdf_ = generation_pdf;}
    /**
     *  \brief Setter for expected yield to use for dataset generation
     *
     *  The expected yield determines the number of events for the data sample to 
     *  be generated. It will always be Poisson distributed around the expected 
     *  value, as in an extended likelihood manner. A value of 0 means to use the
     *  yield as set in the generation PDF itself (in case of RooExtendPdf).
     */
    void set_expected_yield(double expected_yield) {expected_yield_ = expected_yield;}
    /**
     *  \brief Setter for RooArgSet* with all observables to generate directly
     */
    void set_argset_generation_observables(const RooArgSet* argset_generation_observables) 
    {argset_generation_observables_ = argset_generation_observables;}
    /**
     *  @brief Setter for workspace to use for getting PDFs and argument sets
     *
     *  Setting this workspace is only sensible if also using setters for PDF and
     *  argument set names.
     *
     *  @see ToyFactoryStdConfig::set_argset_generation_observables_workspace(const string&)
     *  @see ToyFactoryStdConfig::set_generation_pdf_workspace(const std::string&)
     *
     *  @param ws RooWorkspace to use
     */
    void set_workspace(RooWorkspace* ws) {workspace_ = ws;} 
    /**
     *  @brief Setter for RooAbsPdf* name to use for dataset generation on linked 
     *         workspace
     *
     *  If a workspace is linked and no RooAbsPdf* is set via the appropriate 
     *  setter itself, a RooAbsPdf* will be loaded from the workspace with the 
     *  supplied name.
     *
     *  @see ToyFactoryStdConfig::set_workspace(const RooWorkspace*)
     *  @see ToyFactoryStdConfig::set_generation_pdf_workspace(const std::string&)
     *
     *  @param name Name of RooAbsPdf on workspace
     */
    void set_generation_pdf_workspace(const std::string& name);
    /**
     *  @brief Setter for RooArgSet* name with all observables to generate 
     *         directly on linked workspace
     *
     *  If a workspace is linked and no RooArgSet* is set via the appropriate 
     *  setter itself, a RooArgSet* will be loaded from the workspace with the 
     *  supplied name.
     *
     *  @see ToyFactoryStdConfig::set_workspace(const RooWorkspace*)
     *  @see ToyFactoryStdConfig::set_generation_pdf_workspace(const std::string&)
     *
     *  @param name Name of RooArgSet on workspace
     */
    void set_argset_generation_observables_workspace(const std::string& name); 
    /**
     *  \brief Setter for random seed
     *
     *  Will be used at initialization of the toy factory. A value of 0 means to 
     *  use random seed for irreproducible results.
     *
     *  @param seed the seed to use
     */
    void set_random_seed(int seed) {random_seed_ = seed;}
    /**
     *  \brief Setter for discrete probability distributions
     *
     *  Directly set the vector of discrete probability distributions. For each 
     *  entry for which the variable is also in the argument set to generate a 
     *  discrete distribution data set will be generated.
     *
     *  @see Config::DiscreteProbabilityDistribution
     *  @see ToyFactoryStdConfig::AddDiscreteProbability(const Config::DiscreteProbabilityDistribution&)
     *
     *  @param prop_dists vector of Config::DiscreteProbabilityDistribution to use
     */
    void set_discrete_probabilities(const std::vector<Config::DiscreteProbabilityDistribution>& prop_dists) {discrete_probabilities_ = prop_dists;}
    /**
     *  \brief Setter for proto dataset sections
     *
     *  Directly set the vector of proto dataset sections. For each sub PDF name
     *  in this vector a proto dataset will be generated according to the 
     *  corresponding section name in the config file.
     *
     *  @see @ref proto-sets
     *  @see Config::CommaSeparatedPair
     *  @see ToyFactoryStdConfig::AddProtoSections(const Config::CommaSeparatedPair&)
     *
     *  @param proto_sections vector of Config::CommaSeparatedPair to use
     */
    void set_proto_sections(const std::vector<Config::CommaSeparatedPair>& proto_sections) {proto_sections_ = proto_sections;}
    /**
     *  @brief Setter for fixed data set size (in contrast to poisson smearing)
     *
     *  This option determines if the size of the dataset is to be fixed or 
     *  varied according to a poisson distribution. For extended fits the latter
     *  is normally the case. However, one might want to generate a sample with 
     *  a fixed and well defined size.
     *
     *  @param dataset_size_fixed flag if a fixed size is to be generated (true)
     *                            or not (false)
     */
    void set_dataset_size_fixed(bool dataset_size_fixed) {dataset_size_fixed_ = dataset_size_fixed;} 
    /**
     *  @brief Setter for file to load workspace from (and name of workspace in file)
     *
     *  To load an external workspace from a ROOT file, one can specify the 
     *  filename and name of workspace in the file here (as 
     *  Config::CommaSeparatedPair in the order filename,workspacename). The 
     *  workspace will be loaded on demand, but only if no workspace is set 
     *  directly.
     *
     *  @param ws_filename_name ROOT file containing workspace and workspace 
     *                          name in this file as Config::CommaSeparatedPair
     */
    void set_workspace_filename_name(Config::CommaSeparatedPair ws_filename_name) {workspace_filename_name_ = ws_filename_name;} 
    /**
     *  @brief Setter for file name to save dataset to (and name of dataset on file)
     *
     *  To save the dataset after generation, one can specify the filename and 
     *  name of dataset in this file (as Config::CommaSeparatedPair in the order 
     *  filename,datasetname).
     *
     *  @param dataset_filename_name ROOT file to save dataset to and dataset 
     *                               name in this file as 
     *                               Config::CommaSeparatedPair
     */
    void set_dataset_filename_name(Config::CommaSeparatedPair dataset_filename_name) {dataset_filename_name_ = dataset_filename_name;}
    /**
     *  @brief Setter for file to read parameters from before generation
     *
     *  @param filename filename to read from
     */
    void set_parameter_read_file(const string& filename) {parameter_read_file_ = filename;}
    /**
     *  @brief Setter for file to save parameters to after generation
     *
     *  @param filename filename to save to
     */
    void set_parameter_save_file(const string& filename) {parameter_save_file_ = filename;}
    
    /**
     *  \brief Add a discrete probability distribution
     *
     *  Add a discrete probability distribution to the vector of such 
     *  distributions. For each entry for which the variable is also in the 
     *  argument set to generate a discrete distribution data set will be 
     *  generated.
     *
     *  @see Config::DiscreteProbabilityDistribution
     *  @see ToyFactoryStdConfig::set_discrete_probabilities(const std::vector<Config::DiscreteProbabilityDistribution>&)
     *
     *  @param prop_dist Config::DiscreteProbabilityDistribution to add
     */
    void AddDiscreteProbability(const Config::DiscreteProbabilityDistribution& prop_dist) {discrete_probabilities_.push_back(prop_dist);}
    /**
     *  \brief Add a proto dataset section
     *
     *  Add a proto dataset section to the according vector. For each sub PDF 
     *  name in this vector a proto dataset will be generated according to the 
     *  corresponding section name in the config file.
     *
     *  @see @ref proto-sets
     *  @see Config::CommaSeparatedPair
     *  @see ToyFactoryStdConfig::set_proto_sections(const std::vector<Config::CommaSeparatedPair>&)
     *
     *  @param proto_section Config::CommaSeparatedPair to add
     */
    void AddProtoSections(const Config::CommaSeparatedPair& proto_section) {proto_sections_.push_back(proto_section);}
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
     *  @see Config::DefineOptions()
     */
    void DefineOptions();
    
    /**
     *  \brief Load program options into own member variables.
     *
     *  This function will store all options from the variable map into own 
     *  members.
     *  
     *  @see Config::LoadOptions()
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
     *  \brief RooAbsPdf to generate toys for
     *
     *  You might ask, why this needs to be a mutable member. Refer to 
     *  ToyFactoryStdConfig::set_generation_pdf(RooAbsPdf*) for details on that.
     */
    mutable RooAbsPdf* generation_pdf_;
    /**
     *  \brief Expected yield to use
     */
    double expected_yield_;
    /**
     *  \brief RooArgSet with all observables to generate directly
     */
    const RooArgSet* argset_generation_observables_;
    /**
     *  @brief RooWorkspace to use to get PDFs and argument sets
     *
     *  You might ask, why this needs to be a mutable member. It's the same reason
     *  as for ToyFactoryStdConfig::generation_pdf_ (in short: ROOT developers
     *  giving a shit on const correctness).
     */  
    mutable RooWorkspace* workspace_;
    /**
     *  @brief Name of RooAbsPdf to generate toys for on linked workspace
     *
     *  @see ToyFactoryStdConfig::generation_pdf_
     *  @see ToyFactoryStdConfig::workspace_
     */  
    std::string generation_pdf_workspace_;
    /**
     *  @brief Name of RooArgSet with all observables to generate directly on 
     *         linked workspace
     *
     *  @see ToyFactoryStdConfig::argset_generation_observables_
     *  @see ToyFactoryStdConfig::workspace_
     */  
    std::string argset_generation_observables_workspace_;
    /**
     *  \brief Random seed to use for the toy factory upon initialization
     */
    int random_seed_;
    /**
     *  @brief Vector of discrete probability distributions
     *
     *  @see Config::DiscreteProbabilityDistribution
     *  @see ToyFactoryStdConfig::set_discrete_probabilities(const std::vector<Config::DiscreteProbabilityDistribution>&)
     *  @see ToyFactoryStdConfig::AddDiscreteProbability(const Config::DiscreteProbabilityDistribution&)
     */
    std::vector<Config::DiscreteProbabilityDistribution> discrete_probabilities_;
    /**
     *  @brief Vector of section names for proto datasets.
     *
     *  Refer to Toy::ToyFactoryStd for usage documentation. First string is sub
     *  PDF name, second the section to use.
     *
     *  @see @ref proto-sets
     *  @see Config::CommaSeparatedPair
     *  @see ToyFactoryStdConfig::set_proto_sections(const std::vector<Config::CommaSeparatedPair>&)
     *  @see ToyFactoryStdConfig::AddProtoSections(const Config::CommaSeparatedPair&)
     */
    std::vector<Config::CommaSeparatedPair> proto_sections_;
    /**
     *  @brief Option for fixed data set size (in contrast to poisson smearing)
     *
     *  This option determines if the size of the dataset is to be fixed or 
     *  varied according to a poisson distribution. For extended fits the latter
     *  is normally the case. However, one might want to generate a sample with 
     *  a fixed and well defined size.
     */
    bool dataset_size_fixed_;
    /**
     *  @brief File name to get workspace from (and name of workspace on file)
     */
    Config::CommaSeparatedPair workspace_filename_name_;
    /**
     *  @brief File name to save dataset to (and name of dataset on file)
     */
    Config::CommaSeparatedPair dataset_filename_name_;
    /**
     *  @brief File to read parameters from before generation
     */
    string parameter_read_file_;
    /**
     *  @brief File to save parameters to after generation
     */
    string parameter_save_file_;
    ///@}
    
    /** @name Other private members
     *  Private members besides options.
     */
    ///@{
    // let ROOT Cint not bother about this
#ifndef __CINT__
    /**
     *  @brief TFile to read workspace from
     * 
     *  This file will be set automatically upon requeting of a workspace (if 
     *  workspace_filename_name_ is set and workspace was not set directly 
     *  before).
     *
     *  This is a mutable member as it has to be used dynamically by const 
     *  member functions.
     */
    mutable TFile* ws_file_;
#endif /* __CINT __ */
    ///@}
    
    /**
     *  @brief ClassDef statement for CINT dictionary generation
     */
    ClassDef(ToyFactoryStdConfig,1);
  };
  
  // let ROOT Cint not bother about this
#ifndef __CINT__
  /** \struct WorkspaceNotSetException
   *  \brief Exception for not set workspace in ToyFactoryStdConfig
   */
  struct WorkspaceNotSetException: public virtual boost::exception, public virtual std::exception { 
    virtual const char* what() const throw() { return "Workspace not set or not loadable"; }
  };
  
  /** \struct PdfNotSetException
   *  \brief Exception for not set PDF in ToyFactoryStdConfig
   */
  struct PdfNotSetException: public virtual boost::exception, public virtual std::exception { 
    virtual const char* what() const throw() { return "PDF to generate not set"; }
  };
  
  /** \struct ArgSetNotSetException
   *  \brief Exception for not set RooArgSet in ToyFactoryStdConfig
   */
  struct ArgSetNotSetException: public virtual boost::exception, public virtual std::exception { 
    virtual const char* what() const throw() { return "Observables argument set not set"; }
  };
#endif /* __CINT __ */
}

#endif // TOYFACTORYSTDCONFIG_h
