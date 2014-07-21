#ifndef DOOFIT_PLOTTING_CORRELATIONS_CORRELATIONPLOT_h
#define DOOFIT_PLOTTING_CORRELATIONS_CORRELATIONPLOT_h

// STL
#include <string>

// BOOST

// ROOT

// from RooFit

// from DooCore

// from project

// forward declarations
class RooFitResult;
class TAxis;
class RooArgList;

namespace doofit {
namespace plotting {
namespace correlations {
  
/** @class CorrelationPlot
 *  @brief Plotter for a correlation matrix from a RooFitResult
 *
 *  This class plots a correlation matrix based on a supplied RooFitResult as a 
 *  2D histogram into a PDF file.
 *
 *  @section usage Usage
 *
 *  Usage is shown via this example:
 *
 * @code
 * doofit::fitter::easyfit::EasyFit efit_full("fit_toy")
 * efit_full.Fit();
 * fit_result = efit_full.GetFitResult();
 * doofit::plotting::correlations::CorrelationPlot cplot(*fit_result);
 * cplot.Plot("PlotCorrelation/");
 * @endcode
 *
 *  @author Florian Kruse
 *  @author Julian Wishahi
 */

class CorrelationPlot {
 public:
  /**
   *  @brief Default constructor for CorrelationPlot
   *
   *  Detailed description.
   *
   *  @param fit_result the fit result to plot the correlation matrix of
   */
  CorrelationPlot(const RooFitResult& fit_result);
  
  /**
   *  @brief Destructor for CorrelationPlot
   *
   *  Detailed description.
   *
   */
  ~CorrelationPlot() {}
  
  /**
   *  @brief Plot the RooFitResult's correlation matrix into files
   *
   *  @param plot_path path for the plots
   */
  void Plot(const std::string& plot_path) const {
    PlotHandler(plot_path);
  }
  
 protected:
  
  /**
   *  @brief Plot the RooFitResult's correlation matrix into files (internal handler)
   *
   *  @param plot_path path for the plots
   */
  void PlotHandler(const std::string& plot_path) const;
  
  /**
   *  @brief Rename axis labels with parameter titles
   *
   *  @param axis axis to rename
   *  @param arg_list argument list with parameters
   */
  double RenameAxisLabel(TAxis* axis, const RooArgList& arg_list) const;
  
 private:
  const RooFitResult& fit_result_;
};

} // namespace correlations
} // namespace plotting
} // namespace doofit

#endif //DOOFIT_PLOTTING_CORRELATIONS_CORRELATIONPLOT_h