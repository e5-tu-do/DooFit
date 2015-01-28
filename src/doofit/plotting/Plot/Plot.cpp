#include "doofit/plotting/Plot/Plot.h"

// STL
#include <string>
#include <sstream>
#include <vector>

// boost
#include <boost/regex.hpp>

// ROOT
#include "TIterator.h" 
#include "TString.h"
#include "TAxis.h"

// from RooFit
#include "RooArgList.h"
#include "RooAbsRealLValue.h"
#include "RooAbsData.h"
#include "RooAbsPdf.h"
#include "RooPlot.h"
#include "RooHist.h"
#include "RooBinning.h"
#include <RooMsgService.h>

// from DooCore
#include "doocore/io/MsgStream.h"
#include "doocore/lutils/lutils.h"
#include <doocore/io/Progress.h>

// from Project
#include "doofit/plotting/Plot/PlotConfig.h"

using namespace ROOT;
using namespace RooFit;
using namespace doocore::io;

namespace doofit {
namespace plotting {

Plot::Plot(const PlotConfig& cfg_plot, const RooAbsRealLValue& dimension, const RooAbsData& dataset, const RooArgList& pdfs, const std::string& plot_name)
: config_plot_(cfg_plot),
  dimension_(dimension),
  datasets_(),
  plot_name_(plot_name),
  ignore_num_cpu_(false),
  plot_asymmetry_(false)
{
  datasets_.push_back(&dataset);
  pdf_ = dynamic_cast<RooAbsPdf*>(pdfs.first());
  
  if (&dimension_ == NULL) {
    serr << "Plot::Plot(): Dimension is invalid." << endmsg;
    throw 1;
  }
  if (datasets_.front() == NULL) {
    serr << "Plot::Plot(): Dataset is invalid." << endmsg;
    throw 1;
  }
  if (plot_name_ == "") {
    plot_name_ = dimension_.GetName();
  }
  
  for (int i=1; i<pdfs.getSize(); ++i) {
    RooAbsArg* sub_arg = pdfs.at(i);
    const RooAbsPdf* sub_pdf = dynamic_cast<RooAbsPdf*>(sub_arg);
    
    if (sub_pdf != NULL) {
      components_.push_back(RooArgSet(*sub_pdf));
    }
  }
}

Plot::Plot(const PlotConfig& cfg_plot, const RooAbsRealLValue& dimension, const RooAbsData& dataset, const RooAbsPdf& pdf, const std::vector<std::string>& components, const std::string& plot_name)
: config_plot_(cfg_plot),
  dimension_(dimension),
  datasets_(),
  plot_name_(plot_name),
  ignore_num_cpu_(false),
  plot_asymmetry_(false)
{
  datasets_.push_back(&dataset);
  pdf_ = &pdf;
  
  if (pdf_ == NULL) {
    serr << "Plot::Plot(): Main PDF is invalid." << endmsg;
    throw 1;
  }
  if (&dimension_ == NULL) {
    serr << "Plot::Plot(): Dimension is invalid." << endmsg;
    throw 1;
  }
  if (datasets_.front() == NULL) {
    serr << "Plot::Plot(): Dataset is invalid." << endmsg;
    throw 1;
  }
  if (plot_name_ == "") {
    plot_name_ = dimension_.GetName();
  }
  
  // iterate over sub PDFs and match supplied regular expressions
  RooArgSet nodes;
  pdf.branchNodeServerList(&nodes);
  
  for (std::vector<std::string>::const_iterator it = components.begin();
       it != components.end(); ++it) {
    boost::regex r(*it);
    components_.push_back(RooArgSet());
    
    TIterator* it_nodes = nodes.createIterator();
    RooAbsArg* node = NULL;
    
    while ((node = dynamic_cast<RooAbsArg*>(it_nodes->Next()))) {
      RooAbsPdf* pdf_node = dynamic_cast<RooAbsPdf*>(node);
      if (pdf_node != NULL) {
        std::string pdf_name = pdf_node->GetName();

        // exclude resolution models generated by RooFit and match the rest
        if (pdf_name.find("_conv_") == std::string::npos && regex_match(pdf_name,r)) {
          components_.back().add(*pdf_node);
        }
      }
    }
    delete it_nodes;
  }
}
  
void Plot::PlotHandler(ScaleType sc_y, std::string suffix) const {
  if (suffix == "") suffix = "_log";
  
  std::string plot_name = plot_name_;
  
  std::stringstream log_plot_name_sstr;
  log_plot_name_sstr << plot_name << suffix;
  std::string log_plot_name = log_plot_name_sstr.str();
  
  std::stringstream pull_plot_sstr;
  pull_plot_sstr << plot_name << "_pull";
  std::string pull_plot_name = pull_plot_sstr.str();
  
  std::stringstream log_pull_plot_sstr;
  log_pull_plot_sstr << plot_name << "_pull" << suffix;
  std::string log_pull_plot_name = log_pull_plot_sstr.str();

  sinfo << "Plotting " << dimension_.GetName() << " into directory " << config_plot_.plot_directory() << " as " << plot_name << endmsg;
  
  doocore::lutils::setStyle(config_plot_.plot_style());
  
  RooCmdArg range_arg;
  
  // x range
  if (!dimension_.hasMin() && !dimension_.hasMax()) {
    double min, max;
    
    // ugly const_cast because RooFit is stupid (RooDataSet::getRange needs non-const RooRealVar)
    RooRealVar* dimension_non_const = const_cast<RooRealVar*>(dynamic_cast<const RooRealVar*>(&dimension_));
    datasets_.front()->getRange(*dimension_non_const, min, max);
    
    double min_t, max_t;
    for (std::vector<const RooAbsData*>::const_iterator it = datasets_.begin()+1;
         it != datasets_.end(); ++it) {
      (*it)->getRange(*dimension_non_const, min_t, max_t);
      if (min_t < min) min = min_t;
      if (max_t > max) max = max_t;
    }
    
    range_arg = Range(min, max);
  }
  
  RooCmdArg cut_range_arg, projection_range_arg, frame_range_arg;
  RooBinning* binning = NULL;
  RooAbsData* dataset_reduced = NULL;
  if (plot_range_.length() > 0) {
    sinfo << "doofit::plotting: Plotting on named range " << plot_range_ << endmsg;
    cut_range_arg = CutRange(plot_range_.c_str());
    projection_range_arg = ProjectionRange(plot_range_.c_str());
    range_arg = Range(plot_range_.c_str());
    binning = new RooBinning(dimension_.getBinning().numBins(), dimension_.getMin(plot_range_.c_str()), dimension_.getMax(plot_range_.c_str()));

    // ugly const cast as RooFit (once again) gives a shit about const correctness
    RooAbsData* dataset = const_cast<RooAbsData*>(datasets_.front());
    //dataset_reduced = dataset->reduce(cut_range_arg);
    dataset_reduced = dataset->reduce(CutRange(plot_range_.c_str()));
        
    //sdebug << "Created reduced dataset with " << dataset_reduced->numEntries() << " (original dataset has " << dataset->numEntries() << ")" << endmsg;
  }

  // I feel so stupid doing this but apparently RooFit leaves me no other way...
  RooCmdArg arg_data1, arg_data2, arg_data3, arg_data4, arg_data5, arg_data6, arg_data7;
  if (plot_args_data_.size() > 0) arg_data1 = plot_args_data_[0];
  if (plot_args_data_.size() > 1) arg_data2 = plot_args_data_[1];
  if (plot_args_data_.size() > 2) arg_data3 = plot_args_data_[2];
  if (plot_args_data_.size() > 3) arg_data4 = plot_args_data_[3];
  if (plot_args_data_.size() > 4) arg_data5 = plot_args_data_[4];
  if (plot_args_data_.size() > 5) arg_data6 = plot_args_data_[5];
  if (plot_args_data_.size() > 6) arg_data7 = plot_args_data_[6];

  RooPlot* plot_frame = dimension_.frame(range_arg);
    
  RooCmdArg weight_arg;
  RooAbsData* dataset_normalisation = NULL;
  if (dataset_reduced != NULL) {
    if (dataset_reduced->isWeighted()) {
      //sdebug << "Spotted a weighted dataset, setting SumW2 errors." << endmsg;
      weight_arg = DataError(RooAbsData::SumW2);
    }
    dataset_normalisation = dataset_reduced;
    
    RooMsgService::instance().setStreamStatus(1, false);
    if (binning != NULL) {
      dataset_reduced->plotOn(plot_frame, Binning(*binning), cut_range_arg, weight_arg, MultiArg(arg_data1, arg_data2, arg_data3, arg_data4, arg_data5, arg_data6, arg_data7)/*, Rescale(1.0/(*it)->sumEntries())*/);
    } else {
      dataset_reduced->plotOn(plot_frame, Binning(dimension_.getBinning()), cut_range_arg, weight_arg, MultiArg(arg_data1, arg_data2, arg_data3, arg_data4, arg_data5, arg_data6, arg_data7)/*, Rescale(1.0/(*it)->sumEntries())*/);
    }
    RooMsgService::instance().setStreamStatus(1, true);
  } else {
    dataset_normalisation = const_cast<RooAbsData*>(datasets_.front());
    for (std::vector<const RooAbsData*>::const_iterator it = datasets_.begin();
         it != datasets_.end(); ++it) {
      if ((*it)->isWeighted()) {
        sdebug << "Weighted dataset, setting SumW2 errors." << endmsg;
        weight_arg = DataError(RooAbsData::SumW2);
      }
      
      RooMsgService::instance().setStreamStatus(1, false);
      if (binning != NULL) {
        (*it)->plotOn(plot_frame, Binning(*binning), cut_range_arg, weight_arg, MultiArg(arg_data1, arg_data2, arg_data3, arg_data4, arg_data5, arg_data6, arg_data7)/*, Rescale(1.0/(*it)->sumEntries())*/);
      } else {
        (*it)->plotOn(plot_frame, Binning(dimension_.getBinning()), cut_range_arg, weight_arg, MultiArg(arg_data1, arg_data2, arg_data3, arg_data4, arg_data5, arg_data6, arg_data7)/*, Rescale(1.0/(*it)->sumEntries())*/);
      }
      RooMsgService::instance().setStreamStatus(1, true);
    }
  }
  
  // y range adaptively for log scale
  RooHist * data = (RooHist*) plot_frame->findObject(0,RooHist::Class());
  double x,y;
  data->GetPoint(0,x,y);
  double min_data_entry = y;
  for (int i = 1; i < data->GetN(); ++i) {
    data->GetPoint(i,x,y);
    if (min_data_entry > y) min_data_entry = y;
  }
  if (min_data_entry == 0.0) min_data_entry = 1.0;
  if (min_data_entry < 0.0) min_data_entry = 0.01;
//  sdebug << "minimum data entry in dataset: " << min_data_entry << endmsg;
  double min_plot = TMath::Power(10.0,TMath::Log10(min_data_entry)-0.9);
  
//  sdebug << "minimum entry in histogram: " << min_data_entry << endmsg;
//  sdebug << "minimum for plot range: " << min_plot << endmsg;
  
  std::string label_string(config_plot_.label_text());
  double label_x(0.65), label_y(0.85);
  TLatex label_add(0,0,plot_label_additional_.c_str());
  double xsize = label_add.GetXsize();
  // sdebug << "additional label x size: " << xsize << endmsg;
  if (label_string.length() > 0 && plot_label_additional_.length() > 0) {
    label_string = "#splitline{" + label_string + "}{" + plot_label_additional_ + "}";
    label_y = 0.82;
  } else if (plot_label_additional_.length() > 0) {
    label_string = plot_label_additional_;
  }
  if (xsize > 0.25) {
    if (config_plot_.plot_style().find("Enlarged") != std::string::npos) {
      // Enlarged plot needs different treatment
      label_x = 0.65 - (std::max(0.0, xsize-0.4)*0.8);
    } else {
      label_x = 0.65 - (std::max(0.0, xsize-0.25)*0.8);
    }
  }

  TLatex label(label_x, label_y, label_string.c_str());
  
  config_plot_.OnDemandOpenPlotStack();
  if (pdf_ != NULL) {
//    RooPlot* plot_frame_pull = dimension_.frame(range_arg);
    
    // I feel so stupid doing this but apparently RooFit leaves me no other way...
    RooCmdArg arg1, arg2, arg3, arg4, arg5, arg6, arg7;
    if (plot_args_pdf_.size() > 0) arg1 = plot_args_pdf_[0];
    if (plot_args_pdf_.size() > 1) arg2 = plot_args_pdf_[1];
    if (plot_args_pdf_.size() > 2) arg3 = plot_args_pdf_[2];
    if (plot_args_pdf_.size() > 3) arg4 = plot_args_pdf_[3];
    if (plot_args_pdf_.size() > 4) arg5 = plot_args_pdf_[4];
    if (plot_args_pdf_.size() > 5) arg6 = plot_args_pdf_[5];
    if (plot_args_pdf_.size() > 6) arg7 = plot_args_pdf_[6];
    
//    if (dataset_reduced != NULL) {
//      serr << "Reduced dataset available. Plotting this." << endmsg;
//      if (binning != NULL) {
//        dataset_reduced->plotOn(plot_frame_pull, Binning(*binning), cut_range_arg);
//      } else {
//        dataset_reduced->plotOn(plot_frame_pull, Binning(dimension_.getBinning()), cut_range_arg);
//      }
//    } else {
//      for (std::vector<const RooAbsData*>::const_iterator it = datasets_.begin();
//           it != datasets_.end(); ++it) {
//        if (binning != NULL) {
//          (*it)->plotOn(plot_frame_pull, Binning(*binning), cut_range_arg);
//        } else {
//          (*it)->plotOn(plot_frame_pull, Binning(dimension_.getBinning()), cut_range_arg);
//        }
//      }
//    }
    
    RooCmdArg normalisation_hack;
    if (config_plot_.num_cpu() > 1 && !ignore_num_cpu_) {
      for (std::vector<RooCmdArg>::const_iterator it = plot_args_pdf_.begin();
           it != plot_args_pdf_.end(); ++it) {
        if (std::string(it->GetName()) == "ProjData") {
          RooArgSet* set_project = new RooArgSet(*dynamic_cast<const RooArgSet*>(it->getObject(0)));
          
          TIterator* arg_it = set_project->createIterator();
          RooAbsArg* arg = NULL;
          while ((arg = (RooAbsArg*)arg_it->Next())) {
            RooRealVar* var = dynamic_cast<RooRealVar*>(arg);
            if (var != NULL) {
              if (pdf_->observableOverlaps(dataset_normalisation, *var)) {
                swarn << "Warning in Plot::PlotHandler(...): Plotting with multiple processes and projection dataset. PDF depends upon " << *var << ". Will manipulate normalisation to fix RooFit bugs." << endmsg;
                normalisation_hack = Normalization(1./dataset_normalisation->sumEntries());
              }
            }
          }
          delete arg_it;
        }
      }
    }

    RooCmdArg arg_num_cpu;
    if (config_plot_.num_cpu() > 1 && !ignore_num_cpu_) {
      arg_num_cpu = NumCPU(config_plot_.num_cpu());
    } else if (ignore_num_cpu_ && config_plot_.num_cpu() > 1) {
      swarn << "Warning in Plot::PlotHandler(...): Multicore plotting is requested but intentionally disabled for this plot to avoid nasty RooFit plotting bugs." << endmsg;
    }
    
    int i=1;
    int num_steps = components_.size()+1;

    using namespace doocore::io;
    Progress p("Plotting components and full PDF", num_steps);

    for (std::vector<RooArgSet>::const_iterator it = components_.begin();
         it != components_.end(); ++it) {
      if (it->getSize() > 0) {
//        sinfo << "Plotting component " << it->first()->GetName() << ", sum entries: " << dataset_normalisation->sumEntries() << endmsg;
        RooMsgService::instance().setStreamStatus(1, false);
        RooMsgService::instance().setStreamStatus(0, false);
        pdf_->plotOn(plot_frame, MultiArg(Components(*it), LineColor(config_plot_.GetPdfLineColor(i)), LineStyle(config_plot_.GetPdfLineStyle(i)), projection_range_arg, arg_num_cpu, normalisation_hack, Precision(1e-4)), MultiArg(arg1, arg2, arg3, arg4, arg5, arg6, arg7));
//        pdf_->plotOn(plot_frame_pull, Components(*it), LineColor(config_plot_.GetPdfLineColor(i)), LineStyle(config_plot_.GetPdfLineStyle(i)), projection_range_arg/*, NumCPU(8)*/, arg1, arg2, arg3, arg4, arg5, arg6);
        RooMsgService::instance().setStreamStatus(1, true);
        RooMsgService::instance().setStreamStatus(0, true);
        ++i;
      }
      ++p;
    }
    
    RooMsgService::instance().setStreamStatus(1, false);
    RooMsgService::instance().setStreamStatus(0, false);
    pdf_->plotOn(plot_frame, MultiArg(LineColor(config_plot_.GetPdfLineColor(0)), LineStyle(config_plot_.GetPdfLineStyle(0)), projection_range_arg, arg_num_cpu, normalisation_hack, Precision(1e-4)), MultiArg(arg1, arg2, arg3, arg4, arg5, arg6, arg7));
//    pdf_->plotOn(plot_frame_pull, LineColor(config_plot_.GetPdfLineColor(0)), LineStyle(config_plot_.GetPdfLineStyle(0)), projection_range_arg/*, NumCPU(8)*/, arg1, arg2, arg3, arg4, arg5, arg6);
    RooMsgService::instance().setStreamStatus(1, true);
    RooMsgService::instance().setStreamStatus(0, true);
    ++p;
    p.Finish();
    
    RooArgSet* parameters = pdf_->getParameters(dataset_normalisation);
    TIterator* it = parameters->createIterator();
    RooAbsArg* arg = NULL;
    int num_free_parameters = 0;
    while((arg = dynamic_cast<RooAbsArg*>(it->Next()))) {
      RooRealVar* var = dynamic_cast<RooRealVar*>(arg);
      if (var != NULL) {
//        sdebug << *var << endmsg;
        if (!var->isConstant()) {
          num_free_parameters++;
        }
      }
    }
    delete it;
    delete parameters;
        
    // =10^(ln(11)/ln(10)-0.5)
    //plot_frame_pull->SetMinimum(0.5);
    
    plot_frame->SetMinimum(0.0);
    plot_frame->SetMaximum(1.3*plot_frame->GetMaximum());
    if(plot_asymmetry_) {
      plot_frame->SetMinimum(-1.0);
      plot_frame->SetMaximum(1.0);
    }

    // plot_frame->SetMinimum(-1.0);
    // plot_frame->SetMaximum(+1.0);
    
    TString ylabel = plot_frame->GetYaxis()->GetTitle();
    ylabel.ReplaceAll("Events","Candidates");
    if(plot_asymmetry_) ylabel = "Raw mixing Asymmetry";
    plot_frame->GetYaxis()->SetTitle(ylabel);
    
    if (sc_y == kLinear || sc_y == kBoth) {
      doocore::lutils::PlotSimple(plot_name, plot_frame, label, config_plot_.plot_directory(), false);
      doocore::lutils::PlotSimple("AllPlots"+config_plot_.plot_appendix(), plot_frame, label, config_plot_.plot_directory(), false);
    }
    
    plot_frame->SetMinimum(min_plot);
    if (sc_y == kLogarithmic || sc_y == kBoth) {
      doocore::lutils::PlotSimple(log_plot_name, plot_frame, label, config_plot_.plot_directory(), true);
      doocore::lutils::PlotSimple("AllPlots"+config_plot_.plot_appendix(), plot_frame, label, config_plot_.plot_directory(), true);
    }
    
    plot_frame->SetMinimum(0.5);
    plot_frame->SetMaximum(1.3*plot_frame->GetMaximum());
    if(plot_asymmetry_) {
      plot_frame->SetMinimum(-1.0);
      plot_frame->SetMaximum(1.0);
    }
    
//    TString ylabel = plot_frame->GetYaxis()->GetTitle();
//    ylabel.ReplaceAll("Events","Candidates");
//    plot_frame->GetYaxis()->SetTitle(ylabel);
    
    if (sc_y == kLinear) {
      doocore::lutils::PlotPulls(pull_plot_name, plot_frame, label, config_plot_.plot_directory(), false, false, "_gauss", num_free_parameters);
      doocore::lutils::PlotPulls("AllPlots"+config_plot_.plot_appendix(), plot_frame, label, config_plot_.plot_directory(), false, false, "", num_free_parameters);
    } else if (sc_y == kBoth) {
      doocore::lutils::PlotPulls(pull_plot_name, plot_frame, label, config_plot_.plot_directory(), false, false, "nogauss", num_free_parameters);
      doocore::lutils::PlotPulls("AllPlots"+config_plot_.plot_appendix(), plot_frame, label, config_plot_.plot_directory(), false, false, "nogauss", num_free_parameters);
    }
    
//    sdebug << "Plot y axis minimum for log scale plot: " << min_plot << endmsg;
    plot_frame->SetMinimum(min_plot);
    if (sc_y == kLogarithmic || sc_y == kBoth) {
      doocore::lutils::PlotPulls(log_pull_plot_name, plot_frame, label, config_plot_.plot_directory(), true, false, "_gauss", num_free_parameters);
      doocore::lutils::PlotPulls("AllPlots"+config_plot_.plot_appendix(), plot_frame, label, config_plot_.plot_directory(), true, false, "", num_free_parameters);
    }
    
//    delete plot_frame_pull;
  } else {
    plot_frame->SetMinimum(0.0);
    plot_frame->SetMaximum(1.3*plot_frame->GetMaximum());
    if(plot_asymmetry_) {
      plot_frame->SetMinimum(-1.0);
      plot_frame->SetMaximum(1.0);
    }

    TString ylabel = plot_frame->GetYaxis()->GetTitle();
    ylabel.ReplaceAll("Events","Candidates");
    if(plot_asymmetry_) ylabel = "Raw mixing Asymmetry";
    plot_frame->GetYaxis()->SetTitle(ylabel);
    
    if (sc_y == kLinear || sc_y == kBoth) {
      doocore::lutils::PlotSimple(plot_name, plot_frame, label, config_plot_.plot_directory(), false);
      doocore::lutils::PlotSimple("AllPlots"+config_plot_.plot_appendix(), plot_frame, label, config_plot_.plot_directory(), false);
    }
    
    plot_frame->SetMinimum(min_plot);
    if (sc_y == kLogarithmic || sc_y == kBoth) {
      doocore::lutils::PlotSimple(log_plot_name, plot_frame, label, config_plot_.plot_directory(), true);
      doocore::lutils::PlotSimple("AllPlots"+config_plot_.plot_appendix(), plot_frame, label, config_plot_.plot_directory(), true);
    }
  }
  
  if (dataset_reduced != NULL) delete dataset_reduced;
  if (binning != NULL) delete binning;
  delete plot_frame;
}
  
Plot::~Plot() {}

} // namespace plotting
} // namespace doofit
