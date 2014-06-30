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

doofit::plotting::fitresult::FitResultPrinter::FitResultPrinter(const RooFitResult& fit_result)
: fit_result_(fit_result),
  full_precision_printout_(false),
  print_pulls_(true),
  print_const_pars_(true)
{
}

void doofit::plotting::fitresult::FitResultPrinter::PlotHandler() const {
  using namespace doocore::statistics::general;
  using namespace doocore::io;
  using boost::format;
  using boost::io::group;

  std::cout << std::endl;
  std::cout << "FitResultPrinter: NLL = " << fit_result_.minNll() << ", EDM = " << fit_result_.edm() << std::endl;
  std::cout << "                  covariance matrix quality: ";
  switch(fit_result_.covQual()) {
    case -1:
      std::cout << TerminalColorCode(2) << fit_result_.covQual() << ": Unknown, matrix was externally provided" << TerminalResetCode() << std::endl; 
      break;
    case 0:
      std::cout << TerminalColorCode(2) << fit_result_.covQual() << ": Not calculated at all" << TerminalResetCode() << std::endl; 
      break;
    case 1:
      std::cout << TerminalColorCode(2) << fit_result_.covQual() << ": Approximation only, not accurate" << TerminalResetCode() << std::endl; 
      break;
    case 2:
      std::cout << TerminalColorCode(1) << fit_result_.covQual() << ": Full matrix, but forced positive-definite" << TerminalResetCode() << std::endl; 
      break; 
    case 3:
      std::cout << TerminalColorCode(0) << fit_result_.covQual() << ": Full, accurate covariance matrix" << TerminalResetCode() << std::endl; 
      break;  
  }

  std::cout << "                  Fit status: ";
  for (unsigned int i=0; i<fit_result_.numStatusHistory(); ++i) {
    if (fit_result_.statusCodeHistory(i) == 0) {
      std::cout << TerminalColorCode(0);
    } else if (fit_result_.statusCodeHistory(i) == 1) {
      std::cout << TerminalColorCode(1);
    } else {
      std::cout << TerminalColorCode(2);
    }
    std::cout << fit_result_.statusLabelHistory(i) << "=" << fit_result_.statusCodeHistory(i) << TerminalResetCode();
    if (i<(fit_result_.numStatusHistory()-1)) {
      std::cout << ", ";
    }
  }
  std::cout << std::endl;
  std::cout << std::endl;

  if (print_const_pars_){
    RooArgList par_list_const = fit_result_.constPars();

    if (par_list_const.getSize() > 0) {
      std::cout << format("%-30s %15s") % "          Parameter          " % "     Value     " << std::endl;
      std::cout << format("%-30s %15s") % "-----------------------------" % "---------------" << std::endl;

      TIterator* it_const = par_list_const.createIterator();
      RooAbsArg* arg = NULL;
      while ((arg = dynamic_cast<RooAbsArg*>(it_const->Next()))) {
        RooRealVar* var = dynamic_cast<RooRealVar*>(arg);
        if (var != NULL) {
          std::cout << format("%-30s %15g") % var->GetName() % var->getVal() << std::endl;
        }
      }
      std::cout << std::endl;
    }
  }

	RooArgList par_list_float_final = fit_result_.floatParsFinal();
  RooArgList par_list_float_init  = fit_result_.floatParsInit();

  TIterator* it_float = par_list_float_final.createIterator();
  RooAbsArg* arg = NULL;

  std::cout << format("%-30s %15s %15s +/- %-15s ") % "          Parameter          " % "   InitValue   " % "    Fit result" % "Error         ";
  if (print_pulls_) {
    std::cout << format("%15s") % "      Pull     ";
  }
  std::cout << std::endl;

  std::cout << format("%-30s %15s %15s-----%-15s ") % "-----------------------------" % "---------------" % "--------------" % "--------------";
  if (print_pulls_) {
    std::cout << format("%15s") % "---------------";
  }
  std::cout << std::endl;

  while ((arg = dynamic_cast<RooAbsArg*>(it_float->Next()))) {
    RooRealVar* var = dynamic_cast<RooRealVar*>(arg);
    if (var != NULL) {
      ValueWithError<double> val(var->getVal(), var->getError());
      val.set_full_precision_printout(full_precision_printout_);
      RooRealVar* var_init = dynamic_cast<RooRealVar*>(par_list_float_init.find(var->GetName()));
      double val_init = 0.0, pull = 0.0;
      if (var_init != NULL) {
        val_init = var_init->getVal();
        pull = (val_init-val.value)/val.error;
      }

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

      std::cout << format("%-30s %15g %15s +/- %-15s ") % var->GetName() % val_init % val.str_value() % val.str_error();
      if (print_pulls_) {
        std::cout << format("%s%15g%s") % str_color % pull % str_term;
      }
      std::cout << std::endl;
    }
  }

  std::cout << std::endl;
}
