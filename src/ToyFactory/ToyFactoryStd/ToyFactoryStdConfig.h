/** \class ToyFactoryStdConfig
 *  \brief DooFit Config class for the ToyFactoryStd class.
 *
 *  This class is responsible for all ToyFactoryStd related configuration options
 *  which are not covered by CommonConfig.
 */

#ifndef TOYFACTORYSTDCONFIG_h
#define TOYFACTORYSTDCONFIG_h

// STL

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
  ///@}
  
  /**
   *  @brief ClassDef statement for CINT dictionary generation
   */
  ClassDef(ToyFactoryStdConfig,1);
};


#endif // TOYFACTORYSTDCONFIG_h
