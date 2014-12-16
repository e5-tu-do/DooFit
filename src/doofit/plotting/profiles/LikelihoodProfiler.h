#ifndef DOOFIT_PLOTTING_PROFILES_LIKELIHOODPROFILER_H
#define DOOFIT_PLOTTING_PROFILES_LIKELIHOODPROFILER_H

// STL
#include <string>
#include <vector>
#include <map>

// BOOST

// ROOT

// from RooFit

// from DooCore

// from project
#include "doofit/plotting/Plot/PlotConfig.h"

// forward declarations
namespace doofit { namespace fitter {
  class AbsFitter;
}}
namespace doofit { namespace toy {
  class ToyStudyStd;
}}
class RooRealVar;
class RooFitResult;

namespace doofit {
namespace plotting {

/** @namespace doofit::plotting::profiles
 *
 *  @brief Plotting namespace for profiles.
 *
 *  This namespace is responsible for plotting profiles.
 *
 */

namespace profiles {
  
/** @class LikelihoodProfiler
 *  @brief Plotter for likelihood profiles for a given AbsFitter
 *
 *  This class plots a likelihood profile for a given AbsFitter and one or more
 *  RooRealVars to scan. For each scan point the AbsFitter will fit the data 
 *  with only the parameters of interest fixed to the appropriate value.
 *
 *  @section usage Usage
 *
 *  Usage is shown via this example:
 *
 * @code
 * - tbd -
 * @endcode
 *
 *  @author Florian Kruse
 */

class LikelihoodProfiler {
 public:
  /**
   *  @brief Default constructor for LikelihoodProfiler
   */
  LikelihoodProfiler(const PlotConfig& cfg_plot);
  
  /**
   *  @brief Destructor for LikelihoodProfiler
   *
   *  Detailed description.
   *
   */
  ~LikelihoodProfiler() {}
  
  void set_fitter(doofit::fitter::AbsFitter& fitter) {
    fitter_ = &fitter;
  }

  void AddScanVariable(RooRealVar* variable) {
    scan_vars_.push_back(variable);
  }

  /**
   *  @brief Perform likelihood profile scan.
   */
  void Scan();

  /**
   *  @brief Read fit results from a ToyStudyStd
   */
  void ReadFitResults(doofit::toy::ToyStudyStd& toy_study);

  /**
   *  @brief Plot likelihood profile
   *
   *  @param plot_path path for the plots
   */
  void Plot(const std::string& plot_path) {
    PlotHandler(plot_path);
  }
  
 protected:

  /**
   *  @brief Plot likelihood profile (internal handler)
   *
   *  @param plot_path path for the plots
   */
  void PlotHandler(const std::string& plot_path);

  std::vector<double> SetSamplePoint(unsigned int step);
  
  /**
   *  @brief Evaluate fit result quality
   *
   *  For a given fit result evaluate the fit quality (i.e. convergence, 
   *  covariance matrix quality and so on).
   *
   *  @param fit_result RooFitResult to use for evaluation
   *  @return true if fit result is okay, false if not
   */
  bool FitResultOkay(const RooFitResult& fit_result) const;

 private:
  /**
   *  @brief PlotConfig instance to use
   */
  const PlotConfig& config_plot_;

  doofit::fitter::AbsFitter* fitter_;

  std::vector<RooRealVar*> scan_vars_;
  std::vector<std::string> scan_vars_titles_;
  std::vector<std::string> scan_vars_names_;

  std::map<std::string, double> start_vals_;

  std::vector<const RooFitResult*> fit_results_;

  unsigned int num_samples_;
};

} // namespace profiles
} // namespace plotting
} // namespace doofit

#endif //DOOFIT_PLOTTING_PROFILES_LIKELIHOODPROFILER_H