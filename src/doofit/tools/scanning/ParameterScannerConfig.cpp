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
  (GetOptionString("const_p1").c_str(), po::value<bool>(&const_parameter1_)->default_value(true),"Set parameter 1 constant")
  (GetOptionString("name_p2").c_str(), po::value<std::string>(&name_parameter2_)->default_value(""),"Name of scan parameter 2")
  (GetOptionString("val_p2").c_str(), po::value<double>(&value_parameter2_)->default_value(0.0),"Value of scan parameter 2")
  (GetOptionString("const_p2").c_str(), po::value<bool>(&const_parameter2_)->default_value(true),"Set parameter 2 constant");
  
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
  if (name_parameter2_.length() > 0) {
    scfg << "Scan Parameter 2: " << name_parameter2_ << " = " << value_parameter2_;
    if (const_parameter2_) {
      scfg << " (constant).";
    } else {
      scfg << " (non-constant).";
    }
    scfg << endmsg;
  }
}

void ParameterScannerConfig::SetParameters(doofit::builder::EasyPdf& epdf) const {
  if (name_parameter1_.length() > 0) {
    epdf.Var(name_parameter1_).setVal(value_parameter1_);
    epdf.Var(name_parameter1_).setConstant(const_parameter1_);
  }
  if (name_parameter2_.length() > 0) {
    epdf.Var(name_parameter2_).setVal(value_parameter2_);
    epdf.Var(name_parameter2_).setConstant(const_parameter2_);
  }
}

void ParameterScannerConfig::SetParameters(RooArgSet& argset) const {
  RooRealVar* parameter1 = dynamic_cast<RooRealVar*>(argset.find(name_parameter1_.c_str()));
  if (parameter1 != nullptr) {
    parameter1->setVal(value_parameter1_);
    parameter1->setConstant(const_parameter1_);
  }
  RooRealVar* parameter2 = dynamic_cast<RooRealVar*>(argset.find(name_parameter2_.c_str()));
  if (parameter2 != nullptr) {
    parameter2->setVal(value_parameter2_);
    parameter2->setConstant(const_parameter2_);
  }
}

} // namespace scanning
} // namespace tools
} // namespace doofit
