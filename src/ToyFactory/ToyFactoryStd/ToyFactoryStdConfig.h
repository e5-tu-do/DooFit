/** \class ToyFactoryStdConfig
 *  \brief DooFit Config class for the ToyFactoryStd class.
 *
 *  This class is responsible for all ToyFactoryStd related configuration options
 *  which are not covered by CommonConfig.
 */

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
#include "Config/Config.h"
#include "Config/DiscreteProbabilityDistribution.h"
#else
// ROOT Cint hacks...
#include "../../Config/Config.h"
#include "../../Config/DiscreteProbabilityDistribution.h"

#endif /* __CINT __ */

// forward declarations



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
   *  @see ToyFactoryStdConfig::set_discrete_probabilities(const std::vector<DiscreteProbabilityDistribution>&)
   */
  const std::vector<Config::DiscreteProbabilityDistribution>& discrete_probabilities() const {return discrete_probabilities_;}
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
   *  @see DiscreteProbabilityDistribution
   *  @see ToyFactoryStdConfig::AddDiscreteProbability(const DiscreteProbabilityDistribution&)
   *
   *  @param prop_dists vector of DiscreteProbabilityDistribution to use
   */
  void set_discrete_probabilities(const std::vector<Config::DiscreteProbabilityDistribution>& prop_dists) {discrete_probabilities_ = prop_dists;}
  /**
   *  \brief Add a discrete probability distribution
   *
   *  Add a discrete probability distribution to the vector of such 
   *  distributions. For each entry for which the variable is also in the 
   *  argument set to generate a discrete distribution data set will be 
   *  generated.
   *
   *  @see DiscreteProbabilityDistribution
   *  @see ToyFactoryStdConfig::set_discrete_probabilities(const std::vector<DiscreteProbabilityDistribution>&)
   *
   *  @param prop_dist DiscreteProbabilityDistribution to use
   */
  void AddDiscreteProbability(const Config::DiscreteProbabilityDistribution& prop_dist) {discrete_probabilities_.push_back(prop_dist);}
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
   *  @see DiscreteProbabilityDistribution
   */
  std::vector<Config::DiscreteProbabilityDistribution> discrete_probabilities_;
  ///@}
  
  /**
   *  @brief ClassDef statement for CINT dictionary generation
   */
  ClassDef(ToyFactoryStdConfig,1);
};

#endif // TOYFACTORYSTDCONFIG_h
