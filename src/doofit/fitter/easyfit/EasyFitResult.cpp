#include "EasyFitResult.h"

// from STL
#include <string>
#include <map>

// from ROOT
#include "TIterator.h"

// from RooFit
#include "RooRealVar.h"
#include "RooFitResult.h"
#include "RooArgList.h"

// from DooCore
#include "doocore/io/MsgStream.h"

doofit::fitter::easyfit::EasyFitResult::EasyFitResult(const RooFitResult& fit_result) :
 quality_covariance_matrix_(fit_result.covQual()),
 fcn_(fit_result.minNll()),
 edm_(fit_result.edm()),
 initialized_(false)
{
  // transfer fit status
  for (unsigned int i=0; i<fit_result.numStatusHistory(); ++i) {
    status_.push_back(std::make_pair(fit_result.statusLabelHistory(i),
                                     fit_result.statusCodeHistory(i)));
  }

  // transfer constant parameters
  const RooArgList& pars_const(fit_result.constPars());
  TIterator* iter = pars_const.createIterator();
  RooRealVar* par_const = nullptr;
  while ((par_const = dynamic_cast<RooRealVar*>(iter->Next()))) {
    EasyFitVariable evar(*par_const);
    parameters_const_.insert(std::make_pair(evar.name(), evar));
  }
  delete iter;

  // transfer floating final parameters
  const RooArgList& pars_final(fit_result.floatParsFinal());
  iter = pars_final.createIterator();
  RooRealVar* par_final = nullptr;
  while ((par_final = dynamic_cast<RooRealVar*>(iter->Next()))) {
    EasyFitVariable evar(*par_final);
    parameters_float_final_.insert(std::make_pair(evar.name(), evar));
  }
  delete iter;

  // transfer floating init parameters
  const RooArgList& pars_init(fit_result.floatParsInit());
  iter = pars_init.createIterator();
  RooRealVar* par_init = nullptr;
  while ((par_init = dynamic_cast<RooRealVar*>(iter->Next()))) {
    EasyFitVariable evar(*par_init);
    parameters_float_init_.insert(std::make_pair(evar.name(), evar));
  }
  delete iter;
}

doofit::fitter::easyfit::EasyFitResult::ConvertRooFitResult(const RooFitResult& fit_result) {
  using namespace doocore::io;

  if (!initialized_) {

    initialized_ = true;
  }

  quality_covariance_matrix_ = fit_result.covQual();
  fcn_ = fit_result.minNll();
  edm_ = fit_result.edm();

  // transfer fit status
  if (status_.size() == 0) {
  } else {
    if (status_.size() != fit_result.numStatusHistory()) {
      serr << "Stored fit result and new fit result have different number of fit status."
    } else {

    }
  }
}

