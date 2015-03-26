#include "FeldmanCousinsProfiler.h"

// from STL
#include <set>

// from ROOT
#include "TCanvas.h"
#include "TGraph.h"
#include "TGraphAsymmErrors.h"
#include "TH1D.h" 
#include "TH2D.h" 
#include "TH1F.h" 
#include "TAxis.h"
#include "TStyle.h"
#include "TColor.h"
#include "TLine.h"
#include "TMath.h"
#include "TEfficiency.h"
#include "TLatex.h"
#include "TDirectory.h"

// from RooFit
#include "RooFitResult.h"

// from DooCore
#include <doocore/io/MsgStream.h>
#include <doocore/io/Progress.h>
#include <doocore/lutils/lutils.h>
#include <doocore/io/Tools.h>
#include <doocore/statistics/general.h>

// from DooFit
#include "doofit/plotting/Plot/PlotConfig.h"
#include "doofit/fitter/AbsFitter.h"
#include "doofit/toy/ToyStudyStd/ToyStudyStd.h"
#include "doofit/fitter/easyfit/EasyFitResult.h"

doofit::plotting::profiles::FeldmanCousinsProfiler::FeldmanCousinsProfiler(const PlotConfig& cfg_plot)
: config_plot_(cfg_plot),
  num_samples_(30),
  time_total_(0.0)
{}

void doofit::plotting::profiles::FeldmanCousinsProfiler::ReadFitResultDataNominal(doofit::toy::ToyStudyStd& toy_study) {
  using namespace doofit::toy;
  using namespace doocore::io;
  using namespace doofit::fitter::easyfit;

  FitResultContainer fit_result_container(toy_study.GetFitResult());
  const EasyFitResult fit_result(*std::get<0>(fit_result_container));

  time_total_ += std::get<2>(fit_result_container);

  for (auto var : scan_vars_) {
    //RooRealVar* var_fixed = dynamic_cast<RooRealVar*>(fit_result->floatParsFinal().find(var->GetName()));
    const EasyFitVariable* var_fixed = nullptr;
    if (fit_result.parameters_float_final().count(var->GetName()) > 0) {
      var_fixed = &fit_result.parameters_float_final().at(var->GetName());
    } else if (fit_result.parameters_const().count(var->GetName()) > 0) {
      var_fixed = &fit_result.parameters_const().at(var->GetName());
    }
    if (var_fixed != nullptr) {
      scan_vars_titles_.push_back(var->GetTitle());
      scan_vars_names_.push_back(var_fixed->name());

      // sdebug << "We take: " << var_fixed->GetTitle() << endmsg;
      // sdebug << "while we should take: " << var->GetTitle() << endmsg;
    } else {
      serr << "Cannot get scan parameter " << var->GetName() << " from nominal data fit result!" << endmsg;
      throw;
    }
  }

  nll_data_nominal_ = fit_result.fcn();
  // sdebug << "nll_data_nominal_ = " << nll_data_nominal_ << endmsg;

  toy_study.ReleaseFitResult(fit_result_container);
}

void doofit::plotting::profiles::FeldmanCousinsProfiler::ReadFitResultsDataScan(doofit::toy::ToyStudyStd& toy_study) {
  using namespace doofit::toy;
  using namespace doocore::io;
  using namespace doofit::fitter::easyfit;

  FitResultContainer fit_result_container(toy_study.GetFitResult());
  //const RooFitResult* fit_result(std::get<0>(fit_result_container));

  unsigned int num_ignored(0);
  const EasyFitResult* fit_result = nullptr;
  while (std::get<0>(fit_result_container) != nullptr) { 
    fit_result = new EasyFitResult(*std::get<0>(fit_result_container));
    time_total_ += std::get<2>(fit_result_container);

    if (FitResultOkay(*fit_result)) {
      std::vector<double> scan_vals;
      for (auto var : scan_vars_) {
        const EasyFitVariable& var_fixed = fit_result->parameters_const().at(var->GetName());
        //RooRealVar* var_fixed = dynamic_cast<RooRealVar*>(fit_result->constPars().find(var->GetName()));

        // protection against 0.0 being 1e-16 and not being properly matched
        double val(var_fixed.value());
        if (std::abs(val) < 1e-14) {
          val = 0.0;
        } 

        scan_vals.push_back(val);
      }

      // if (std::abs(scan_vals[0]-0.745732) < 1e-3) {
      //   sdebug << "@" << scan_vals[0] << ":" << fit_result->minNll() << "-" << nll_data_nominal_ << " = " << fit_result->minNll()-nll_data_nominal_ << endmsg;
      // }

      delta_nlls_data_scan_[scan_vals] = fit_result->fcn()-nll_data_nominal_;
      scan_vals_data_.insert(scan_vals);
      // sdebug << "scan_vals = " << scan_vals << endmsg;
      // sdebug << "delta_nll = " << delta_nlls_data_scan_[scan_vals] << endmsg;

      fit_results_data_scan_.emplace(std::make_pair(scan_vals,*fit_result));
    } else {
      ++num_ignored;
    }

    toy_study.ReleaseFitResult(fit_result_container);

    fit_result_container = toy_study.GetFitResult();
    delete fit_result;
    if (std::get<0>(fit_result_container) != nullptr) {
      fit_result = new EasyFitResult(*std::get<0>(fit_result_container));
    }
  }

  if (num_ignored > 0) {
    swarn << "Ignored " << num_ignored << " data scan results due to bad fit quality." << endmsg;
  }

  sinfo << "Available data scan points: " << scan_vals_data_ << endmsg;
}

int doofit::plotting::profiles::FeldmanCousinsProfiler::ProcessToyFitResult(const doofit::fitter::easyfit::EasyFitResult& fr0, const doofit::fitter::easyfit::EasyFitResult& fr1) {
  using namespace doofit::fitter::easyfit;
  using namespace doocore::io;

  int num_ignored=0;

  if (FitResultOkay(fr0) && FitResultOkay(fr1)) {
    std::vector<double> scan_vals;
    for (auto var : scan_vars_) {
      const EasyFitVariable& var_fixed = fr1.parameters_const().at(var->GetName());

      // protection against 0.0 being 1e-16 and not being properly matched
      double val(var_fixed.value());
      if (std::abs(val) < 1e-14) {
        val = 0.0;
      } 

      scan_vals.push_back(val);
    }
    double delta_nll(fr1.fcn() - fr0.fcn());

    // TODO:
    // hist.Fill(delta_nll);
    // if (scan_vals.size() == 1) {
    //   if (hists_dchisq.count(scan_vals.front()) > 0) {
    //     hists_dchisq[scan_vals.front()]->Fill(delta_nll);
    //   } else {
    //     std::string name_hist = "hist" + std::to_string(scan_vals.front());
    //     std::string title_hist = "#Delta#chi^{2}_{toy} @" + std::to_string(scan_vals.front());
    //     hists_dchisq[scan_vals.front()] = new TH1D(name_hist.c_str(), title_hist.c_str(), 250, -2.0, 6.0);
    //     hists_dchisq[scan_vals.front()]->Fill(delta_nll);
    //   }
    // }

    if (delta_nll < 0.0) {
      if (num_neglected_fits_.count(scan_vals) > 0) {
        num_neglected_fits_[scan_vals]++;
      } else {
        num_neglected_fits_[scan_vals] = 1;
      }

      ++num_ignored;
    } else {
      if (delta_nlls_toy_scan_.count(scan_vals) > 0) {
        delta_nlls_toy_scan_[scan_vals].push_back(delta_nll);
      } else {
        std::vector<double> vector;
        vector.push_back(delta_nll);
        delta_nlls_toy_scan_[scan_vals] = vector;
      }
      scan_vals_toy_.insert(scan_vals);
    }
  } else {
    ++num_ignored;
  }
  return num_ignored;
}

void doofit::plotting::profiles::FeldmanCousinsProfiler::ReadFitResultsToy(doofit::toy::ToyStudyStd& toy_study) {
  using namespace doofit::toy;
  using namespace doocore::io;
  using namespace doofit::fitter::easyfit;

  unsigned int num_ignored(0);

  FitResultContainer fit_result_container(toy_study.GetFitResult());
  // const RooFitResult* fit_result_0(std::get<0>(fit_result_container));
  // const RooFitResult* fit_result_1(std::get<1>(fit_result_container));
  const EasyFitResult* fit_result_0 = nullptr;
  const EasyFitResult* fit_result_1 = nullptr;

  TCanvas c("c", "c", 800, 600);
  TH1D hist("hist", "#Delta#chi^{2}_{toy}", 250, -2.0, 6.0);

  std::map<double, TH1D*> hists_dchisq;

  unsigned long long num_roofitresults = 0;

  // TODO: Warn if fit_result_0 valid, but fit_result_1 not!
  while (std::get<0>(fit_result_container) != nullptr) { 
    if (std::get<1>(fit_result_container) == nullptr) {
      swarn << "Only one fit result stored, expected a pair. The result tree might be broken!" << endmsg;
    } else {
      fit_result_0 = new EasyFitResult(*std::get<0>(fit_result_container));
      fit_result_1 = new EasyFitResult(*std::get<1>(fit_result_container));

      time_total_ += std::get<2>(fit_result_container);
      time_total_ += std::get<3>(fit_result_container);

      num_ignored += ProcessToyFitResult(*fit_result_0, *fit_result_1);
      ++num_roofitresults;
    }
    toy_study.ReleaseFitResult(fit_result_container);

    fit_result_container = toy_study.GetFitResult();
    delete fit_result_0;
    delete fit_result_1;
    if (std::get<0>(fit_result_container) != nullptr && std::get<1>(fit_result_container) != nullptr) {
      fit_result_0 = new EasyFitResult(*std::get<0>(fit_result_container));
      fit_result_1 = new EasyFitResult(*std::get<1>(fit_result_container));
    }

    // sdebug << "fit_result_0 = " << fit_result_0 << endmsg;
    // sdebug << "fit_result_1 = " << fit_result_1 << endmsg;
  }
  sinfo << "Read in " << num_roofitresults << " RooFitResults." << endmsg;

  toy_study.PurgeReleasedFitResults();

  // Start processing of EasyFitResults
  // const EasyFitResult* fit_result_0 = nullptr;
  // const EasyFitResult* fit_result_1 = nullptr;
  unsigned long long num_easyfitresults = toy_study.NumberOfAvailableEasyFitResults();
  if (toy_study.NumberOfAvailableEasyFitResults() > 0) {
    Progress p("Reading EasyFitResults", toy_study.NumberOfAvailableEasyFitResults());
    EasyFitResultContainer efit_results(toy_study.GetEasyFitResult());
    const EasyFitResult* fr0(std::get<0>(efit_results));
    const EasyFitResult* fr1(std::get<1>(efit_results));
    ++p;

    num_ignored += ProcessToyFitResult(*fr0, *fr1);

    while (toy_study.NumberOfAvailableEasyFitResults() > 0) {
      efit_results = toy_study.GetEasyFitResult();

      fr0 = std::get<0>(efit_results);
      fr1 = std::get<1>(efit_results);

      num_ignored += ProcessToyFitResult(*fr0, *fr1);
      ++p;
    }
    p.Finish();
    sinfo << "Read in " << num_easyfitresults << " EasyFitResults." << endmsg;
  }

  hist.Draw();
  c.SaveAs("delta_chisq.pdf");

  for (auto hist : hists_dchisq) {
    hist.second->Draw();
    std::string name_hist = "delta_chisq_" + std::to_string(hist.first) + ".pdf";
    c.SaveAs(name_hist.c_str());
  }

  if (num_ignored > 0) {
    swarn << "Ignored " << num_ignored << " toy scan result pairs due to bad fit quality." << endmsg;
  }

  sinfo << "Scanned toy scan points: " << scan_vals_toy_ << endmsg;
}

const doofit::fitter::easyfit::EasyFitResult& doofit::plotting::profiles::FeldmanCousinsProfiler::GetDataScanResult(const std::vector<double>& scan_point) const {
  using namespace doofit::toy;
  using namespace doocore::io;
  using namespace doofit::fitter::easyfit;

  std::vector<double> scan_point_copy(scan_point);
  for (auto& value : scan_point_copy) {
    if (std::abs(value) < 1e-14) {
      value = 0.0;
    } 
  }

  if (fit_results_data_scan_.count(scan_point_copy) > 0) {
    const EasyFitResult& fit_result = fit_results_data_scan_.at(scan_point_copy);
    return fit_result;
  } 
}

void doofit::plotting::profiles::FeldmanCousinsProfiler::ReleaseAllFitResults(doofit::toy::ToyStudyStd& toy_study) {
  using namespace doofit::toy;
  using namespace doocore::io;

  // for (auto fit_results_data : fit_results_data_scan_) {
  //   toy_study.ReleaseFitResult(fit_results_data.second);
  // }
}


bool doofit::plotting::profiles::FeldmanCousinsProfiler::FitResultOkay(const doofit::fitter::easyfit::EasyFitResult& fit_result) const {
  using namespace doocore::io;
  if (fit_result.quality_covariance_matrix() < 2 && fit_result.quality_covariance_matrix() != -1) {
    // sdebug << "rejected for covariance: " << fit_result.quality_covariance_matrix() << endmsg;
    return false;
  } else if (fit_result.status().at(0).second < 0) {
    // sdebug << "rejected for status " << fit_result.status().at(0).second << endmsg;
    return false;
  } else if(fit_result.fcn() == -1e+30) {
    // sdebug << "rejected for fcn " << fit_result.fcn() << endmsg;
    return false;
  } else {
    return true;
  }
}

double doofit::plotting::profiles::FeldmanCousinsProfiler::FindGraphXValues(TGraph& graph, double xmin, double xmax, double value, double direction) const {
  using namespace doocore::io;
  TAxis* xaxis = graph.GetXaxis();

  double x_lo(xmin);
  double x_hi(xmax);

  double x_start(xmin);
  double x_end(xmax);
  if (direction < 0.0) {
    x_start = xmax;
    x_end = xmin;
  }

  // sdebug << (x_end - x_start)/100.0 << endmsg;
  // sdebug << x_start << endmsg;
  // sdebug << x_end << endmsg;
  // sdebug << value << endmsg;

  for (double x=x_start; x*direction<x_end*direction; x+=(x_end - x_start)/100.0) {
    double y(graph.Eval(x, nullptr, ""));
    // sdebug << "x = " << x << ", y = " << y << endmsg;

    if (y < value) {
      x_lo = x;
    } else if (y > value) {
      x_hi = x;
      break;
    }
  }

  double eps(1e-6);
  double x_test, y;
  while (std::abs(x_hi-x_lo) > eps) {
    x_test = (x_lo+x_hi)/2.0;
    y = graph.Eval(x_test, nullptr, "");
    
    // sdebug << "it:  " << x_lo << " - " << x_hi << endmsg;

    if (y < value) {
      x_lo = x_test;
    } else if (y > value) {
      x_hi = x_test;
    }
  }

  // sdebug << "after:  " << x_lo << " - " << x_hi << endmsg;
  return (x_lo+x_hi)/2.0;
}

void doofit::plotting::profiles::FeldmanCousinsProfiler::PlotHandler(const std::string& plot_path) {
  using namespace doocore::io;

  std::vector<std::vector<double>> vals_scan;
  std::vector<double> vals_x;
  std::vector<double> vals_x_error;
  std::vector<double> vals_y;
  std::vector<double> cls;
  std::vector<double> cl_errors;
  std::vector<double> cl_lows;
  std::vector<double> cl_highs;
  std::vector<double> cls_wilks;

  std::map<std::string, double> min_scan_val;
  std::map<std::string, double> max_scan_val;

  for (auto delta_nll_data : delta_nlls_data_scan_) {
    // sdebug << delta_nll_data.first << endmsg;

    // TODO: Check if delta_nlls_toy_scan_[delta_nll_data.first] exists!
    const std::vector<double>& delta_nlls_toy(delta_nlls_toy_scan_[delta_nll_data.first]);

    unsigned int num_toys(0);
    unsigned int num_toys_exceed(0);
    for (auto delta_nll_toy : delta_nlls_toy) {

      // if (std::abs(delta_nll_data.first[0]-0.745732) < 1e-3) {
      //   if (delta_nll_data.second >= delta_nll_toy) {
      //     sdebug << "WHAAAAAAT!" << endmsg;
      //     sdebug << "@" << delta_nll_data.first[0] << " (toy)  : " << delta_nll_toy << endmsg;
      //     sdebug << "@" << delta_nll_data.first[0] << " (data) : " << delta_nll_data.second << endmsg;
      //   }
      // }

      // sdebug << delta_nll_data.second << " - " << delta_nll_toy << endmsg;
      ++num_toys;
      if (delta_nll_data.second < delta_nll_toy) {
        ++num_toys_exceed;
      }
    }

    vals_scan.push_back(delta_nll_data.first);
    vals_x.push_back(delta_nll_data.first[0]);
    vals_x_error.push_back(0.0);
    if (delta_nll_data.first.size() > 1) {
      vals_y.push_back(delta_nll_data.first[0]);
    }
    double cl, cl_error;
    std::pair<double, double> cl_low_high;
    if (num_toys > 100) {
      cl = static_cast<double>(num_toys_exceed)/static_cast<double>(num_toys);
      //cl_error = 1.0/static_cast<double>(num_toys)*std::sqrt(static_cast<double>(num_toys_exceed)*(1-cl));
      cl_error    = doocore::statistics::general::EfficiencyBinomialError(num_toys_exceed, num_toys);
      cl_low_high = doocore::statistics::general::EfficiencyBayesianErrorClopperPearson(num_toys_exceed, num_toys);

    } else {
      cl = 0.0;
      cl_error = 0.0;
    }
    cls.push_back(cl);
    cl_errors.push_back(cl_error);
    cl_lows.push_back(cl-cl_low_high.first);
    cl_highs.push_back(cl_low_high.second-cl);

    if (num_toys > 0) {
      if (cl == 0.0) {
        swarn << "Scan point " << delta_nll_data.first << " has too little statistics: N(DeltaChi^2(data) < DeltaChi^2(toy))/N_toys = " << num_toys_exceed << "/" << num_toys << endmsg;
      } else if (cl_error > 0.02 || num_toys_exceed < 10) {
        swarn << "Scan point " << delta_nll_data.first << " has little statistics: N(DeltaChi^2(data) < DeltaChi^2(toy))/N_toys = " << num_toys_exceed << "/" << num_toys << endmsg;
      }
      sinfo << "Scan point " << delta_nll_data.first << " with number of neglected toys: " << num_neglected_fits_[delta_nll_data.first] << "/" << num_toys+num_neglected_fits_[delta_nll_data.first] << " (" << static_cast<double>(num_neglected_fits_[delta_nll_data.first])/static_cast<double>(num_toys+num_neglected_fits_[delta_nll_data.first])*100.0 << "%)" << endmsg;
    }

    double cl_wilks(TMath::Prob(2*delta_nll_data.second, delta_nll_data.first.size()));
    // infinitesimal small DeltaChi2 can become < 0.0, that is the case for the best estimate
    // set cl_wilks to 1.0 here
    if (delta_nll_data.second < 0.0 && std::abs(delta_nll_data.second) < 1e-4) {
      cl_wilks = 1.0;
    }
    cls_wilks.push_back(cl_wilks);
    // sdebug << "x = " << delta_nll_data.first << ", DeltaChi2 = " << delta_nll_data.second << " , 1-CL(Wilk's Theorem) = " << cls_wilks.back() << ", 1-CL(FC) = " << cl << endmsg;

    // sdebug << "       scan_val = " << delta_nll_data.first << endmsg;
    // sdebug << "       num_toys = " << num_toys << endmsg;
    // sdebug << "num_toys_exceed = " << num_toys_exceed << endmsg;
    // sdebug << "             cl = " << cl << endmsg;
  }

  if (vals_scan.size() > 0 && vals_scan[0].size() == 1) {
    doocore::lutils::setStyle("LHCbOptimized");
    gStyle->SetPadLeftMargin(0.12);
    gStyle->SetTitleOffset(0.75, "y");
  } else if (vals_scan.size() > 0 && vals_scan[0].size() == 2) {
    doocore::lutils::setStyle("2d");
    //gStyle->SetNumberContours(999);
    //gStyle->SetPadRightMargin(0.16);
    gStyle->SetPadRightMargin(0.06);
    gStyle->SetTitleOffset(0.75, "z");
  }

  TCanvas c("c", "c", 800, 600);
  c.SetLogy(true);

  if (vals_scan.size() > 0 && vals_scan[0].size() == 1) {
    // stupidly sort both x and y vectors simultaneously
    // to avoid glitches in ROOT's TGraph plotting

    // sdebug << "cls: " << cls << endmsg;

    std::map<double, std::tuple<double, double, double>> values;
    for (unsigned int i=0; i<cls.size(); ++i) {
      if (cls[i] != 0.0) {
        values.emplace(std::make_pair(vals_x[i], std::make_tuple(cls[i], cl_lows[i], cl_highs[i])));
      }
    }
    std::vector<double> vals_x_sort, cls_sort, cl_errors_sort, cls_sort_lower, cls_sort_upper;
    std::vector<double> cl_lows_sort, cl_highs_sort;

    vals_x_sort.reserve(values.size());
    cls_sort.reserve(values.size());
    cls_sort_lower.reserve(values.size());
    cls_sort_upper.reserve(values.size());
    cl_errors_sort.reserve(values.size());
    for (auto value : values) {
      vals_x_sort.push_back(value.first);
      cls_sort.push_back(std::get<0>(value.second));
      cl_errors_sort.push_back(0.5*(std::get<1>(value.second) + std::get<2>(value.second)));

      cl_lows_sort.push_back(std::get<1>(value.second));
      cl_highs_sort.push_back(std::get<2>(value.second));

      cls_sort_lower.push_back(std::get<0>(value.second) - std::get<1>(value.second));
      cls_sort_upper.push_back(std::get<0>(value.second) + std::get<2>(value.second));
      
    }

    std::map<double, double> values_wilks;
    for (unsigned int i=0; i<cls_wilks.size(); ++i) {
      if (cls_wilks[i] != 0.0) {
        values_wilks.emplace(std::make_pair(vals_x[i], cls_wilks[i]));
      }
    }
    std::vector<double> vals_x_wilks_sort, cls_wilks_sort;
    vals_x_wilks_sort.reserve(values_wilks.size());
    cls_wilks_sort.reserve(values_wilks.size());
    for (auto value : values_wilks) {
      vals_x_wilks_sort.push_back(value.first);
      cls_wilks_sort.push_back(value.second);
    }

    TGraphAsymmErrors graph        (cls_sort.size(), &vals_x_sort[0], &cls_sort[0], &vals_x_error[0], &vals_x_error[0], &cl_lows_sort[0], &cl_highs_sort[0]);
    TGraphAsymmErrors graph_errband(cls_sort.size(), &vals_x_sort[0], &cls_sort[0], &vals_x_error[0], &vals_x_error[0], &cl_lows_sort[0], &cl_highs_sort[0]);
    TGraph graph_wilks(cls_wilks_sort.size(), &vals_x_wilks_sort[0], &cls_wilks_sort[0]);

    TGraph graph_lower(cls_sort.size(), &vals_x_sort[0], &cls_sort_lower[0]);
    TGraph graph_upper(cls_sort.size(), &vals_x_sort[0], &cls_sort_upper[0]);

    if (cls.size() < 400) {
      graph_wilks.Draw("AL");
      graph_wilks.SetMarkerStyle(7);
      graph_wilks.SetMarkerSize(1);

      graph_errband.SetFillColor(kBlue-8);
      graph_errband.Draw("3");

      graph.Draw("PL");
      graph.SetMarkerStyle(2);
      graph.SetMarkerSize(1);
      graph.SetMarkerColor(kBlue+3);
      graph.SetLineColor(kBlue+3);

      graph_wilks.Draw("L");

      // graph_lower.Draw("L");
      // graph_upper.Draw("L");

      // graph_wilks.SetMarkerColor(kBlue+3);
      // graph_wilks.SetLineColor(kBlue+3);
    } else {
      graph_wilks.Draw("AL");
      graph_wilks.SetMarkerStyle(1);
      graph.Draw("PL");
      graph.SetMarkerStyle(1);
      graph.SetMarkerColor(kBlue+3);
      graph.SetLineColor(kBlue+3);
      
      // graph_wilks.SetMarkerColor(kBlue+3);
      // graph_wilks.SetLineColor(kBlue+3);
    }
    
    // double x_range = max_scan_val[val_scan.begin()->first] - min_scan_val[val_scan.begin()->first];

    // double x_range_lo = min_scan_val[val_scan.begin()->first] - x_range*0.1;
    // double x_range_hi = max_scan_val[val_scan.begin()->first] + x_range*0.1;

    sdebug << "cls_sort_lower: " << cls_sort_lower << endmsg;

    if (cls_sort_lower.size() > 0) {
      auto min_cl_it = std::min_element(cls_sort_lower.begin(), cls_sort_lower.end());
      // graph.GetXaxis()->SetRangeUser(x_range_lo, x_range_hi);
      graph_wilks.GetYaxis()->SetRangeUser(std::max(*min_cl_it*0.5, 1e-3), 1.01);
    }
    graph_wilks.GetXaxis()->SetTitle(scan_vars_titles_.at(0).c_str());
    graph_wilks.GetYaxis()->SetTitle("1 #minus CL");

    TAxis* xaxis = graph_wilks.GetXaxis();

    double x_lo(xaxis->GetXmin());
    double x_hi(xaxis->GetXmax());

    double level_1sigma(1-0.682690);
    double level_2sigma(1-0.954500);
    double level_3sigma(1-0.997300);

    double xmin(vals_x_sort.front());
    double xmax(vals_x_sort.back());
    double xmin_wilks(vals_x_wilks_sort.front());
    double xmax_wilks(vals_x_wilks_sort.back());

    std::pair<double, double> cl_1sigma(FindGraphXValues(graph, xmin, xmax, level_1sigma), FindGraphXValues(graph, xmin, xmax, level_1sigma, -1.0));
    std::pair<double, double> cl_2sigma(FindGraphXValues(graph, xmin, xmax, level_2sigma), FindGraphXValues(graph, xmin, xmax, level_2sigma, -1.0));
    std::pair<double, double> cl_3sigma(FindGraphXValues(graph, xmin, xmax, level_3sigma), FindGraphXValues(graph, xmin, xmax, level_3sigma, -1.0));

    std::pair<double, double> cl_1sigma_lower(FindGraphXValues(graph_lower, xmin, xmax, level_1sigma), FindGraphXValues(graph_lower, xmin, xmax, level_1sigma, -1.0));
    std::pair<double, double> cl_2sigma_lower(FindGraphXValues(graph_lower, xmin, xmax, level_2sigma), FindGraphXValues(graph_lower, xmin, xmax, level_2sigma, -1.0));
    std::pair<double, double> cl_3sigma_lower(FindGraphXValues(graph_lower, xmin, xmax, level_3sigma), FindGraphXValues(graph_lower, xmin, xmax, level_3sigma, -1.0));

    std::pair<double, double> cl_1sigma_upper(FindGraphXValues(graph_upper, xmin, xmax, level_1sigma), FindGraphXValues(graph_upper, xmin, xmax, level_1sigma, -1.0));
    std::pair<double, double> cl_2sigma_upper(FindGraphXValues(graph_upper, xmin, xmax, level_2sigma), FindGraphXValues(graph_upper, xmin, xmax, level_2sigma, -1.0));
    std::pair<double, double> cl_3sigma_upper(FindGraphXValues(graph_upper, xmin, xmax, level_3sigma), FindGraphXValues(graph_upper, xmin, xmax, level_3sigma, -1.0));

    // sdebug << cl_1sigma.first << " - " <<  std::max(cl_1sigma.first-cl_1sigma_upper.first,0.0)   << " - " << std::max(cl_1sigma_lower.first-cl_1sigma.first,0.0) << endmsg;
    // sdebug << cl_1sigma.second << " - " << std::max(cl_1sigma.second-cl_1sigma_lower.second,0.0) << " - " << std::max(cl_1sigma_upper.second-cl_1sigma.second,0.0) << endmsg;
    // sdebug << cl_2sigma.first << " - " <<  std::max(cl_2sigma.first-cl_2sigma_upper.first,0.0)   << " - " << std::max(cl_2sigma_lower.first-cl_2sigma.first,0.0) << endmsg;
    // sdebug << cl_2sigma.second << " - " << std::max(cl_2sigma.second-cl_2sigma_lower.second,0.0) << " - " << std::max(cl_2sigma_upper.second-cl_2sigma.second,0.0) << endmsg;
    // sdebug << cl_3sigma.first << " - " <<  std::max(cl_3sigma.first-cl_3sigma_upper.first,0.0)   << " - " << std::max(cl_3sigma_lower.first-cl_3sigma.first,0.0) << endmsg;
    // sdebug << cl_3sigma.second << " - " << std::max(cl_3sigma.second-cl_3sigma_lower.second,0.0) << " - " << std::max(cl_3sigma_upper.second-cl_3sigma.second,0.0) << endmsg;

    using namespace doocore::statistics::general;
    ValueWithError<double> cl_1sigma_low(cl_1sigma.first,  0.0, std::max(cl_1sigma.first-cl_1sigma_upper.first,0.0),   std::max(cl_1sigma_lower.first-cl_1sigma.first,0.0));
    ValueWithError<double> cl_1sigma_hig(cl_1sigma.second, 0.0, std::max(cl_1sigma.second-cl_1sigma_lower.second,0.0), std::max(cl_1sigma_upper.second-cl_1sigma.second,0.0));
    ValueWithError<double> cl_2sigma_low(cl_2sigma.first,  0.0, std::max(cl_2sigma.first-cl_2sigma_upper.first,0.0),   std::max(cl_2sigma_lower.first-cl_2sigma.first,0.0));
    ValueWithError<double> cl_2sigma_hig(cl_2sigma.second, 0.0, std::max(cl_2sigma.second-cl_2sigma_lower.second,0.0), std::max(cl_2sigma_upper.second-cl_2sigma.second,0.0));
    ValueWithError<double> cl_3sigma_low(cl_3sigma.first,  0.0, std::max(cl_3sigma.first-cl_3sigma_upper.first,0.0),   std::max(cl_3sigma_lower.first-cl_3sigma.first,0.0));
    ValueWithError<double> cl_3sigma_hig(cl_3sigma.second, 0.0, std::max(cl_3sigma.second-cl_3sigma_lower.second,0.0), std::max(cl_3sigma_upper.second-cl_3sigma.second,0.0));

    sinfo << "1 sigma interval (FC): [" << cl_1sigma_low << ", " << cl_1sigma_hig << "]" << endmsg;
    sinfo << "2 sigma interval (FC): [" << cl_2sigma_low << ", " << cl_2sigma_hig << "]" << endmsg;
    sinfo << "3 sigma interval (FC): [" << cl_3sigma_low << ", " << cl_3sigma_hig << "]" << endmsg;

    cl_1sigma_low.set_full_precision_printout(true);
    cl_1sigma_hig.set_full_precision_printout(true);
    cl_2sigma_low.set_full_precision_printout(true);
    cl_2sigma_hig.set_full_precision_printout(true);
    cl_3sigma_low.set_full_precision_printout(true);
    cl_3sigma_hig.set_full_precision_printout(true);

    // sdebug << cl_1sigma.first << endmsg;

    sinfo << "1 sigma interval (FC): [" << cl_1sigma_low << ", " << cl_1sigma_hig << "]" << endmsg;
    sinfo << "2 sigma interval (FC): [" << cl_2sigma_low << ", " << cl_2sigma_hig << "]" << endmsg;
    sinfo << "3 sigma interval (FC): [" << cl_3sigma_low << ", " << cl_3sigma_hig << "]" << endmsg;

    std::pair<double, double> cl_1sigma_wilks(FindGraphXValues(graph_wilks, xmin_wilks, xmax_wilks, level_1sigma), FindGraphXValues(graph_wilks, xmin_wilks, xmax_wilks, level_1sigma, -1.0));
    std::pair<double, double> cl_2sigma_wilks(FindGraphXValues(graph_wilks, xmin_wilks, xmax_wilks, level_2sigma), FindGraphXValues(graph_wilks, xmin_wilks, xmax_wilks, level_2sigma, -1.0));
    std::pair<double, double> cl_3sigma_wilks(FindGraphXValues(graph_wilks, xmin_wilks, xmax_wilks, level_3sigma), FindGraphXValues(graph_wilks, xmin_wilks, xmax_wilks, level_3sigma, -1.0));

    sinfo << "1 sigma interval (Wilks): [" << cl_1sigma_wilks.first << ", " << cl_1sigma_wilks.second << "]" << endmsg;
    sinfo << "2 sigma interval (Wilks): [" << cl_2sigma_wilks.first << ", " << cl_2sigma_wilks.second << "]" << endmsg;
    sinfo << "3 sigma interval (Wilks): [" << cl_3sigma_wilks.first << ", " << cl_3sigma_wilks.second << "]" << endmsg;

    double y_min(graph_wilks.GetHistogram()->GetMinimum());

    TLine line_1sigma(x_lo, level_1sigma, x_hi, level_1sigma);
    line_1sigma.SetLineColor(kRed-8);
    line_1sigma.SetLineWidth(2);
    line_1sigma.SetLineStyle(3);
    if (level_1sigma > y_min) {
      line_1sigma.Draw();
    }

    TLine line_2sigma(x_lo, level_2sigma, x_hi, level_2sigma);
    line_2sigma.SetLineColor(kRed-8);
    line_2sigma.SetLineWidth(2);
    line_2sigma.SetLineStyle(3);
    if (level_2sigma > y_min) {
      line_2sigma.Draw();
    }

    TLine line_3sigma(x_lo, level_3sigma, x_hi, level_3sigma);
    line_3sigma.SetLineColor(kRed-8);
    line_3sigma.SetLineWidth(2);
    line_3sigma.SetLineStyle(3);
    if (level_3sigma > y_min) {
      line_3sigma.Draw();
    }

    std::string label_string(config_plot_.label_text());
    double label_x(0.65), label_y(0.85);
    TLatex label_base(0,0,label_string.c_str());
    double xsize = label_base.GetXsize();
    if (xsize > 0.25) {
      if (config_plot_.plot_style().find("Enlarged") != std::string::npos) {
        // Enlarged plot needs different treatment
        label_x = 0.65 - (std::max(0.0, xsize-0.4)*0.8);
      } else {
        label_x = 0.65 - (std::max(0.0, xsize-0.25)*0.8);
      }
    }

    TLatex label(label_x, label_y, label_string.c_str());
    //label.SetTextSize(0.08);
    label.SetNDC();
    label.Draw();

    //c.SaveAs("profile.pdf");
    doocore::lutils::printPlot(&c, "fc", plot_path);

    sinfo << "Total time spent for all Feldman-Cousins fits (d:hh:mm:ss): " << doocore::io::tools::SecondsToTimeString(time_total_) << endmsg;
  }
}


