#include "doofit/plotting/Plot/PlotConfig.h"

// from STL
#include <vector>

// from Boost
#include <boost/assign/std/vector.hpp> // for 'operator+=()'
#ifdef __GNUG__
#define BOOST_NO_CXX11_SCOPED_ENUMS
#endif
#include <boost/filesystem.hpp>

// from ROOT
#include "TCanvas.h"

// from DooCore
#include <doocore/io/MsgStream.h>
#include <doocore/config/Summary.h>
#include <doocore/lutils/lutils.h>

// from project
#include "doofit/config/CommaSeparatedList.h"

using namespace std;
namespace po = boost::program_options;
using namespace doocore::io;
using namespace boost::assign; // bring 'operator+=()' into scope

namespace doofit {
namespace plotting {
PlotConfig::PlotConfig(const std::string& name)
: config::AbsConfig(name),
  plot_directory_("Plot"),
  plot_stack_open_(false),
  plot_stack_canvas_(NULL)
{
  pdf_linecolor_map_.Parse("1,214,210,226,222,206,217,94,138,220");
  pdf_linestyle_map_.Parse("1,2,3,4,5,6,7,8,9,10");
}
  
PlotConfig::~PlotConfig() {
  ClosePlotStack();
}
  
int PlotConfig::GetPdfLineColor(int index) const {
  if (index >= pdf_linecolor_map_.size()) {
    return 1;
  } else {
    return pdf_linecolor_map_[index];
  }
}
  
int PlotConfig::GetPdfLineStyle(int index) const {
  if (index >= pdf_linestyle_map_.size()) {
    return 1;
  } else {
    return pdf_linestyle_map_[index];
  }
}
  
void PlotConfig::DefineOptions() {
  po::options_description* generation = new po::options_description("Plot style options");
  
  generation->add_options()
  (GetOptionString("pdf_linecolors").c_str(), po::value<config::CommaSeparatedList<int> >(&pdf_linecolor_map_),"Line colors for plotted PDFs (comma-separated as col1,col2,...)")
  (GetOptionString("pdf_linestyles").c_str(), po::value<config::CommaSeparatedList<int> >(&pdf_linestyle_map_),"Line styles for plotted PDFs (comma-separated as style1,style2,...)")
  (GetOptionString("plot_directory").c_str(), po::value<std::string>(&plot_directory_)->default_value("Plot"),"Output directory for plots");
  
  descs_visible_.push_back(generation);
}
  
void PlotConfig::LoadOptions() {}
  
void PlotConfig::PrintOptions() const {
  scfg << "PDF line colors: " << pdf_linecolor_map_ << endmsg;
  scfg << "PDF line styles: " << pdf_linestyle_map_ << endmsg;
  scfg << "Plot output directory: " << plot_directory() << endmsg;
}
  
void PlotConfig::OnDemandOpenPlotStack() const {
  if (!plot_stack_open_) {
    namespace fs = boost::filesystem;
    plot_stack_canvas_ = new TCanvas("dummy_canvas", "dummy_canvas", 900, 900);
    doocore::config::Summary::GetInstance().AddFile(fs::path(plot_directory()) / fs::path("pdf/AllPlots.pdf"));
    doocore::lutils::printPlotOpenStack(plot_stack_canvas_, "AllPlots", plot_directory());
    
    plot_stack_open_ = true;
  }
}
  
void PlotConfig::ClosePlotStack() const {
  if (plot_stack_open_) {
    doocore::lutils::printPlotCloseStack(plot_stack_canvas_, "AllPlots", plot_directory());
    delete plot_stack_canvas_;
    plot_stack_open_ = false;
  }
}

} // namespace plotting
} // namespace doofit
