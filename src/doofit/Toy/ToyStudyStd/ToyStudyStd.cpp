#include "doofit/Toy/ToyStudyStd/ToyStudyStd.h"

// STL
#include <string>
#include <utility>
#include <queue>
#include <list>

// boost
#include "boost/interprocess/sync/file_lock.hpp"
#include "boost/filesystem.hpp"
#include "boost/random/random_device.hpp"
#include <boost/thread.hpp>

// ROOT
#include "TTree.h"
#include "TFile.h"
#include "TBranch.h"
#include "TMath.h"
#include "TStopwatch.h"
#include "TROOT.h"
#include "TString.h"
#include "TThread.h"

// from RooFit
#include "RooFitResult.h"
#include "RooArgList.h"
#include "RooDataSet.h"
#include "RooRealVar.h"
#include "RooGaussian.h"

// from Project
#include "doofit/Config/CommonConfig.h"
#include "doofit/Config/CommaSeparatedPair.h"
#include "doofit/Toy/ToyStudyStd/ToyStudyStdConfig.h"
#include "doofit/utils/MsgStream.h"
#include "doofit/utils/utils.h"
#include "doofit/utils/FileLock.h"

using namespace ROOT;
using namespace RooFit;
using namespace doofit::utils;

namespace doofit {
namespace Toy {
  namespace fs = boost::filesystem;
  bool ToyStudyStd::abort_save_ = false;
  
  ToyStudyStd::ToyStudyStd(const Config::CommonConfig& cfg_com, const ToyStudyStdConfig& cfg_tstudy) :
  config_common_(cfg_com),
  config_toystudy_(cfg_tstudy),
  fit_results_(),
  fit_results_bookkeep_(),
  evaluated_values_(NULL),
  accepting_fit_results_(true)
  {
    abort_save_ = false;
    fitresult_save_worker_ = boost::thread(&ToyStudyStd::SaveFitResultWorker, this);
  }
  
  ToyStudyStd::~ToyStudyStd() {
    accepting_fit_results_ = false;
    fit_results_save_queue_.disable_queue();
    fitresult_save_worker_.join();
    
    if (evaluated_values_ != NULL) delete evaluated_values_;
    for (std::vector<RooFitResult*>::const_iterator it_results = fit_results_bookkeep_.begin(); it_results != fit_results_bookkeep_.end(); ++it_results) {
      delete *it_results;
    }
  }
  
  void ToyStudyStd::StoreFitResult(RooFitResult* fit_result1, 
                                  RooFitResult* fit_result2) {
    if (!accepting_fit_results_) {
      serr << "No longer accepting fit results." << endmsg;
    } else {
      const string& filename = config_toystudy_.store_result_filename_treename().first();
      const string& treename = config_toystudy_.store_result_filename_treename().second();
      if (filename.length() == 0 || treename.length() == 0) {
        serr << "File name and or tree name to save fit result into not set! Cannot store fit result." << endmsg;
        throw ExceptionCannotStoreFitResult();
      }
      
      RooFitResult* fit_result1_copy = new RooFitResult(*fit_result1);
      RooFitResult* fit_result2_copy = NULL;
      
      if (fit_result2 != NULL) {
        fit_result2_copy = new RooFitResult(*fit_result2);
      }
      
      fit_results_save_queue_.push(make_pair(fit_result1_copy,fit_result2_copy));
      sinfo << "Accepting fit result 1 for deferred saving" << endmsg;
      if (fit_result2 != NULL) {
        sinfo << "Accepting fit result 2 for deferred saving" << endmsg;
      }
    }
    
    /*
     signal(SIGINT, ToyStudyStd::HandleSignal);
    signal(SIGTERM, ToyStudyStd::HandleSignal);
    
    

    TStopwatch sw_lock;
    sw_lock.Start();
    utils::FileLock flock(filename);
    boost::random::random_device rnd;
    int wait_time, wait_i;
    if (!flock.Lock()) {
      swarn << "File to save fit result to " << filename << " is locked. Waiting for unlock." << endmsg;
    }
    while (!flock.Lock() && !abort_save_) {
      wait_time = (static_cast<double>(rnd())/(rnd.max()-rnd.min())-rnd.min()*10.0)+10;
      wait_i    = 0;
      while (wait_i<wait_time && !abort_save_) { 
        utils::Sleep(1.0);
        wait_i++;
      }
    }
    sinfo << "File locking deadtime: " << sw_lock << endmsg;

    if (!abort_save_) {
      sinfo << "Saving fit result to file " << filename << endmsg;
      bool file_existing = fs::exists(filename);
      TFile f(fs::absolute(filename).string().c_str(),"update");
      if (f.IsZombie() || !f.IsOpen()) {
        serr << "Cannot open file which may be corrupted." << endmsg;
        flock.Unlock();
        throw ExceptionCannotStoreFitResult();
      } else {
        if (!abort_save_) {
          TTree* tree_results = NULL;
          if (file_existing) {      
            tree_results = (TTree*)f.Get(treename.c_str());
          } 
          
          if (tree_results == NULL) {
            tree_results = new TTree(treename.c_str(), "Tree for toy study fit results");
            tree_results->Branch(config_toystudy_.fit_result1_branch_name().c_str(), "RooFitResult", &fit_result1, 64000, 0);
            if (fit_result2 != NULL) {
              tree_results->Branch(config_toystudy_.fit_result2_branch_name().c_str(), "RooFitResult", &fit_result2, 64000, 0);
            }
          } else {  
            if (tree_results->GetBranch(config_toystudy_.fit_result1_branch_name().c_str()) == NULL) {
              serr << "Cannot store fit result! Tree exists but branch " << config_toystudy_.fit_result1_branch_name() << " is unknown." << endmsg;
              f.Close();
              flock.Unlock();
              throw ExceptionCannotStoreFitResult();
            }
            if (fit_result2 != NULL && tree_results->GetBranch(config_toystudy_.fit_result2_branch_name().c_str()) == NULL) {
              serr << "Cannot store fit result! Tree exists but branch " << config_toystudy_.fit_result2_branch_name() << " is unknown." << endmsg;
              f.Close();
              flock.Unlock();
              throw ExceptionCannotStoreFitResult();
            }
            
            tree_results->SetBranchAddress(config_toystudy_.fit_result1_branch_name().c_str(), &fit_result1);
            if (fit_result2 != NULL) {
              tree_results->SetBranchAddress(config_toystudy_.fit_result2_branch_name().c_str(), &fit_result2);
            }
          }
          
          tree_results->Fill();
          tree_results->Write("",TObject::kOverwrite);
        } else {
          swarn << "Caught signal. Did not save fit result although file was opened. Trying to close file gracefully." << endmsg;
        }
          
        f.Close();
        
        flock.Unlock();
      }
    } else {
      flock.Unlock();
      swarn << "Aborting save as signal to end execution was caught before." << endmsg;
    }
    
    if (abort_save_) {
      swarn << "Caught signal. If we reached this, the file should have been closed gracefully without damage." << endmsg;
      exit(1);
    }
    
    signal(SIGINT, SIG_DFL);
    signal(SIGTERM, SIG_DFL);
     */
  }
  
  void ToyStudyStd::ReadFitResults(const std::string& branch_name) {
    // stop saving if there are still deferred fit results to be saved.
    accepting_fit_results_ = false;
    fit_results_save_queue_.disable_queue();
    fitresult_save_worker_.join();
    
    const std::vector<doofit::Config::CommaSeparatedPair>& results_files = config_toystudy_.read_results_filename_treename();
    
    sinfo.Ruler();
    
    if (results_files.size() == 0) {
      serr << "No files to read fit results from are specified. Cannot read in." << endmsg;
      throw ExceptionCannotReadFitResult();
    }
    
    if (fit_results_.size() > 0) {
      serr << "Reading in fit results while results are already stored. Empty stored fit results first." << endmsg;
      throw ExceptionCannotReadFitResult();
    } 
    
    int results_stored = 0;
    int results_neglected = 0;
    
    for (std::vector<doofit::Config::CommaSeparatedPair>::const_iterator it_files = results_files.begin(); it_files != results_files.end(); ++it_files) {
      sinfo << "Loading fit results from " << (*it_files).first() 
            << " from branch " << branch_name << endmsg;
      TFile file((*it_files).first().c_str(), "read");
      if (file.IsZombie() || !file.IsOpen()) {
        serr << "Cannot open file " << (*it_files).first() << " which may be not existing or corrupted." << endmsg;
        throw ExceptionCannotReadFitResult();
      }

      TTree* tree = (TTree*)file.Get((*it_files).second().c_str());
      if (tree == NULL) {
        serr << "Cannot find tree " << (*it_files).second() << " in file. Cannot read in fit results." << endmsg;
        throw ExceptionCannotReadFitResult();
      }

      TBranch* result_branch = tree->GetBranch(branch_name.c_str());
      if (result_branch == NULL) {
        serr << "Cannot find branch " << branch_name << " in tree. Cannot read in fit results." << endmsg;
        throw ExceptionCannotReadFitResult();
      }

      RooFitResult* fit_result = NULL;
      result_branch->SetAddress(&fit_result);
      
      for (int i=0; i<tree->GetEntries(); ++i) {
        result_branch->GetEntry(i);
        
        // save a copy
        if (FitResultOkay(*fit_result)) {
          //fit_results_.push_back(new RooFitResult(*fit_result));
          fit_results_.push_back(fit_result);
          fit_results_bookkeep_.push_back(fit_result);
          results_stored++;
        } else {
          results_neglected++;
          swarn << "Fit result number " << i << " in file " << *it_files << " negelected." << endmsg;
        }
        //delete fit_result;
        fit_result = NULL;
      }
      
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
    evaluated_values_->add(parameter_set);
    int i = 1;
    if (fit_results_.size() > 1) {
      for (std::vector<RooFitResult*>::const_iterator it_results = fit_results_.begin()+1; it_results != fit_results_.end(); ++it_results) {
        RooArgSet params = BuildEvaluationArgSet(**it_results);
        
        evaluated_values_->add(params);
        ++i;
      }
    }
    
    PrintUnexpectedPullOverview();
    
    sinfo << "Evaluated " << i << " fit results." << endmsg;
    sinfo.Ruler();
  }
  
  void ToyStudyStd::PlotEvaluatedParameters() {
    if (evaluated_values_->numEntries() <= 0) {
      serr << "Cannot plot as no fit results are evaluated." << endmsg;
      throw ExceptionCannotEvaluateFitResults();
    }
 
    sinfo.Ruler();
    sinfo << "Plotting parameter distributions." << endmsg;
    
    gROOT->SetStyle("Plain");
    utils::setStyle();
    TCanvas canvas("canvas_toystudy", "canvas", 800, 600);
    utils::printPlotOpenStack(&canvas, "AllPlots", config_toystudy_.plot_directory());
    
    const RooArgSet* parameters = evaluated_values_->get();
    TIterator* parameter_iter   = parameters->createIterator();
    RooRealVar* parameter       = NULL;
    while ((parameter = (RooRealVar*)parameter_iter->Next())) {
      std::string param_name = parameter->GetName();
      std::pair<double,double> minmax = utils::MedianLimitsForTuple(*evaluated_values_, param_name);
      sinfo << "Plotting parameter " << param_name << " in range [" << minmax.first << "," << minmax.second << "]" << endmsg;
            
      RooRealVar* mean             = NULL;
      RooRealVar* sigma            = NULL;
      RooGaussian* gauss           = NULL;
      RooPlot* param_frame         = NULL;
      RooDataSet* fit_plot_dataset = NULL;
      
      TString cut = "";
      if (config_toystudy_.fit_plot_on_quantile_window()) {
        cut = param_name + ">" + boost::lexical_cast<std::string>(minmax.first) + "&&" + param_name + "<" + boost::lexical_cast<std::string>(minmax.second);
      }
      if (config_toystudy_.neglect_parameters_at_limit()) {
        if (cut.Length() > 0) cut = cut + "&&";
        cut = cut + "parameters_at_limit < 0.5";
      }
      if (config_toystudy_.neglect_minos_problems()) {
        if (cut.Length() > 0) cut = cut + "&&";
        cut = cut + "minos_problems < 0.5";
      }
      if (cut.Length() > 0) {
        fit_plot_dataset = new RooDataSet("fit_plot_dataset", "Plotting and fitting dataset for ToyStudyStd", evaluated_values_, RooArgSet(*parameters), cut);
      } else {
        fit_plot_dataset = evaluated_values_;
      }

      // for all pulls fit a gaussian
      if (param_name.substr(param_name.length()-5).compare("_pull") == 0 || 
          param_name.substr(param_name.length()-4).compare("_res") == 0) {
        mean  = new RooRealVar("m", "mean of pull", (minmax.first+minmax.second)/2.0,minmax.first,minmax.second);
        sigma = new RooRealVar("s", "sigma of pull", (minmax.second-minmax.first)/10.0,0,minmax.second-minmax.first);
        gauss = new RooGaussian("pdf_pull", "Gaussian PDF of pull", *parameter, *mean, *sigma);
        
        sinfo.increment_indent(2);
        sinfo << "Fitting Gaussian distribution for parameter " << param_name << endmsg;
        if (fit_plot_dataset != evaluated_values_) {
          sinfo << "Losing " << evaluated_values_->numEntries() - fit_plot_dataset->numEntries() << "(" << (evaluated_values_->numEntries() - fit_plot_dataset->numEntries())/static_cast<double>(evaluated_values_->numEntries())*100 << "%) toys for this fit due to cuts applied." << endmsg;
        }
        RooFitResult* fit_result = gauss->fitTo(*fit_plot_dataset, NumCPU(1), Verbose(false), PrintLevel(-1), PrintEvalErrors(-1), Warnings(false), Save(true));
        fit_result->Print("v");
        delete fit_result;
        sinfo.increment_indent(-2);
      }
      
      RooPlot* frame = parameter->frame(Range(minmax.first,minmax.second));
      fit_plot_dataset->plotOn(frame);
      if (fit_plot_dataset != evaluated_values_) delete fit_plot_dataset;
      
      if (gauss != NULL) {
        gauss->plotOn(frame);
        param_frame = gauss->paramOn(frame, Layout(0.6, 0.9, 0.9));
      }
      frame->Draw();
      TString plot_name = parameter->GetName();
      utils::printPlot(&canvas, plot_name, config_toystudy_.plot_directory());
      utils::printPlot(&canvas, "AllPlots", config_toystudy_.plot_directory());
      
      delete frame;
      
      if (gauss != NULL) {
        //delete param_frame;
        delete gauss;
        delete sigma;
        delete mean;
      }
    }
    
    utils::printPlotCloseStack(&canvas, "AllPlots", config_toystudy_.plot_directory());
    sinfo.Ruler();
  }
  
  const std::vector<const RooFitResult*> ToyStudyStd::GetFitResults() const {
    std::vector<const RooFitResult*> new_vector;
    new_vector.reserve(fit_results_.size());
    
    for (std::vector<RooFitResult*>::const_iterator it = fit_results_.begin();
         it != fit_results_.end(); ++it) {
      new_vector.push_back(*it);
    }
    
    return new_vector;
  }
  
  RooArgSet ToyStudyStd::BuildEvaluationArgSet(const RooFitResult& fit_result) {
    RooArgSet parameters;
    
    RooArgSet parameter_init_list    = fit_result.floatParsInit();
    if (config_toystudy_.parameter_genvalue_read_file().size() > 0) {
      parameter_init_list.readFromFile(config_toystudy_.parameter_genvalue_read_file().c_str());
    }
    
    const RooArgList& parameter_list = fit_result.floatParsFinal();
    TIterator* parameter_iter        = parameter_list.createIterator();
    RooRealVar* parameter            = NULL;
    bool problems                    = false;
    
    RooRealVar* parameters_at_limit = new RooRealVar("parameters_at_limit","number of parameters close to limits", 0.0, 0.0, 100.0);
    RooRealVar* minos_problems      = new RooRealVar("minos_problems","number of parameters with MINOS problems", 0.0, 0.0, 100.0);
    
    while ((parameter = (RooRealVar*)parameter_iter->Next())) {
      TString pull_name = parameter->GetName() + TString("_pull");
      TString pull_desc = TString("Pull of ") + parameter->GetTitle();
      TString init_name = parameter->GetName() + TString("_init");
      TString init_desc = TString("Init value of ") + parameter->GetTitle();
      TString res_name  = parameter->GetName() + TString("_res");
      TString res_desc  = TString("Residual of ") + parameter->GetTitle();
      
      double par_error = ((RooRealVar*)parameter_list.find(parameter->GetName()))->getError();
      
      RooRealVar& par  = CopyRooRealVar(*parameter);
      RooRealVar* pull = new RooRealVar(pull_name, pull_desc, 0.0);
      RooRealVar* res  = new RooRealVar(res_name, res_desc, 0.0);
      RooRealVar& init = CopyRooRealVar(*(RooRealVar*)parameter_init_list.find(par.GetName()), std::string(init_name), std::string(init_desc));
      
      double pull_value = 0.0;
      
      // asymmetric error handling
      if (par.hasAsymError() && config_toystudy_.handle_asymmetric_errors()) {
        if (par.getVal() <= init.getVal()) {
          pull_value = (init.getVal() - par.getVal())/par.getErrorHi();
        } else {
          pull_value = (par.getVal() - init.getVal())/par.getErrorLo();
        }
      } else {
        pull_value = (init.getVal() - par.getVal())/par.getError();
      }
      pull->setVal(pull_value);
      
      double res_value = (init.getVal() - par.getVal());
      res->setVal(res_value);
            
      if (TMath::Abs(pull_value) > 5.0) {
        swarn << "Pull for \"" << parameter->GetName() << "\" is " << pull_value
              << " (too large deviation from expectation)" << endmsg;
        if (unexpected_pulls_counter_.count(parameter->GetName()) == 0) {
          unexpected_pulls_counter_[parameter->GetName()] = 1;
        } else {
          unexpected_pulls_counter_[parameter->GetName()]++;
        }
        problems = true;
      }

      double min_distance_to_limits = 
      TMath::Min(TMath::Abs((par.getVal() - par.getMax())/
                            ((par.getMax() - par.getMin())/2)),
                 TMath::Abs((par.getVal() - par.getMin())/
                            ((par.getMax() - par.getMin())/2)));
      
      if (min_distance_to_limits < 1e-5) {
        swarn << "Parameter distance limit low for: " << par << endmsg;
        problems = true;
        parameters_at_limit->setVal(parameters_at_limit->getVal() + 1.0);
      }
      
      if (par.hasAsymError() && (par.getAsymErrorHi() == 0.0 || par.getAsymErrorLo() == 0.0)) {
        swarn << "MINOS could not determine asymmetric errors for: " << par << endmsg;
        problems = true;
        minos_problems->setVal(minos_problems->getVal() + 1.0);
      }
        
      parameters.addOwned(par);
      parameters.addOwned(*pull);
      parameters.addOwned(*res);
      parameters.addOwned(init);
    }
    if (problems) {
      fit_result.Print("v");
    }
    
    parameters.addOwned(*parameters_at_limit);
    parameters.addOwned(*minos_problems);

    delete parameter_iter;
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

  void ToyStudyStd::PrintUnexpectedPullOverview() const {
    for (std::map<std::string, int>::const_iterator it=unexpected_pulls_counter_.begin(); it!=unexpected_pulls_counter_.end(); ++it) {
      sinfo << "Pull for parameter " << (*it).first << ", number of times outside of -5..+5 range: " << (*it).second << endmsg;
    }
  }
  
  void ToyStudyStd::HandleSignal(int param) {
    swarn << "Caught signal " << param << " during save of fit result. Will try to end gracefully without damaging the file." << endmsg;
    abort_save_ = true;
  }
  
  void ToyStudyStd::SaveFitResultWorker() {
    TThread this_tthread;
    TStopwatch sw_lock;
    sw_lock.Reset();
    std::queue<std::pair<RooFitResult*, RooFitResult*> > saver_queue;

    // list of last 5 dead times for averaging to find a new flexible wait time
    std::list<double> deadtimes;
    deadtimes.push_back(10.0);
    
    while (accepting_fit_results_ || !fit_results_save_queue_.empty() || !saver_queue.empty()) { 
      
      sdebug << "SaveFitResultWorker(): starting loop with " << saver_queue.size() << " elements in our own queue." << endmsg;
      
      const string& filename = config_toystudy_.store_result_filename_treename().first();
      const string& treename = config_toystudy_.store_result_filename_treename().second();
      unsigned int save_counter = 1;
      
      // if our own queue is empty, we need to wait for new fit results to come 
      // in anyway.
      if (saver_queue.empty()) {
        sdebug << "SaveFitResultWorker(): waiting for fit results as we have none." << endmsg;
        
        std::pair<RooFitResult*, RooFitResult*> fit_results;
        if (fit_results_save_queue_.wait_and_pop(fit_results)) {
          saver_queue.push(fit_results);
          sdebug << "SaveFitResultWorker(): got a fit result pair." << endmsg;
        } else {
          sdebug << "SaveFitResultWorker(): got no fit result pair." << endmsg;
        }
      }
      
      // if global queue has entries, we should get them all
      while (!fit_results_save_queue_.empty()) {
        std::pair<RooFitResult*, RooFitResult*> fit_results;
        if (fit_results_save_queue_.wait_and_pop(fit_results)) {
          saver_queue.push(fit_results);
          sdebug << "SaveFitResultWorker(): got another fit result pair." << endmsg;
        } else {
          sdebug << "SaveFitResultWorker(): got no other fit result pair." << endmsg;
        }
      }
      
      if (!saver_queue.empty()) {
        utils::FileLock flock(filename);
        boost::random::random_device rnd;
        int wait_time, wait_i;
        sw_lock.Start(false);
        if (!flock.Lock()) {
          // get new wait time based on last deadtimes's average
          double new_wait_time = 0.0;
          for (std::list<double>::const_iterator it = deadtimes.begin();
               it != deadtimes.end(); ++it) {
            new_wait_time += *it;
          }
          new_wait_time += sw_lock.RealTime();
          sdebug << sw_lock.RealTime() << endmsg;
          sw_lock.Start(false);
          new_wait_time /= deadtimes.size()+1;
          
          wait_time = (static_cast<double>(rnd())/static_cast<double>(rnd.max()-rnd.min())-rnd.min())*new_wait_time+10.0;
          swarn << "File to save fit result to " << filename << " is locked. Will try again in " << wait_time << " s." << endmsg;
          utils::Sleep(wait_time);
          boost::this_thread::yield();
        } else {
          signal(SIGINT, ToyStudyStd::HandleSignal);
          signal(SIGTERM, ToyStudyStd::HandleSignal);
          sw_lock.Stop();
          sinfo << "File locking deadtime: " << sw_lock << endmsg;
          deadtimes.push_back(sw_lock.RealTime());
          while (deadtimes.size() > 5) deadtimes.pop_front();
          
          if (!abort_save_) {
            sinfo << "Saving fit result to file " << filename << endmsg;
            bool file_existing = fs::exists(filename);
            TFile f(fs::absolute(filename).string().c_str(),"update");
            if (f.IsZombie() || !f.IsOpen()) {
              serr << "Cannot open file which may be corrupted." << endmsg;
              flock.Unlock();
              throw ExceptionCannotStoreFitResult();
            } else {
              if (!abort_save_) {
                sdebug << "SaveFitResultWorker(): number of results in queue: " << saver_queue.size() << endmsg;
                std::pair<RooFitResult*, RooFitResult*> fit_results = saver_queue.front();
                saver_queue.pop();
                RooFitResult* fit_result1 = fit_results.first;
                RooFitResult* fit_result2 = fit_results.second;
                
                TTree* tree_results = NULL;
                if (file_existing) {      
                  tree_results = (TTree*)f.Get(treename.c_str());
                } 
                
                if (tree_results == NULL) {
                  tree_results = new TTree(treename.c_str(), "Tree for toy study fit results");
                  tree_results->Branch(config_toystudy_.fit_result1_branch_name().c_str(), "RooFitResult", &fit_result1, 64000, 0);
                  if (fit_result2 != NULL) {
                    tree_results->Branch(config_toystudy_.fit_result2_branch_name().c_str(), "RooFitResult", &fit_result2, 64000, 0);
                  }
                } else {  
                  if (tree_results->GetBranch(config_toystudy_.fit_result1_branch_name().c_str()) == NULL) {
                    serr << "Cannot store fit result! Tree exists but branch " << config_toystudy_.fit_result1_branch_name() << " is unknown." << endmsg;
                    f.Close();
                    flock.Unlock();
                    throw ExceptionCannotStoreFitResult();
                  }
                  if (fit_result2 != NULL && tree_results->GetBranch(config_toystudy_.fit_result2_branch_name().c_str()) == NULL) {
                    serr << "Cannot store fit result! Tree exists but branch " << config_toystudy_.fit_result2_branch_name() << " is unknown." << endmsg;
                    f.Close();
                    flock.Unlock();
                    throw ExceptionCannotStoreFitResult();
                  }
                  
                  tree_results->SetBranchAddress(config_toystudy_.fit_result1_branch_name().c_str(), &fit_result1);
                  if (fit_result2 != NULL) {
                    tree_results->Print();
                    fit_result2->Print();
                    tree_results->SetBranchAddress(config_toystudy_.fit_result2_branch_name().c_str(), &fit_result2);
                  }
                }
                                
                tree_results->Fill();
                
                delete fit_result1;
                delete fit_result2;
                
                while (!saver_queue.empty()) {
                  fit_results = saver_queue.front();
                  saver_queue.pop();
                  fit_result1 = fit_results.first;
                  fit_result2 = fit_results.second;
                  
                  fit_result1->Print();
                  fit_result2->Print();
                  
                  tree_results->Fill();
                  save_counter++;
                  delete fit_result1;
                  delete fit_result2;
                }
                
                tree_results->Write("",TObject::kOverwrite);
                
                sinfo << "Deferred saving of " << save_counter << " fit results (resp. pairs of fit results) successful." << endmsg;
              } else {
                swarn << "Caught signal. Did not save fit result although file was opened. Trying to close file gracefully." << endmsg;
              }
              
              f.Close();
              
              flock.Unlock();
            }
          } else {
            flock.Unlock();
            swarn << "Aborting save as signal to end execution was caught before." << endmsg;
          }
          sw_lock.Stop();
          sw_lock.Reset();
        }
        if (abort_save_) {
          swarn << "Caught signal. If we reached this, the file should have been closed gracefully without damage." << endmsg;
          exit(1);
        }
      }
      
      signal(SIGINT, SIG_DFL);
      signal(SIGTERM, SIG_DFL);
    }
  }
} // namespace Toy
} // namespace doofit
