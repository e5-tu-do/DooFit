#include "EasyFitResult.h"

// from STL
#include <string>
#include <map>

// from ROOT
#include "TIterator.h"
#include "TTree.h"

// from RooFit
#include "RooRealVar.h"
#include "RooFitResult.h"
#include "RooArgList.h"

// from DooCore
#include "doocore/io/MsgStream.h"

doofit::fitter::easyfit::EasyFitResult::EasyFitResult(const RooFitResult& fit_result) :
 num_status_(0),
 quality_covariance_matrix_(fit_result.covQual()),
 fcn_(fit_result.minNll()),
 edm_(fit_result.edm()),
 initialized_(false)
{
  status_ptrs_.reserve(10);

  for (unsigned int i=0; i<10; ++i) {
    status_ptrs_.push_back(new std::string(""));
  }

  ConvertRooFitResult(fit_result);
}

void doofit::fitter::easyfit::EasyFitResult::ConvertRooFitResult(const RooFitResult& fit_result) {
  using namespace doocore::io;

  if (!initialized_) {
    quality_covariance_matrix_ = fit_result.covQual();
    fcn_ = fit_result.minNll();
    edm_ = fit_result.edm();

    // transfer fit status
    for (unsigned int i=0; i<fit_result.numStatusHistory(); ++i) {
      status_.push_back(std::make_pair(fit_result.statusLabelHistory(i),
                                       fit_result.statusCodeHistory(i)));
      
      num_status_ = fit_result.numStatusHistory();
      status_ptrs_.at(i) = new std::string(fit_result.statusLabelHistory(i));
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

    initialized_ = true;
  } else {
    // before the other fit result is transferred, do consistency checks
    bool fit_result_transferable = true;
    // TODO: Implement consistency checks.
    // if (status_.size() != fit_result.numStatusHistory()) {
    //   serr << "Stored fit result and new fit result have different number of fit status. Cannot continue." << endmsg;
    //   fit_result_transferable = false;
    // }

    quality_covariance_matrix_ = fit_result.covQual();
    fcn_ = fit_result.minNll();
    edm_ = fit_result.edm();

    // for (int i=0; i<status_.size(); ++i) {
    //   sdebug << &status_.at(i).first << " - " << &status_.at(i).second << endmsg;
    // }
    // for (auto var : parameters_const_) {
    //   sdebug << var.first << " - " << &parameters_const_.at(var.first) << " -- " << &(parameters_const_.at(var.first).value_) << endmsg;
    // }

    // transfer fit status
    for (unsigned int i=0; i<fit_result.numStatusHistory(); ++i) {
      status_.at(i).first  = fit_result.statusLabelHistory(i);
      status_.at(i).second = fit_result.statusCodeHistory(i);

      num_status_ = fit_result.numStatusHistory();
      delete status_ptrs_.at(i);
      status_ptrs_.at(i) = new std::string(fit_result.statusLabelHistory(i));
    }
    for (unsigned int i=fit_result.numStatusHistory(); i<10; ++i) {
      delete status_ptrs_.at(i);
      status_ptrs_.at(i) = new std::string("");
    }

    // transfer constant parameters
    const RooArgList& pars_const(fit_result.constPars());
    TIterator* iter = pars_const.createIterator();
    RooRealVar* par_const = nullptr;
    while ((par_const = dynamic_cast<RooRealVar*>(iter->Next()))) {
      EasyFitVariable evar(*par_const);
      parameters_const_.at(evar.name()) = evar;
    }
    delete iter;

    // transfer floating final parameters
    const RooArgList& pars_final(fit_result.floatParsFinal());
    iter = pars_final.createIterator();
    RooRealVar* par_final = nullptr;
    while ((par_final = dynamic_cast<RooRealVar*>(iter->Next()))) {
      EasyFitVariable evar(*par_final);
      parameters_float_final_.at(evar.name()) = evar;
    }
    delete iter;

    // transfer floating init parameters
    const RooArgList& pars_init(fit_result.floatParsInit());
    iter = pars_init.createIterator();
    RooRealVar* par_init = nullptr;
    while ((par_init = dynamic_cast<RooRealVar*>(iter->Next()))) {
      EasyFitVariable evar(*par_init);
      parameters_float_init_.at(evar.name()) = evar;
    }
    delete iter;

    // for (int i=0; i<status_.size(); ++i) {
    //   sdebug << &status_.at(i).first << " - " << &status_.at(i).second << endmsg;
    // }
    // for (auto var : parameters_const_) {
    //   sdebug << var.first << " - " << &parameters_const_.at(var.first) << " -- " << &(parameters_const_.at(var.first).value_) << endmsg;
    // }

  }
}

void doofit::fitter::easyfit::EasyFitResult::CreateBranchesInTree(TTree& tree, std::string prefix) {
  std::string str_name;
  std::string str_leaf;

  RegisterBranch(tree, &num_status_, prefix+"status_num", prefix+"status_num/b");
  RegisterBranch(tree, &quality_covariance_matrix_, prefix+"quality_covariance_matrix", prefix+"quality_covariance_matrix/I");
  RegisterBranch(tree, &fcn_, prefix+"fcn", prefix+"fcn/D");
  RegisterBranch(tree, &edm_, prefix+"edm", prefix+"edm/D");

  for (auto it=parameters_const_.begin(), end=parameters_const_.end(); it!=end; ++it) {
    CreateBranchesForVariable(tree, it->second, prefix+"const_"+it->first);
  }
  for (auto it=parameters_float_init_.begin(), end=parameters_float_init_.end(); it!=end; ++it) {
    CreateBranchesForVariable(tree, it->second, prefix+"init_"+it->first);
  }
  for (auto it=parameters_float_final_.begin(), end=parameters_float_final_.end(); it!=end; ++it) {
    CreateBranchesForVariable(tree, it->second, prefix+"final_"+it->first);
  }

  for (unsigned int i=0; i<status_.size(); ++i) {
    RegisterBranch(tree, &status_.at(i).second, prefix+"status_code"+std::to_string(i), prefix+"status_code"+std::to_string(i)+"/I");
  }

  if (tree.GetEntries() == 0) {
    for (unsigned int i=0; i<10; ++i) {
      str_name = prefix+"status_label"+std::to_string(i);
      tree.Branch(str_name.c_str(), &(status_ptrs_.at(i)));
    }

  } else { // if (tree.GetEntries() == 0) {
    // TODO: Check if branches actually exist

    for (unsigned int i=0; i<10; ++i) {
      str_name = prefix+"status_label"+std::to_string(i);
      tree.SetBranchAddress(str_name.c_str(), &(status_ptrs_.at(i)));
    }
  }
}

void doofit::fitter::easyfit::EasyFitResult::RegisterBranch(TTree& tree, void* ptr, std::string name, std::string leaflist) {
  if (tree.GetEntries() == 0) {
    tree.Branch(name.c_str(), ptr, leaflist.c_str());
  } else {
    tree.SetBranchAddress(name.c_str(), ptr);
  }
}

void doofit::fitter::easyfit::EasyFitResult::CreateBranchesForVariable(TTree& tree, EasyFitVariable& var, std::string name) {
  RegisterBranch(tree, &var.value_, name+"_value", name+"_value/D");
}



