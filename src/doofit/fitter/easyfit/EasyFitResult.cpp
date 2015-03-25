#include "EasyFitResult.h"

// from STL
#include <string>
#include <map>

// from ROOT
#include "TIterator.h"
#include "TTree.h"
#include "TLeaf.h"
#include "TObjArray.h"

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
    status_ptrs_.push_back(std::make_pair(new std::string(""), 0));
  }

  ConvertRooFitResult(fit_result);
}

doofit::fitter::easyfit::EasyFitResult::EasyFitResult(TTree& tree, std::string prefix) :
 num_status_(0),
 initialized_(false)
{
  using namespace doocore::io;

  status_ptrs_.reserve(10);
  for (unsigned int i=0; i<10; ++i) {
    status_ptrs_.push_back(std::make_pair(new std::string(""), 0));
  }

  sdebug << "Initializing tree: " << endmsg;

  TObjArray* list_leaves  = tree.GetListOfLeaves();
  unsigned int num_leaves = list_leaves->GetEntries();

  for (unsigned int i=0; i<num_leaves; ++i) {
    TLeaf& leaf(*dynamic_cast<TLeaf*>((*list_leaves)[i]));
    std::string name_leaf(leaf.GetName());

    if (name_leaf.substr(0,prefix.length()) == prefix) {
      if (name_leaf.substr(prefix.length(), prefix.length()+6) == "const_") {
        if (name_leaf.substr(name_leaf.length()-6, name_leaf.length()) == "_value") {
          std::string name_var = name_leaf.substr(prefix.length()+6, name_leaf.length()-prefix.length()-12);
          sdebug << name_var << " is const variable!" << endmsg;
          EasyFitVariable evar(name_var, "", 0.0);
          parameters_const_.insert(std::make_pair(evar.name(), evar));
        }
      }
      if (name_leaf.substr(prefix.length(), prefix.length()+5) == "init_") {
        if (name_leaf.substr(name_leaf.length()-6, name_leaf.length()) == "_value") {
          std::string name_var = name_leaf.substr(prefix.length()+5, name_leaf.length()-prefix.length()-11);
          sdebug << name_var << " is init variable!" << endmsg;
          EasyFitVariable evar(name_var, "", 0.0);
          parameters_float_init_.insert(std::make_pair(evar.name(), evar));
        }
      }
      if (name_leaf.substr(prefix.length(), prefix.length()+6) == "final_") {
        if (name_leaf.substr(name_leaf.length()-6, name_leaf.length()) == "_value") {
          std::string name_var = name_leaf.substr(prefix.length()+6, name_leaf.length()-prefix.length()-12);
          sdebug << name_var << " is final variable!" << endmsg;
          EasyFitVariable evar(name_var, "", 0.0);
          parameters_float_final_.insert(std::make_pair(evar.name(), evar));
        }
      }
    }
  }


  RegisterBranchesInTree(tree, prefix);

  initialized_ = true;
}

doofit::fitter::easyfit::EasyFitResult::EasyFitResult(const EasyFitResult& other) :
  parameters_float_init_(other.parameters_float_init_),
  parameters_float_final_(other.parameters_float_final_),
  parameters_const_(other.parameters_const_),
  num_status_(other.num_status_),
  quality_covariance_matrix_(other.quality_covariance_matrix_),
  fcn_(other.fcn_),
  edm_(other.edm_),
  initialized_(other.initialized_)
{
  status_ptrs_.reserve(10);
  for (unsigned int i=0; i<10; ++i) {
    status_ptrs_.push_back(std::make_pair(new std::string(*other.status_ptrs_.at(i).first), other.status_ptrs_.at(i).second));
  }
}


doofit::fitter::easyfit::EasyFitResult::~EasyFitResult() {
  for (unsigned int i=0; i<10; ++i) {
    delete status_ptrs_.at(i).first;
  }
}

void doofit::fitter::easyfit::EasyFitResult::ConvertRooFitResult(const RooFitResult& fit_result) {
  using namespace doocore::io;

  if (!initialized_) {
    quality_covariance_matrix_ = fit_result.covQual();
    fcn_ = fit_result.minNll();
    edm_ = fit_result.edm();

    // transfer fit status
    for (unsigned int i=0; i<fit_result.numStatusHistory(); ++i) {
      num_status_ = fit_result.numStatusHistory();
      status_ptrs_.at(i) = std::make_pair(new std::string(fit_result.statusLabelHistory(i)),
                                          fit_result.statusCodeHistory(i));
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
      num_status_ = fit_result.numStatusHistory();
      delete status_ptrs_.at(i).first;
      status_ptrs_.at(i).first = new std::string(fit_result.statusLabelHistory(i));
      status_ptrs_.at(i).second = fit_result.statusCodeHistory(i);
    }
    for (unsigned int i=fit_result.numStatusHistory(); i<10; ++i) {
      delete status_ptrs_.at(i).first;
      status_ptrs_.at(i).first = new std::string("");
      status_ptrs_.at(i).second = 0;
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

const std::vector<std::pair<std::string, int>> doofit::fitter::easyfit::EasyFitResult::status() const {
  std::vector<std::pair<std::string, int>> status;

  for (unsigned int i=0; i<num_status_; ++i) {
    status.push_back(std::make_pair(*status_ptrs_.at(i).first,
                                    status_ptrs_.at(i).second));
  }

  return status;
}

void doofit::fitter::easyfit::EasyFitResult::RegisterBranchesInTree(TTree& tree, std::string prefix) {
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

  // for (unsigned int i=0; i<status_.size(); ++i) {
  //   RegisterBranch(tree, &status_.at(i).second, prefix+"status_code"+std::to_string(i), prefix+"status_code"+std::to_string(i)+"/I");
  // }

  for (unsigned int i=0; i<10; ++i) {
    // str_name = prefix+"status_label"+std::to_string(i);
    // tree.Branch(str_name.c_str(), &(status_ptrs_.at(i)));
    RegisterStringBranch(tree, &(status_ptrs_.at(i).first), prefix+"status_label"+std::to_string(i));
    RegisterBranch(tree, &status_ptrs_.at(i).second, prefix+"status_code"+std::to_string(i), prefix+"status_code"+std::to_string(i)+"/I");
  }

  if (tree.GetEntries() == 0) {
  } else { // if (tree.GetEntries() == 0) {
    // TODO: Check if branches actually exist
  }
}

void doofit::fitter::easyfit::EasyFitResult::RegisterBranch(TTree& tree, void* ptr, std::string name, std::string leaflist) {
  if (tree.GetEntries() == 0) {
    tree.Branch(name.c_str(), ptr, leaflist.c_str());
  } else {
    tree.SetBranchAddress(name.c_str(), ptr);
  }
}

void doofit::fitter::easyfit::EasyFitResult::RegisterStringBranch(TTree& tree, std::string** ptr, std::string name) {
  if (tree.GetEntries() == 0) {
    tree.Branch(name.c_str(), ptr);
  } else {
    tree.SetBranchAddress(name.c_str(), ptr);
  }
}


void doofit::fitter::easyfit::EasyFitResult::CreateBranchesForVariable(TTree& tree, EasyFitVariable& var, std::string name) {
  RegisterStringBranch(tree, &var.title_, name+"_title");
  RegisterStringBranch(tree, &var.unit_, name+"_unit");

  RegisterBranch(tree, &var.value_, name+"_value", name+"_value/D");
  RegisterBranch(tree, &var.min_, name+"_min", name+"_min/D");
  RegisterBranch(tree, &var.max_, name+"_max", name+"_max/D");

  RegisterBranch(tree, &var.has_error_, name+"_has_error", name+"_has_error/O");
  RegisterBranch(tree, &var.has_asym_error_, name+"_has_asym_error", name+"_has_asym_error/O");

  RegisterBranch(tree, &var.error_, name+"_error", name+"_error/D");
  RegisterBranch(tree, &var.error_low_, name+"_error_low", name+"_error_low/D");
  RegisterBranch(tree, &var.error_high_, name+"_error_high", name+"_error_high/D");

  RegisterBranch(tree, &var.constant_, name+"_constant", name+"_constant/O");
}



