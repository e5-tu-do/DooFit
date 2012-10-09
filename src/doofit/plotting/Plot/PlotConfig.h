#ifndef DOOFIT_PLOTTING_PLOT_PLOTCONFIG_h
#define DOOFIT_PLOTTING_PLOT_PLOTCONFIG_h

// STL

// BOOST

// ROOT

// from RooFit

// from project
#ifndef __CINT__
#include "doofit/Config/AbsConfig.h"
#include "doofit/Config/CommaSeparatedPair.h"
#else
// ROOT Cint hacks...
#include "../../Config/AbsConfig.h"
#include "../../Config/CommaSeparatedPair.h"
#endif /* __CINT __ */

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
     *  @brief Default constructor for PlotConfig
     *
     *  Unfortunately this constructor is needed for ROOT CINT and streaming to
     *  ROOT files.
     */
    PlotConfig();
    
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
    
  protected:
    
  private:
  };
  
} // namespace plotting
} // namespace doofit
  
#endif // DOOFIT_PLOTTING_PLOT_PLOTCONFIG_h