#ifndef DOOFIT_PLOTTING_PLOT_PLOTSIMULTANEOUS_h
#define DOOFIT_PLOTTING_PLOT_PLOTSIMULTANEOUS_h

// STL
#include <vector>
#include <string>

// BOOST

// ROOT

// from RooFit

// from project
#include "doofit/plotting/Plot/PlotConfig.h"
#include "doofit/plotting/Plot/Plot.h"

// forward declarations
class RooAbsData;
class RooSimultaneous;
class RooAbsRealLValue;

namespace doofit {
namespace plotting {
  /** @class PlotSimultaneous
   *  @brief A simple plot for simultaneous PDFs and corresponding datasets
   *
   *  This class is built to produce basic plots for simultaneous PDFs. As of 
   *  now, it can split the simultanseous PDF into its sub PDFs and sub datasets
   *  and plot these. Support for summed plotting is planned for the future.
   *
   *  You can supply datasets and PDFs to plot. To plot components, you can 
   *  supply regular expressions of component PDF names. Any PDF matching the 
   *  patterns will be plotted as well.
   *
   *  @section usage Usage
   *
   *  Usage is similar to Plot. Regard to Plot documentation for examples.
   *  Consider using PlotConfig::set_simultaneous_plot_all_categories() and
   *  PlotConfig::set_simultaneous_plot_all_slices() to enable plotting of
   *  sub-categories and slices.
   *
   */
  
class PlotSimultaneous : public Plot {
 public:
  /**
   *  @brief Constructor for PlotSimultaneous with regular expressions for plotted components
   *
   *  This will initialise the PlotSimultaneous which can then be plotted via
   *  PlotSimultaneous::PlotIt() or similar functions. A vector of regular 
   *  expressions with component patterns can be supplied. Each sub PDF of the 
   *  sub PDFs will be matched against this pattern and plotted as a component 
   *  if matching.
   *
   *  @param cfg_plot PlotConfig holding plotting configuration
   *  @param dimension the dimension to plot (e.g. a RooRealVar)
   *  @param dataset the dataset to plot (e.g. a RooDataSet)
   *  @param pdf main simultaneous PDF to plot
   *  @param components vector of components to plot as regular expression strings
   *  @param plot_name (optional) a name for this plot (i.e. the output files; will be the variable name if empty)
   */
  PlotSimultaneous(const PlotConfig& cfg_plot, const RooAbsRealLValue& dimension, const RooAbsData& dataset, const RooSimultaneous& pdf, const std::vector<std::string>& components, const std::string& plot_name="");
  
  /**
   *  @brief Destructor for PlotSimultaneous
   */
  virtual ~PlotSimultaneous() {}
  
  /**
   *  @brief Set asymmetry plotting mode
   *
   *  Set true, if an asymmetry shall be plotted. The y-axis  range will be set
   *  from -1 to 1 
   *  
   *  @param plot_asymmetry set true for plotting asymmetry
   */
  void set_plot_asymmetry(bool plot_asymmetry) { plot_asym_ = plot_asymmetry; }
 protected:
  /**
   *  @brief Internal plotting handler
   *
   *  This function will perform the actual plotting and is called by public
   *  functions.
   *
   *  @param sc_x ScaleType for x-axis
   *  @param sc_y ScaleType for y-axis
   */
  virtual void PlotHandler(ScaleType sc_x, ScaleType sc_y) const;
  
 private:
  /**
   *  @brief Components to plot as regular expressions
   */
  const std::vector<std::string> components_regexps_;

  bool plot_asym_;
};
} // namespace plotting
} // namespace doofit

#endif // DOOFIT_PLOTTING_PLOT_PLOTSIMULTANEOUS_h