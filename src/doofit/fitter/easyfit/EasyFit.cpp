#include "EasyFit.h"

// from STL

// from boost
#include <boost/foreach.hpp>

// from RooFit
#include "RooAbsData.h"
#include "RooAbsPdf.h"
#include "RooFit.h"
#include "RooFitResult.h"
#include "RooWorkspace.h"

// from project - Utils
#include <doocore/io/MsgStream.h>

using std::set;
using std::string;
using std::cout;
using std::endl;
using std::map;
using doocore::io::serr;
using doocore::io::endmsg;

namespace doofit {
namespace fitter {
namespace easyfit {

EasyFit::EasyFit(const string& fit_name) 
    : fit_name_(fit_name)
    , prepared_(false)
    , fitted_(false)
    , finalized_(false)
    , pdf_(NULL)
    , data_(NULL)
    , fit_result_(NULL)
    , minimizer_combs_()
    , fc_map_()
    , fc_linklist_()
    , fc_num_cpu_(1),
    , fc_num_cpu_strategy(0)
    , fc_extended_(false)
    , fc_constrained_(false)
    , fc_constrained_externally_(false)
    , fc_external_constraints_(NULL)
    , fc_conditional_observables_set_(false)
    , fc_conditional_observables_(NULL)
    , fc_strategy_(1)
    , fc_optimize_(true)
    , fc_sumw2err_(false)
    , fc_minimizer_type_("Minuit2")
    , fc_minimizer_algo_("minimize")
    , fc_hesse_init_(false)
    , fc_hesse_(true)
    , fc_minos_(true)
    , fc_minos_wpars_(false)
    , fc_minos_pars_(NULL)
    , fc_save_(true)
    , fc_verbose_(false)
    , fc_warnings_(true)
    , fc_printlevel_(1)
    , fc_numevalerr_(10)
    , fc_timer_(true)
{
  // define allowed combinations of minimizer type and algo
  string OldMinuit[]={"migrad","simplex","minimize","migradimproved"};
  minimizer_combs_["OldMinuit"] = set<string>(OldMinuit,OldMinuit+4);

  string Minuit[]={"migrad","simplex","minimize","migradimproved"};
  minimizer_combs_["Minuit"] = set<string>(Minuit,Minuit+4);

  string Minuit2[]={"migrad","simplex","minimize","scan"};
  minimizer_combs_["Minuit2"] = set<string>(Minuit2,Minuit2+4);

  string GSLMultiMin[] = {"conjugatefr","conjugatepr","bfgs","bfgs2","steepestdescent"};
  minimizer_combs_["GSLMultiMin"] = set<string>(GSLMultiMin,GSLMultiMin+5);

  string GSLSimAn[] = {""};
  minimizer_combs_["GSLSimAn"] = set<string>(GSLSimAn,GSLSimAn+1);
}

EasyFit::~EasyFit() {

}

void EasyFit::SetPdfAndDataSet(RooAbsPdf* pdf, RooAbsData* data) {
  if (!PdfAndDataReady()) {
    pdf_ = pdf; 
    data_ = data;
  }
}

void EasyFit::SetPdfAndDataSet(const RooWorkspace& ws, const std::string& pdf_name, RooAbsData* data) {
  RooAbsPdf* temp_pdf = ws.pdf(pdf_name.c_str());
  if (temp_pdf != NULL) {
    SetPdfAndDataSet(temp_pdf,data);
  }
}



void EasyFit::PrepareFit() {
  if (prepared_ || fitted_ || finalized_){
    // something went wrong
    serr << "Cannot prepare Fit " << fit_name_ << ". Fit is in advanced state: " << endmsg;
    serr << "  Prepared: " << prepared_ 
         << ", Fitted: " << fitted_ 
         << ", Finalized: " << finalized_ 
         << "." << endmsg;
  return;
  }
  fc_map_["NumCPU"]      = RooFit::NumCPU(fc_num_cpu_, fc_num_cpu_strategy_);
  fc_map_["Extended"]    = RooFit::Extended(fc_extended_);

  if (fc_constrained_) {
    fc_map_["Contrained"]  = RooFit::Constrained();
  }
  if (fc_constrained_externally_) {
    fc_map_["ExternalConstraints"] = RooFit::ExternalConstraints(*fc_external_constraints_);
  }
  
  if (fc_conditional_observables_set_) {
    fc_map_["ConditionalObservables"] = RooFit::ConditionalObservables(*fc_conditional_observables_);
  }
  
  fc_map_["Strategy"]    = RooFit::Strategy(fc_strategy_);
  fc_map_["Optimize"]    = RooFit::Optimize(fc_optimize_);

  fc_map_["SumW2Err"]    = RooFit::SumW2Error(fc_sumw2err_);

  // Check consistency
  fc_map_["Minimizer"]   = RooFit::Minimizer(fc_minimizer_type_.c_str(),fc_minimizer_algo_.c_str());
  

  fc_map_["HessInit"]    = RooFit::InitialHesse(fc_hesse_init_);
  fc_map_["Hesse"]       = RooFit::Hesse(fc_hesse_);
  
  // Check consistency
  if (fc_minos_ && !fc_minos_wpars_) {
    fc_map_["Minos"]       = RooFit::Minos(fc_minos_);
  }
  else if (fc_minos_wpars_) {
    fc_map_["MinosSet"]    = RooFit::Minos(*fc_minos_pars_);
  }
  
  fc_map_["Save"]          = RooFit::Save(fc_save_);
  fc_map_["Verbose"]       = RooFit::Verbose(fc_verbose_);
  fc_map_["Warnings"]      = RooFit::Warnings(fc_warnings_);
  fc_map_["PrintLevel"]    = RooFit::PrintLevel(fc_printlevel_);
  fc_map_["PrintEvalErrs"] = RooFit::PrintEvalErrors(fc_numevalerr_);
  fc_map_["Timer"]         = RooFit::Timer(fc_timer_);
  


  // fill linked list
  typedef std::map<std::string,RooCmdArg> CmdArgMap;
  BOOST_FOREACH(CmdArgMap::value_type &entry, fc_map_) {
    fc_linklist_.Add(dynamic_cast<TObject*>(&(entry.second)));
  }
  //fc_linklist_.Print("v");
  prepared_ = true;
}

void EasyFit::ExecuteFit() {
  if (!prepared_ || fitted_ || finalized_ || !PdfAndDataReady()){
    // something went wrong
    serr << "Cannot execute Fit " << fit_name_ << ". Fit is in inappropriate state: " << endmsg;
    serr << "  Prepared: "  << prepared_ 
         << ", Fitted: "    << fitted_ 
         << ", Finalized: " << finalized_
         << ", Pdf and Data ready: " << PdfAndDataReady()
         << "." << endmsg;
  } else {
    fit_result_ = pdf_->fitTo(*data_,fc_linklist_);
    fitted_ = true;
  }
}

void EasyFit::FinalizeFit() {
  if (!prepared_ || !fitted_ || finalized_){
    // something went wrong
    serr << "Cannot finalize Fit " << fit_name_ << ". Fit is in inappropriate state: " << endmsg;
    serr << "  Prepared: "  << prepared_ 
         << ", Fitted: "    << fitted_ 
         << ", Finalized: " << finalized_ 
         << "." << endmsg;
  } else {
    finalized_ = true;
  }
}

const RooFitResult* EasyFit::GetFitResult() {
  if (!prepared_ || !fitted_ || !finalized_ || fit_result_ == NULL){
    // something went wrong
    serr << "Cannot get the FitResult for Fit " << fit_name_ << ". Fit is in inappropriate state: " << endmsg;
    serr << "  Prepared: "  << prepared_ 
         << ", Fitted: "    << fitted_ 
         << ", Finalized: " << finalized_ 
         << "." << endmsg;

    return NULL;
  } else {
    return fit_result_;
  }
}

bool EasyFit::PdfAndDataReady() {
  return (pdf_ != NULL && data_ != NULL);
}

bool EasyFit::CheckSettingOptionsOk() {
  if (prepared_) {
    serr << "Cannot change fit settings after Fit " << fit_name_ << "has reached the state 'prepared'." << endmsg;
  }
  return !prepared_;
}

bool EasyFit::CheckMinimizerCombiOk(const std::string& type, const std::string& algo) {
  map<string,set<string> >::const_iterator comb_iter = minimizer_combs_.find(type);
  if ( comb_iter != minimizer_combs_.end()) {
    set<string>::const_iterator algo_iter = (*comb_iter).second.find(algo);
    if (algo_iter != (*comb_iter).second.end()) {
      return true;
    } else {
      serr << "Fit " << fit_name_ << ": Could not set Minimizer. Specified minimizer " 
           << type << " has no algorithm called " << algo << "." << endmsg;
      return false;
    }
  } else {
    serr << "Fit " << fit_name_ << ": Could not set Minimizer. Specified minimizer " 
           << type << " does not exist." << endmsg;
    return false;
  }

}

//------------------------------------------------------------------------------
// Setters for fit options
EasyFit& EasyFit::SetNumCPU(int fc_num_cpu, int fc_num_cpu_strategy) {
  if (CheckSettingOptionsOk()) {
    if (fc_num_cpu > 0) {
      fc_num_cpu_ = fc_num_cpu;
      fc_num_cpu_strategy_ = fc_num_cpu_strategy;
    } else {
      serr << "Fit " << fit_name_ << ": Cannot set number of CPU < 1." << endmsg;
    }
  }
  return *this;
}

EasyFit& EasyFit::SetExtended(bool fc_extended) {
  if (CheckSettingOptionsOk()) {
    fc_extended_ = fc_extended;
  }
  return *this;
}

EasyFit& EasyFit::SetConstrained(bool fc_constrained) {
  if (CheckSettingOptionsOk()) {
    fc_constrained_ = fc_constrained;
  }
  return *this;
}

EasyFit& EasyFit::SetExternalConstraints(const RooArgSet* fc_external_constraints) {
  if (CheckSettingOptionsOk()) {
    fc_constrained_externally_ = true;
    fc_external_constraints_ = fc_external_constraints;
  }
  return *this;
}
  
EasyFit& EasyFit::SetConditionalObservables(const RooArgSet* fc_conditional_observables) {
  if (CheckSettingOptionsOk()) {
    fc_conditional_observables_set_ = true;
    fc_conditional_observables_ = fc_conditional_observables;
  }
  return *this;
}

EasyFit& EasyFit::SetStrategy(int fc_strategy) {
  if (CheckSettingOptionsOk()) {
    if (fc_strategy >= 0 && fc_strategy < 3) {
      fc_strategy_ = fc_strategy;
    } else {
      serr << "Fit " << fit_name_ << ": Cannot set Strategy to " << fc_strategy 
           << ". Allowed valued are 0, 1, 2." << endmsg;
    }
  }
  return *this;
}

EasyFit& EasyFit::SetOptimize(int fc_optimize) {
  if (CheckSettingOptionsOk()) {
    fc_optimize_ = fc_optimize;
  }
  return *this;
}

EasyFit& EasyFit::SetSumW2Error(bool fc_sumw2err) {
  if (CheckSettingOptionsOk()) {
    fc_sumw2err_ = fc_sumw2err;
  }
  return *this;
}

EasyFit& EasyFit::SetMinimizer(const std::string& fc_minimizer_type, const std::string& fc_minimizer_algo) {
  if (CheckSettingOptionsOk()) {
    fc_minimizer_type_ = fc_minimizer_type;
    fc_minimizer_algo_ = fc_minimizer_algo;
  }
  return *this;
}

EasyFit& EasyFit::SetHesseInit(bool fc_hesse_init) {
  if (CheckSettingOptionsOk()) {
    fc_hesse_init_ = fc_hesse_init;
  }
  return *this;
}

EasyFit& EasyFit::SetHesse(bool fc_hesse) {
  if (CheckSettingOptionsOk()) {
    fc_hesse_ = fc_hesse;
  }
  return *this;
}

EasyFit& EasyFit::SetMinos(bool fc_minos) {
  if (CheckSettingOptionsOk()) {
    fc_minos_ = fc_minos;
  }
  return *this;
}

EasyFit& EasyFit::SetMinosPars(const RooArgSet* fc_minos_pars) {
  if (CheckSettingOptionsOk()) {
    fc_minos_ = true;
    fc_minos_wpars_ = true;
    fc_minos_pars_ = fc_minos_pars;
  }
  return *this;
}

EasyFit& EasyFit::SetSave(bool fc_save) {
  if (CheckSettingOptionsOk()) {
    fc_save_ = fc_save;
  }
  return *this;
}

EasyFit& EasyFit::SetVerbose(bool fc_verbose) {
  if (CheckSettingOptionsOk()) {
    fc_verbose_ = fc_verbose;
  }
  return *this;
}

EasyFit& EasyFit::SetWarnings(bool fc_warnings) {
  if (CheckSettingOptionsOk()) {
    fc_warnings_ = fc_warnings;
  }
  return *this;
}

EasyFit& EasyFit::SetPrintLevel(int fc_printlevel) {
  if (CheckSettingOptionsOk()) {
    fc_printlevel_ = fc_printlevel;
  }
  return *this;
}

EasyFit& EasyFit::SetNumEvalErrors(int fc_numevalerr) {
  if (CheckSettingOptionsOk()) {
    fc_numevalerr_ = fc_numevalerr;
  }
  return *this;
}

EasyFit& EasyFit::SetTimer(bool fc_timer) {
  if (CheckSettingOptionsOk()) {
    fc_timer_ = fc_timer;
  }
  return *this;
}


} // namespace easyfit
} // namespace fitter
} // namespace doofit

