#ifndef DOOFIT_FITTER_EASYFIT_EASYFIT_H
#define DOOFIT_FITTER_EASYFIT_EASYFIT_H

// from STL
#include <map>
#include <set>
#include <string>

// from ROOT
#include "TStopwatch.h"

// from RooFit
#include "RooArgSet.h"
#include "RooCmdArg.h"
#include "RooLinkedList.h"

// forward declarations - RooFit
class RooAbsData;
class RooAbsPdf;
class RooFitResult;
class RooWorkspace;

/** @namespace doofit::fitter::easyfit 
 *  @brief Namespace for the EasyFit framework. 
 */

/** @class doofit::fitter::easyfit::EasyFit
 *  @brief Class to handle a single fit of a RooAbsPdf to a RooDataSet. 
 *
 *  This class interfaces the fitTo method of RooAbsPdf. It is intended as a 
 *  general interface to allow steering of any fit with a RooAbsPdf. The final 
 *  goal is to implement helperfunctions and clear structures to allow an easy
 *  configuration of the fit behaviour of one single fit with all the special
 *  fit options that RooFit offers (see the <a href="http://root.cern.ch/root/html/RooAbsPdf.html#RooAbsPdf:fitTo">RooFit documentation</a>).
 *  
 *  @author Julian Wishahi 
 *  @date 2012-05-28
 */ 

namespace doofit {
namespace fitter {
namespace easyfit {

class EasyFit
{
 public:
  EasyFit(const std::string& fit_name);
  ~EasyFit();

  void SetPdfAndDataSet(RooAbsPdf* pdf, RooAbsData* data);
  void SetPdfAndDataSet(const RooWorkspace& ws, const std::string& pdf_name, RooAbsData* data);

  void Fit() { PrepareFit(); ExecuteFit(); FinalizeFit();}
  const RooFitResult* GetFitResult();
  
  /**
   *  @brief Get timing information from fit
   *
   *  Return consumed fit time as std::pair<double, double> with real wall time
   *  and cpu time. The latter will be misleading in a multi-core fit as the 
   *  standard utilities only count the CPU time of the mother process and not
   *  its children.
   *
   *  @return timing information as pair containing real (first) and CPU (second) time
   */
  std::pair<double, double> FitTime() const;
  
  /** @name FitOptionSetters
   *
   *  Function to set fit control options as documented in @ref LogLLOptions,
   *  @ref FitFlowOptions and @ref InformationalOptions.
   */
  /**@{*/

  /** @brief Sets the number of CPUs to be used (via @ref fc_num_cpu_)
   *
   *  Sets the number of processes that are created for -logLL evaluation. Only 
   *  positive values are allowed. Default is 1.
   */
  EasyFit& SetNumCPU(int fc_num_cpu, int fc_num_cpu_strategy=0);

  /** @brief This method adds an extended likelihood term to the -logLL.
   *
   *  Default is false.
   */
  EasyFit& SetExtended(bool fc_extended);
  
  /** @brief Applies all constrained terms of the @ref pdf_ in the likelihood.
   *
   *
   *  Default is false.
   */
  EasyFit& SetConstrained(bool fc_constrained);


  /** @brief Adds argset of constraining pdfs */
  EasyFit& SetExternalConstraints(const RooArgSet* fc_external_constraints);

  /** @brief Adds argset of conditional observables */
  EasyFit& SetConditionalObservables(const RooArgSet* fc_conditional_observables);

  /** @brief Defines the Migrad strategy. 
   *
   *  Accepted codes are 0,1,2 and represent MINUIT strategies for dealing
   *  most efficiently with fast FCNs (0), expensive FCNs (2)
   *  and 'intermediate' FCNs (1). 
   *  Default is 1.
   */
  EasyFit& SetStrategy( int fc_strategy);

  /** @brief Activate constant term optimization of test statistic during minimization.
   *
   *  Default is true.
   */
  EasyFit& SetOptimize(int fc_optimize);
  
  /** @brief Apply correction to errors and covariance matrix using sum-of-weights covariance matrix.
   *
   *  Apply correction to errors and covariance matrix using sum-of-weights covariance matrix
   *  to obtain correct error for weighted likelihood fits. If this option is activated the
   *  corrected covariance matrix is calculated as Vcorr = V C-1 V, where V is the original 
   *  covariance matrix and C is the inverse of the covariance matrix calculated using the
   *  weights squared. 
   *  Default is false.
   *  
   *  While the estimated values of the parameters will always be calculated taking the weights into account,
   *  there are multiple ways to estimate the errors on these parameter values. You are advised to make an
   *  explicit choice on the error calculation:
   *    - Either set it to true, to calculate a sum-of-weights corrected HESSE error matrix
   *      (error will be proportional to the number of events)
   *    - Or set it to false, to return errors from original HESSE error matrix
   *      (which will be proportional to the sum of the weights)
   *  If you want the errors to reflect the information contained in the 
   *  provided dataset, choose true. If you want the errors to reflect the 
   *  precision you would be able to obtain with an unweighted dataset
   *  with 'sum-of-weights' events, choose false.
   *
   *  <b>Warning:</b> Please be aware that the sum-of-weights correction does not apply to
   *  MINOS errors.
   */  
  EasyFit& SetSumW2Error(bool fc_sumw2err);
  
  /** @brief Choose minimization package and algorithm to use..
   *
   *  Allowed combinations are:
   *  <table border="0">
   *  <tr> <td> <b>Type</b> </td> <td> <b>Algorithm</b> </td> </tr>  
   *  <tr> <td> OldMinuit   </td> <td> migrad, simplex, minimize (=migrad+simplex), migradimproved (=migrad+improve) </td> </tr>
   *  <tr> <td> Minuit      </td> <td> migrad, simplex, minimize (=migrad+simplex), migradimproved (=migrad+improve) </td> </tr>
   *  <tr> <td> Minuit2     </td> <td> migrad, simplex, minimize, scan </td> </tr>
   *  <tr> <td> GSLMultiMin </td> <td> conjugatefr, conjugatepr, bfgs, bfgs2, steepestdescent </td> </tr>
   *  <tr> <td> GSLSimAn    </td> <td> - </td> </tr>
   *  </table>
   *  
   *  Default is Minuit2,minimize.
   */
  EasyFit& SetMinimizer(const std::string& fc_minimizer_type, const std::string& fc_minimizer_algo);
  
  /** @brief Controls if HESSE should be run before MIGRAD.
   *
   *  Default is false.
   */
  EasyFit& SetHesseInit(bool fc_hesse_init);

  /** @brief Controls if HESSE is run after MIGRAD.
   *
   *  Default is true.
   */
  EasyFit& SetHesse(bool fc_hesse);

  /** @brief Controls if MINOS is run after HESSE.
   *
   *  Default is true.
   */
  EasyFit& SetMinos(bool fc_minos);

  /** @brief When applied, MINOS runs only on the defined subset of parameters.
   *
   *  If used, it sets @ref fc_minos_wpars_ to true (default is false). The @ref
   *  fc_minos_pars_ set is set to the parameter set. 
   *  Default is that no set is defined.
   */
  EasyFit& SetMinosPars(const RooArgSet* fc_minos_pars);

  /** @brief Controls if a RooFitResult is saved on fitting.
   *
   *  Default is true.
   */
  EasyFit& SetSave(bool fc_save);

  /** @brief Controls if verbose fit output is printed.
   *
   *  Default is false.
   */
  EasyFit& SetVerbose(bool fc_verbose);

  /** @brief Enable or disable MINUIT warnings.
   *
   *  Default is true.
   */
  EasyFit& SetWarnings(bool fc_warnings);

  /** @brief Sets the MINUIT print level.
   *
   *  Takes values from -1 through 3. At -1 all RooFit informational 
   *  messages are suppressed as well.
   *  Default is 1.
   */
  EasyFit& SetPrintLevel(int fc_printlevel);

  /** @brief Control number of p.d.f evaluation errors printed per likelihood evaluation.
   *
   *  Control number of p.d.f evaluation errors printed per likelihood evaluation. A negative
   *  value suppress output completely, a zero value will only print the error count per p.d.f component,
   *  a positive value is will print details of each error up to numErr messages per p.d.f component.
   *  Default is 10.
   */
  EasyFit& SetNumEvalErrors(int fc_numevalerr);

  /** @brief Print out time CPU and wall clock consumption of fit steps.
   *
   *  Default is true.
   */
  EasyFit& SetTimer(bool fc_timer);
  /**@}*/


 private:
  void PrepareFit();
  void ExecuteFit();
  void FinalizeFit();

  bool PdfAndDataReady(); ///< Helper function to check that @ref pdf_ and @ref data_ are set.
  bool CheckSettingOptionsOk(); ///< Helper function to check if setting or changing an option is allowed in the current state of the object.
  bool CheckMinimizerCombiOk(const std::string& type, const std::string& algo); ///< Helper function to check allowed combinations of minimizer type and algo (using @ref minimizer_combs_).

  std::string fit_name_; ///< Name of the fit used to identify if several fits are performed.

  bool prepared_;   ///< If true, no changes are allowed to the fit options.
  bool fitted_;     ///< If true, no subsequent fit can be started with the active EasyFit object.
  bool finalized_;  ///< Set to true after all steps have finished and standard output has been written.

  RooAbsPdf* pdf_;            ///< PDF to be used for fit.
  RooAbsData* data_;          ///< Dataset to be fitted.
  RooFitResult* fit_result_;  ///< RooFitResult of the fit.

  std::map<std::string,std::set<std::string> > minimizer_combs_; ///< Defines allowed combination of minimizer type and algo.

  std::map<std::string,RooCmdArg> fc_map_; ///< Intermediate map to save RooCmdArg that are parsed from the fit options. 
  RooLinkedList fc_linklist_; ///< RooLinkedList containing the RooCmdArg of the @fc_map_ to be given to the fitTo statement.

  /**
   *  @brief Total CPU time consumed by fit (in ms)
   */
  double time_cpu_;
  
  /**
   *  @brief Total real time consumed during fit (in ms)
   */
  double time_real_;
  
  /** @name LogLLOptions
   *  
   *  These members are used to control RooFit's -log(L) construction. 
   *  They are based on the options documented in 
   *  <a href="http://root.cern.ch/root/html/RooAbsPdf.html#RooAbsPdf:fitTo">RooFit</a>. 
   *  These members are parsed to RooCmdArg objects and added to the 
   *  @ref fit_cmds_ list.
   */
  /**@{*/
  int  fc_num_cpu_;     ///< Number of CPUs to be used. Default is 1.
  int  fc_num_cpu_strategy_; ///< Parallelisation strategy to be used. Default is 0.
  bool fc_extended_;    ///< Add extended likelihood term (false by default).
  bool fc_constrained_; ///< Apply all constrained contained in the p.d.f. in the likelihood (false by default).
  bool fc_constrained_externally_; ///< Flag for internal checks if external constraints are set.
  const RooArgSet* fc_external_constraints_; ///< RooArgSet of constraining pdfs.
  
  bool fc_conditional_observables_set_; ///< Flag for internal checks if conditional observables are set.
  const RooArgSet* fc_conditional_observables_; ///< RooArgSet of conditional observables.
  /**@}*/

  /** @name FitFlowOptions
   *  
   *  These members are used to control the flow of RooFit's fit procedure. 
   *  They are based on the options documented in 
   *  <a href="http://root.cern.ch/root/html/RooAbsPdf.html#RooAbsPdf:fitTo">RooFit</a>. 
   *  These members are parsed to RooCmdArg objects and added to the 
   *  @ref fc_map_.
   */
  /**@{*/
  int fc_strategy_;   ///< Fit strategy (0 through 2, default is 1).
  int fc_optimize_;           ///< Activate constant term optimization during minimization (true by default).

  /** @brief Correct errors and covariance matrix using sum-of-weights covariance matrix (false by default).
   *
   *  This should obtain correct error for weighted likelihood fits. If this 
   *  option is activated the corrected covariance matrix is calculated as 
   *  Vcorr = V C-1 V, where V is the original covariance matrix and C is the 
   *  inverse of the covariance matrix calculated using the weights squared.
   *  Please be aware that the sum-of-weights correction does not apply to
   *  MINOS errors.
   */
  bool fc_sumw2err_;  

  std::string fc_minimizer_type_;
  std::string fc_minimizer_algo_;

  bool       fc_hesse_init_;   ///< Flag controls if HESSE before MIGRAD (false by default).
  bool       fc_hesse_;        ///< Flag controls if HESSE is run after MIGRAD (true by default).
  bool       fc_minos_;        ///< Flag controls if MINOS is run after HESSE (true by default).
  bool       fc_minos_wpars_;  ///< Flag controls if MINOS should be run only on a subset of parameters (false by default).
  const RooArgSet* fc_minos_pars_;   ///< Only run MINOS on given subset of arguments. Requires @ref fc_minos_ to be set to true.

  /**@}*/

  /** @name InformationalOptions
   *  
   *  These members are used to control RooFit's informational output. 
   *  They are based on the options documented in 
   *  <a href="http://root.cern.ch/root/html/RooAbsPdf.html#RooAbsPdf:fitTo">RooFit</a>. 
   *  These members are parsed to RooCmdArg objects and added to the 
   *  @ref fc_map_.
   */
  /**@{*/

  bool fc_save_;        ///< Controls if the FitResult is saved (true by default).
  bool fc_verbose_;     ///< Flag controls if verbose output is printed (false by default).
  bool fc_warnings_;    ///< Enable or disable MINUIT warnings (enabled by default).

  /** @brief Minuit print level (-1 through 3, default is 1).
   *
   *  When set to a value of -1 all RooFit informational messages are suppressed 
   *  as well.
   */ 
  int  fc_printlevel_;
  
  /**  @brief Control number of p.d.f evaluation errors printed per likelihood evaluation.
   *  
   *   Takes values -1 through inf. Default is 10. A negative value suppresses 
   *   output completely, a zero value will only print the error count per PDF 
   *   component, a positive value will print details of each error up to 
   *   that number of messages per p.d.f component. 
   */
  int  fc_numevalerr_;  

  bool fc_timer_;       ///< Time CPU and wall clock consumption (true by default).
  /**@}*/

}; // class EasyFit

} // namespace easyfit
} // namespace fitter
} // namespace doofit

#endif // DOOFIT_FITTER_EASYFIT_EASYFIT_H
