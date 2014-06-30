#include "FitResultPrinter.h"

// STL
#include <iomanip>

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
#include <doocore/statistics/general.h>

doofit::plotting::fitresult::FitResultPrinter::FitResultPrinter(const RooFitResult& fit_result)
: fit_result_(fit_result),
  full_precision_printout_(false),
  print_pulls_(true)
{
}

void doofit::plotting::fitresult::FitResultPrinter::PlotHandler() const {
  using namespace doocore::statistics::general;
  using namespace doocore::io;
  using boost::format;
  using boost::io::group;

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

      std::string str_color;
      if (std::abs(pull) < 2.0) {
        str_color = "\033[1;32m";
      } else if (std::abs(pull) > 2.0 && std::abs(pull) <= 3.0) {
        str_color = "\033[1;33m";
      } else if (std::abs(pull) > 3.0) {
        str_color = "\033[1;31m";
      }

      val.FormatString();

      std::cout << format("%-30s %15g %15s +/- %-15s ") % var->GetName() % val_init % val.str_value() % val.str_error();
      if (print_pulls_) {
        std::cout << format("%s%15g%s") % str_color % pull % "\033[0m";
      }
      std::cout << std::endl;
    }
  }
}
