#ifndef DOOFIT_FITTER_EASYFIT_EASYFITRESULT_H
#define DOOFIT_FITTER_EASYFIT_EASYFITRESULT_H

// from STL
#include <string>
#include <map>
#include <limits>

// from ROOT

// from RooFit
#include "RooRealVar.h"

// forward declarations
class RooFitResult; 

/** @class doofit::fitter::easyfit::EasyFitResult
 *  @brief Smarter fit result container with no direct RooFit dependencies
 *
 *  EasyFitResult aims at providing the same functionality as RooFitResult 
 *  without having direct RooFit dependencies (apart from functionality to 
 *  convert a RooFitResult into an EasyFitResult). Advantages are faster access 
 *  and less memory leaks than RooFitResults. This especially helps in mass fit
 *  result handling of toy studies.
 *
 *  Support for input from and output to TTree tuples is available.
 *  
 *  @author Florian Kruse 
 *  @date 2015-03-24
 */ 

namespace doofit {
namespace fitter {
namespace easyfit {

class EasyFitVariable;

class EasyFitResult {
 public:
  EasyFitResult(const RooFitResult& fit_result);

  /** @name Standard getters
   */
  ///@{
  double fcn() const { return fcn_; }
  double edm() const { return edm_; }
  int quality_covariance_matrix() const { return quality_covariance_matrix_; }
  const std::vector<std::pair<std::string, int>>& status() const { return status_; }

  const std::map<std::string, EasyFitVariable>& parameters_const() const { return parameters_const_; }
  const std::map<std::string, EasyFitVariable>& parameters_float_final() const { return parameters_float_final_; }
  const std::map<std::string, EasyFitVariable>& parameters_float_init() const { return parameters_float_init_; }
  ///@}

 private:
  /**
   * @brief All floating parameters (initial state before fit)
   */
  std::map<std::string, EasyFitVariable> parameters_float_init_;

  /**
   * @brief All floating parameters (final state after fit)
   */
  std::map<std::string, EasyFitVariable> parameters_float_final_;

  /**
   * @brief All constant parameters
   */
  std::map<std::string, EasyFitVariable> parameters_const_;

  /**
   * @brief Status codes of fitting algorithms (algorithm name + status code)
   */
  std::vector<std::pair<std::string, int>> status_;

  /**
   * @brief Covariance matrix quality
   */
  int quality_covariance_matrix_;

  /**
   * @brief FCN of the fit
   */
  double fcn_;

  /**
   * @brief EDM of the fit
   */
  double edm_;
}; // class EasyFitResult


/** @class doofit::fitter::easyfit::EasyFitVariable
 *  @brief Smarter variable container with no direct RooFit dependencies
 *
 *  EasyFitVariable aims at providing the same functionality as RooRealVar 
 *  without having direct RooFit dependencies (apart from functionality to 
 *  convert a RooRealVar into an EasyFitVariable). Advantages are faster access 
 *  and less memory leaks than RooRealVars. 
 *  
 *  @author Florian Kruse 
 *  @date 2015-03-24
 */ 
class EasyFitVariable {
 public:
  EasyFitVariable(const std::string& name, const std::string& title, 
                  double value, double min, double max,
                  const std::string& unit="") :
    name_(name),
    title_(title),
    unit_(unit),
    value_(value),
    min_(min),
    max_(max),
    has_error_(false),
    has_asym_error_(false),
    error_(std::numeric_limits<double>::quiet_NaN()),
    error_low_(std::numeric_limits<double>::quiet_NaN()),
    error_high_(std::numeric_limits<double>::quiet_NaN()),
    constant_(false)
  {}

  EasyFitVariable(const std::string& name, const std::string& title, 
                  double value, const std::string& unit="") :
    EasyFitVariable(name, title, value, std::numeric_limits<double>::infinity(),
                    std::numeric_limits<double>::infinity(), unit)
  {}

  EasyFitVariable(const std::string& name, const std::string& title, 
                  double min, double max, const std::string& unit="") :
    EasyFitVariable(name, title, (min+max)/2.0, min, max, unit)
  {}

  EasyFitVariable(const RooRealVar& var) :
    EasyFitVariable(var.GetName(), var.GetTitle(), var.getVal(), 
                    std::numeric_limits<double>::infinity(), 
                    std::numeric_limits<double>::infinity(), var.getUnit())
  {
    constant_ = var.isConstant();
    if (var.hasMin()) {
      min_ = var.getMin();
    }
    if (var.hasMax()) {
      max_ = var.getMax();
    }
    if (var.hasError()) {
      has_error_ = true;
      error_ = var.getError();
    }
    if (var.hasAsymError()) {
      has_asym_error_ = true;
      error_low_ = var.getErrorLo();
      error_high_ = var.getErrorHi();
    }
  }

  /** @name Standard getters
   */
  ///@{
  const std::string& name() const { return name_; }
  const std::string& title() const { return title_; }
  const std::string& unit() const { return unit_; }

  double value() const { return value_; }
  double min() const { return min_; }
  double max() const { return max_; }

  bool has_error() const { return has_error_; }
  bool has_asym_error() const { return has_asym_error_; }

  double error() const { return error_; }
  double error_low() const { return error_low_; }
  double error_high() const { return error_high_; }

  bool constant() const { return constant_; }
  ///@}

  /** @name Standard setters
   */
  ///@{
  void set_name(const std::string& name) { name_ = name; }
  void set_title(const std::string& title) { title_ = title; }
  void set_unit(const std::string& unit) { unit_ = unit; }

  void set_value(double value) { value_ = value; }
  void set_min(double min) { min_ = min; }
  void set_max(double max) { max_ = max; }

  void set_error(double error) { has_error_ = true; error_ = error; }
  void set_error_low(double error_low) { has_asym_error_ = true; error_low_ = error_low; }
  void set_error_high(double error_high) { has_asym_error_ = true; error_high_ = error_high; }

  void set_constant(bool constant) { constant_ = constant; }
  ///@}

 private:
  std::string name_;
  std::string title_;
  std::string unit_;

  double value_;
  double min_;
  double max_;

  bool has_error_;
  bool has_asym_error_;

  double error_;
  double error_low_;
  double error_high_;

  bool constant_;
}; // class EasyFitVariable


} // namespace easyfit
} // namespace fitter
} // namespace doofit

#endif // DOOFIT_FITTER_EASYFIT_EASYFITRESULT_H
