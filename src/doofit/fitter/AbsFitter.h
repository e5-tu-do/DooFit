#ifndef DOOFIT_FITTER_ABSFITTER_H
#define DOOFIT_FITTER_ABSFITTER_H

// from STL
#include <string>

// from RooFit
#include "RooArgSet.h"

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
  AbsFitter() : identifier_("") {}
  
  /**
   *  @brief Constructor with name
   */
  AbsFitter(std::string identifier) : identifier_(identifier) {}
  
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
   *  @brief Write parameters to file
   *
   *  Write parameter information (ranges etc.) to an ASCII file
   *
   *  @param filename file name to write to
   */
  void WriteParametersFile(std::string filename) {
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
    Parameters().readFromFile(filename.c_str());
  }
  
  /**
   *  @brief Write observables to file
   *
   *  Write observable information (ranges etc.) to an ASCII file
   *
   *  @param filename file name to write to
   */
  void WriteObservablesFile(std::string filename) {
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
    Observables().readFromFile(filename.c_str());
  }
  
  /**
   *  @brief Set dataset to use
   *
   *  @param dataset The dataset to use for fitting/plotting.
   */
  void set_dataset(const RooAbsData* dataset) {dataset_ = dataset;}
  
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
  
 protected:
  /**
   *  @brief Dataset to work on
   */
  const RooAbsData* dataset_;
  
  /**
   *  @brief PDF to use for fitting
   */
  RooAbsPdf* pdf_;
  
 private:
  /**
   *  @brief Identifier
   */
  std::string identifier_;
};
} // namespace fitter
} // namespace doofit

#endif // DOOFIT_FITTER_ABSFITTER_H
