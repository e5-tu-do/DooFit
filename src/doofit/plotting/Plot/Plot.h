#ifndef DOOFIT_PLOTTING_PLOT_PLOT_h
#define DOOFIT_PLOTTING_PLOT_PLOT_h

// STL
#include <string>
#include <vector>

// BOOST

// ROOT

// from RooFit
#include "RooArgList.h"

// from project
#include "doofit/plotting/Plot/PlotConfig.h"

// forward declarations
class RooAbsRealLValue;
class RooAbsData;

namespace doofit {
/** @namespace doofit::plotting
 *
 *  @brief Plotting subsystem namespace.
 *
 *  This namespace is responsible for plotting.
 *
 */
  
namespace plotting {
  /** @class Plot
   *  @brief A simple plot based on a dataset and optional PDFs
   *
   *  This class is built to produce a basic plot. It is not intended to suit 
   *  every plotting need someone might ever have. Derived classes are intended
   *  for more specific support. Main focus is convenience and ease of use.
   * 
   *  As of now only very basic plotting support is available. More will be 
   *  added later.
   *
   *  @section usage Usage
   *
   *  Usage is shown via this example:
   *
   * @code
   * #include "doofit/plotting/Plot/Plot.h"
   * #include "doofit/plotting/Plot/PlotConfig.h"
   * #include "RooRealVar.h"
   * #include "RooGaussian.h"
   * #include "RooExponential.h"
   * #include "RooAddPdf.h"
   * #include "RooDataSet.h"
   * #include "RooArgList.h"
   *
   * int main(int argc, char *argv[]) {
   *   // generate a PDF and a dataset here
   *   RooRealVar mass("mass","mass",5200,5400,"MeV/c^{2}");
   *   RooRealVar mean("mean","mean",5300);
   *   RooRealVar sigma("sigma","sigma",20);
   *   RooGaussian g("g","g",mass,mean,sigma);
   *
   *   RooRealVar c("c","c",0.01);
   *   RooExponential e("e","e",mass,c);
   *
   *   RooRealVar f("f","f",0.5);
   *   RooAddPdf p("p","p",g,e,f);
   *
   *   RooDataSet* data = p.generate(mass, 2000);
   *
   *   // DooFit's plotting usage actually starts here
   *   using namespace doofit::plotting;
   *
   *   PlotConfig cfg_plot("cfg_plot");
   *   cfg_plot.InitializeOptions(argc, argv);
   *
   *   Plot myplot(cfg_plot, mass, *data, RooArgList(p,g,e));
   *   myplot.PlotIt();
   * }
   * @endcode
   *
   *  Plot will simply plot the given dataset in the supplied variable. If PDFs
   *  are also supplied (optional), they are plotted as well. The first PDF 
   *  needs to be the overall PDF, all following must be components of the 
   *  first. A pull plot is automatically created if PDFs are supplied. A 
   *  non-pull plot will be created in any way.
   *
   *  Configuration can be done via the associated PlotConfig object that also 
   *  supports config files and command line options using DooFit's Config 
   *  architecture. Refer to PlotConfig for possible options.
   *
   *  doocore::lutils methods are used for plotting assuring nice and shiny plots.
   */
  
  class Plot {
  public:
    /**
     *  @brief Constructor for Plot
     *
     *  This will initialise the Plot which can then be plotted via 
     *  Plot::PlotIt()
     *
     *  @param cfg_plot PlotConfig holding plotting configuration
     *  @param dimension the dimension to plot (e.g. a RooRealVar)
     *  @param dataset the dataset to plot (e.g. a RooDataSet)
     *  @param pdfs (optional) a RooArgList of PDFs to plot; the first PDF must be the overall PDF, further PDFs must be components of this PDF
     *  @param plot_name (optional) a name for this plot (i.e. the output files; will be the variable name if empty)
     */
    Plot(const PlotConfig& cfg_plot, const RooAbsRealLValue& dimension, const RooAbsData& dataset, const RooArgList& pdfs=RooArgList(), const std::string& plot_name="");
    
    /**
     *  @brief Constructor for Plot
     *
     *  This will initialise the Plot which can then be plotted via
     *  Plot::PlotIt()
     *
     *  @param cfg_plot PlotConfig holding plotting configuration
     *  @param dimension the dimension to plot (e.g. a RooRealVar)
     *  @param datasets the datasets to plot (e.g. vector of RooDataSets)
     *  @param pdfs (optional) a RooArgList of PDFs to plot; the first PDF must be the overall PDF, further PDFs must be components of this PDF
     *  @param plot_name (optional) a name for this plot (i.e. the output files; will be the variable name if empty)
     */
    Plot(const PlotConfig& cfg_plot, const RooAbsRealLValue& dimension, const std::vector<const RooAbsData*>& datasets, const RooArgList& pdfs=RooArgList(), const std::string& plot_name="");
    
    /**
     *  @brief Actually plot the plot in normal scale
     *
     *  This function will plot the data in linear scale.
     */
    void PlotIt() const { PlotHandler(false); }

    /**
     *  @brief Actually plot the plot in log y scale
     *
     *  This function will plot the data in logarithmic scale on y axis.
     */
    void PlotItLogY() const { PlotHandler(true); }
    
    /**
     *  @brief Actually plot the plot in normal and log y scale
     *
     *  This function will plot the data in linear and logarithmic y axis scale.
     */
    void PlotItLogNoLogY() const { PlotHandler(false); PlotHandler(true, "_log"); }
    
    /**
     *  @brief Destructor for Plot
     */
    virtual ~Plot();
    
  protected:
    /**
     *  \brief PlotConfig instance to use
     */
    const PlotConfig& config_plot_;

    /**
     *  @brief Internal plotting handler
     *
     *  This function will perform the actual plotting and is called by public
     *  functions.
     *
     *  @param logy use logarithmic y scale
     *  @param suffix suffix to put after file names
     */
    void PlotHandler(bool logy, const std::string& suffix="") const;
    
    /**
     *  @brief Dimension to plot in
     */
    const RooAbsRealLValue& dimension_;
    
    /**
     *  @brief Dataset to plot
     */
    std::vector<const RooAbsData*> datasets_;
    
    /**
     *  @brief PDF and sub components to plot
     */
    RooArgList pdfs_;
    
    /**
     *  @brief Plot name
     */
    std::string plot_name_;
    
    /**
     *  @brief Plot directory
     */
    std::string plot_dir_;
    
  private:
  };  
} // namespace plotting
} // namespace doofit

#endif // DOOFIT_PLOTTING_PLOT_PLOT_h