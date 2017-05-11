#include "FitResultPrinter.h"

// STL
#include <iomanip>
#include <string>
#include <vector>

// from Boost
#include <boost/format.hpp>

// from ROOT
#include <TIterator.h>

// from RooFit
#include <RooFitResult.h>
#include <RooArgList.h>
#include <RooAbsArg.h>
#include <RooRealVar.h>

// from DooCore
#include <doocore/io/MsgStream.h>
#include <doocore/statistics/general.h>

// from project
#include "doofit/fitter/easyfit/EasyFitResult.h"

doofit::fitter::easyfit::FitResultPrinter::FitResultPrinter(const RooFitResult& fit_result)
: fit_result_(fit_result),
  full_precision_printout_(false),
  print_pulls_(true),
  print_const_pars_(true)
{
}

doofit::fitter::easyfit::FitResultPrinter::FitResultPrinter(const doofit::fitter::easyfit::EasyFitResult& fit_result)
: fit_result_(fit_result),
  full_precision_printout_(false),
  print_pulls_(true),
  print_const_pars_(true)
{
}

void doofit::fitter::easyfit::FitResultPrinter::PlotHandler() const {
  using namespace doocore::statistics::general;
  using namespace doocore::io;
  using boost::format;
  using boost::io::group;
  using namespace doofit::fitter::easyfit;

  // if (&fit_result_ == nullptr) {
  //   serr << "Error in FitResultPrinter::PlotHandler(...): Fit result invalid." << endmsg;
  // } else {
  std::cout << std::endl;
  std::cout << "FitResultPrinter: NLL = " << fit_result_.fcn() << ", EDM = " << fit_result_.edm() << std::endl;
  std::cout << "                  covariance matrix quality: ";
  switch(fit_result_.quality_covariance_matrix()) {
    case -1:
      std::cout << TerminalColorCode(3) << fit_result_.quality_covariance_matrix() << ": Unknown, matrix was externally provided" << TerminalResetCode() << std::endl; 
      break;
    case 0:
      std::cout << TerminalColorCode(2) << fit_result_.quality_covariance_matrix() << ": Not calculated at all" << TerminalResetCode() << std::endl; 
      break;
    case 1:
      std::cout << TerminalColorCode(2) << fit_result_.quality_covariance_matrix() << ": Approximation only, not accurate" << TerminalResetCode() << std::endl; 
      break;
    case 2:
      std::cout << TerminalColorCode(1) << fit_result_.quality_covariance_matrix() << ": Full matrix, but forced positive-definite" << TerminalResetCode() << std::endl; 
      break; 
    case 3:
      std::cout << TerminalColorCode(0) << fit_result_.quality_covariance_matrix() << ": Full, accurate covariance matrix" << TerminalResetCode() << std::endl; 
      break;  
  }

  const std::vector<std::pair<std::string, int>>& status(fit_result_.status());
  unsigned int i{0};
  std::cout << "                  Fit status: ";
  for (auto s : status) {
  //for (unsigned int i=0; i<fit_result_.numStatusHistory(); ++i) {
    if (s.second == 0) {
      std::cout << TerminalColorCode(0);
    } else if (s.second == 1) {
      std::cout << TerminalColorCode(1);
    } else {
      std::cout << TerminalColorCode(2);
    }
    std::cout << s.first << "=" << s.second << TerminalResetCode();
    if (i<(status.size()-1)) {
      std::cout << ", ";
    }
    ++i;
  }
  std::cout << std::endl;
  std::cout << std::endl;

  if (print_const_pars_){
    //RooArgList par_list_const = fit_result_.constPars();
    const std::map<std::string, EasyFitVariable>& par_list_const = fit_result_.parameters_const();

    if (par_list_const.size() > 0) {
      std::cout << format("%-40s %17s") % "          Constant Parameter           " % "      Value      " << std::endl;
      std::cout << format("%-40s %17s") % "---------------------------------------" % "-----------------" << std::endl;

      // TIterator* it_const = par_list_const.createIterator();
      // RooAbsArg* arg = NULL;
      // while ((arg = dynamic_cast<RooAbsArg*>(it_const->Next()))) {
      //   RooRealVar* var = dynamic_cast<RooRealVar*>(arg);
      //   if (var != NULL) {
      //     std::cout << format("%-40s %17g") % var->GetName() % var->getVal() << std::endl;
      //   }
      // }
      for (auto par : par_list_const) {
        std::cout << format("%-40s %17g") % par.first % par.second.value() << std::endl;
      }

      std::cout << std::endl;
      // delete it_const;
      // it_const = nullptr;
    }
  }

  // RooArgList par_list_float_final = fit_result_.floatParsFinal();
  // RooArgList par_list_float_init  = fit_result_.floatParsInit();  
  const std::map<std::string, EasyFitVariable>& par_list_float_final = fit_result_.parameters_float_final();
  const std::map<std::string, EasyFitVariable>& par_list_float_init = fit_result_.parameters_float_init();

  // TIterator* it_float = par_list_float_final.createIterator();
  // RooAbsArg* arg = NULL;

  bool asym_errors_present = false;
  // while ((arg = dynamic_cast<RooAbsArg*>(it_float->Next()))) {
  //   RooRealVar* var = dynamic_cast<RooRealVar*>(arg);
  //   if (var != NULL) {
  //     ValueWithError<double> val(var->getVal(), var->getError());

  //     if (var->hasAsymError()) {
  //       asym_errors_present = true;
  //     }
  //   }
  // }
  for (auto par : par_list_float_final) {
    // ValueWithError<double> val(par.second.value(), par.second.error());

    if (par.second.has_asym_error()) {
      asym_errors_present = true;
    }
  }
  // delete it_float;
  // it_float = nullptr;

  // it_float = par_list_float_final.createIterator();

  if (asym_errors_present) {
    std::cout << format("%-40s %17s %17s +/- %-37s ") % "               Parameter               " % "    InitValue    " % "      Fit result" % "Error                               ";
  } else {
    std::cout << format("%-40s %17s %17s +/- %-17s ") % "               Parameter               " % "    InitValue    " % "      Fit result" % "Error           ";
  }
  if (print_pulls_) {
    std::cout << format("%17s") % "       Pull      ";
  }
  std::cout << std::endl;

  if (asym_errors_present) {
    std::cout << format("%-40s %17s %17s-----%-37s ") % "---------------------------------------" % "-----------------" % "----------------" % "------------------------------------";
  } else {
    std::cout << format("%-40s %17s %17s-----%-17s ") % "---------------------------------------" % "-----------------" % "----------------" % "----------------";
  }
  if (print_pulls_) {
    std::cout << format("%17s") % "-----------------";
  }
  std::cout << std::endl;

  // while ((arg = dynamic_cast<RooAbsArg*>(it_float->Next()))) {
  for (auto par : par_list_float_final) {
    // RooRealVar* var = dynamic_cast<RooRealVar*>(arg);
    // if (var != NULL) {
    ValueWithError<double> val(par.second.value(), par.second.error());

    bool var_has_asym_error = par.second.has_asym_error();
    if (var_has_asym_error) {
      val.error_lo = -par.second.error_low();
      val.error_hi = par.second.error_high();
    }

    val.set_full_precision_printout(full_precision_printout_);
    // RooRealVar* var_init = dynamic_cast<RooRealVar*>(par_list_float_init.find(var->GetName()));
    const EasyFitVariable& var_init = par_list_float_init.at(par.first);
    double val_init = 0.0, pull = 0.0;
    // if (var_init != NULL) {
    val_init = var_init.value();
    pull = (val_init-val.value)/val.error;
    // }

    std::string str_color, str_term;
    if (std::abs(pull) < 2.0) {
      str_color = TerminalColorCode(0);
    } else if (std::abs(pull) > 2.0 && std::abs(pull) <= 3.0) {
      str_color = TerminalColorCode(1);
    } else if (std::abs(pull) > 3.0) {
      str_color = TerminalColorCode(2);
    }
    str_term = TerminalResetCode();

    val.FormatString();

    if (asym_errors_present) {
      if (var_has_asym_error) {
        std::string str_asym_error = "+ " + val.str_error_hi() + " - " + val.str_error_lo();
        std::cout << format("%-40s %17g %17s %-37s ") % par.second.name() % val_init % val.str_value() % str_asym_error;
      } else {
        std::cout << format("%-40s %17g %17s +/- %-33s ") % par.second.name() % val_init % val.str_value() % val.str_error();
      }
    } else {
      std::cout << format("%-40s %17g %17s +/- %-17s ") % par.second.name() % val_init % val.str_value() % val.str_error();
    }
    if (print_pulls_) {
      std::cout << format("%s%17g%s") % str_color % pull % str_term;
    }
    std::cout << std::endl;
    // }
  }
  // delete it_float;
  // it_float = nullptr;

  std::cout << std::endl;
  // }
}
