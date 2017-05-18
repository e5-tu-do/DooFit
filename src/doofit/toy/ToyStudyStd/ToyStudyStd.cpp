#include "doofit/toy/ToyStudyStd/ToyStudyStd.h"

// STL
#include <string>
#include <utility>
#include <queue>
#include <list>
#include <csignal>

// boost
#include "boost/interprocess/sync/file_lock.hpp"
#include "boost/filesystem.hpp"
#include "boost/random/random_device.hpp"
#include <boost/thread.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>

// ROOT
#include "TTree.h"
#include "TFile.h"
#include "TBranch.h"
#include "TMath.h"
#include "TStopwatch.h"
#include "TROOT.h"
#include "TString.h"
#include "TThread.h"
#include "TCanvas.h"
#include "TStopwatch.h"
#include "TGraph.h"
#include "TAxis.h"
#include "TPaveText.h"

// from RooFit
#include "RooFitResult.h"
#include "RooArgList.h"
#include "RooDataSet.h"
#include "RooRealVar.h"
#include "RooGaussian.h"
#include "RooPlot.h"
#include "RooTreeDataStore.h"

// from DooCore
#include "doocore/io/MsgStream.h"
#include "doocore/lutils/lutils.h"
#include "doocore/system/FileLock.h"
#include <doocore/io/Progress.h>
#include <doocore/statistics/general.h>

// from Project
#include "doofit/config/CommonConfig.h"
#include "doofit/config/CommaSeparatedPair.h"
#include "doofit/config/CommaSeparatedList.h"
#include "doofit/toy/ToyStudyStd/ToyStudyStdConfig.h"
#include "doofit/plotting/Plot/PlotConfig.h"
#include <doofit/fitter/easyfit/EasyFitResult.h>

using namespace ROOT;
using namespace RooFit;
using namespace doocore::io;

namespace doofit {
namespace toy {
  namespace fs = boost::filesystem;
  bool ToyStudyStd::abort_save_ = false;
  
  ToyStudyStd::ToyStudyStd(const config::CommonConfig& cfg_com, const ToyStudyStdConfig& cfg_tstudy, const doofit::plotting::PlotConfig& cfg_plot) :
  config_common_(cfg_com),
  config_toystudy_(cfg_tstudy),
  config_plot_(cfg_plot),
  fit_results_(),
  fit_results_bookkeep_(),
  evaluated_values_(NULL),
  accepting_fit_results_(true),
  reading_fit_results_(false),
  fit_results_read_queue_(),
  file_tree_easyfit_(std::make_pair(nullptr, nullptr)),
  num_easyfit_results_(0),
  easyfit_result_0_(nullptr),
  easyfit_result_1_(nullptr),
  debug_(false)
  {
    LockSaveFitResultMutex();
    abort_save_ = false;
    fitresult_save_worker_   = boost::thread(&ToyStudyStd::SaveFitResultWorker, this);
  }
  
  ToyStudyStd::~ToyStudyStd() {
    FinishFitResultSaving();
    reading_fit_results_ = false;
    if (fitresult_reader_worker_.joinable()) fitresult_reader_worker_.join();
    
    if (evaluated_values_ != NULL) delete evaluated_values_;
    for (std::vector<FitResultContainer>::const_iterator it_results = fit_results_bookkeep_.begin(); it_results != fit_results_bookkeep_.end(); ++it_results) {
      delete std::get<0>(*it_results);
      if (std::get<1>(*it_results) != NULL) delete std::get<1>(*it_results);
    }

    if (easyfit_result_0_ != nullptr) {
      delete easyfit_result_0_;
    }
    if (easyfit_result_1_ != nullptr) {
      delete easyfit_result_1_;
    }
  }
  
  void ToyStudyStd::StoreFitResult(const RooFitResult* fit_result1, 
                                   const RooFitResult* fit_result2,
                                   TStopwatch* stopwatch1,
                                   TStopwatch* stopwatch2,
                                   int seed,
                                   int run_id) {
    if (!accepting_fit_results_) {
      serr << "No longer accepting fit results." << endmsg;
    } else {
      fitresult_save_worker_mutex_.unlock();
      
      const std::string& filename = config_toystudy_.store_result_filename_treename().first();
      const std::string& treename = config_toystudy_.store_result_filename_treename().second();
      if (filename.length() == 0 || treename.length() == 0) {
        serr << "File name and or tree name to save fit result into not set! Cannot store fit result." << endmsg;
        throw ExceptionCannotStoreFitResult();
      }
      
      //RooFitResult* fit_result1_copy = new RooFitResult(*fit_result1);
      const RooFitResult* fit_result1_copy = fit_result1;
      const RooFitResult* fit_result2_copy = NULL;
      
      if (fit_result2 != NULL) {
        //fit_result2_copy = new RooFitResult(*fit_result2);
        fit_result2_copy = fit_result2;
      }
      
      double time_cpu1 = 0.0, time_real1 = 0.0, time_cpu2 = 0.0, time_real2 = 0.0;
      if (stopwatch1 != NULL) {
        time_cpu1  = stopwatch1->CpuTime();
        time_real1 = stopwatch1->RealTime();
      }
      if (stopwatch2 != NULL) {
        time_cpu2  = stopwatch2->CpuTime();
        time_real2 = stopwatch2->RealTime();
      }
      
      fit_results_save_queue_.push(std::make_tuple(fit_result1_copy,
                                                   fit_result2_copy,
                                                   time_cpu1,
                                                   time_real1,
                                                   time_cpu2,
                                                   time_real2,
                                                   seed, 
                                                   run_id));
      sinfo << "Accepting fit result 1 for deferred saving" << endmsg;
      if (fit_result2 != NULL) {
        sinfo << "Accepting fit result 2 for deferred saving" << endmsg;
      }
      
      LockSaveFitResultMutex();
    }
  }
  
  void ToyStudyStd::ReadFitResults() {
    // stop saving if there are still deferred fit results to be saved.
    FinishFitResultSaving();
    
    reading_fit_results_ = true;

    ReadFitResultWorker();

    // if (!fitresult_reader_worker_.joinable()) {
    //   fitresult_reader_worker_ = boost::thread(&ToyStudyStd::ReadFitResultWorker, this);
    // }
  }
  
  FitResultContainer ToyStudyStd::GetFitResult() {
    const RooFitResult* dummy = nullptr;
    FitResultContainer fit_results(dummy, dummy, 0.0, 0.0, 0.0, 0.0, 0, 0);
    
    // std::cout << "fitresult_reader_worker_.joinable(): " << fitresult_reader_worker_.joinable() << std::endl;
    // std::cout << "fit_results_read_queue_.size(): " << fit_results_read_queue_.size() << " entries" << std::endl;
    // std::cout << "reading_fit_results_ : " << reading_fit_results_ << std::endl;

    // if (!fitresult_reader_worker_.joinable()) {
    //   return fit_results;
    // } else {
      bool got_one = false;
      // while (!got_one && (reading_fit_results_ || fit_results_read_queue_.size() > 0)) {
      while (!got_one && (fit_results_read_queue_.size() > 0)) {
        got_one = fit_results_read_queue_.wait_and_pop(fit_results);
      }

      // std::cout << std::get<0>(fit_results) << std::endl;
      // std::cout << std::get<1>(fit_results) << std::endl;

      // if we got_one, return, if not return default NULL pointer pair
      // (this will happen if the worker stopped)
      return fit_results;
    // }
  }

  unsigned long long ToyStudyStd::NumberOfAvailableEasyFitResults() {
    using namespace doofit::fitter::easyfit;
    using namespace doocore::io;

    if (num_easyfit_results_ == 0 && results_files_easyfit_.size() > 0) {
      for (auto file_tree : results_files_easyfit_) {
        TFile file(file_tree.first().c_str(), "read");
        TTree* tree = dynamic_cast<TTree*>(file.Get(file_tree.second().c_str()));
        if (tree != nullptr) {
          num_easyfit_results_ += tree->GetEntries();
        }
      }
    }

    return num_easyfit_results_;
  }

  EasyFitResultContainer ToyStudyStd::GetEasyFitResult() {
    using namespace doofit::fitter::easyfit;
    using namespace doocore::io;

    TFile*& file(file_tree_easyfit_.first);
    TTree*& tree(file_tree_easyfit_.second);

    if (file == nullptr && tree == nullptr && results_files_easyfit_.size() == 0) {
      if (easyfit_result_0_ != nullptr) {
        delete easyfit_result_0_;
        easyfit_result_0_ = nullptr;
      }
      if (easyfit_result_1_ != nullptr) {
        delete easyfit_result_1_;
        easyfit_result_1_ = nullptr;
      }
      return std::make_tuple(easyfit_result_0_, easyfit_result_1_);
    }

    // on demand open new file
    if (file == nullptr && tree == nullptr) {
      while (file == nullptr && tree == nullptr) {
        doofit::config::CommaSeparatedPair<std::string> file_tree = results_files_easyfit_.front();
        results_files_easyfit_.pop_front();

        // sdebug << "Opening " << file_tree.first().c_str() << endmsg;
        file = new TFile(file_tree.first().c_str(), "read");
        tree = dynamic_cast<TTree*>(file->Get(file_tree.second().c_str()));
      }

      // sdebug << "Opened new file" << endmsg;
      // sdebug << file_tree_easyfit_.first << endmsg;
      // sdebug << file_tree_easyfit_.second << endmsg;

      position_tree_easyfit_ = 0;
      if (easyfit_result_0_ != nullptr) {
        delete easyfit_result_0_;
      }
      if (easyfit_result_1_ != nullptr) {
        delete easyfit_result_1_;
      }

      easyfit_result_0_ = new EasyFitResult(*tree, "fr0_");
      easyfit_result_1_ = new EasyFitResult(*tree, "fr1_");
    }

    tree->GetEntry(position_tree_easyfit_);
    --num_easyfit_results_;
    ++position_tree_easyfit_;
    
    if (position_tree_easyfit_ == tree->GetEntries()) {
      delete tree;
      file->Close();
      delete file;

      tree = nullptr;
      file = nullptr;

      // sdebug << "Closing and releasing file" << endmsg;
      // sdebug << file_tree_easyfit_.first << endmsg;
      // sdebug << file_tree_easyfit_.second << endmsg;
    }

    return std::make_tuple<const doofit::fitter::easyfit::EasyFitResult*,const doofit::fitter::easyfit::EasyFitResult*>(easyfit_result_0_, easyfit_result_1_);
  }

  
  void ToyStudyStd::ReleaseFitResult(FitResultContainer fit_results) {
    fit_results_release_queue_.push(fit_results);
  }

  void ToyStudyStd::PurgeReleasedFitResults() {
    while (fit_results_release_queue_.size() > 0) {
      const RooFitResult* dummy = nullptr;
      FitResultContainer fit_results(dummy,dummy,0.0,0.0,0.0,0.0,0,0);
      if (fit_results_release_queue_.wait_and_pop(fit_results)) {
        //sdebug << "Deleting fit results." << endmsg;
        if (std::get<0>(fit_results) != nullptr) delete std::get<0>(fit_results);
        if (std::get<1>(fit_results) != nullptr) delete std::get<1>(fit_results);
      }
    }
  }
  
  void ToyStudyStd::EvaluateFitResults() {
    RooDataSet::setDefaultStorageType(RooAbsData::Tree);
    const RooFitResult* dummy = nullptr;
    FitResultContainer fit_results(dummy, dummy, 0.0, 0.0, 0.0, 0.0, 0, 0);
    do {
      fit_results = GetFitResult();
      
      if (std::get<0>(fit_results) != NULL) {
        fit_results_.push_back(fit_results);
        fit_results_bookkeep_.push_back(fit_results);

        int seed = std::get<6>(fit_results);
        //if (seed != 0) {
        fit_results_by_seed_.insert(std::pair<int,FitResultContainer>(seed, fit_results));
        //}
      }
    } while (std::get<0>(fit_results) != NULL);
    
    sinfo.Ruler();
    sinfo << "Evaluating fit results" << endmsg;
    if (fit_results_.size() <= 0) {
      serr << "Cannot evaluate as no fit results are loaded." << endmsg;
      throw ExceptionCannotEvaluateFitResults();
    }
        
    // build list of all parameters, pulls, etc.
    auto it_results(fit_results_by_seed_.begin());
    RooArgSet* parameter_set = new RooArgSet(BuildEvaluationArgSet(it_results->second));
    // sdebug << "#1 size: " << parameter_set->getSize() << endmsg;
    while (parameter_set->getSize() == 0 && it_results != fit_results_by_seed_.end()) {
      ++it_results;
      delete parameter_set;
      parameter_set = new RooArgSet(BuildEvaluationArgSet(it_results->second));
      // sdebug << "#1 size: " << parameter_set->getSize() << endmsg;
      // sdebug << "is end (1st attempt): " << (it_results == fit_results_by_seed_.end()) << endmsg;
    }
    
    int i(0);

    // loop over fit results and fill all values into RooDataSet
    if (evaluated_values_ != NULL) delete evaluated_values_;
    if (parameter_set->getSize() > 0) {
      evaluated_values_ = new RooDataSet("evaluated_values", "evaluated_values", *parameter_set);
      // sdebug << "Adding first!" << endmsg;
      evaluated_values_->add(*parameter_set);
      i = 1;
    }
    delete parameter_set;
    parameter_set = nullptr;

    // sdebug << "is end (before loop): " << (it_results == fit_results_by_seed_.end()) << endmsg;
    if (fit_results_.size() > 1 && it_results != fit_results_by_seed_.end()) {
      ++it_results;
      // sdebug << "is end (in loop): " << (it_results == fit_results_by_seed_.end()) << endmsg;
      //for (std::vector<FitResultContainer>::const_iterator it_results = fit_results_.begin()+1; it_results != fit_results_.end(); ++it_results) {
      while (it_results != fit_results_by_seed_.end()) {
        // sdebug << "#" << i << " seed: " << it_results->first << endmsg;
        parameter_set = new RooArgSet(BuildEvaluationArgSet(it_results->second));
        ++it_results;
        // sdebug << "#" << i << " size: " << parameter_set->getSize() << endmsg;
        while (parameter_set->getSize() == 0 && it_results != fit_results_by_seed_.end()) {
          
          // sdebug << "is end (in looploop): " << (it_results == fit_results_by_seed_.end()) << endmsg;
          delete parameter_set;
          parameter_set = new RooArgSet(BuildEvaluationArgSet(it_results->second));
          ++it_results;
          // sdebug << "#" << i << " size: " << parameter_set->getSize() << endmsg;
        }

        if (parameter_set->getSize() > 0) {
          // sdebug << "Adding one inside loop!" << endmsg;
          evaluated_values_->add(*parameter_set);
          ++i;
        }

        // sdebug << "is end: " << (it_results == fit_results_by_seed_.end()) << endmsg;
        delete parameter_set;
        parameter_set = nullptr;
      }
    }
    
    PrintUnexpectedPullOverview();
    
    sinfo << "Evaluated " << i << " fit results." << endmsg;
    sinfo.Ruler();
  }
  
  void ToyStudyStd::PlotEvaluatedParameters() {
    if (evaluated_values_ == nullptr || evaluated_values_->numEntries() <= 0) {
      serr << "Cannot plot as no fit results are evaluated." << endmsg;
      throw ExceptionCannotEvaluateFitResults();
    }

    TFile data_tree_file("data_tree.root", "RECREATE");
    (evaluated_values_->tree())->Write();
    data_tree_file.Close();
 
    sinfo.Ruler();
    sinfo << "Plotting parameter distributions." << endmsg;
    
    gROOT->SetStyle("Plain");
    doocore::lutils::setStyle("LHCbOptimized");
    TCanvas canvas("canvas_toystudy", "canvas", 800, 600);
    doocore::lutils::printPlotOpenStack(&canvas, "AllPlots", config_toystudy_.plot_directory());
    
    const RooArgSet* parameters = evaluated_values_->get();
    int num_parameters          = parameters->getSize();
    TIterator* parameter_iter   = parameters->createIterator();
    RooRealVar* parameter       = NULL;
    
    using namespace doocore::io;
    Progress p("Evaluating parameter distributions", num_parameters);
    
    std::map<std::string, std::tuple<double, double, double, double>> parameter_cls;

    while ((parameter = (RooRealVar*)parameter_iter->Next())) {
      // std::chrono::high_resolution_clock::time_point time_start(std::chrono::high_resolution_clock::now());

      std::string param_name = parameter->GetName();

      std::vector<std::string> postfixes_error;

      if (config_toystudy_.plot_parameter_vs_error_correlation()) {
        postfixes_error.push_back("_err");
        postfixes_error.push_back("_lerr");
        postfixes_error.push_back("_herr");

        for (auto postfix_error : postfixes_error) {
          std::string name_error = param_name + postfix_error;
          const RooRealVar* error = dynamic_cast<const RooRealVar*>(parameters->find(name_error.c_str()));
          if (error != nullptr) {
            //sinfo << "Parameter " << param_name << " can be correlated against an error." << endmsg;

            std::vector<double> values;
            std::vector<double> errors;
            values.reserve(evaluated_values_->numEntries());
            errors.reserve(evaluated_values_->numEntries());

            for (int i=0; i<evaluated_values_->numEntries(); ++i) {
              const RooArgSet* params = evaluated_values_->get(i);
              //sdebug << param_name << " - " << params->getRealValue(param_name.c_str()) << " +/- " << params->getRealValue(name_error.c_str()) << endmsg;
              values.push_back(params->getRealValue(param_name.c_str()));
              errors.push_back(params->getRealValue(name_error.c_str()));
            }

            TGraph graph_value_error(evaluated_values_->numEntries(), &values[0], &errors[0]);
            TCanvas canvas("c", "c", 800, 600);
            graph_value_error.Draw("AP");

            graph_value_error.GetXaxis()->SetTitle(parameter->GetTitle());
            graph_value_error.GetYaxis()->SetTitle(error->GetTitle());

            std::string plot_name = param_name + "_err_corr" + postfix_error;
            doocore::lutils::printPlot(&canvas, plot_name, config_toystudy_.plot_directory(), true);
          }
        }
      }

      std::vector<std::string> params_for_cor{"parS_pull",
                                              "parC_pull"};

      if (false && param_name.find("_pull") != std::string::npos) { //make configurable here

        for (auto param_for_cor_name : params_for_cor) {
          const RooRealVar* par_for_cor = dynamic_cast<const RooRealVar*>(parameters->find(param_for_cor_name.c_str()));

          if (par_for_cor != nullptr && param_for_cor_name != param_name) {
            sinfo << "Detailed! correlation plot for Parameter " << param_for_cor_name << " and " << param_name << "." << endmsg;

            std::vector<double> values;
            std::vector<double> par_for_cor_values;
            values.reserve(evaluated_values_->numEntries());
            par_for_cor_values.reserve(evaluated_values_->numEntries());

            for (int i=0; i<evaluated_values_->numEntries(); ++i) {
              const RooArgSet* params = evaluated_values_->get(i);
              sdebug << param_name << " vs. " << param_for_cor_name << ": " << params->getRealValue(param_name.c_str()) << " vs. " << params->getRealValue(param_for_cor_name.c_str()) << endmsg;
              double value = params->getRealValue(param_name.c_str());
              double par_for_cor_value = params->getRealValue(param_for_cor_name.c_str());

              if(isinf(value)){
                value = 10000;
              }
              if(isinf(par_for_cor_value)){
                par_for_cor_value = 10000;
              }
              values.push_back(value);
              par_for_cor_values.push_back(par_for_cor_value);
            }

            TGraph graph_value_error(evaluated_values_->numEntries(), &values[0], &par_for_cor_values[0]);
            TCanvas canvas("c", "c", 800, 600);
            graph_value_error.Draw("AP");

            graph_value_error.GetXaxis()->SetTitle(parameter->GetTitle());
            graph_value_error.GetYaxis()->SetTitle(par_for_cor->GetTitle());

            std::string plot_name = param_name + "_" + param_for_cor_name.c_str() + "_corr";
            doocore::lutils::printPlot(&canvas, plot_name, config_toystudy_.plot_directory(), true);
          }
        }
      }

      // std::chrono::high_resolution_clock::time_point time_correlation(std::chrono::high_resolution_clock::now());      
      // double duration_correlation(std::chrono::duration_cast<std::chrono::microseconds>(time_correlation - time_start).count());
      // sdebug << "duration_correlation = " << duration_correlation*1e-3 << endmsg;

      std::vector<std::string> ignore_for_cls{"_err",
                                              "_lerr",
                                              "_herr",
                                              "_init",
                                              "_res",
                                              "_refresidual",
                                              "_pull",
                                              "minos_problems",
                                              "parameters_at_limit"
                                              };

      std::pair<double,double> minmax = doocore::lutils::MedianLimitsForTuple(*evaluated_values_, param_name);

      if (config_toystudy_.plot_on_full_range()) {
        minmax = doocore::lutils::MinMaxLimitsForDataSet(*evaluated_values_, param_name);
      }
      
      // std::chrono::high_resolution_clock::time_point time_sort(std::chrono::high_resolution_clock::now());      
      // double duration_sort(std::chrono::duration_cast<std::chrono::microseconds>(time_sort - time_correlation).count());
      // sdebug << "duration_sort = " << duration_sort*1e-3 << endmsg;

      // if (param_name == "par_bssig_time_S_err") {
      //   minmax.second = 2.3;
      // }

      // sinfo << "Plotting parameter " << param_name << " in range [" << minmax.first << "," << minmax.second << "]" << endmsg;
            
      RooRealVar* mean             = nullptr;
      RooRealVar* sigma            = nullptr;
      RooGaussian* gauss           = nullptr;
      // RooPlot* param_frame         = nullptr;
      RooDataSet* fit_plot_dataset = nullptr;
      RooArgSet parameters_copy(*parameter);
      int fit_status = 0;

      TString cut = "";
      if (config_toystudy_.fit_plot_on_quantile_window()) {
        cut = param_name + ">" + boost::lexical_cast<std::string>(minmax.first) + "&&" + param_name + "<" + boost::lexical_cast<std::string>(minmax.second);
      }
      //sdebug << cut << endmsg;

      // if (param_name == "par_bssig_time_S_pull") {
      //   //cut = param_name + ">" + boost::lexical_cast<std::string>(minmax.first) + "&&" + param_name + "<0.75"; 
      //   parameters_copy.add(*parameters->find("par_bssig_time_S_herr"));
      //   cut = cut + "&&" + "par_bssig_time_S_herr<0.4";
      // }
      // parameters_copy.Print();
      // sdebug << cut << endmsg;

      if (config_toystudy_.neglect_parameters_at_limit()) {
        if (cut.Length() > 0) cut = cut + "&&";
        cut = cut + "parameters_at_limit < 0.5";
        parameters_copy.add(*parameters->find("parameters_at_limit"));
      }
      if (config_toystudy_.neglect_minos_problems()) {
        if (cut.Length() > 0) cut = cut + "&&";
        cut = cut + "minos_problems < 0.5";
        parameters_copy.add(*parameters->find("minos_problems"));
      }
      if (cut.Length() > 0) {
        fit_plot_dataset = new RooDataSet("fit_plot_dataset", "Plotting and fitting dataset for ToyStudyStd", evaluated_values_, parameters_copy, cut);
      } else {
        fit_plot_dataset = evaluated_values_;
      }

      //CL calculation 
      bool isparameteritself = true;
      for(auto i : ignore_for_cls){
        if (param_name.find(i) != std::string::npos)
          isparameteritself = false;
      }
      if(isparameteritself){
        std::vector<double> sorted_data(fit_plot_dataset->numEntries());
        double mean = doocore::statistics::general::get_mean_from_dataset(fit_plot_dataset, param_name);
        double median = doocore::statistics::general::get_median_from_dataset(fit_plot_dataset, param_name);
        double CL_boundary_lo = doocore::statistics::general::get_quantile_from_dataset(fit_plot_dataset, param_name, 0.15865, sorted_data);
        double CL_boundary_hi = doocore::statistics::general::get_quantile_from_dataset(sorted_data, 0.84135);

        parameter_cls.insert(std::pair<std::string, std::tuple<double, double, double, double> >(param_name, std::make_tuple(mean, median, CL_boundary_lo,CL_boundary_hi)));
      }

      // std::chrono::high_resolution_clock::time_point time_prefit(std::chrono::high_resolution_clock::now());      
      // double duration_prefit(std::chrono::duration_cast<std::chrono::microseconds>(time_prefit - time_sort).count());
      // sdebug << "duration_prefit = " << duration_prefit*1e-3 << endmsg;

      // for all pulls fit a gaussian
      RooFitResult* fit_result = nullptr;
      if (param_name.length() > 4 &&
          (param_name.substr(param_name.length()-5).compare("_pull") == 0 ||
           param_name.substr(param_name.length()-4).compare("_res") == 0 ||
           param_name.substr(param_name.length()-4).compare("_err") == 0 ||
           param_name.substr(param_name.length()-5).compare("_init") == 0 ||
           (param_name.length() > 12 && param_name.substr(param_name.length()-12).compare("_refresidual") == 0))) {
           // || param_name.substr(0,4).compare("time") == 0)) {
        mean  = new RooRealVar("m", "mean of pull", (minmax.first+minmax.second)/2.0,minmax.first,minmax.second);
        sigma = new RooRealVar("s", "sigma of pull", (minmax.second-minmax.first)/10.0,0,minmax.second-minmax.first);
        gauss = new RooGaussian("pdf_pull", "Gaussian PDF of pull", *parameter, *mean, *sigma);
        
        sinfo.increment_indent(2);
        //sinfo << "Fitting Gaussian distribution for parameter " << param_name << endmsg;
        int num_lost_entries     = evaluated_values_->numEntries() - fit_plot_dataset->numEntries();
        double frac_lost_entries = (evaluated_values_->numEntries() - fit_plot_dataset->numEntries())/static_cast<double>(evaluated_values_->numEntries())*100;
        if (fit_plot_dataset != evaluated_values_ && frac_lost_entries>1.0) {
          sinfo << "Losing " << num_lost_entries << " (" << frac_lost_entries << "%) toys for this fit due to cuts applied for " << param_name << "." << endmsg;
        }

        // supress RooFit spam
        RooMsgService::instance().setStreamStatus(0, false);
        RooMsgService::instance().setStreamStatus(1, false);

        int num_cores = std::max(fit_plot_dataset->numEntries() < 200 ? fit_plot_dataset->numEntries()/1000+1 : 1, 8);
        //sdebug << fit_plot_dataset->numEntries() << " thus " << num_cores << " cores." << endmsg;
        num_cores = 1;
        fit_result = gauss->fitTo(*fit_plot_dataset, NumCPU(num_cores), Verbose(false), PrintLevel(-1), PrintEvalErrors(-1), Warnings(false), Save(true),  Minimizer("Minuit2","minimize"), Optimize(0));
        fit_status = fit_result->statusCodeHistory(0);

        // un-supress RooFit spam
        RooMsgService::instance().setStreamStatus(0, true);
        RooMsgService::instance().setStreamStatus(1, true);

        //fit_result->Print("v");
        sinfo.increment_indent(-2);
      }

      // std::chrono::high_resolution_clock::time_point time_postfit(std::chrono::high_resolution_clock::now());      
      // double duration_postfit(std::chrono::duration_cast<std::chrono::microseconds>(time_postfit - time_prefit).count());
      // sdebug << "duration_postfit = " << duration_postfit*1e-3 << endmsg;
      
      int num_bins = fit_plot_dataset->numEntries() < 1000 ? fit_plot_dataset->numEntries()/10 : 100;
      if (num_bins < 10) num_bins = 10;
      parameter->setBins(num_bins);
      
      RooPlot* frame = parameter->frame(Range(minmax.first,minmax.second));

      if (config_toystudy_.plot_symmetric_around_mean() && gauss != nullptr /* && param_name.substr(0,4).compare("time") != 0 */) {
        // Define plot range symmetrical around mean of Gaussian fit
        double range_limit = std::max(mean->getVal() - minmax.first, minmax.second - mean->getVal());
        minmax.first = mean->getVal() - range_limit;
        minmax.second = mean->getVal() + range_limit;
        frame = parameter->frame(Range(mean->getVal() - range_limit, mean->getVal() + range_limit));
      }

      fit_plot_dataset->plotOn(frame);
      if (fit_plot_dataset != evaluated_values_) delete fit_plot_dataset;
      
      // get fit pars
      RooRealVar* var_mean = nullptr;
      RooRealVar* var_sigma = nullptr;

      // only plot the fit gauss for init distributions for constrained parameters
      // in the next line we check if the width of the gaussian is wide enough,
      // if not, then set bool to false.
      bool plot_gauss_pdf_for_init_distributions = true;
      if (gauss != NULL && fit_status == 0){
        const RooArgList& list_fit_params(fit_result->floatParsFinal());
        var_mean  = dynamic_cast<RooRealVar*>(list_fit_params.find("m"));
        var_sigma = dynamic_cast<RooRealVar*>(list_fit_params.find("s"));
        if (var_sigma->getVal()<1e-6) plot_gauss_pdf_for_init_distributions = false;
      }

      TPaveText* pt = nullptr;
      if (gauss != NULL && fit_status == 0) {
        // sdebug << config_plot_.GetPdfLineColor(1) << endmsg;

        if (plot_gauss_pdf_for_init_distributions) gauss->plotOn(frame, LineColor(config_plot_.GetPdfLineColor(1)));
        //param_frame = gauss->paramOn(frame, Layout(0.6, 0.9, 0.9));

        using namespace doocore::statistics::general;
        ValueWithError<double> val_mean(var_mean->getVal(), var_mean->getError());
        ValueWithError<double> val_sigma(var_sigma->getVal(), var_sigma->getError());

        pt = new TPaveText(0.57, 0.77, 0.92, 0.92, "NB NDC");
        pt->SetFillColor(kWhite);
        //pt->SetOption("NB");
        pt->SetTextAlign(12);
        pt->SetBorderSize(1);
        pt->SetMargin(0.05);
        pt->SetTextFont(133);
        pt->SetTextSize(30);

        std::string str_mean("m = ");
        str_mean += val_mean.FormatStringTLatex();
        std::string str_sigma("s = ");
        str_sigma += val_sigma.FormatStringTLatex();

        if (param_name.length() > 4 &&
          (param_name.substr(param_name.length()-5).compare("_pull") == 0)) {
          double pull_mean(val_mean.value/val_mean.error);
          double pull_sigma((val_sigma.value-1.0)/val_sigma.error);

          std::stringstream sstr_pull_mean;
          sstr_pull_mean << std::setprecision(2) << std::fixed << pull_mean;
          std::stringstream sstr_pull_sigma;
          sstr_pull_sigma << std::setprecision(2) << std::fixed << pull_sigma;

          str_mean  += " (" + sstr_pull_mean.str()  + "#kern[0.3]{#sigma})";
          str_sigma += " (" + sstr_pull_sigma.str() + "#kern[0.3]{#sigma})";

          boost::replace_all(str_mean,  "-", "#minus");
          boost::replace_all(str_sigma, "-", "#minus");

          pt->SetX1(0.46);
        }

        pt->AddText(str_mean.c_str());
        pt->AddText(str_sigma.c_str());

        frame->SetMaximum(frame->GetMaximum()*1.3);

      } else if (fit_status != 0) {
        swarn << "ToyStudyStd::PlotEvaluatedParameters(): Gaussian fit for " << parameter->GetName() << " failed. Will not plot." << endmsg;
      }
      frame->Draw();

      if (pt != nullptr) {
        pt->Draw();
      }

      double width_bin = static_cast<double>(minmax.second-minmax.first)/static_cast<double>(num_bins);
      std::stringstream str;
      using boost::format;
      str << format("%1$.1g") % width_bin;

      TString ylabel = frame->GetYaxis()->GetTitle();
      ylabel.ReplaceAll("Events","Pseudo-experiments");

      std::string label_y = "Pseudo-experiments / " + str.str();

      // sdebug << label_y << endmsg;

      frame->GetYaxis()->SetTitle(label_y.c_str());

      TString plot_name = parameter->GetName();
      doocore::lutils::printPlot(&canvas, plot_name, config_toystudy_.plot_directory(), true);
      doocore::lutils::printPlot(&canvas, "AllPlots", config_toystudy_.plot_directory(), true);
      
      if (fit_result != nullptr) {
        delete fit_result;
      }
      if (pt != nullptr) {
        delete pt;
      }

      delete frame;
      
      if (gauss != NULL) {
        //delete param_frame;
        delete gauss;
        delete sigma;
        delete mean;
      }

      // std::chrono::high_resolution_clock::time_point time_plotvars(std::chrono::high_resolution_clock::now());      
      // double duration_plotvars(std::chrono::duration_cast<std::chrono::microseconds>(time_plotvars - time_correlation).count());
      // sdebug << "duration_plotvars = " << duration_plotvars*1e-3 << endmsg;
      
      ++p;
    }
    p.Finish();
    
    doocore::lutils::printPlotCloseStack(&canvas, "AllPlots", config_toystudy_.plot_directory());
    sinfo.Ruler();

    sinfo << "Quantile-based estimates for symmetric 68.27% CLs:" << endmsg;
    for(auto i : parameter_cls){
      sinfo << i.first << " : Mean = " << std::get<0>(i.second) << ", Median = " << std::get<1>(i.second) <<  ", Quantiles = (" << std::get<2>(i.second) << ", " << std::get<3>(i.second) << ")" << endmsg;
      doocore::statistics::general::ValueWithError<double> mean_with_errors(std::get<0>(i.second), (std::fabs(std::get<0>(i.second)-std::get<2>(i.second))+std::fabs(std::get<0>(i.second)-std::get<3>(i.second)))/2., std::fabs(std::get<0>(i.second)-std::get<2>(i.second)), std::fabs(std::get<0>(i.second)-std::get<3>(i.second)));
      doocore::statistics::general::ValueWithError<double> median_with_errors(std::get<1>(i.second), (std::fabs(std::get<1>(i.second)-std::get<2>(i.second))+std::fabs(std::get<1>(i.second)-std::get<3>(i.second)))/2., std::fabs(std::get<1>(i.second)-std::get<2>(i.second)), std::fabs(std::get<1>(i.second)-std::get<3>(i.second)));
      sinfo.increment_indent(5);
      mean_with_errors.set_full_precision_printout(false);
      median_with_errors.set_full_precision_printout(false);
      sinfo << "Derived uncertainties on mean (median): " << mean_with_errors << ", (" << median_with_errors << ")" << endmsg;
      mean_with_errors.set_full_precision_printout(true);
      median_with_errors.set_full_precision_printout(true);
      sinfo << "Derived uncertainties on mean (median): " << mean_with_errors << ", (" << median_with_errors << ")" << endmsg;
      sinfo.increment_indent(-5);
    }
  }
    
  RooArgSet ToyStudyStd::BuildEvaluationArgSet(FitResultContainer fit_results) {
    RooArgSet parameters;
    
    int seed                        = std::get<6>(fit_results);
    int run_id                      = std::get<7>(fit_results);
    
    int run_id_reference            = config_toystudy_.reference_toys_id();
    bool evaluate_reference_toys    = config_toystudy_.evaluate_reference_toys();

    // sdebug << "run_id = " << run_id << endmsg;

    int num_toys_with_seed = fit_results_by_seed_.count(seed);
    const RooFitResult* fit_result_ptr = nullptr;
    const RooFitResult* fit_result_ptr_reference = nullptr;
    if (evaluate_reference_toys) {
      //sdebug << "Seed #" << seed << " is found " << fit_results_by_seed_.count(seed) << " times." << endmsg;
      
      if (num_toys_with_seed > 2) {
        serr << "Seed #" << seed << " is found " << num_toys_with_seed << " times." << endmsg;
        return parameters;
      } else if (num_toys_with_seed == 1) {
        serr << "Seed #" << seed << " is found only once although reference analysis is requested." << endmsg;
        return parameters;
      } else {
        if (run_id == run_id_reference) {
          fit_result_ptr = std::get<0>(fit_results);

          // sdebug << "Trying to find partner fit result!" << endmsg;

          // pick the correct fit result with the proper id
          auto it_this_seed = fit_results_by_seed_.lower_bound(seed);
          auto it_next_seed = fit_results_by_seed_.upper_bound(seed);

          for (auto it = it_this_seed; it != it_next_seed; ++it) {
            FitResultContainer fit_results_test = it->second;
            int run_id_test = std::get<7>(fit_results_test);
            if (run_id_test != run_id) {
              fit_result_ptr_reference = std::get<0>(fit_results_test);
            }
          }

        } // else {
          // // pick the correct fit result with the proper id
          // auto it_this_seed = fit_results_by_seed_.lower_bound(seed);
          // auto it_next_seed = fit_results_by_seed_.upper_bound(seed);

          // for (auto it = it_this_seed; it != it_next_seed; ++it) {
          //   sdebug << it->first << endmsg;

          //   FitResultContainer fit_results_test = it->second;
          //   int run_id_test = std::get<7>(fit_results_test);
          //   if (run_id_test == run_id_reference) {
          //     fit_result_ptr = std::get<0>(fit_results_test);
          //   }
          // }
        // }
      } 

      if (fit_result_ptr == nullptr) {
        return parameters;
      } else if (fit_result_ptr_reference == nullptr) {
        swarn << "Cannot find reference fit for fit with seed " << seed << endmsg;
        return parameters;
      }
    } else {
      fit_result_ptr = std::get<0>(fit_results);
    }    

    if (fit_result_ptr_reference != nullptr) {
      // sdebug << "Found company!" << endmsg;
    }

    const RooFitResult& fit_result = *fit_result_ptr;

    RooArgSet parameter_init_list    = fit_result.floatParsInit();
    if (config_toystudy_.parameter_genvalue_read_file().size() > 0) {
      parameter_init_list.readFromFile(config_toystudy_.parameter_genvalue_read_file().c_str());
    }


    RooRealVar* parameters_at_limit = new RooRealVar("parameters_at_limit","number of parameters close to limits", 0.0, 0.0, 100.0);
    RooRealVar* minos_problems      = new RooRealVar("minos_problems","number of parameters with MINOS problems", 0.0, 0.0, 100.0);
    
    RooRealVar* time_cpu            = new RooRealVar("time_cpu", "Fit time (CPU)", 0.0, "s");
    RooRealVar* time_real           = new RooRealVar("time_real", "Fit time (Real)", 0.0, "s");

    *time_cpu  = std::get<2>(fit_results);
    *time_real = std::get<3>(fit_results);

    RooArgList list_parameters;
    list_parameters.addClone(fit_result.floatParsFinal());
    RooArgList list_parameters_init;
    list_parameters_init.addClone(fit_result.floatParsInit());
    RooArgList list_parameters_ref;
    if (fit_result_ptr_reference != nullptr) {
      list_parameters_ref.addClone(fit_result_ptr_reference->floatParsFinal());
    }

    const std::map<std::string, std::tuple<const RooFormulaVar*,const RooFormulaVar*, std::string>>& additional_variables(config_toystudy_.additional_variables());

    if (additional_variables.size() > 0) {
      for (auto add_var_container : additional_variables) {
        const RooFormulaVar* add_var(std::get<0>(add_var_container.second));
        const RooFormulaVar* add_var_err(std::get<1>(add_var_container.second));
        std::string title(std::get<2>(add_var_container.second));

        EvaluateFormula(list_parameters, *add_var);
        RooRealVar add_var_real(add_var->GetName(), title.c_str(), add_var->getVal());

        if (add_var_err != nullptr) {
          EvaluateFormula(list_parameters, *add_var_err);
          add_var_real.setError(add_var_err->getVal());
        } else {
          add_var_real.setError(1.0);
        }
        list_parameters.addClone(add_var_real);

        EvaluateFormula(list_parameters_init, *add_var);
        RooRealVar add_var_real_init(add_var->GetName(), add_var->GetName(), add_var->getVal());
        list_parameters_init.addClone(add_var_real_init);

        if (list_parameters_ref.getSize() > 0) {
          EvaluateFormula(list_parameters_ref, *add_var);

          // sdebug << list_parameters_ref << endmsg;
          // sdebug << "add_var->getVal() = " << add_var->getVal() << endmsg;

          RooRealVar add_var_real_ref(add_var->GetName(), add_var->GetName(), add_var->getVal());
          list_parameters_ref.addClone(add_var_real_ref);
        }
      }
    }

    TIterator* parameter_iter        = list_parameters.createIterator();
    RooRealVar* parameter            = NULL;
    bool problems                    = false;
    RooArgSet list_fit_biases;
    TIterator* fit_biases_iter       = list_parameters.createIterator();
    RooRealVar par;
    while ((parameter = (RooRealVar*)fit_biases_iter->Next())) {
      par = CopyRooRealVar(*parameter);
      par.removeRange();
      par.setVal(0);
      list_fit_biases.addClone(par);
    }
    if (config_toystudy_.parameter_fitbias_read_file().size() > 0) {
      list_fit_biases.readFromFile(config_toystudy_.parameter_fitbias_read_file().c_str());
    }
    
    while ((parameter = (RooRealVar*)parameter_iter->Next())) {
      TString pull_name = parameter->GetName() + TString("_pull");
      TString pull_desc = TString("Pull of ") + parameter->GetTitle();
      TString init_name = parameter->GetName() + TString("_init");
      TString init_desc = TString("Init value of ") + parameter->GetTitle();
      TString res_name  = parameter->GetName() + TString("_res");
      TString res_desc  = TString("Residual of ") + parameter->GetTitle();
      TString err_name  = parameter->GetName() + TString("_err");
      TString err_desc  = TString("Error of ") + parameter->GetTitle();
      TString lerr_name = parameter->GetName() + TString("_lerr");
      TString lerr_desc = TString("Low error of ") + parameter->GetTitle();
      TString herr_name = parameter->GetName() + TString("_herr");
      TString herr_desc = TString("High error of ") + parameter->GetTitle();
      TString fit_bias_name = parameter->GetName() + TString("_fit_bias");
      TString fit_bias_desc = TString("Fit bias of ") + parameter->GetTitle();
      
      //double par_error = ((RooRealVar*)parameter_list.find(parameter->GetName()))->getError();
      
      RooRealVar& par  = CopyRooRealVar(*parameter);
      RooRealVar* pull = new RooRealVar(pull_name, pull_desc, 0.0);
      RooRealVar* res  = new RooRealVar(res_name, res_desc, 0.0);
      RooRealVar* err  = new RooRealVar(err_name, err_desc, 0.0);
      RooRealVar* lerr = new RooRealVar(lerr_name, lerr_desc, 0.0);
      RooRealVar* herr = new RooRealVar(herr_name, herr_desc, 0.0);
      RooRealVar& init = CopyRooRealVar(*(RooRealVar*)list_parameters_init.find(par.GetName()), std::string(init_name), std::string(init_desc));
      RooRealVar& fit_bias = CopyRooRealVar(*(RooRealVar*)list_fit_biases.find(par.GetName()), std::string(fit_bias_name), std::string(fit_bias_desc));

      std::string ref_residual_name = std::string(parameter->GetName()) + std::string("_refresidual");
      std::string ref_residual_desc = std::string("#mu for ") + parameter->GetTitle();
      std::string ref_name = std::string(parameter->GetName()) + std::string("_reference");
      std::string ref_desc = std::string("Reference value for ") + parameter->GetTitle();

      RooRealVar* ref_residual  = nullptr;
      RooRealVar* par_reference = nullptr;
      if (list_parameters_ref.getSize() > 0) {
        ref_residual = new RooRealVar(ref_residual_name.c_str(), ref_residual_desc.c_str(), 0.0);

        //const RooArgList& parameter_list_ref = fit_result_ptr_reference->floatParsFinal();
        RooRealVar*       par_reference_orig = dynamic_cast<RooRealVar*>(list_parameters_ref.find(par.GetName()));

        if (par_reference_orig != nullptr) {
          par_reference = &CopyRooRealVar(*par_reference_orig, ref_name, ref_desc);
        }
      }

      
      double pull_value = 0.0;
      double err_value  = par.getError();
      double lerr_value = 0.0;
      double herr_value = 0.0;

      double ref_residual_value(0.0);
      if (list_parameters_ref.getSize() > 0 && par_reference != nullptr) {
        ref_residual_value = par.getVal() - par_reference->getVal() - fit_bias.getVal();
        ref_residual->setVal(ref_residual_value);
      }

      
      // Log 20131121 (FK): Changing definitions of pulls after a discussion with
      //                    Julian. We agree that a positive pull/residual should
      //                    reflect a parameter being overestimated. The previous
      //                    definition was in accordance with the CDF pull paper.

      std::string name_parameter(parameter->GetName());

      // asymmetric error handling
      if (par.hasAsymError() && config_toystudy_.handle_asymmetric_errors()) {
        lerr_value = par.getErrorLo();
        herr_value = par.getErrorHi();

        // if (name_parameter == "par_bdsig_time_C") {
        //   sdebug << "lerr_value = " << lerr_value << " -- herr_value = " << herr_value << endmsg;
        // }

        lerr->setVal(lerr_value);
        herr->setVal(herr_value);
        
        if (par.getVal() <= init.getVal()) {
          pull_value = -(fit_bias.getVal() + init.getVal() - par.getVal())/herr_value;
        } else {
          pull_value = -(par.getVal() - init.getVal() - fit_bias.getVal())/lerr_value;
        }
      } else {
        // if (name_parameter == "par_bdsig_time_C") {
        //   sdebug << "parameter par_bdsig_time_C ain't got no asymmetric errors." << endmsg;
        // }
        pull_value = -(init.getVal() - par.getVal() - fit_bias.getVal())/err_value;
      }
      pull->setVal(pull_value);
      
      double res_value = -(fit_bias.getVal() + init.getVal() - par.getVal());
      res->setVal(res_value);
      
      err->setVal(err_value);

      // if (name_parameter == "par_bssig_time_S") {
      //   if (herr_value > 0.6) {
      //     swarn << "Upper error of par_bssig_time_S is >0.6" << endmsg;
      //     problems = true;
      //   }
      // }

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
      TMath::Min(TMath::Abs((par.getVal() - par.getMax())*par.getError()/
                            ((par.getMax() - par.getMin())/2)),
                 TMath::Abs((par.getVal() - par.getMin())*par.getError()/
                            ((par.getMax() - par.getMin())/2)));
      
      if (min_distance_to_limits < 1e-5 && !(par.getVal() == 0.0 && par.getError() == 0.0)) {
        swarn << "Parameter distance limit low for: " << par << endmsg;
        problems = true;
        parameters_at_limit->setVal(parameters_at_limit->getVal() + 1.0);
      }
      
      for(int i = 0; i < config_toystudy_.minos_parameters().size(); i++){
        if(parameter->GetName() == config_toystudy_.minos_parameters().at(i) && !par.hasAsymError()){
          swarn << "MINOS could not determine asymmetric errors for: " << par << endmsg;
          minos_problems->setVal(minos_problems->getVal() + 1.0);
        }
      }
      
      // if (par.hasAsymError() && (par.getAsymErrorHi() == 0.0 || par.getAsymErrorLo() == 0.0)) {
      //   swarn << "MINOS could not determine asymmetric errors for: " << par << endmsg;
      //   problems = true;
      //   minos_problems->setVal(minos_problems->getVal() + 1.0);
      // }
        
      parameters.addOwned(par);
      parameters.addOwned(*pull);
      parameters.addOwned(*res);
      parameters.addOwned(init);
      parameters.addOwned(*err);
      
      if (par.hasAsymError()) {
        parameters.addOwned(*lerr);
        parameters.addOwned(*herr);
      }

      if (fit_result_ptr_reference != nullptr && par_reference != nullptr) {
        parameters.addOwned(*ref_residual);
        parameters.addOwned(*par_reference);
      }
        
    }
    if (problems) {
      fit_result.Print("v");
    }
    
    parameters.addOwned(*parameters_at_limit);
    parameters.addOwned(*minos_problems);

    parameters.addOwned(*time_cpu);
    parameters.addOwned(*time_real);
    
    delete parameter_iter;
    delete fit_biases_iter;
    return parameters;
  }
  
  bool FitResultOkay(const RooFitResult& fit_result, int min_acceptable_cov_matrix_quality) {    
    int max_status_code = 0;
    std::map<std::string, int> status_codes;
    for (unsigned int i = 0; i < fit_result.numStatusHistory(); i++){
      status_codes.insert(std::pair<std::string, int>(fit_result.statusLabelHistory(i), fit_result.statusCodeHistory(i)));
      if (fit_result.statusCodeHistory(i) > max_status_code) max_status_code = fit_result.statusCodeHistory(i);
    }
    
    if (fit_result.covQual() < min_acceptable_cov_matrix_quality && fit_result.covQual() != -1) {
      swarn << "Fit result has insufficient covariance matrix quality." << endmsg;
      return false;
    } else if (fit_result.statusCodeHistory(0) < 0) {
      swarn << "Fit result has negative status code." << endmsg;
      return false;
    // } else if (max_status_code > 0) {
    //   swarn << "Fit result has max status code " << max_status_code << endmsg;
    //   return false;
    } else if (FitResultNotVariedParameterSet(fit_result)) {
      swarn << "Fit result has more than 80% of nonvaried parameters." << endmsg;
      //fit_result.Print("v");
      return false;
    // } else if (FitResultNoAsymmetricErrors(fit_result)) {
    //   return false;
    } else {
      return true;
    }
  }


  bool ToyStudyStd::FitResultOkay(const RooFitResult& fit_result) const {    
    return doofit::toy::FitResultOkay(fit_result, config_toystudy_.min_acceptable_cov_matrix_quality());
  }
  
  bool FitResultNoAsymmetricErrors(const RooFitResult& fit_result) {
    //const RooArgSet& parameter_init_list = fit_result.floatParsInit();
    const RooArgList& parameter_list     = fit_result.floatParsFinal();
    
    TIterator* parameter_iter        = parameter_list.createIterator();
    RooRealVar* parameter            = NULL;

    bool all_asymm = true;
    while ((parameter = (RooRealVar*)parameter_iter->Next())) {
      if (!parameter->isConstant()) {
        all_asymm &= parameter->hasAsymError();
      }
    }
    return !all_asymm;
  }

  bool FitResultNotVariedParameterSet(const RooFitResult& fit_result) {
    const RooArgSet& parameter_init_list = fit_result.floatParsInit();
    const RooArgList& parameter_list     = fit_result.floatParsFinal();
    
    TIterator* parameter_iter        = parameter_list.createIterator();
    RooRealVar* parameter            = NULL;
    int num_nonvaried = 0;
    int num_nonfixed  = 0;
    
    while ((parameter = (RooRealVar*)parameter_iter->Next())) {
      if (!parameter->isConstant()) {
        if (TMath::Abs(parameter->getVal()-dynamic_cast<RooRealVar*>(parameter_init_list.find(parameter->GetName()))->getVal())/parameter->getError() < 1e-11) {
          num_nonvaried++;
        }
        // else{
        //   double par_init_value = dynamic_cast<RooRealVar*>(parameter_init_list.find(parameter->GetName()))->getVal();
        //   double par_fit_value  = parameter->getVal();
        //   double par_fit_error  = parameter->getError();
        //   double diff           = std::fabs(par_init_value - par_fit_value)/par_fit_error;

        //   std::string paramname = parameter->GetName();
        //   if ( paramname == "parSigTimeSin2b" && std::fabs(0.8 - par_fit_value) < 0.003 ){
        //     return true;
        //     if (par_fit_error > 0.1) return false;
        //     if (par_fit_error < 0.01) return false;
        //     sinfo << "par_init_value = " << par_init_value << ", par_fit_value = " << par_fit_value << ", par_fit_error = " << par_fit_error << endmsg;
        //     sinfo << "(par_init_value - par_fit_value)/par_fit_error = " << diff << endmsg;
        //     fit_result.Print();
        //   }
        // }
        num_nonfixed++;
      }
    }
    
    double rate_nonvaried = static_cast<double>(num_nonvaried)/num_nonfixed;
    if (rate_nonvaried >= 0.8) {
      return true;
    } else {
      return false;
    }
  }
  
  void ToyStudyStd::EvaluateFormula(const RooAbsCollection& args, const RooFormulaVar& formula) const {
    unsigned int i(0);
    RooAbsArg* arg(formula.getParameter(i));

    while (arg != nullptr) {
      RooRealVar* var(dynamic_cast<RooRealVar*>(arg));
      if (var != nullptr) {
        std::string name_var(var->GetName());
        if (name_var.substr(name_var.length() - 4) == "_err") {
          name_var = name_var.substr(0, name_var.length() - 4);
          RooRealVar* var_result(dynamic_cast<RooRealVar*>(args.find(name_var.c_str())));
          if (var_result != nullptr) {
            var->setVal(var_result->getError());

            // sdebug << " " << var->GetName() << " = error of " << var_result->GetName() << " = " << var_result->getError() << endmsg;
          }
        } else {
          RooRealVar* var_result(dynamic_cast<RooRealVar*>(args.find(name_var.c_str())));
          if (var_result != nullptr) {
            var->setVal(var_result->getVal());

            // sdebug << " " << var->GetName() << " = " << var_result->getVal() << endmsg;
          }
        }
      }

      ++i;
      arg = formula.getParameter(i);
    }

    // sdebug << formula.GetName() << " = " << formula.getVal() << endmsg;
  }

  RooRealVar& ToyStudyStd::CopyRooRealVar(const RooRealVar& other, const std::string& new_name, const std::string& new_title) const {
    RooRealVar* new_var = new RooRealVar(new_name.length()>0 ? new_name.c_str() : other.GetName(),
                                         new_title.length()>0 ? new_title.c_str() : other.GetTitle(),
                                         other.getVal());
    
    if (other.hasMin()) {
      new_var->setMin(other.getMin());
    }
    if (other.hasMax()) {
      new_var->setMax(other.getMax());
    }
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
  
  void ToyStudyStd::ReadFitResultWorker() {
    //TThread this_tthread;
    
    const std::vector<doofit::config::CommaSeparatedPair<std::string>>& results_files = config_toystudy_.read_results_filename_treename();
    
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
    // std::vector<RooFitResult*> vector_debug;
    
    for (std::vector<doofit::config::CommaSeparatedPair<std::string>>::const_iterator it_files = results_files.begin(); it_files != results_files.end(); ++it_files) {
      sinfo << "Loading fit results from " << (*it_files).first() 
      << " from branch " << config_toystudy_.fit_result1_branch_name() << endmsg;
      TFile file((*it_files).first().c_str(), "read");
      if (file.IsZombie() || !file.IsOpen()) {
        serr << "Cannot open file " << (*it_files).first() << " which may be not existing or corrupted. Ignoring this file." << endmsg;
        //throw ExceptionCannotReadFitResult();
      } else {
        
        TTree* tree = (TTree*)file.Get((*it_files).second().c_str());
        if (tree == NULL) {
          serr << "Cannot find tree " << (*it_files).second() << " in file. Cannot read in fit results. Ignoring this file." << endmsg;
          //throw ExceptionCannotReadFitResult();
        } else { // if (tree == NULL) {
          
          TBranch* result_branch = tree->GetBranch(config_toystudy_.fit_result1_branch_name().c_str());
          TBranch* result2_branch = tree->GetBranch(config_toystudy_.fit_result2_branch_name().c_str());
          
          if (result_branch == NULL) {
            // TODO: Check for EasyFitResult container
            // results_files_easyfit_

            TBranch* branch_easyfit_probe = tree->GetBranch("fr0_fcn");

            if (branch_easyfit_probe != nullptr) {
              sinfo << "Tree " << it_files->first() << ":" << it_files->second() << " is an EasyFitResult container. Will process separately." << endmsg;
              results_files_easyfit_.push_back(*it_files);
            } else {
              serr << "Cannot find branch " << config_toystudy_.fit_result1_branch_name() << " in tree. Cannot read in fit results." << endmsg;
              //throw ExceptionCannotReadFitResult();
            }
          } else { // if (result_branch == NULL) {
            // Fit times
            TBranch* time_cpu1_branch  = tree->GetBranch("time_cpu1");
            TBranch* time_real1_branch = tree->GetBranch("time_real1");
            TBranch* time_cpu2_branch  = tree->GetBranch("time_cpu2");
            TBranch* time_real2_branch = tree->GetBranch("time_real2");
            TBranch* seed_branch       = tree->GetBranch("seed");
            TBranch* run_id_branch     = tree->GetBranch("run_id");
            
            RooFitResult* fit_result  = NULL;
            RooFitResult* fit_result2 = NULL;
            double time_cpu1 = 0.0, time_real1 = 0.0;
            double time_cpu2 = 0.0, time_real2 = 0.0;
            int seed(0);
            int run_id(0);

            tree->SetCacheEntryRange(0,tree->GetEntries());
            
            tree->AddBranchToCache(result_branch, true);
            result_branch->SetAddress(&fit_result);
            
            if (result2_branch != NULL) {
              tree->AddBranchToCache(result2_branch, true);
              result2_branch->SetAddress(&fit_result2);
            }
            if (time_cpu1_branch != NULL) {
              tree->AddBranchToCache(time_cpu1_branch, true);
              time_cpu1_branch->SetAddress(&time_cpu1);
            }
            if (time_real1_branch != NULL) {
              tree->AddBranchToCache(time_real1_branch, true);
              time_real1_branch->SetAddress(&time_real1);
            }
            if (time_cpu2_branch != NULL) {
              tree->AddBranchToCache(time_cpu2_branch, true);
              time_cpu2_branch->SetAddress(&time_cpu2);
            }
            if (time_real2_branch != NULL) {
              tree->AddBranchToCache(time_real2_branch, true);
              time_real2_branch->SetAddress(&time_real2);
            }
            if (seed_branch != NULL) {
              tree->AddBranchToCache(seed_branch, true);
              seed_branch->SetAddress(&seed);
            }
            if (run_id_branch != NULL) {
              tree->AddBranchToCache(run_id_branch, true);
              run_id_branch->SetAddress(&run_id);
            }
            tree->StopCacheLearningPhase();
            
            using namespace doocore::io;
            std::string title_progress = "Reading fit results from " + (*it_files).first() + ":" + (*it_files).second();
            Progress p(title_progress, tree->GetEntries());
            for (int i=0; i<tree->GetEntries(); ++i) {
              tree->GetEntry(i);
              
              // save a copy
              if (fit_result != NULL && FitResultOkay(*fit_result)) {
                fit_results_read_queue_.push(std::make_tuple(fit_result,
                                                             fit_result2,
                                                             time_cpu1,
                                                             time_real1,
                                                             time_cpu2,
                                                             time_real2,
                                                             seed,
                                                             run_id));
                results_stored++;
              } else {
                if (fit_result == NULL) {
                  serr << "Fit result number " << i << " in file " << *it_files << " is NULL and therefore neglected. This indicates corrupted files and should never happen." << endmsg;
                  while (true) {}
                } else {
                  delete fit_result;
                  if (fit_result2 != NULL) {
                    delete fit_result2;
                  }
                  fit_result = nullptr;
                  fit_result2 = nullptr;
                  
                  swarn << "Fit result number " << i << " in file " << *it_files << " neglected." << endmsg;
                }
                results_neglected++;
              }
              fit_result = nullptr;
              fit_result2 = nullptr;

              PurgeReleasedFitResults();

              if (config_toystudy_.num_toys_read() > 0 && results_stored >= config_toystudy_.num_toys_read()) {
                break;
              }

              ++p;
            }
            p.Finish();

            delete tree;
            file.Close();

            if (config_toystudy_.num_toys_read() > 0 && results_stored >= config_toystudy_.num_toys_read()) {
              //sinfo << "Read in " << results_stored << " toys as requested. Finishing." << endmsg;
              break;
            }
          } // if (result_branch == NULL) {
        } // if (tree == NULL) {
      } // if (file.IsZombie() || !file.IsOpen()) {
    } // for (std::vector<doofit::config::CommaSeparatedPair<std::string>>::const_iterator it_files = results_files.begin(); it_files != results_files.end(); ++it_files) {
    fit_results_read_queue_.disable_queue();
    sinfo << "Read in " << results_stored << " fit results. (" << results_neglected << " results negelected, that is " << static_cast<double>(results_neglected)/static_cast<double>(results_stored+results_neglected)*100.0 << "%)" << endmsg;
    sinfo.Ruler();

    // if (debug_) {
    //   for (auto& fr : vector_debug) {
    //     delete fr;
    //   }
    // }
    
    reading_fit_results_ = false;
  }
  
  void ToyStudyStd::SaveFitResultWorker() {
    TThread this_tthread;
    TStopwatch sw_lock;
    sw_lock.Reset();
    std::queue<FitResultContainer > saver_queue;

    // list of last 5 dead times for averaging to find a new flexible wait time
    std::list<double> deadtimes;
    deadtimes.push_back(10.0);
    
    while (accepting_fit_results_ || !fit_results_save_queue_.empty() || !saver_queue.empty()) { 
      
      //sdebug << "SaveFitResultWorker(): starting loop with " << saver_queue.size() << " elements in our own queue." << endmsg;
      
      const std::string& filename = config_toystudy_.store_result_filename_treename().first();
      const std::string& treename = config_toystudy_.store_result_filename_treename().second();
      unsigned int save_counter = 1;
      
      // if our own queue is empty, we need to wait for new fit results to come 
      // in anyway.
      if (saver_queue.empty()) {
        //sdebug << "SaveFitResultWorker(): waiting for fit results as we have none." << endmsg;
        
        FitResultContainer fit_results;
        if (fit_results_save_queue_.wait_and_pop(fit_results)) {
          saver_queue.push(fit_results);
          //sdebug << "SaveFitResultWorker(): got a fit result pair." << endmsg;
        } else {
          //sdebug << "SaveFitResultWorker(): got no fit result pair." << endmsg;
        }
      }
      
      // if global queue has entries, we should get them all
      while (!fit_results_save_queue_.empty()) {
        FitResultContainer fit_results;
        if (fit_results_save_queue_.wait_and_pop(fit_results)) {
          saver_queue.push(fit_results);
          //sdebug << "SaveFitResultWorker(): got another fit result pair." << endmsg;
        } else {
          //sdebug << "SaveFitResultWorker(): got no other fit result pair." << endmsg;
        }
        //sdebug << "queue size: " << fit_results_save_queue_.size() << endmsg;   
      }
      
      bool sleep = false;
      int wait_time;
      if (!saver_queue.empty()) {
        sw_lock.Stop();
        boost::mutex::scoped_lock fitresult_save_worker_local_lock(fitresult_save_worker_mutex_);
        
        doocore::system::FileLock flock(filename);
        boost::random::random_device rnd;
        
        sw_lock.Start(false);
        if (!flock.Lock()) {
          // get new wait time based on last deadtimes's average
          double new_wait_time = 0.0;
          for (std::list<double>::const_iterator it = deadtimes.begin();
               it != deadtimes.end(); ++it) {
            new_wait_time += *it;
          }
          new_wait_time += sw_lock.RealTime();
          sw_lock.Start(false);
          new_wait_time /= deadtimes.size()+1;
          
          wait_time = (static_cast<double>(rnd())/static_cast<double>(rnd.max()-rnd.min())-rnd.min())*new_wait_time+1.0;
	        wait_time = std::min(wait_time, 180);
          swarn << "File to save fit result to " << filename << " is locked. Will try again in " << wait_time << " s." << endmsg;
          sleep = true;
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
                //sdebug << "SaveFitResultWorker(): number of results in queue: " << saver_queue.size() << endmsg;
                FitResultContainer fit_results = saver_queue.front();
                saver_queue.pop();
                const RooFitResult* fit_result1 = std::get<0>(fit_results);
                const RooFitResult* fit_result2 = std::get<1>(fit_results);
                double time_cpu1 = std::get<2>(fit_results);
                double time_real1 = std::get<3>(fit_results);
                double time_cpu2 = std::get<4>(fit_results);
                double time_real2 = std::get<5>(fit_results);
                int seed(std::get<6>(fit_results));
                int run_id(std::get<7>(fit_results));
                
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
                  tree_results->Branch("time_cpu1",  &time_cpu1,  "time_cpu1/D");
                  tree_results->Branch("time_real1", &time_real1, "time_real1/D");
                  tree_results->Branch("time_cpu2",  &time_cpu2,  "time_cpu2/D");
                  tree_results->Branch("time_real2", &time_real2, "time_real2/D");
                  tree_results->Branch("seed", &seed, "seed/I");
                  tree_results->Branch("run_id", &run_id, "run_id/I");
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
                  
                  if (tree_results->GetBranch("time_cpu1") != NULL &&
                      tree_results->GetBranch("time_real1") != NULL &&
                      tree_results->GetBranch("time_cpu2") != NULL &&
                      tree_results->GetBranch("time_real2") != NULL) {
                    tree_results->SetBranchAddress("time_cpu1", &time_cpu1);
                    tree_results->SetBranchAddress("time_real1", &time_real1);
                    tree_results->SetBranchAddress("time_cpu2", &time_cpu2);
                    tree_results->SetBranchAddress("time_real2", &time_real2);
                  }

                  if (tree_results->GetBranch("seed") != NULL) {
                    tree_results->SetBranchAddress("seed", &seed);
                  }
                  if (tree_results->GetBranch("run_id") != NULL) {
                    tree_results->SetBranchAddress("run_id", &run_id);
                  }
                }
                     
                tree_results->Fill();
                
                delete fit_result1;
                if (fit_result2 != NULL) { 
                  delete fit_result2;
                }
                
                while (!saver_queue.empty()) {
                  fit_results = saver_queue.front();
                  saver_queue.pop();
                  fit_result1 = std::get<0>(fit_results);
                  fit_result2 = std::get<1>(fit_results);
                  time_cpu1   = std::get<2>(fit_results);
                  time_real1  = std::get<3>(fit_results);
                  time_cpu2   = std::get<4>(fit_results);
                  time_real2  = std::get<5>(fit_results);
                  seed        = std::get<6>(fit_results);
                  run_id      = std::get<7>(fit_results);
                  
                  tree_results->Fill();
                  save_counter++;
                  delete fit_result1;
                  if (fit_result2 != NULL) {
                    delete fit_result2;
                  }
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
      boost::this_thread::yield();
      if (sleep) {
        doocore::lutils::Sleep(wait_time);
      }
      
      
      signal(SIGINT, SIG_DFL);
      signal(SIGTERM, SIG_DFL);
    }
  }
} // namespace toy
} // namespace doofit
