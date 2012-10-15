#ifndef DOOFIT_PLOTTING_PLOT_PLOTCONFIG_h
#define DOOFIT_PLOTTING_PLOT_PLOTCONFIG_h

// STL
#include <vector>

// BOOST

// ROOT

// from RooFit

// from project
#include "doofit/Config/AbsConfig.h"
#include "doofit/Config/CommaSeparatedList.h"

// forward declarations

namespace doofit {
namespace plotting {
/** @class PlotConfig
 *  @brief DooFit Config class for the Plot class
 *
 *  This class is responsible for all Plot related configuration options
 *  which are not covered by CommonConfig.
 */
class PlotConfig : public Config::AbsConfig {
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
  
  /** 
   * @brief Getter for pdf_linecolor_map_
   **/
  const Config::CommaSeparatedList<int>& pdf_linecolor_map() const { return pdf_linecolor_map_; }
  /** 
   * @brief Setter for pdf_linecolor_map_
   **/
  void set_pdf_linecolor_map(const Config::CommaSeparatedList<int>& pdf_linecolor_map) { pdf_linecolor_map_ = pdf_linecolor_map; }
  
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
   *  @see Config::AbsConfig::DefineOptions()
   */
  void DefineOptions();
  
  /**
   *  \brief Load program options into own member variables.
   *
   *  This function will store all options from the variable map into own
   *  members.
   *
   *  @see Config::AbsConfig::LoadOptions()
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
  Config::CommaSeparatedList<int> pdf_linecolor_map_;
  /**
   *  @brief Line style map for PDF lines
   */
  Config::CommaSeparatedList<int> pdf_linestyle_map_;
};

} // namespace plotting
} // namespace doofit
  
#endif // DOOFIT_PLOTTING_PLOT_PLOTCONFIG_h