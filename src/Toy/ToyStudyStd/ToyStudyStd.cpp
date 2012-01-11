#include "Toy/ToyStudyStd/ToyStudyStd.h"

// STL

// boost
#include "boost/interprocess/sync/file_lock.hpp"
#include "boost/filesystem.hpp"

// ROOT
#include "TTree.h"
#include "TFile.h"
#include "TBranch.h"

// from RooFit
#include "RooFitResult.h"
#include "RooArgList.h"
#include "RooDataSet.h"

// from Project
#include "Config/CommonConfig.h"
#include "Toy/ToyStudyStd/ToyStudyStdConfig.h"
#include "Utils/MsgStream.h"
#include "Utils/Utils.h"
#include "Utils/FileLock.h"

using namespace ROOT;
using namespace RooFit;

namespace Toy {
  namespace fs = boost::filesystem;
  
  ToyStudyStd::ToyStudyStd(const CommonConfig& cfg_com, const ToyStudyStdConfig& cfg_tstudy) :
  config_common_(cfg_com),
  config_toystudy_(cfg_tstudy),
  fit_results_(),
  evaluated_values_(NULL)
  {
    
  }
  
  ToyStudyStd::~ToyStudyStd() {
    if (evaluated_values_ != NULL) delete evaluated_values_;
  }
  
  void ToyStudyStd::StoreFitResult(const RooFitResult* fit_result) const {
    const string& filename = config_toystudy_.store_result_filename_treename().first();
    const string& treename = config_toystudy_.store_result_filename_treename().second();
    Utils::FileLock flock(filename);
    
    if (!flock.Lock()) {
      swarn << "File to save fit result to " << filename << " is locked. Waiting for unlock." << endmsg;
    }
    while (!flock.Lock()) {
      Utils::Sleep(2);
    }

    sinfo << "Saving fit result to file " << filename << endmsg;
    bool file_existing = fs::exists(filename);
    TFile f(fs::absolute(filename).string().c_str(),"update");
    if (f.IsZombie() || !f.IsOpen()) {
      serr << "Cannot open file which may be corrupted." << endmsg;
      flock.Unlock();
      throw ExceptionCannotStoreFitResult();
    } else {
      TTree* tree_results = NULL;
      if (file_existing) {      
        tree_results = (TTree*)f.Get(treename.c_str());
      } 
      
      if (tree_results == NULL) {
        tree_results = new TTree(treename.c_str(), "Tree for toy study fit results");
        tree_results->Branch("fit_results", "RooFitResult", &fit_result, 64000, 0);
      } else {      
        tree_results->SetBranchAddress("fit_results", &fit_result);
      }
      
      tree_results->Fill();
      tree_results->Write("",TObject::kOverwrite);
      f.Close();
      
      flock.Unlock();
    }
  }
  
  void ToyStudyStd::ReadFitResults() {
    const std::vector<Config::CommaSeparatedPair>& results_files = config_toystudy_.read_results_filename_treename();
    
    if (fit_results_.size() > 0) {
      swarn << "Reading in fit results while results are already stored." << endmsg;
    }
    
    int results_stored = 0;
    
    for (std::vector<Config::CommaSeparatedPair>::const_iterator it_files = results_files.begin(); it_files != results_files.end(); ++it_files) {
      TFile file((*it_files).first().c_str(), "read");
      TTree* tree = (TTree*)file.Get((*it_files).second().c_str());
            
      TBranch* result_branch = tree->GetBranch("fit_results");
      RooFitResult* fit_result = NULL;
      result_branch->SetAddress(&fit_result);
      
      for (int i=0; i<tree->GetEntries(); ++i) {
        result_branch->GetEntry(i);
        
        // save a copy
        fit_results_.push_back(new RooFitResult(*fit_result));
        results_stored++;
      }
    }
    sinfo << "Read in " << results_stored << " fit results." << endmsg;
  }
  
  void ToyStudyStd::EvaluateFitResults() {
    if (fit_results_.size() <= 0) {
      serr << "Cannot evaluate as no fit results are loaded." << endmsg;
      throw ExceptionCannotEvaluateFitResults();
    }
    
    // build list of all parameters, pulls, etc.
    RooArgSet parameter_set = BuildEvaluationArgSet(*fit_results_.front());
    
    // loop over fit results and fill all values into RooDataSet
    if (evaluated_values_ != NULL) delete evaluated_values_;
    evaluated_values_ = new RooDataSet("evaluated_values", "evaluated_values", parameter_set);
    for (std::vector<RooFitResult*>::const_iterator it_results = fit_results_.begin(); it_results != fit_results_.end(); ++it_results) {
      RooArgSet params = BuildEvaluationArgSet(**it_results);
      evaluated_values_->add(params);
    }
    
    evaluated_values_->Print();
  }
  
  RooArgSet ToyStudyStd::BuildEvaluationArgSet(const RooFitResult& fit_result) {
    RooArgSet parameters;
    
    const RooArgList& parameter_list = fit_result.floatParsFinal();
    TIterator* parameter_iter        = parameter_list.createIterator();
    RooRealVar* parameter            = NULL;
    while ((parameter = (RooRealVar*)parameter_iter->Next())) {
      TString pull_name = parameter->GetName() + TString("_pull");
      TString pull_desc = TString("Pull of ") + parameter->GetTitle();
      
      RooRealVar* par  = new RooRealVar(*parameter);
      //RooRealVar* pull = new RooRealVar(pull_name, pull_desc, -10, 10);
      
      parameters.addOwned(*par);
      //parameters.addOwned(*pull);
    }
    return parameters;
  }
}
