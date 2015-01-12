#include "FeldmanCousinsProfiler.h"

// from STL
#include <set>

// from ROOT
#include "TCanvas.h"
#include "TGraph.h"
#include "TGraphErrors.h"
#include "TH2D.h" 
#include "TH1F.h" 
#include "TAxis.h"
#include "TStyle.h"
#include "TColor.h"
#include "TLine.h"
#include "TMath.h"

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

doofit::plotting::profiles::FeldmanCousinsProfiler::FeldmanCousinsProfiler(const PlotConfig& cfg_plot)
: config_plot_(cfg_plot),
  num_samples_(30),
  time_total_(0.0)
{}

void doofit::plotting::profiles::FeldmanCousinsProfiler::ReadFitResultDataNominal(doofit::toy::ToyStudyStd& toy_study) {
  using namespace doofit::toy;
  using namespace doocore::io;

  FitResultContainer fit_result_container(toy_study.GetFitResult());
  const RooFitResult* fit_result(std::get<0>(fit_result_container));
  
  time_total_ += std::get<2>(fit_result_container);

  for (auto var : scan_vars_) {
    RooRealVar* var_fixed = dynamic_cast<RooRealVar*>(fit_result->floatParsFinal().find(var->GetName()));
    if (var_fixed == nullptr) {
      var_fixed = dynamic_cast<RooRealVar*>(fit_result->constPars().find(var->GetName()));
    }
    if (var_fixed != nullptr) {
      scan_vars_titles_.push_back(var_fixed->GetTitle());
      scan_vars_names_.push_back(var_fixed->GetName());
    } else {
      serr << "Cannot get scan parameter " << var->GetName() << " from nominal data fit result!" << endmsg;
      throw;
    }
  }

  nll_data_nominal_ = fit_result->minNll();
  // sdebug << "nll_data_nominal_ = " << nll_data_nominal_ << endmsg;

  toy_study.ReleaseFitResult(fit_result_container);
}

void doofit::plotting::profiles::FeldmanCousinsProfiler::ReadFitResultsDataScan(doofit::toy::ToyStudyStd& toy_study) {
  using namespace doofit::toy;
  using namespace doocore::io;

  FitResultContainer fit_result_container(toy_study.GetFitResult());
  const RooFitResult* fit_result(std::get<0>(fit_result_container));

  unsigned int num_ignored(0);
  while (fit_result != nullptr) { 
    time_total_ += std::get<2>(fit_result_container);

    if (FitResultOkay(*fit_result)) {
      std::vector<double> scan_vals;
      for (auto var : scan_vars_) {
        RooRealVar* var_fixed = dynamic_cast<RooRealVar*>(fit_result->constPars().find(var->GetName()));

        // protection against 0.0 being 1e-16 and not being properly matched
        double val(var_fixed->getVal());
        if (std::abs(val) < 1e-14) {
          val = 0.0;
        } 

        scan_vals.push_back(val);
      }
      delta_nlls_data_scan_[scan_vals] = fit_result->minNll()-nll_data_nominal_;
      scan_vals_data_.insert(scan_vals);
      // sdebug << "scan_vals = " << scan_vals << endmsg;
      // sdebug << "delta_nll = " << delta_nlls_data_scan_[scan_vals] << endmsg;
    } else {
      ++num_ignored;
    }

    toy_study.ReleaseFitResult(fit_result_container);

    fit_result_container = toy_study.GetFitResult();
    fit_result = std::get<0>(fit_result_container);
  }

  if (num_ignored > 0) {
    swarn << "Ignored " << num_ignored << " data scan results due to bad fit quality." << endmsg;
  }

  sinfo << "Available data scan points: " << scan_vals_data_ << endmsg;
}

void doofit::plotting::profiles::FeldmanCousinsProfiler::ReadFitResultsToy(doofit::toy::ToyStudyStd& toy_study) {
  using namespace doofit::toy;
  using namespace doocore::io;

  unsigned int num_ignored(0);

  FitResultContainer fit_result_container(toy_study.GetFitResult());
  const RooFitResult* fit_result_0(std::get<0>(fit_result_container));
  const RooFitResult* fit_result_1(std::get<1>(fit_result_container));
  // TODO: Warn if fit_result_0 valid, but fit_result_1 not!
  while (fit_result_0 != nullptr) { 
    if (fit_result_1 == nullptr) {
      swarn << "Only one fit result stored, expected a pair. The result tree might be broken!" << endmsg;
    } else {
      time_total_ += std::get<2>(fit_result_container);
      time_total_ += std::get<3>(fit_result_container);

      if (FitResultOkay(*fit_result_0) && FitResultOkay(*fit_result_1)) {
        std::vector<double> scan_vals;
        for (auto var : scan_vars_) {
          RooRealVar* var_fixed = dynamic_cast<RooRealVar*>(fit_result_1->constPars().find(var->GetName()));

          // protection against 0.0 being 1e-16 and not being properly matched
          double val(var_fixed->getVal());
          if (std::abs(val) < 1e-14) {
            val = 0.0;
          } 

          scan_vals.push_back(val);
        }
        double delta_nll(fit_result_1->minNll() - fit_result_0->minNll());
        if (delta_nlls_toy_scan_.count(scan_vals) > 0) {
          delta_nlls_toy_scan_[scan_vals].push_back(delta_nll);
        } else {
          std::vector<double> vector;
          vector.push_back(delta_nll);
          delta_nlls_toy_scan_[scan_vals] = vector;
        }
        scan_vals_toy_.insert(scan_vals);
        // sdebug << "scan_vals = " << scan_vals << endmsg;
        // sdebug << "delta_nll = " << delta_nll << endmsg;
      } else {
        ++num_ignored;
      }
    }
    toy_study.ReleaseFitResult(fit_result_container);

    fit_result_container = toy_study.GetFitResult();
    fit_result_0 = std::get<0>(fit_result_container);
    fit_result_1 = std::get<1>(fit_result_container);
  }

  if (num_ignored > 0) {
    swarn << "Ignored " << num_ignored << " toy scan results due to bad fit quality." << endmsg;
  }

  sinfo << "Scanned toy scan points: " << scan_vals_toy_ << endmsg;
}

bool doofit::plotting::profiles::FeldmanCousinsProfiler::FitResultOkay(const RooFitResult& fit_result) const {
  using namespace doocore::io;
  if (fit_result.covQual() < 2) {
    return false;
  } else if (fit_result.statusCodeHistory(0) < 0) {
    return false;
  } else if(fit_result.minNll() == -1e+30) {
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
    if (num_toys > 0) {
      cl = static_cast<double>(num_toys_exceed)/static_cast<double>(num_toys);
      cl_error = 1.0/static_cast<double>(num_toys)*std::sqrt(static_cast<double>(num_toys_exceed)*(1-cl));
    } else {
      cl = 0.0;
      cl_error = 0.0;
    }
    cls.push_back(cl);
    cl_errors.push_back(cl_error);

    if (cl == 0.0) {
      swarn << "Scan point " << delta_nll_data.first << " has too little statistics: N(DeltaChi^2(data) < DeltaChi^2(toy))/N_toys = " << num_toys_exceed << "/" << num_toys << endmsg;
    } else if (cl_error > 0.02 || num_toys_exceed < 10) {
      swarn << "Scan point " << delta_nll_data.first << " has little statistics: N(DeltaChi^2(data) < DeltaChi^2(toy))/N_toys = " << num_toys_exceed << "/" << num_toys << endmsg;
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

    std::map<double, std::pair<double, double>> values;
    for (unsigned int i=0; i<cls.size(); ++i) {
      if (cls[i] != 0.0) {
        values.emplace(std::make_pair(vals_x[i], std::make_pair(cls[i], cl_errors[i])));
      }
    }
    std::vector<double> vals_x_sort, cls_sort, cl_errors_sort, cls_sort_lower, cls_sort_upper;
    vals_x_sort.reserve(values.size());
    cls_sort.reserve(values.size());
    cls_sort_lower.reserve(values.size());
    cls_sort_upper.reserve(values.size());
    cl_errors_sort.reserve(values.size());
    for (auto value : values) {
      vals_x_sort.push_back(value.first);
      cls_sort.push_back(value.second.first);
      cls_sort_lower.push_back(value.second.first - value.second.second);
      cls_sort_upper.push_back(value.second.first + value.second.second);
      cl_errors_sort.push_back(value.second.second);
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

    TGraphErrors graph(cls_sort.size(), &vals_x_sort[0], &cls_sort[0], &vals_x_error[0], &cl_errors_sort[0]);
    TGraphErrors graph_errband(cls_sort.size(), &vals_x_sort[0], &cls_sort[0], &vals_x_error[0], &cl_errors_sort[0]);
    TGraph graph_wilks(cls_wilks_sort.size(), &vals_x_wilks_sort[0], &cls_wilks_sort[0]);

    TGraph graph_lower(cls_sort.size(), &vals_x_sort[0], &cls_sort_lower[0]);
    TGraph graph_upper(cls_sort.size(), &vals_x_sort[0], &cls_sort_upper[0]);

    if (cls.size() < 400) {
      graph_wilks.Draw("APL");
      graph_wilks.SetMarkerStyle(7);
      graph_wilks.SetMarkerSize(1);

      graph_errband.SetFillColor(kBlue-8);
      graph_errband.Draw("3");

      graph.Draw("PL");
      graph.SetMarkerStyle(2);
      graph.SetMarkerSize(1);
      graph.SetMarkerColor(kBlue+3);
      graph.SetLineColor(kBlue+3);

      graph_wilks.Draw("PL");

      // graph_lower.Draw("L");
      // graph_upper.Draw("L");

      // graph_wilks.SetMarkerColor(kBlue+3);
      // graph_wilks.SetLineColor(kBlue+3);
    } else {
      graph_wilks.Draw("APL");
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

    // graph.GetXaxis()->SetRangeUser(x_range_lo, x_range_hi);
    graph_wilks.GetYaxis()->SetRangeUser(1e-3, 1.01);
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

    std::pair<double, double> cl_1sigma_wilks(FindGraphXValues(graph_wilks, xmin, xmax, level_1sigma), FindGraphXValues(graph_wilks, xmin, xmax, level_1sigma, -1.0));
    std::pair<double, double> cl_2sigma_wilks(FindGraphXValues(graph_wilks, xmin, xmax, level_2sigma), FindGraphXValues(graph_wilks, xmin, xmax, level_2sigma, -1.0));
    std::pair<double, double> cl_3sigma_wilks(FindGraphXValues(graph_wilks, xmin, xmax, level_3sigma), FindGraphXValues(graph_wilks, xmin, xmax, level_3sigma, -1.0));

    sinfo << "1 sigma interval (Wilks): [" << cl_1sigma_wilks.first << ", " << cl_1sigma_wilks.second << "]" << endmsg;
    sinfo << "2 sigma interval (Wilks): [" << cl_2sigma_wilks.first << ", " << cl_2sigma_wilks.second << "]" << endmsg;
    sinfo << "3 sigma interval (Wilks): [" << cl_3sigma_wilks.first << ", " << cl_3sigma_wilks.second << "]" << endmsg;


    TLine line_1sigma(x_lo, level_1sigma, x_hi, level_1sigma);
    line_1sigma.SetLineColor(kRed-8);
    line_1sigma.SetLineWidth(2);
    line_1sigma.SetLineStyle(3);
    line_1sigma.Draw();

    TLine line_2sigma(x_lo, level_2sigma, x_hi, level_2sigma);
    line_2sigma.SetLineColor(kRed-8);
    line_2sigma.SetLineWidth(2);
    line_2sigma.SetLineStyle(3);
    line_2sigma.Draw();

    TLine line_3sigma(x_lo, level_3sigma, x_hi, level_3sigma);
    line_3sigma.SetLineColor(kRed-8);
    line_3sigma.SetLineWidth(2);
    line_3sigma.SetLineStyle(3);
    line_3sigma.Draw();


    //c.SaveAs("profile.pdf");
    doocore::lutils::printPlot(&c, "fc", plot_path);

    sinfo << "Total time spent for all Feldman-Cousins fits (d:hh:mm:ss): " << doocore::io::tools::SecondsToTimeString(time_total_) << endmsg;
  }

  // std::map<std::string, std::vector<double>> val_scan;
  // std::vector<double> val_nll;

  // for (auto var : scan_vars_) {
  //   RooRealVar* var_fixed = dynamic_cast<RooRealVar*>(fit_results_.front()->constPars().find(var->GetName()));
  //   scan_vars_titles_.push_back(var_fixed->GetTitle());
  //   scan_vars_names_.push_back(var_fixed->GetName());
  // }

  // // int i = 0;
  // double min_nll(0.0);
  // double max_nll(0.0);
  // std::map<std::string, double> min_scan_val;
  // std::map<std::string, double> max_scan_val;

  // Progress p("Processing read in fit results", fit_results_.size());
  // unsigned int num_neglected(0);
  // for (auto result : fit_results_) {
  //   if (FitResultOkay(*result)) {
  //     for (auto var : scan_vars_) {
  //       RooRealVar* var_fixed = dynamic_cast<RooRealVar*>(result->constPars().find(var->GetName()));

  //       if (var_fixed == nullptr) {
  //         serr << "Error in FeldmanCousinsProfiler::PlotHandler(): Cannot find fixed parameter " 
  //              << var->GetName() << " in fit result!" << endmsg;
  //         throw;
  //       }

  //       // sdebug << var->GetName() << " = " << var_fixed->getVal() << ", ";

  //       val_scan[var->GetName()].push_back(var_fixed->getVal());

  //       if (min_scan_val.count(var->GetName()) == 0 || min_scan_val[var->GetName()] > var_fixed->getVal()) {
  //         min_scan_val[var->GetName()] = var_fixed->getVal();
  //       }
  //       if (max_scan_val.count(var->GetName()) == 0 || max_scan_val[var->GetName()] < var_fixed->getVal()) {
  //         max_scan_val[var->GetName()] = var_fixed->getVal();
  //       }
  //     }

  //     // sdebug << endmsg;
  //     // sdebug << "  nll = " << result->minNll() << endmsg;

  //     if (min_nll == 0.0 || min_nll > result->minNll()) {
  //       min_nll = result->minNll();
  //     }
  //     if (max_nll == 0.0 || max_nll < result->minNll()) {
  //       max_nll = result->minNll();
  //     }
  //     val_nll.push_back(result->minNll());

  //     ++p;
  //   } else { // if (FitResultOkay(*result)) {
  //     //swarn << "Neglecting fit result!" << endmsg;
  //     ++num_neglected;
  //     //result->Print();
  //   } // if (FitResultOkay(*result)) {
  // }
  // p.Finish();

  // if (num_neglected > 0) {
  //   swarn << "Number of neglected fit results: " << num_neglected << " (" << static_cast<double>(num_neglected)/val_nll.size()*100.0 << "%)" << endmsg;
  // }

  // for (auto &nll : val_nll) {
  //   if (nll != 0.0) {
  //     nll -= min_nll;
  //   }
  // }

  // if (val_scan.size() == 1) {
  //   doocore::lutils::setStyle();
  //   gStyle->SetPadLeftMargin(0.12);
  //   gStyle->SetTitleOffset(0.75, "y");
  // } else if (val_scan.size() == 2) {
  //   doocore::lutils::setStyle("2d");
  //   //gStyle->SetNumberContours(999);
  //   //gStyle->SetPadRightMargin(0.16);
  //   gStyle->SetPadRightMargin(0.06);
  //   gStyle->SetTitleOffset(0.75, "z");
  // }

  // TCanvas c("c", "c", 800, 600);

  // if (val_scan.size() == 1) {

  //   // stupidly sort both x and y vectors simultaneously
  //   // to avoid glitches in ROOT's TGraph plotting
  //   const std::vector<double>& val_x = val_scan.begin()->second;
  //   std::map<double, double> values;
  //   for (unsigned int i=0; i<val_nll.size(); ++i) {
  //     values.emplace(std::make_pair(val_x[i], val_nll[i]));
  //   }

  //   std::vector<double> val_x_sort, val_nll_sort;
  //   val_x_sort.reserve(val_nll.size());
  //   val_nll_sort.reserve(val_nll.size());
  //   for (auto value : values) {
  //     val_x_sort.push_back(value.first);
  //     val_nll_sort.push_back(value.second);
  //   }

  //   TGraph graph(val_nll.size(), &val_x_sort[0], &val_nll_sort[0]);

  //   if (val_nll.size() < 25) {
  //     graph.Draw("APC");
  //     graph.SetMarkerStyle(2);
  //     graph.SetMarkerSize(2);
  //     graph.SetMarkerColor(kBlue+3);
  //     graph.SetLineColor(kBlue+3);
  //   } else {
  //     graph.Draw("APC");
  //     graph.SetMarkerStyle(1);
  //     graph.SetMarkerColor(kBlue+3);
  //     graph.SetLineColor(kBlue+3);
  //   }
    
  //   double x_range = max_scan_val[val_scan.begin()->first] - min_scan_val[val_scan.begin()->first];

  //   double x_range_lo = min_scan_val[val_scan.begin()->first] - x_range*0.1;
  //   double x_range_hi = max_scan_val[val_scan.begin()->first] + x_range*0.1;

  //   graph.GetXaxis()->SetRangeUser(x_range_lo, x_range_hi);
  //   graph.GetXaxis()->SetTitle(scan_vars_titles_.at(0).c_str());
  //   graph.GetYaxis()->SetTitle("#DeltaLL");

  //   //c.SaveAs("profile.pdf");
  //   doocore::lutils::printPlot(&c, "profile", plot_path);
  // } else if (val_scan.size() == 2) {
  //   const std::vector<double>& val_x = val_scan[scan_vars_names_.at(0)];
  //   const std::vector<double>& val_y = val_scan[scan_vars_names_.at(1)];

  //   // sdebug << val_x << endmsg;
  //   // sdebug << val_y << endmsg;

  //   auto minmax_x = std::minmax_element(val_x.begin(), val_x.end());
  //   auto minmax_y = std::minmax_element(val_y.begin(), val_y.end());

  //   Progress p_distinct("Counting distinct x and y values", val_x.size()+val_y.size());
  //   std::set<double> distinct_x, distinct_y;
  //   for (auto x : val_x) {
  //     distinct_x.insert(x);
  //     ++p_distinct;
  //   }
  //   for (auto y : val_y) {
  //     distinct_y.insert(y);
  //     ++p_distinct;
  //   }
  //   p_distinct.Finish();

  //   sinfo << "Scanned x values: " << distinct_x << endmsg;
  //   sinfo << "Scanned y values: " << distinct_y << endmsg;

  //   double min_nll(0.0), max_nll(0.0);
  //   for (auto nll : val_nll) {
  //     if (nll != 0 && (min_nll == 0.0 || nll < min_nll)) {
  //       min_nll = nll;
  //     }
  //     if (nll != 0 && (max_nll == 0.0 || nll > max_nll)) {
  //       max_nll = nll;
  //     }
  //   }

  //   // find smallest stepping in x and y
  //   std::set<double>::const_iterator it_x(distinct_x.begin());
  //   std::set<double>::const_iterator it_y(distinct_y.begin());
  //   double val_x_before(*it_x), val_y_before(*it_y);
  //   ++it_x; ++it_y;
  //   double step_x(*it_x - val_x_before),
  //          step_y(*it_y - val_y_before);
  //   val_x_before = *it_x;
  //   val_y_before = *it_y;
  //   ++it_x; ++it_y;
  //   double min_step_x(step_x), min_step_y(step_y);

  //   for (; it_x != distinct_x.end(); ++it_x) {
  //     step_x = *it_x - val_x_before;

  //     if (step_x < min_step_x) {
  //       min_step_x = step_x;
  //     }
  //   }
  //   for (; it_y != distinct_y.end(); ++it_y) {
  //     step_y = *it_y - val_y_before;

  //     if (step_y < min_step_y) {
  //       min_step_y = step_y;
  //     }
  //   }

  //   sinfo << "Smallest x stepping: " << min_step_x << endmsg;
  //   sinfo << "Smallest y stepping: " << min_step_y << endmsg;

  //   // sdebug << "(*minmax_x.second-*minmax_x.first)/(distinct_x.size()-1) = " << (*minmax_x.second-*minmax_x.first)/(distinct_x.size()-1) << endmsg;
  //   // sdebug << "(*minmax_y.second-*minmax_y.first)/(distinct_y.size()-1) = " << (*minmax_y.second-*minmax_y.first)/(distinct_y.size()-1) << endmsg;

  //   double min_x = *minmax_x.first  - min_step_x*0.5;
  //   double max_x = *minmax_x.second + min_step_x*0.5;
  //   double min_y = *minmax_y.first  - min_step_y*0.5;
  //   double max_y = *minmax_y.second + min_step_y*0.5;

  //   if (config_plot_.plot_range_x().first != 0.0 || config_plot_.plot_range_x().second != 0.0) {
  //     min_x = config_plot_.plot_range_x().first  - min_step_x*0.5;
  //     max_x = config_plot_.plot_range_x().second + min_step_x*0.5;
  //   }
  //   if (config_plot_.plot_range_y().first != 0.0 || config_plot_.plot_range_y().second != 0.0) {
  //     min_y = config_plot_.plot_range_y().first  - min_step_y*0.5;
  //     max_y = config_plot_.plot_range_y().second + min_step_y*0.5;
  //   }

  //   double num_bins_x((max_x - min_x)/min_step_x);
  //   double num_bins_y((max_y - min_y)/min_step_y);

  //   // sdebug << "x range for histogram: " << min_x << " - " << max_x << ", stepping: " << min_step_x << ", nbins: " << num_bins_x << endmsg;
  //   // sdebug << "y range for histogram: " << min_y << " - " << max_y << ", stepping: " << min_step_y << ", nbins: " << num_bins_y << endmsg;

  //   // sdebug << "num_bins_x        = " << num_bins_x << endmsg;
  //   // sdebug << "distinct_x.size() = " << distinct_x.size() << endmsg;
  //   // sdebug << "num_bins_y        = " << num_bins_y << endmsg;
  //   // sdebug << "distinct_y.size() = " << distinct_y.size() << endmsg;

  //   TH2D histogram("histogram", "histogram", num_bins_x, min_x, max_x, num_bins_y, min_y, max_y);
  //   TH2D histogram_dbg("histogram_dbg", "histogram_dbg", num_bins_x, min_x, max_x, num_bins_y, min_y, max_y);

  //   // sdebug << "histogram x: " << *minmax_x.first << " - " <<  *minmax_x.second << endmsg;
  //   // sdebug << "histogram y: " << *minmax_y.first << " - " <<  *minmax_y.second << endmsg;

  //   Progress p_hist("Filling 2D profile histogram", val_nll.size());
  //   for (unsigned int i=0; i<val_nll.size(); ++i) {
  //     // sdebug << val_x.at(i) << ", " << val_y.at(i) << " - " << val_nll.at(i) << endmsg;
  //     // sdebug << "Bin: " << histogram.FindBin(val_x.at(i), val_y.at(i)) << endmsg;
      
  //     int nbin_x, nbin_y, nbin_z;
  //     histogram.GetBinXYZ(histogram.FindBin(val_x.at(i), val_y.at(i)), nbin_x, nbin_y, nbin_z);
  //     histogram_dbg.GetBinXYZ(histogram_dbg.FindBin(val_x.at(i), val_y.at(i)), nbin_x, nbin_y, nbin_z);

  //     // sdebug << "Bin center x: " << histogram.GetXaxis()->GetBinCenter(nbin_x) << endmsg;
  //     // sdebug << "Bin center y: " << histogram.GetYaxis()->GetBinCenter(nbin_y) << endmsg;

  //     histogram.SetBinContent(histogram.FindBin(val_x.at(i), val_y.at(i)), val_nll.at(i));
  //     histogram_dbg.SetBinContent(histogram_dbg.FindBin(val_x.at(i), val_y.at(i)), val_nll.at(i));
  //     ++p_hist;
  //   }
  //   p_hist.Finish();

  //   unsigned int num_interpolated_bins(0);
  //   for (int i=1; i<=histogram.GetNbinsX(); ++i) {
  //     for (int j=1; j<=histogram.GetNbinsY(); ++j) {
  //       if (histogram.GetBinContent(i,j) == 0.0) {
  //         int num_interpolation(0);
  //         double interpolation(0.0);
  //         for (int ii=std::max(i-1,1); ii<std::min(i+1,histogram.GetNbinsX()); ++ii) {
  //           for (int jj=std::max(j-1,1); jj<std::min(j+1,histogram.GetNbinsY()); ++jj) {
  //             if (histogram.GetBinContent(ii,jj) != 0.0) {
  //               // sdebug << ii << " - " << jj << endmsg;
  //               interpolation += histogram.GetBinContent(ii,jj);
  //               ++num_interpolation;
  //             }
  //           }
  //         }

  //         interpolation /= num_interpolation;
  //         // sdebug << "Bin (" << i << "," << j << ") is zero. Will interpolate with " <<  num_interpolation << " bins to " << interpolation << "." << endmsg;
  //         histogram.SetBinContent(i,j, interpolation);
  //         ++num_interpolated_bins;
  //       }
  //     }
  //   }
  //   sinfo << "FeldmanCousinsProfiler::PlotHandler(...): Number of interpolated bins: " << num_interpolated_bins << " (" << static_cast<double>(num_interpolated_bins)/static_cast<double>(num_bins_x*num_bins_y)*100.0 << "%)" << endmsg;

  //   std::vector<double> stops_cl;
  //   std::vector<int> colours;

  //   // 2D limits from Numerical Recipies Third Edition Sec 15.6 (p. 815)

  //   stops_cl.push_back(0.0);
  //   colours.push_back(kGray+2);
  //   stops_cl.push_back(2.30/2.0);
  //   colours.push_back(kBlue-8);

  //   if (max_nll > 6.18/2.0) {
  //     stops_cl.push_back(6.18/2.0);
  //     colours.push_back(kYellow-8);
  //   }
  //   if (max_nll > 11.8/2.0) {
  //     stops_cl.push_back(11.8/2.0);
  //     colours.push_back(kRed-8);
  //   }
  //   if (max_nll > 18.4/2.0) {
  //     stops_cl.push_back(18.4/2.0);
  //     colours.push_back(kCyan-6);
  //   }
  //   // if (max_nll > 12.50) {
  //   //   stops_cl.push_back(12.50);
  //   //   colours.push_back(kGreen-5);
  //   // }

  //   // debug plot
  //   const Int_t NRGBs = 6;
  //   const Int_t NCont = 6;    
  //   Double_t stops[NRGBs] = { 0.00 , 0.50 , 2.00 , 4.50 , 8.00 , 12.5 };
  //   Double_t red[NRGBs]   = { 0.00 , 0.00 , 0.20 , 1.00 , 1.00 , 1.00 };
  //   Double_t green[NRGBs] = { 0.00 , 0.00 , 0.20 , 1.00 , 1.00 , 1.00 };
  //   Double_t blue[NRGBs]  = { 0.20 , 1.00 , 1.00 , 1.00 , 1.00 , 1.00 };
  //   TColor::CreateGradientColorTable(NRGBs, stops, red, green, blue, NCont);
  //   gStyle->SetNumberContours(NCont);
  //   gStyle->SetPaintTextFormat(".1f");
  //   histogram_dbg.Draw("COLZ");
  //   doocore::lutils::printPlot(&c, "profile_dbg", plot_path, true);

  //   // fancy plot
  //   gStyle->SetPalette(colours.size(), colours.data());
  //   histogram.SetContour(stops_cl.size(), stops_cl.data());
  //   sinfo << "FeldmanCousinsProfiler::PlotHandler(): Drawing histogram." << endmsg;
  //   histogram.Draw("CONT1");

  //   histogram.GetZaxis()->SetRangeUser(min_nll, max_nll);
  //   histogram.SetXTitle(scan_vars_titles_.at(0).c_str());
  //   histogram.SetYTitle(scan_vars_titles_.at(1).c_str());
  //   histogram.SetZTitle("#DeltaLL");

  //   //c.SaveAs("profile.pdf");
  //   sinfo << "FeldmanCousinsProfiler::PlotHandler(): Saving linear histograms to output files." << endmsg;
  //   doocore::lutils::printPlot(&c, "profile", plot_path, true);
  //   // sinfo << "FeldmanCousinsProfiler::PlotHandler(): Saving logarithmic histograms to output files." << endmsg;
  //   // c.SetLogz(true);
  //   // doocore::lutils::printPlot(&c, "profile_log", plot_path, true);
  //   sinfo << "FeldmanCousinsProfiler::PlotHandler(): All done." << endmsg;
  // } else {
  //   serr << "Error in FeldmanCousinsProfiler::PlotHandler(): Cannot (yet) plot 3D likelihood." << endmsg;
  //   throw;
  // }
}


