#include "doofit/plotting/Plot/PlotConfig.h"

// from STL
#include <vector>

// from Boost
#include <boost/assign/std/vector.hpp> // for 'operator+=()'
//#ifdef __GNUG__
//#define BOOST_NO_CXX11_SCOPED_ENUMS
//#endif
#include <boost/filesystem.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/lexical_cast.hpp>

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
  num_cpu_(1),
  plot_stack_open_(false),
  plot_stack_canvas_(NULL),
  plot_appendix_(""),
  simultaneous_plot_all_categories_(false),
  simultaneous_plot_all_slices_(false)
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
  (GetOptionString("num_cpu").c_str(), po::value<int>(&num_cpu_)->default_value(1),"Number of CPUs for plotting")
  (GetOptionString("pdf_linecolors").c_str(), po::value<config::CommaSeparatedList<int> >(&pdf_linecolor_map_),"Line colors for plotted PDFs (comma-separated as col1,col2,...)")
  (GetOptionString("pdf_linestyles").c_str(), po::value<config::CommaSeparatedList<int> >(&pdf_linestyle_map_),"Line styles for plotted PDFs (comma-separated as style1,style2,...)")
  (GetOptionString("plot_directory").c_str(), po::value<std::string>(&plot_directory_)->default_value("Plot"),"Output directory for plots")
  (GetOptionString("plot_appendix").c_str(), po::value<std::string>(&plot_appendix_)->default_value(""),"Plot appendix for stacked plots")
  (GetOptionString("simultaneous_plot_all_categories").c_str(), po::value<bool>(&simultaneous_plot_all_categories_)->default_value(false),"Plot all individual sub categories of a simultaneous PDF (i.e. long,tagged,2011 vs. down,tagged,2011 vs. ...) (default: false)")
  (GetOptionString("simultaneous_plot_all_slices").c_str(), po::value<bool>(&simultaneous_plot_all_slices_)->default_value(false),"Plot each slice of a simultaneous PDF (i.e. all long, all tagged, etc.) (default: false)");
  
  descs_visible_.push_back(generation);
}
  
void PlotConfig::LoadOptions() {}
  
void PlotConfig::PrintOptions() const {
  scfg << "Number of CPUs: " << num_cpu_ << endmsg;
  scfg << "PDF line colors: " << pdf_linecolor_map_ << endmsg;
  scfg << "PDF line styles: " << pdf_linestyle_map_ << endmsg;
  scfg << "Plot output directory: " << plot_directory() << endmsg;
  scfg << "Stacked plot file name appendix: " << plot_appendix_ << endmsg;
  scfg << "Plotting of all individual simultaneous categories: " << simultaneous_plot_all_categories_ << endmsg;
  scfg << "Plotting of all simultaneous slices: " << simultaneous_plot_all_slices_ << endmsg;
}
  
void PlotConfig::OnDemandOpenPlotStack() const {
  if (!plot_stack_open_) {
    boost::uuids::uuid uuid;
    std::string s_uuid = "canvas_" + boost::lexical_cast<std::string>(uuid);
    
    namespace fs = boost::filesystem;
    plot_stack_canvas_ = new TCanvas(s_uuid.c_str(), "dummy_canvas", 900, 900);
    doocore::config::Summary::GetInstance().AddFile(fs::path(plot_directory()) / fs::path("pdf/AllPlots"+plot_appendix_+".pdf"));
    doocore::lutils::printPlotOpenStack(plot_stack_canvas_, "AllPlots"+plot_appendix_, plot_directory());
    
    plot_stack_open_ = true;
  }
}
  
void PlotConfig::ClosePlotStack() const {
  if (plot_stack_open_) {
    doocore::lutils::printPlotCloseStack(plot_stack_canvas_, "AllPlots"+plot_appendix_, plot_directory());
    delete plot_stack_canvas_;
    plot_stack_open_ = false;
  }
}

} // namespace plotting
} // namespace doofit
