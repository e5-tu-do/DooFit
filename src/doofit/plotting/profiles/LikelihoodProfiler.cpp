#include "LikelihoodProfiler.h"

// from STL
#include <set>

// from ROOT
#include "TCanvas.h"
#include "TGraph.h"
#include "TH2D.h" 
#include "TAxis.h"

// from RooFit
#include "RooFitResult.h"

// from DooCore
#include <doocore/io/MsgStream.h>
#include <doocore/io/Progress.h>

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

  FitResultContainer fit_result_container(toy_study.GetFitResult());
  const RooFitResult* fit_result(std::get<0>(fit_result_container));

  while (fit_result != nullptr) {
    fit_results_.push_back(fit_result);

    fit_result_container = toy_study.GetFitResult();
    fit_result = std::get<0>(fit_result_container);
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
      }

      val_nll.push_back(result->minNll());
    }
  }

  TCanvas c("c", "c", 800, 600);

  if (val_scan.size() == 1) {
    const std::vector<double>& val_x = val_scan.begin()->second;

    TGraph g(val_nll.size(), &val_x[0], &val_nll[0]);
    g.Draw("AP");
    c.SaveAs("profile.pdf");
  } else if (val_scan.size() == 2) {
    const std::vector<double>& val_x = val_scan.begin()->second;
    const std::vector<double>& val_y = val_scan.rbegin()->second;

    auto minmax_x = std::minmax_element(val_x.begin(), val_x.end());
    auto minmax_y = std::minmax_element(val_y.begin(), val_y.end());

    std::set<double> distinct_x, distinct_y;
    for (auto x : val_x) {
      distinct_x.insert(x);
    }
    for (auto y : val_y) {
      distinct_y.insert(y);
    }

    double min_x = *minmax_x.first  - (*minmax_x.second-*minmax_x.first)/(distinct_x.size()-1)*0.5;
    double max_x = *minmax_x.second + (*minmax_x.second-*minmax_x.first)/(distinct_x.size()-1)*0.5;
    double min_y = *minmax_y.first  - (*minmax_y.second-*minmax_y.first)/(distinct_y.size()-1)*0.5;
    double max_y = *minmax_y.second + (*minmax_y.second-*minmax_y.first)/(distinct_y.size()-1)*0.5;

    TH2D histogram("histogram", "histogram", distinct_x.size(), min_x, max_x, distinct_y.size(), min_y, max_y);

    // sdebug << "histogram x: " << *minmax_x.first << " - " <<  *minmax_x.second << endmsg;
    // sdebug << "histogram y: " << *minmax_y.first << " - " <<  *minmax_y.second << endmsg;

    for (int i=0; i<val_nll.size(); ++i) {
      // sdebug << val_x.at(i) << ", " << val_y.at(i) << " - " << val_nll.at(i) << endmsg;
      // sdebug << "Bin: " << histogram.FindBin(val_x.at(i), val_y.at(i)) << endmsg;
      
      int nbin_x, nbin_y, nbin_z;
      histogram.GetBinXYZ(histogram.FindBin(val_x.at(i), val_y.at(i)), nbin_x, nbin_y, nbin_z);

      // sdebug << "Bin center x: " << histogram.GetXaxis()->GetBinCenter(nbin_x) << endmsg;
      // sdebug << "Bin center y: " << histogram.GetYaxis()->GetBinCenter(nbin_y) << endmsg;

      histogram.SetBinContent(histogram.FindBin(val_x.at(i), val_y.at(i)), val_nll.at(i));
    }

    histogram.Draw("COLZ");
    c.SaveAs("profile.pdf");
  } else {
    serr << "Error in LikelihoodProfiler::PlotHandler(): Cannot (yet) plot 3D likelihood." << endmsg;
    throw;
  }

  
}


