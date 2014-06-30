#ifndef DOOFIT_PLOTTING_FITRESULT_FITRESULTPRINTER_h
#define DOOFIT_PLOTTING_FITRESULT_FITRESULTPRINTER_h

// STL

// BOOST

// ROOT

// from RooFit

// from DooCore

// from project

// forward declarations
class RooFitResult;

namespace doofit {
namespace plotting {
namespace fitresult {

/** @class FitResultPrinter
 *  @brief Flexible plotter for fitresults
 *
 *  This class plots a RooFitResult in a more flexible way than the 
 *  RooFitResult::Print functionality.
 *
 *  @section usage Usage
 *
 *  Usage is shown via this example:
 *
 * @code
 * doofit::fitter::easyfit::EasyFit efit_full("fit_toy")
 * efit_full.Fit();
 * fit_result = efit_full.GetFitResult();
 * doofit::plotting::correlations::FitResultPrinter printer(*fit_result);
 * printer.Print();
 * @endcode
 *
 *  @author Florian Kruse
 */

class FitResultPrinter {
 public:
  /**
   *  @brief Default constructor for FitResultPrinter
   *
   *  @param fit_result the fit result to print
   */
  FitResultPrinter(const RooFitResult& fit_result);
  
  ~FitResultPrinter() {}
  
  /**
   *  @brief Print the RooFitResult
   */
  void Print() const {
    PlotHandler();
  }

  FitResultPrinter& set_full_precision_printout(bool full_precision_printout) {
  	full_precision_printout_ = full_precision_printout;
  	return *this;
  }

  FitResultPrinter& set_print_pulls(bool print_pulls) {
  	print_pulls_ = print_pulls;
  	return *this;
  }
  
 protected:
  
  /**
   *  @brief Print the RooFitResult (internal handler)
   */
  void PlotHandler() const;
  
 private:
  const RooFitResult& fit_result_;

  bool full_precision_printout_;
  bool print_pulls_;
};


} // namespace fitresult
} // namespace plotting
} // namespace doofit

#endif //DOOFIT_PLOTTING_FITRESULT_FITRESULTPRINTER_h