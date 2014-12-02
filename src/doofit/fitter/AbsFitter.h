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
  AbsFitter() : fit_result_(nullptr), identifier_(""), num_cpu_(1), preserve_state_(false), shutup_(false) {}
  
  /**
   *  @brief Constructor with name
   */
  AbsFitter(std::string identifier) : fit_result_(nullptr), identifier_(identifier), num_cpu_(1), preserve_state_(false), shutup_(false) {}
  
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
   *  @brief Get negative log likelihood after fit
   *
   *  @return NLL (if applicable)
   */
  virtual double NegativeLogLikelihood() const { return 0.0; }
  
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
  std::string identifier() const { return identifier_; }
  
  /**
   *  @brief Set identifier
   */
  void set_identifier(std::string identifier) {  identifier_ = identifier; }
  
  /**
   *  @brief Get number of available CPUs
   */
  unsigned int num_cpu() const { return num_cpu_; }
  
  /**
   *  @brief Set identifier
   */
  void set_num_cpu(unsigned int num_cpu) { num_cpu_ = num_cpu; }

  /**
   *  @brief Get option whether to preserve objects and values
   *
   *  A fitter can rebuild all objects in the Fit() function. This option 
   *  defines if the fitter is requested to re-use existing objects (such as 
   *  PDFs) and not overwrite parameter values (e.g. from a file).
   */
  bool preserve_state() const { return preserve_state_; }

  /**
   *  @brief Set preservation of objects and values
   *
   *  A fitter can rebuild all objects in the Fit() function. Using this option
   *  the fitter is requested to re-use existing objects (such as PDFs) and not 
   *  overwrite parameter values (e.g. from a file).
   */
  void set_preserve_state(bool preserve_state) { preserve_state_ = preserve_state; }

  /**
   *  @brief Get silent mode
   */
  bool shutup() const { return shutup_; }

  /**
   *  @brief Set silent mode
   */
  void set_shutup(bool shutup) { shutup_ = shutup; }

  /**
   *  @brief Get fit result after full fit
   *
   *  @return fit result
   */
  const RooFitResult* fit_result() const { return fit_result_; }

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

  /**
   *  @brief Fit result of full fit
   */
  const RooFitResult* fit_result_;

 private:
  /**
   *  @brief Identifier
   */
  std::string identifier_;
  
  /**
   *  @brief Number of available CPUs
   */
  unsigned int num_cpu_;

  /**
   *  @brief Preserve objects and values (for fitting)
   */
  bool preserve_state_;

  /**
   *  @brief Fitter shutup mode (a.k.a. silent)
   */
  bool shutup_;
};
} // namespace fitter
} // namespace doofit

#endif // DOOFIT_FITTER_ABSFITTER_H
