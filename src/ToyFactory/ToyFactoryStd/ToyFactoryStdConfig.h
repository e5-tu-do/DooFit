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

// from project
#ifndef __CINT__
#include "Config/Config.h"
#else
// ROOT Cint hacks...
#include "../../Config/Config.h"
#endif /* __CINT __ */

// forward declarations


/** @class DiscreteProbabilityDistribution
 *  @brief Distribution for discrete probabilities of variables.
 *
 *  This class is merely a container for Config objects to store the discrete
 *  probability distribution of a certain variable. As it's inheriting from 
 *  ConfigAbstractTypeCommaSeparated, parsing from command line or config file
 *  is possible. 
 *
 *  It contains a variable name and a list of value and corresponding 
 *  probability pairs. String representation is 
 *  
 *  @verbatim var_name,value1,prob1,value2,prob2,...,valueN,probN @endverbatim
 * 
 *  @see ConfigAbstractTypeCommaSeparated
 *  @see ConfigTestAbstractType
 */
class DiscreteProbabilityDistribution : public ConfigAbstractTypeCommaSeparated {
public:
  /**
   *  @brief Default constructor for DiscreteProbabilityDistribution
   */
  DiscreteProbabilityDistribution() {}
  
  /**
   *  @brief Destructor for DiscreteProbabilityDistribution
   */
  ~DiscreteProbabilityDistribution() {}
  
  /**
   *  @brief Parse a given string
   *
   *  The given string @a str is parsed. If parsing is not successful, an 
   *  exception will be thrown. The string is supposed to be a comma-separated 
   *  list formatted like this:
   *
   *  @verbatim var_name,value1,prob1,value2,prob2,...,valueN,probN @endverbatim
   *
   *  @see DiscreteProbabilityDistribution::var_name_
   *  @see DiscreteProbabilityDistribution::probabilities_map_
   *
   *  @param str string to parse
   */
  void Parse(std::string str);
  
  /**
   *  @brief Print this object to an std::ostream
   *
   *  This function is used to print the object. Calling this function directly
   *  is not necessary as it can be streamed directly to any ostream via 
   *  operator<<(std::ostream&, const ConfigAbstractTypeCommaSeparated&).
   *
   *  @param os ostream to print to
   */
  virtual void Print(std::ostream& os) const;
  
  /**
   *  @brief Getter for variable name
   */
  std::string var_name() const {return var_name_;}
  
  /**
   *  @brief Getter for abstract map 
   *         DiscreteProbabilityDistribution::probabilities_map_
   * 
   *  @return abstract member map
   */
  const std::vector<std::pair<double,double> >& probabilities() const {return probabilities_;}
  
protected:
  
private:
  /**
   *  @brief Name of variable to store probabilities for
   */
  std::string var_name_;
  
  /**
   *  @brief Vector for values and corresponding cumulative discrete probabilities 
   * 
   *  This vector contains pair<double, double> value pairs which are 
   *  supposed to be filled via a string. The pair contains the value and 
   *  corresponding cumulative probability.
   *
   *  @see DiscreteProbabilityDistribution::Parse(std::string)
   */
  std::vector<std::pair<double,double> > probabilities_;
  
  /**
   *  @brief ClassDef statement for CINT dictionary generation
   */
  ClassDef(DiscreteProbabilityDistribution,1);
};

class ToyFactoryStdConfig : public Config {
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
  RooAbsPdf* generation_pdf() const {return generation_pdf_;}
  /**
   *  \brief Getter for expected yield to use for dataset generation
   */
  double expected_yield() const {return expected_yield_;}
  /**
   *  \brief Getter for RooArgSet* with all observables to generate directly
   */
  const RooArgSet* argset_generation_observables() const {return argset_generation_observables_;}
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
  const std::vector<DiscreteProbabilityDistribution>& discrete_probabilities() const {return discrete_probabilities_;}
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
  void set_discrete_probabilities(const std::vector<DiscreteProbabilityDistribution>& prop_dists) {discrete_probabilities_ = prop_dists;}
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
  void AddDiscreteProbability(const DiscreteProbabilityDistribution& prop_dist) {discrete_probabilities_.push_back(prop_dist);}
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
   *  \brief Random seed to use for the toy factory upon initialization
   */
  int random_seed_;
  /**
   *  @brief Vector of discrete probability distributions
   *
   *  @see DiscreteProbabilityDistribution
   */
  std::vector<DiscreteProbabilityDistribution> discrete_probabilities_;
  ///@}
  
  /**
   *  @brief ClassDef statement for CINT dictionary generation
   */
  ClassDef(ToyFactoryStdConfig,1);
};

#endif // TOYFACTORYSTDCONFIG_h
