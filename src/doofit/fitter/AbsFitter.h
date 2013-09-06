#ifndef DOOFIT_FITTER_ABSFITTER_H
#define DOOFIT_FITTER_ABSFITTER_H

// from STL
#include <string>

// from RooFit
#include "RooArgSet.h"

// from DooCore
#include <doocore/config/Summary.h>

// forward declarations
class RooAbsData;
class RooAbsPdf;

/** @class doofit::fitter::AbsFitter
 *  @brief Abstract Fitter interface
 *
 *  This is an abstract fitter class which can be used as an interface to make
 *  fitter implementations exchangable.
 */

namespace doofit {
namespace fitter {
class AbsFitter {
 public:
  /**
   *  @brief Constructor
   */
  AbsFitter() : identifier_(""), num_cpu_(1) {}
  
  /**
   *  @brief Constructor with name
   */
  AbsFitter(std::string identifier) : identifier_(identifier), num_cpu_(1) {}
  
  /**
   *  @brief Destructor
   */
  virtual ~AbsFitter() {}
  
  /**
   *  @brief Pre-fit preparations
   *
   *  Do pre-fit preparations
   */
  virtual void PrepareFit() = 0;
  
  /**
   *  @brief Perform the fit
   *
   *  Pure virtual function responsible of performing the acutal fitting.
   */
  virtual void Fit() = 0;

  /**
   *  @brief Perform the post-fit plotting
   *
   *  Pure virtual function responsible of plotting the data and PDF.
   */
  virtual void Plot() {};
  
  /**
   *  @brief Get observables
   *
   *  Return RooArgSet of all observables
   *
   *  @return the observables
   */
  virtual RooArgSet Observables() = 0;

  /**
   *  @brief Get parameters
   *
   *  Return RooArgSet of all parameters
   *
   *  @return the parameters
   */
  virtual RooArgSet Parameters() = 0;
  
  /**
   *  @brief Get signal yield after fit
   *
   *  @return signal yield (if applicable)
   */
  virtual double SignalYield() const { return 0.0; }

  /**
   *  @brief Get background yield after fit
   *
   *  @return background yield (if applicable)
   */
  virtual double BackgroundYield() const { return 0.0; }
  
  /**
   *  @brief Write parameters to file
   *
   *  Write parameter information (ranges etc.) to an ASCII file
   *
   *  @param filename file name to write to
   */
  void WriteParametersFile(std::string filename) {
    doocore::config::Summary::GetInstance().AddFile(filename);
    Parameters().writeToFile(filename.c_str());
  }
  
  /**
   *  @brief Read parameters from file
   *
   *  Read parameter information (ranges etc.) from an ASCII file
   *
   *  @param filename file name to read from
   */
  void ReadParametersFile(std::string filename) {
    file_parameters_ = filename;
    doocore::config::Summary::GetInstance().AddFile(file_parameters_);
    Parameters().readFromFile(file_parameters_.c_str());
  }
  
  /**
   *  @brief Write observables to file
   *
   *  Write observable information (ranges etc.) to an ASCII file
   *
   *  @param filename file name to write to
   */
  void WriteObservablesFile(std::string filename) {
    doocore::config::Summary::GetInstance().AddFile(filename);
    Observables().writeToFile(filename.c_str());
  }
  
  /**
   *  @brief Read observables from file
   *
   *  Read observable information (ranges etc.) from an ASCII file
   *
   *  @param filename file name to read from
   */
  void ReadObservablesFile(std::string filename) {
    file_observables_ = filename;
    doocore::config::Summary::GetInstance().AddFile(file_observables_);
    Observables().readFromFile(file_observables_.c_str());
  }
  
  /**
   *  @brief Set dataset to use
   *
   *  @param dataset The dataset to use for fitting/plotting.
   */
  void set_dataset(RooAbsData* dataset) {dataset_ = dataset;}
  
  /**
   *  @brief Set PDF to use
   *
   *  @param pdf The PDF to use fot fitting/plotting
   */
  void set_pdf(RooAbsPdf* pdf) {pdf_ = pdf;}
  
  /**
   *  @brief Get identifier
   */
  std::string identifier() { return identifier_; }
  
  /**
   *  @brief Set identifier
   */
  void set_identifier(std::string identifier) {  identifier_ = identifier; }
  
  /**
   *  @brief Get number of available CPUs
   */
  unsigned int num_cpu() { return num_cpu_; }
  
  /**
   *  @brief Set identifier
   */
  void set_num_cpu(unsigned int num_cpu) { num_cpu_ = num_cpu; }

protected:
  /**
   *  @brief Dataset to work on
   */
  RooAbsData* dataset_;
  
  /**
   *  @brief PDF to use for fitting
   */
  RooAbsPdf* pdf_;
  
  /**
   *  @brief Observables file (last one used)
   */
  std::string file_observables_;

  /**
   *  @brief Parameters file (last one used)
   */
  std::string file_parameters_;

 private:
  /**
   *  @brief Identifier
   */
  std::string identifier_;
  
  /**
   *  @brief Number of available CPUs
   */
  unsigned int num_cpu_;
};
} // namespace fitter
} // namespace doofit

#endif // DOOFIT_FITTER_ABSFITTER_H
