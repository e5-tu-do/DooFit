#include "doofit/plotting/Plot/Plot.h"

// STL
#include <string>
#include <sstream>

// boost

// ROOT

// from RooFit
#include "RooArgList.h"
#include "RooAbsRealLValue.h"
#include "RooAbsData.h"
#include "RooAbsPdf.h"
#include "RooPlot.h"

// from Project
#include "doocore/io/MsgStream.h"
#include "doocore/lutils/lutils.h"
#include "doofit/plotting/Plot/PlotConfig.h"

using namespace ROOT;
using namespace RooFit;
using namespace doocore::io;

namespace doofit {
namespace plotting {
Plot::Plot(const PlotConfig& cfg_plot, const RooAbsRealLValue& dimension, const RooAbsData& dataset, const RooArgList& pdfs, const std::string& plot_name)
: config_plot_(cfg_plot),
  dimension_(dimension),
  dataset_(dataset),
  pdfs_(pdfs),
  plot_name_(plot_name),
  plot_dir_("Plot/")
{
  if (&dimension_ == NULL) {
    serr << "Plot::Plot(): Dimension is invalid." << endmsg;
    throw 1;
  }
  if (&dataset_ == NULL) {
    serr << "Plot::Plot(): Dataset is invalid." << endmsg;
    throw 1;
  }
  if (plot_name_ == "") {
    plot_name_ = dimension_.GetName();
  }
}

void Plot::PlotHandler(bool logy, const std::string& suffix) const {
  std::stringstream plot_name_sstr;
  plot_name_sstr << plot_name_ << suffix;
  std::string plot_name = plot_name_sstr.str();
  
  std::stringstream pull_plot_sstr;
  pull_plot_sstr << plot_name << "_pull";
  std::string pull_plot_name = pull_plot_sstr.str(); 

  sinfo << "Plotting " << dimension_.GetName() << " into " << plot_dir_ << plot_name << endmsg;
  
  doocore::lutils::setStyle("LHCb");
  
  RooCmdArg range_arg;
  if (!dimension_.hasMin() && !dimension_.hasMax()) {
    double min, max;
    RooRealVar* dimension_non_const = const_cast<RooRealVar*>(dynamic_cast<const RooRealVar*>(&dimension_));
    dataset_.getRange(*dimension_non_const, min, max);
    
    range_arg = Range(min, max);
  }
  RooPlot* plot_frame = dimension_.frame(range_arg);
  
  dataset_.plotOn(plot_frame);
  
  if (pdfs_.getSize() > 0) {
    const RooAbsPdf* pdf = dynamic_cast<RooAbsPdf*>(pdfs_.first());
    if (pdf == NULL) {
      serr << "Plot::PlotIt(): PDF not valid." << endmsg;
      throw 1;
    }
    RooPlot* plot_frame_pull = dimension_.frame(range_arg);
    dataset_.plotOn(plot_frame_pull);
    
    for (int i=1; i<pdfs_.getSize(); ++i) {
      const RooAbsPdf* sub_pdf = dynamic_cast<RooAbsPdf*>(pdfs_.at(i));
      if (pdf == NULL) {
        serr << "Plot::PlotIt(): Sub PDF number " << i <<  " not valid." << endmsg;
        throw 1;
      }
      pdf->plotOn(plot_frame, Components(sub_pdf->GetName()), Name(sub_pdf->GetName()), LineColor(config_plot_.GetPdfLineColor(i)), LineStyle(config_plot_.GetPdfLineStyle(i)));
      pdf->plotOn(plot_frame_pull, Components(sub_pdf->GetName()), Name(sub_pdf->GetName()), LineColor(config_plot_.GetPdfLineColor(i)), LineStyle(config_plot_.GetPdfLineStyle(i)));
    }
      
    pdf->plotOn(plot_frame, LineColor(config_plot_.GetPdfLineColor(0)), LineStyle(config_plot_.GetPdfLineStyle(0)));
    pdf->plotOn(plot_frame_pull, LineColor(config_plot_.GetPdfLineColor(0)), LineStyle(config_plot_.GetPdfLineStyle(0)));
    plot_frame_pull->SetMinimum(0.5);
    doocore::lutils::PlotResiduals(pull_plot_name, plot_frame_pull, &dimension_, NULL, plot_dir_, true, logy);
    
    delete plot_frame_pull;
  }
  plot_frame->SetMinimum(0.5);
  doocore::lutils::PlotSimple(plot_name, plot_frame, &dimension_, plot_dir_, logy);
  
  delete plot_frame;
}
  
Plot::~Plot() {}

} // namespace plotting
} // namespace doofit
