#include "doofit/plotting/Plot/PlotConfig.h"

// from STL
#include <vector>

// from Boost
#include <boost/assign/std/vector.hpp> // for 'operator+=()'

// from ROOT
#include "TCanvas.h"

// from DooCore
#include <doocore/io/MsgStream.h>
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
  if (plot_stack_open_) {
    doocore::lutils::printPlotCloseStack(plot_stack_canvas_, "AllPlots", plot_directory());
    delete plot_stack_canvas_;
  }
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
//  (GetOptionString("fit_result2_branch_name").c_str(), po::value<std::string>(&fit_result2_branch_name_)->default_value("fit_results2"),"Fit result 2 branch name in tree")
//  (GetOptionString("read_results_filename_treename").c_str(), po::value<vector<config::CommaSeparatedPair> >(&read_results_filename_treename_)->composing(), "File names and tree names to read fit results from (set as filename,treename)")
//  (GetOptionString("read_results_filename_treename_pattern").c_str(), po::value<config::CommaSeparatedPair>(&read_results_filename_treename_pattern_),"File name pattern and tree name to read fit result from (set as regexfilenamepattern,treename)")
//  (GetOptionString("plot_directory").c_str(), po::value<std::string>(&plot_directory_), "Plot directory for evaluation of fit results")
//  (GetOptionString("handle_asymmetric_errors").c_str(), po::value<bool>(&handle_asymmetric_errors_)->default_value(true),"Set to false to not use asymmetric errors for pull calculation (c.f. CDF/ANAL/PUBLIC/5776). Default is true. If unsure, use asymmetric errors.")
//  (GetOptionString("fit_plot_on_quantile_window").c_str(), po::value<bool>(&fit_plot_on_quantile_window_)->default_value(true),"Fit and plot pulls and other distributions on a sensible quantile based window instead of the full dataset (default: true; use this to avoid influence of pull values of obviously failed fits and other outliers).")
//  (GetOptionString("neglect_parameters_at_limit").c_str(), po::value<bool>(&neglect_parameters_at_limit_)->default_value(false),"Neglect any toy fit where at least one parameter is near the defined limits (default: false).")
//  (GetOptionString("neglect_minos_problems").c_str(), po::value<bool>(&neglect_minos_problems_)->default_value(false),"Neglect any toy fit where at least one parameter has MINOS problems (default: false; only applies, if MINOS was run).")
//  (GetOptionString("parameter_genvalue_read_file").c_str(), po::value<std::string>(&parameter_genvalue_read_file_),"Read in generation values from this file instead of using the init values in the fit results (default: empty, i.e. use init values).");
  
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
    plot_stack_canvas_ = new TCanvas("dummy_canvas", "dummy_canvas", 900, 900);
    doocore::lutils::printPlotOpenStack(plot_stack_canvas_, "AllPlots", plot_directory());
    plot_stack_open_ = true;
  }
}

} // namespace plotting
} // namespace doofit
