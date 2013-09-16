#ifndef DOOFIT_PLOTTING_PLOT_PLOTCONFIG_h
#define DOOFIT_PLOTTING_PLOT_PLOTCONFIG_h

// STL
#include <vector>

// BOOST

// ROOT

// from RooFit

// from project
#include "doofit/config/AbsConfig.h"
#include "doofit/config/CommaSeparatedList.h"

// forward declarations
class TCanvas;

namespace doofit {
namespace plotting {
/** @class PlotConfig
 *  @brief DooFit Config class for the Plot class
 *
 *  This class is responsible for all Plot related configuration options
 *  which are not covered by CommonConfig.
 */
class PlotConfig : public config::AbsConfig {
 public:  
  /**
   *  @brief Standard constructor for PlotConfig
   *
   *  Sets all members to default values.
   *
   *  @param name Name of this Config object.
   */
  PlotConfig(const std::string& name);
  
  /**
   *  @brief Destructor for PlotConfig
   */
  ~PlotConfig();
  
  /**
   *  @brief Return a color for PDF at given index
   *
   *  If index is larger than internal color map, this function will just return
   *  1.
   *
   *  @param index number of the PDF to get the color for
   *  @return the appropriate color value
   */
  int GetPdfLineColor(int index) const;
  
  /**
   *  @brief Return a line style for PDF at given index
   *
   *  If index is larger than internal line style map, this function will just 
   *  return 1.
   *
   *  @param index number of the PDF to get the line style for
   *  @return the appropriate line style value
   */
  int GetPdfLineStyle(int index) const;
  
  /** @name Getter functions
   *  Implementation of getter functions.
   */
  ///@{
  /**
   * @brief Getter for pdf_linecolor_map_
   * 
   * @see set_pdf_linecolor_map()
   **/
  const config::CommaSeparatedList<int>& pdf_linecolor_map() const { return pdf_linecolor_map_; }
  /**
   * @brief Getter for pdf_linestyle_map_
   *
   * @see set_pdf_linestyle_map()
   **/
  const config::CommaSeparatedList<int>& pdf_linestyle_map() const { return pdf_linestyle_map_; }
  
  /**
   * @brief Getter for plot_directory_
   *
   * @see set_plot_directory()
   **/
  std::string plot_directory() const { return plot_directory_; }
  ///@}

  /** @name Setter functions
   *  Implementation of setter functions.
   */
  ///@{
  /**
   * @brief Setter for pdf_linecolor_map_ with config::CommaSeparatedList
   *
   * Set line colors for plotted PDFs as config::CommaSeparatedList. This can 
   * also be set via command line or config file.
   * 
   * @param pdf_linecolors colors to use in same order as PDFs are supplied
   **/
  void set_pdf_linecolor_map(const config::CommaSeparatedList<int>& pdf_linecolors) { pdf_linecolor_map_ = pdf_linecolors; }
  /**
   * @brief Setter for pdf_linecolor_map_ with string
   *
   * Set line colors for plotted PDFs as comma-separated string, e.g. like 
   * "1,4,2,3,5". This option can also be set via command line or config file.
   *
   * @param pdf_linecolors colors to use in same order as PDFs are supplied as comma-separated string (e.g. "1,4,2,3,5")
   **/
  void set_pdf_linecolor_map(const std::string& pdf_linecolors) {
    pdf_linecolor_map_.Parse(pdf_linecolors);
  }
  /**
   * @brief Setter for pdf_linestyle_map_ with config::CommaSeparatedList
   *
   * Set line syles for plotted PDFs as config::CommaSeparatedList. This can
   * also be set via command line or config file.
   *
   * @param pdf_linestyles line styles to use in same order as PDFs are supplied
   **/
  void set_pdf_linestyle_map(const config::CommaSeparatedList<int>& pdf_linestyles) { pdf_linestyle_map_ = pdf_linestyles; }

  /**
   * @brief Setter for pdf_linestyle_map_ with string
   *
   * Set line sytle for plotted PDFs as comma-separated string, e.g. like
   * "1,4,2,3,5". This option can also be set via command line or config file.
   *
   * @param pdf_linestyles line styles to use in same order as PDFs are supplied as comma-separated string (e.g. "1,4,2,3,5")
   **/
  void set_pdf_linestyle_map(const std::string& pdf_linestyles) {
    pdf_linestyle_map_.Parse(pdf_linestyles);
  }
  
  /**
   * @brief Setter for plot_directory_
   *
   * Set plot output directory.
   *
   * @param plot_directory the plot output directory to use
   **/
  void set_plot_directory(const std::string& plot_directory) {
    plot_directory_ = plot_directory;
  }
  ///@}
  
  /** @name Stacked plotting support functions
   *  Functions to support stacked plotting into one file
   */
  ///@{
  /**
   *  @brief Open plot stack on demand
   *
   *  This helper function will be called by Plot objects before plotting. If
   *  not openend already, it will open a ROOT plot stack called AllPlots.pdf in
   *  which all be put.
   */
  void OnDemandOpenPlotStack() const;
  
  /**
   *  @brief Close plot stack (for saving plots)
   *
   *  This helper function can be called to close the plot stack in order to 
   *  save the all plots file.
   */
  void ClosePlotStack() const;
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
   *  @see config::AbsConfig::DefineOptions()
   */
  void DefineOptions();
  
  /**
   *  \brief Load program options into own member variables.
   *
   *  This function will store all options from the variable map into own
   *  members.
   *
   *  @see config::AbsConfig::LoadOptions()
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
  /**
   *  @brief Color map for PDF lines
   */
  config::CommaSeparatedList<int> pdf_linecolor_map_;
  /**
   *  @brief Line style map for PDF lines
   */
  config::CommaSeparatedList<int> pdf_linestyle_map_;
  /**
   *  @brief Plot output directory
   */
  std::string plot_directory_;
  
  /** @name Stacked plotting support members
   *  Members to support stacked plotting into one file.
   */
  ///@{
  /**
   *  @brief Stack already opened
   */
  mutable bool plot_stack_open_;
  
  /**
   *  @brief Dummy canvas to create plot stack
   */
  mutable TCanvas* plot_stack_canvas_;
  ///@}
};

} // namespace plotting
} // namespace doofit
  
#endif // DOOFIT_PLOTTING_PLOT_PLOTCONFIG_h