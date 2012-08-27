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

// from Project
#include "doofit/utils/MsgStream.h"
#include "doofit/utils/utils.h"

using namespace ROOT;
using namespace RooFit;
using namespace doofit::utils;

namespace doofit {
namespace plotting {
Plot::Plot(const RooAbsRealLValue& dimension, const RooAbsData& dataset, const RooArgList& pdfs, const std::string& plot_name, const std::string plot_dir) 
: dimension_(dimension),
  dataset_(dataset),
  pdfs_(pdfs),
  plot_name_(plot_name),
  plot_dir_(plot_dir)
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
  
void Plot::PlotIt() const {
  sinfo << "Plotting " << dimension_.GetName() << " into " << plot_dir_ << plot_name_ << endmsg;
  std::stringstream pull_plot_sstr;
  pull_plot_sstr << plot_name_ << "_pull";
  std::string pull_plot_name = pull_plot_sstr.str(); 
  
  utils::setStyle();
  RooPlot* plot_frame = dimension_.frame();
  dataset_.plotOn(plot_frame);
  
  if (pdfs_.getSize() > 0) {
    const RooAbsPdf* pdf = dynamic_cast<RooAbsPdf*>(pdfs_.first());
    if (pdf == NULL) {
      serr << "Plot::PlotIt(): PDF not valid." << endmsg;
      throw 1;
    }
    RooPlot* plot_frame_pull = dimension_.frame();
    dataset_.plotOn(plot_frame_pull);
    pdf->plotOn(plot_frame);
    pdf->plotOn(plot_frame_pull);
    utils::PlotResiduals(pull_plot_name, plot_frame_pull, &dimension_, NULL, plot_dir_, true);
    
    delete plot_frame_pull;
  }
  utils::PlotSimple(plot_name_, plot_frame, &dimension_, plot_dir_);
  
  delete plot_frame;
}
  
Plot::~Plot() {}

} // namespace plotting
} // namespace doofit
