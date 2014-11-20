#include "LikelihoodProfiler.h"

// from STL
#include <set>

// from ROOT
#include "TCanvas.h"
#include "TGraph.h"
#include "TH2D.h" 
#include "TAxis.h"
#include "TStyle.h"
#include "TColor.h"

// from RooFit
#include "RooFitResult.h"

// from DooCore
#include <doocore/io/MsgStream.h>
#include <doocore/io/Progress.h>
#include <doocore/lutils/lutils.h>

// from DooFit
#include "doofit/fitter/AbsFitter.h"
#include "doofit/toy/ToyStudyStd/ToyStudyStd.h"

doofit::plotting::profiles::LikelihoodProfiler::LikelihoodProfiler()
: num_samples_(30)
{}

std::vector<double> doofit::plotting::profiles::LikelihoodProfiler::SetSamplePoint(unsigned int step) {
  using namespace doocore::io;

  std::vector<double> sample_vals;
  int i(scan_vars_.size() - 1);

  //sdebug << "step = " << step << endmsg;

  for (auto var : scan_vars_) {
    int step_this = step/std::pow(num_samples_,i);
    step -= step_this*std::pow(num_samples_,i);
    --i;

    double val(start_vals_[var->GetName()] + 3*var->getError()*(-1.0 + 2.0/static_cast<double>(num_samples_)*step_this));
    var->setVal(val);
    var->setConstant(true);

    sample_vals.push_back(val);
    //sdebug << "  " << var->GetName() << " = " << step_this << " @ " << var->getVal() << endmsg;
  }

  return sample_vals;
}

void doofit::plotting::profiles::LikelihoodProfiler::Scan() {
  using namespace doocore::io;

  //RooRealVar* parameter_scan = scan_vars_.front();

  //num_samples_                   = 12;
  unsigned int num_dimensions    = scan_vars_.size();
  unsigned int num_total_samples = std::pow(num_samples_,num_dimensions);
  unsigned int step              = 0;

  // double value_start = parameter_scan->getVal();
  // double value_scan  = value_start - 5.0*parameter_scan->getError();

  for (auto var : scan_vars_) {
    start_vals_[var->GetName()] = var->getVal();
  }

  Progress p("Sampling likelihood", num_total_samples);
  while (step < num_total_samples) {
    //parameter_scan->setVal(value_scan);
    //parameter_scan->setConstant(true);

    //sinfo << "PROFILE: " << value_scan << endmsg;

    std::vector<double> sample_vals(SetSamplePoint(step));

    fitter_->set_shutup(true);
    fitter_->Fit();  

    fit_results_.push_back(fitter_->fit_result());

    //sinfo << fitter_->NegativeLogLikelihood() << endmsg;
    //value_scan += 1*parameter_scan->getError();
    ++step;
    ++p;
  }  
  p.Finish();
}

void doofit::plotting::profiles::LikelihoodProfiler::ReadFitResults(doofit::toy::ToyStudyStd& toy_study) {
  using namespace doofit::toy;
  using namespace doocore::io;

  FitResultContainer fit_result_container(toy_study.GetFitResult());
  const RooFitResult* fit_result(std::get<0>(fit_result_container));
  // int i = 0;
  while (fit_result != nullptr) { // && i < 10000) {
    fit_results_.push_back(fit_result);

    fit_result_container = toy_study.GetFitResult();
    fit_result = std::get<0>(fit_result_container);
    // ++i;
  }
}

bool doofit::plotting::profiles::LikelihoodProfiler::FitResultOkay(const RooFitResult& fit_result) const {
  using namespace doocore::io;
  if (fit_result.covQual() < 2) {
    return false;
  } else if (fit_result.statusCodeHistory(0) < 0) {
    return false;
  } else {
    return true;
  }
}


void doofit::plotting::profiles::LikelihoodProfiler::PlotHandler(const std::string& plot_path) {
  using namespace doocore::io;

  std::map<std::string, std::vector<double>> val_scan;
  std::vector<double> val_nll;

  for (auto var : scan_vars_) {
    RooRealVar* var_fixed = dynamic_cast<RooRealVar*>(fit_results_.front()->constPars().find(var->GetName()));
    scan_vars_titles_.push_back(var_fixed->GetTitle());
  }

  // int i = 0;
  double min_nll(0.0);
  double max_nll(0.0);
  std::map<std::string, double> min_scan_val;
  std::map<std::string, double> max_scan_val;

  Progress p("Processing read in fit results", fit_results_.size());
  for (auto result : fit_results_) {
    if (FitResultOkay(*result)) {
      for (auto var : scan_vars_) {
        RooRealVar* var_fixed = dynamic_cast<RooRealVar*>(result->constPars().find(var->GetName()));

        if (var_fixed == nullptr) {
          serr << "Error in LikelihoodProfiler::PlotHandler(): Cannot find fixed parameter " 
               << var->GetName() << " in fit result!" << endmsg;
          throw;
        }

        val_scan[var->GetName()].push_back(var_fixed->getVal());

        if (min_scan_val.count(var->GetName()) == 0 || min_scan_val[var->GetName()] > var_fixed->getVal()) {
          min_scan_val[var->GetName()] = var_fixed->getVal();
        }
        if (max_scan_val.count(var->GetName()) == 0 || max_scan_val[var->GetName()] < var_fixed->getVal()) {
          max_scan_val[var->GetName()] = var_fixed->getVal();
        }
      }

      if (min_nll == 0.0 || min_nll > result->minNll()) {
        min_nll = result->minNll();
      }
      if (max_nll == 0.0 || max_nll < result->minNll()) {
        max_nll = result->minNll();
      }
      val_nll.push_back(result->minNll());

      ++p;
    }
  }
  p.Finish();

  for (auto &nll : val_nll) {
    if (nll != 0.0) {
      nll -= min_nll;
    }
  }

  if (val_scan.size() == 1) {
    doocore::lutils::setStyle();
    gStyle->SetPadLeftMargin(0.12);
    gStyle->SetTitleOffset(0.75, "y");
  } else if (val_scan.size() == 2) {
    doocore::lutils::setStyle("2d");
    //gStyle->SetNumberContours(999);
    //gStyle->SetPadRightMargin(0.16);
    gStyle->SetPadRightMargin(0.06);
    gStyle->SetTitleOffset(0.75, "z");
  }

  TCanvas c("c", "c", 800, 600);

  if (val_scan.size() == 1) {

    // stupidly sort both x and y vectors simultaneously
    // to avoid glitches in ROOT's TGraph plotting
    const std::vector<double>& val_x = val_scan.begin()->second;
    std::map<double, double> values;
    for (unsigned int i=0; i<val_nll.size(); ++i) {
      values.emplace(std::make_pair(val_x[i], val_nll[i]));
    }

    std::vector<double> val_x_sort, val_nll_sort;
    val_x_sort.reserve(val_nll.size());
    val_nll_sort.reserve(val_nll.size());
    for (auto value : values) {
      val_x_sort.push_back(value.first);
      val_nll_sort.push_back(value.second);
    }

    TGraph graph(val_nll.size(), &val_x_sort[0], &val_nll_sort[0]);

    if (val_nll.size() < 50) {
      graph.Draw("APC");
      graph.SetMarkerStyle(2);
      graph.SetMarkerSize(2);
      graph.SetMarkerColor(kBlue+3);
      graph.SetLineColor(kBlue+3);
    } else {
      graph.Draw("APC");
      graph.SetMarkerStyle(1);
      graph.SetMarkerColor(kBlue+3);
      graph.SetLineColor(kBlue+3);
    }
    
    double x_range = max_scan_val[val_scan.begin()->first] - min_scan_val[val_scan.begin()->first];

    double x_range_lo = min_scan_val[val_scan.begin()->first] - x_range*0.1;
    double x_range_hi = max_scan_val[val_scan.begin()->first] + x_range*0.1;

    graph.GetXaxis()->SetRangeUser(x_range_lo, x_range_hi);
    graph.GetXaxis()->SetTitle(scan_vars_titles_.at(0).c_str());
    graph.GetYaxis()->SetTitle("#DeltaLL");

    //c.SaveAs("profile.pdf");
    doocore::lutils::printPlot(&c, "profile", plot_path);
  } else if (val_scan.size() == 2) {

    const std::vector<double>& val_x = val_scan.begin()->second;
    const std::vector<double>& val_y = val_scan.rbegin()->second;

    auto minmax_x = std::minmax_element(val_x.begin(), val_x.end());
    auto minmax_y = std::minmax_element(val_y.begin(), val_y.end());

    Progress p_distinct("Counting distinct x and y values", val_x.size()+val_y.size());
    std::set<double> distinct_x, distinct_y;
    for (auto x : val_x) {
      distinct_x.insert(x);
      ++p_distinct;
    }
    for (auto y : val_y) {
      distinct_y.insert(y);
      ++p_distinct;
    }
    p_distinct.Finish();

    double min_nll(0.0), max_nll(0.0);
    for (auto nll : val_nll) {
      if (nll != 0 && (min_nll == 0.0 || nll < min_nll)) {
        min_nll = nll;
      }
      if (nll != 0 && (max_nll == 0.0 || nll > max_nll)) {
        max_nll = nll;
      }
    }

    double min_x = *minmax_x.first  - (*minmax_x.second-*minmax_x.first)/(distinct_x.size()-1)*0.5;
    double max_x = *minmax_x.second + (*minmax_x.second-*minmax_x.first)/(distinct_x.size()-1)*0.5;
    double min_y = *minmax_y.first  - (*minmax_y.second-*minmax_y.first)/(distinct_y.size()-1)*0.5;
    double max_y = *minmax_y.second + (*minmax_y.second-*minmax_y.first)/(distinct_y.size()-1)*0.5;

    TH2D histogram("histogram", "histogram", distinct_x.size(), min_x, max_x, distinct_y.size(), min_y, max_y);

    // sdebug << "histogram x: " << *minmax_x.first << " - " <<  *minmax_x.second << endmsg;
    // sdebug << "histogram y: " << *minmax_y.first << " - " <<  *minmax_y.second << endmsg;

    Progress p_hist("Filling 2D profile histogram", val_nll.size());
    for (unsigned int i=0; i<val_nll.size(); ++i) {
      // sdebug << val_x.at(i) << ", " << val_y.at(i) << " - " << val_nll.at(i) << endmsg;
      // sdebug << "Bin: " << histogram.FindBin(val_x.at(i), val_y.at(i)) << endmsg;
      
      int nbin_x, nbin_y, nbin_z;
      histogram.GetBinXYZ(histogram.FindBin(val_x.at(i), val_y.at(i)), nbin_x, nbin_y, nbin_z);

      // sdebug << "Bin center x: " << histogram.GetXaxis()->GetBinCenter(nbin_x) << endmsg;
      // sdebug << "Bin center y: " << histogram.GetYaxis()->GetBinCenter(nbin_y) << endmsg;

      histogram.SetBinContent(histogram.FindBin(val_x.at(i), val_y.at(i)), val_nll.at(i));
      ++p_hist;
    }

    unsigned int num_interpolated_bins(0);
    for (int i=1; i<=histogram.GetNbinsX(); ++i) {
      for (int j=1; j<=histogram.GetNbinsY(); ++j) {
        if (histogram.GetBinContent(i,j) == 0.0) {
          int num_interpolation(0);
          double interpolation(0.0);
          for (int ii=std::max(i-1,1); ii<std::min(i+1,histogram.GetNbinsX()); ++ii) {
            for (int jj=std::max(j-1,1); jj<std::min(j+1,histogram.GetNbinsY()); ++jj) {
              if (histogram.GetBinContent(ii,jj) != 0.0) {
                // sdebug << ii << " - " << jj << endmsg;
                interpolation += histogram.GetBinContent(ii,jj);
                ++num_interpolation;
              }
            }
          }

          interpolation /= num_interpolation;
          // sdebug << "Bin (" << i << "," << j << ") is zero. Will interpolate with " <<  num_interpolation << " bins to " << interpolation << "." << endmsg;
          histogram.SetBinContent(i,j, interpolation);
          ++num_interpolated_bins;
        }
      }
    }
    sinfo << "LikelihoodProfiler::PlotHandler(...): Number of interpolated bins: " << num_interpolated_bins << endmsg;

    p_hist.Finish();

    std::vector<double> stops_cl;
    std::vector<int> colours;

    // 2D limits from Numerical Recipies Third Edition Sec 15.6 (p. 815)

    stops_cl.push_back(0.0);
    colours.push_back(kGray+2);
    stops_cl.push_back(2.30/2.0);
    colours.push_back(kBlue-8);

    if (max_nll > 6.18/2.0) {
      stops_cl.push_back(6.18/2.0);
      colours.push_back(kYellow-8);
    }
    if (max_nll > 11.8/2.0) {
      stops_cl.push_back(11.8/2.0);
      colours.push_back(kRed-8);
    }
    if (max_nll > 18.4/2.0) {
      stops_cl.push_back(18.4/2.0);
      colours.push_back(kCyan-6);
    }
    // if (max_nll > 12.50) {
    //   stops_cl.push_back(12.50);
    //   colours.push_back(kGreen-5);
    // }

    // const Int_t NRGBs = 6;
    // const Int_t NCont = 6;
    
    // Double_t stops[NRGBs] = { 0.00 , 0.50 , 2.00 , 4.50 , 8.00 , 12.5 };
    // Double_t red[NRGBs]   = { 0.00 , 0.00 , 0.20 , 1.00 , 1.00 , 1.00 };
    // Double_t green[NRGBs] = { 0.00 , 0.00 , 0.20 , 1.00 , 1.00 , 1.00 };
    // Double_t blue[NRGBs]  = { 0.20 , 1.00 , 1.00 , 1.00 , 1.00 , 1.00 };
    // TColor::CreateGradientColorTable(NRGBs, stops, red, green, blue, NCont);
    // gStyle->SetNumberContours(NCont);
    // gStyle->SetPaintTextFormat(".1f");


    gStyle->SetPalette(colours.size(), colours.data());
    histogram.SetContour(stops_cl.size(), stops_cl.data());

    sinfo << "LikelihoodProfiler::PlotHandler(): Drawing histogram." << endmsg;
    histogram.Draw("CONT1");
    histogram.GetZaxis()->SetRangeUser(min_nll, max_nll);
    histogram.SetXTitle(scan_vars_titles_.at(0).c_str());
    histogram.SetYTitle(scan_vars_titles_.at(1).c_str());
    histogram.SetZTitle("#DeltaLL");

    //c.SaveAs("profile.pdf");
    sinfo << "LikelihoodProfiler::PlotHandler(): Saving linear histograms to output files." << endmsg;
    doocore::lutils::printPlot(&c, "profile", plot_path, true);
    // sinfo << "LikelihoodProfiler::PlotHandler(): Saving logarithmic histograms to output files." << endmsg;
    // c.SetLogz(true);
    // doocore::lutils::printPlot(&c, "profile_log", plot_path, true);
    sinfo << "LikelihoodProfiler::PlotHandler(): All done." << endmsg;
  } else {
    serr << "Error in LikelihoodProfiler::PlotHandler(): Cannot (yet) plot 3D likelihood." << endmsg;
    throw;
  }
}


