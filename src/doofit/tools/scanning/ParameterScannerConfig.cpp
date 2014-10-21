#include "doofit/tools/scanning/ParameterScannerConfig.h"

// from STL

// from Boost

// from ROOT

// from RooFit
#include "RooArgSet.h"

// from DooCore
#include <doocore/io/MsgStream.h>

// from project
#include "doofit/builder/EasyPdf/EasyPdf.h"

using namespace std;
namespace po = boost::program_options;
using namespace doocore::io;

namespace doofit {
namespace tools {
namespace scanning {
ParameterScannerConfig::ParameterScannerConfig(const std::string& name)
: config::AbsConfig(name)
{
}
  
ParameterScannerConfig::~ParameterScannerConfig() {
}
    
void ParameterScannerConfig::DefineOptions() {
  po::options_description* generation = new po::options_description("Parameter scanning options");
  
  generation->add_options()
  (GetOptionString("name_p1").c_str(), po::value<std::string>(&name_parameter1_)->default_value(""),"Name of scan parameter 1")
  (GetOptionString("val_p1").c_str(), po::value<double>(&value_parameter1_)->default_value(0.0),"Value of scan parameter 1")
  (GetOptionString("const_p1").c_str(), po::value<bool>(&const_parameter1_)->default_value(true),"Set parameter 1 constant");
  // (GetOptionString("num_cpu").c_str(), po::value<int>(&num_cpu_)->default_value(1),"Number of CPUs for plotting")
  // (GetOptionString("pdf_linecolors").c_str(), po::value<config::CommaSeparatedList<int> >(&pdf_linecolor_map_),"Line colors for plotted PDFs (comma-separated as col1,col2,...)")
  // (GetOptionString("pdf_linestyles").c_str(), po::value<config::CommaSeparatedList<int> >(&pdf_linestyle_map_),"Line styles for plotted PDFs (comma-separated as style1,style2,...)")
  // (GetOptionString("plot_directory").c_str(), po::value<std::string>(&plot_directory_)->default_value("Plot"),"Output directory for plots")
  // (GetOptionString("plot_appendix").c_str(), po::value<std::string>(&plot_appendix_)->default_value(""),"Plot appendix for stacked plots")
  // (GetOptionString("simultaneous_plot_all_categories").c_str(), po::value<bool>(&simultaneous_plot_all_categories_)->default_value(false),"Plot all individual sub categories of a simultaneous PDF (i.e. long,tagged,2011 vs. down,tagged,2011 vs. ...) (default: false)")
  // (GetOptionString("simultaneous_plot_all_slices").c_str(), po::value<bool>(&simultaneous_plot_all_slices_)->default_value(false),"Plot each slice of a simultaneous PDF (i.e. all long, all tagged, etc.) (default: false)");
  
  descs_visible_.push_back(generation);
}
  
void ParameterScannerConfig::LoadOptions() {}
  
void ParameterScannerConfig::PrintOptions() const {
  if (name_parameter1_.length() > 0) {
    scfg << "Scan Parameter 1: " << name_parameter1_ << " = " << value_parameter1_;
    if (const_parameter1_) {
      scfg << " (constant).";
    } else {
      scfg << " (non-constant).";
    }
    scfg << endmsg;
  }

  // scfg << "Number of CPUs: " << num_cpu_ << endmsg;
  // scfg << "PDF line colors: " << pdf_linecolor_map_ << endmsg;
  // scfg << "PDF line styles: " << pdf_linestyle_map_ << endmsg;
  // scfg << "Plot output directory: " << plot_directory() << endmsg;
  // scfg << "Stacked plot file name appendix: " << plot_appendix_ << endmsg;
  // scfg << "Plotting of all individual simultaneous categories: " << simultaneous_plot_all_categories_ << endmsg;
  // scfg << "Plotting of all simultaneous slices: " << simultaneous_plot_all_slices_ << endmsg;
}

void ParameterScannerConfig::SetParameters(doofit::builder::EasyPdf& epdf) const {
  if (name_parameter1_.length() > 0) {
    epdf.Var(name_parameter1_).setVal(value_parameter1_);
    epdf.Var(name_parameter1_).setConstant(const_parameter1_);
  }
}

void ParameterScannerConfig::SetParameters(RooArgSet& argset) const {
  RooRealVar* parameter1 = dynamic_cast<RooRealVar*>(argset.find(name_parameter1_.c_str()));
  if (parameter1 != nullptr) {
    parameter1->setVal(value_parameter1_);
    parameter1->setConstant(const_parameter1_);
  }
}

} // namespace scanning
} // namespace tools
} // namespace doofit
