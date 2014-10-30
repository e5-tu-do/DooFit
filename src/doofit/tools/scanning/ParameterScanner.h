#ifndef DOOFIT_TOOLS_SCANNING_PARAMETERSCANNER_h
#define DOOFIT_TOOLS_SCANNING_PARAMETERSCANNER_h

// STL

// BOOST

// ROOT

// from RooFit

// from DooCore

// from project
#include "doofit/tools/scanning/ParameterScannerConfig.h"

// forward declarations
namespace doofit { namespace builder {
  class EasyPdf;
}}
namespace doofit {
namespace tools {
namespace scanning {
    
/** @class ParameterScanner
 *  @brief Helper tool to scan parameters over ranges
 *
 *  Right now this class is without any functionality. For all current use 
 *  cases, please use ParameterScannerConfig directly.
 */

class ParameterScanner {
 public:
  ParameterScanner(const ParameterScannerConfig& cfg_pscanner) :
    config_parameterscanner_(cfg_pscanner) {}

  virtual ~ParameterScanner() {}

 protected:
  /**
   *  \brief ParameterScannerConfig instance to use
   */
  const ParameterScannerConfig& config_parameterscanner_;

 private:
};  

} // namespace scanning
} // namespace tools
} // namespace doofit

#endif // DOOFIT_TOOLS_SCANNING_PARAMETERSCANNER_h