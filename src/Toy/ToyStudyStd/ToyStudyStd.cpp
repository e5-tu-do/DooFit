#include "Toy/ToyStudyStd/ToyStudyStd.h"

// STL
#include <string>

// boost
#include "boost/interprocess/sync/file_lock.hpp"
#include "boost/filesystem.hpp"

// ROOT
#include "TTree.h"
#include "TFile.h"
#include "TBranch.h"
#include "TMath.h"
#include "TStopwatch.h"

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
    for (std::vector<RooFitResult*>::const_iterator it_results = fit_results_.begin(); it_results != fit_results_.end(); ++it_results) {
      delete *it_results;
    }
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
    
    sinfo.Ruler();
    
    if (fit_results_.size() > 0) {
      swarn << "Reading in fit results while results are already stored." << endmsg;
    }
    
    int results_stored = 0;
    int results_neglected = 0;
    
    for (std::vector<Config::CommaSeparatedPair>::const_iterator it_files = results_files.begin(); it_files != results_files.end(); ++it_files) {
      sinfo << "Loading fit results from " << (*it_files).first() << endmsg;
      TFile file((*it_files).first().c_str(), "read");
      TTree* tree = (TTree*)file.Get((*it_files).second().c_str());
            
      TBranch* result_branch = tree->GetBranch("fit_results");
      RooFitResult* fit_result = NULL;
      result_branch->SetAddress(&fit_result);
      
      for (int i=0; i<tree->GetEntries(); ++i) {
        result_branch->GetEntry(i);
        
        // save a copy
        if (FitResultOkay(*fit_result)) {
          //fit_results_.push_back(new RooFitResult(*fit_result));
          fit_results_.push_back(fit_result);
          results_stored++;
        } else {
          results_neglected++;
          swarn << "Fit result number " << i << " in file " << *it_files << " negelected." << endmsg;
        }
        //delete fit_result;
        fit_result = NULL;
      }
      
      delete result_branch;
      delete tree;
      file.Close();
    }
    sinfo << "Read in " << results_stored << " fit results. (" << results_neglected << " results negelected, that is " << static_cast<double>(results_neglected)/static_cast<double>(results_stored+results_neglected)*100.0 << "%)" << endmsg;
    sinfo.Ruler();
  }
  
  void ToyStudyStd::EvaluateFitResults() {
    sinfo.Ruler();
    sinfo << "Evaluating fit results" << endmsg;
    if (fit_results_.size() <= 0) {
      serr << "Cannot evaluate as no fit results are loaded." << endmsg;
      throw ExceptionCannotEvaluateFitResults();
    }
        
    // build list of all parameters, pulls, etc.
    RooArgSet parameter_set = BuildEvaluationArgSet(*fit_results_.front());
    
    // loop over fit results and fill all values into RooDataSet
    if (evaluated_values_ != NULL) delete evaluated_values_;
    evaluated_values_ = new RooDataSet("evaluated_values", "evaluated_values", parameter_set);
    int i = 0;
    for (std::vector<RooFitResult*>::const_iterator it_results = fit_results_.begin(); it_results != fit_results_.end(); ++it_results) {
      RooArgSet params = BuildEvaluationArgSet(**it_results);
      
      evaluated_values_->add(params);
      ++i;
    }
    sinfo << "Evaluated " << i << " fit results." << endmsg;
    sinfo.Ruler();
  }
  
  void ToyStudyStd::PlotEvaluatedParameters() {
    if (evaluated_values_->numEntries() <= 0) {
      serr << "Cannot plot as no fit results are evaluated." << endmsg;
      throw ExceptionCannotEvaluateFitResults();
    }
    
    TCanvas canvas("canvas_toystudy", "canvas", 800, 600);
    const RooArgSet* parameters = evaluated_values_->get();
    TIterator* parameter_iter   = parameters->createIterator();
    RooRealVar* parameter       = NULL;
    while ((parameter = (RooRealVar*)parameter_iter->Next())) {
      std::pair<double,double> minmax = Utils::MedianLimitsForTuple(*evaluated_values_, parameter->GetName());
      sdebug << parameter->GetName() <<  " -> min,max: " << minmax.first << "," << minmax.second << endmsg;
      RooPlot* frame = parameter->frame(Range(minmax.first,minmax.second));
      evaluated_values_->plotOn(frame);
      frame->Draw();
      TString plot_name = parameter->GetName();
      Utils::printPlot(&canvas, plot_name, config_toystudy_.plot_directory());
      
      delete frame;
    }
  }
  
  RooArgSet ToyStudyStd::BuildEvaluationArgSet(const RooFitResult& fit_result) const {
    TStopwatch sw;
    RooArgSet parameters;

    const RooArgList& parameter_list = fit_result.floatParsFinal();
    TIterator* parameter_iter        = parameter_list.createIterator();
    RooRealVar* parameter            = NULL;
    while ((parameter = (RooRealVar*)parameter_iter->Next())) {
      TString pull_name = parameter->GetName() + TString("_pull");
      TString pull_desc = TString("Pull of ") + parameter->GetTitle();
      TString init_name = parameter->GetName() + TString("_init");
      TString init_desc = TString("Init value of ") + parameter->GetTitle();
      TString res_name  = parameter->GetName() + TString("_res");
      TString res_desc  = TString("Residual of ") + parameter->GetTitle();
      
      RooRealVar& par  = CopyRooRealVar(*parameter);
      RooRealVar* pull = new RooRealVar(pull_name, pull_desc, -100, 100);
      RooRealVar& init = CopyRooRealVar(*(RooRealVar*)fit_result.floatParsInit().find(par.GetName()), std::string(init_name), std::string(init_desc));
    
      double pull_value = (par.getVal() - init.getVal())/par.getError();
      pull->setVal(pull_value);
            
      if (TMath::Abs(pull_value) > 5.0) {
        swarn << "Pull for " << parameter->GetName() << " is " << pull_value
              << " (too large deviation from expectation)" << endmsg;
        fit_result.Print();
      }
        
      parameters.addOwned(par);
      parameters.addOwned(*pull);
      parameters.addOwned(init);
    }

    return parameters;
  }
  
  bool ToyStudyStd::FitResultOkay(const RooFitResult& fit_result) const {
    if (fit_result.covQual() != 3) {
      return false;
    } else {
      return true;
    }
  }
  
  RooRealVar& ToyStudyStd::CopyRooRealVar(const RooRealVar& other, const std::string& new_name, const std::string& new_title) const {
    RooRealVar* new_var = new RooRealVar(new_name.length()>0 ? new_name.c_str() : other.GetName(),
                                         new_title.length()>0 ? new_title.c_str() : other.GetTitle(),
                                         other.getVal());
    if (other.hasMin())       new_var->setMin(other.getMin());
    if (other.hasMax())       new_var->setMax(other.getMax());
    if (other.hasError())     new_var->setError(other.getError());
    if (other.hasAsymError()) new_var->setAsymError(other.getErrorLo(), other.getErrorHi());
    new_var->setUnit(other.getUnit());
    
    return *new_var;
  }
}
