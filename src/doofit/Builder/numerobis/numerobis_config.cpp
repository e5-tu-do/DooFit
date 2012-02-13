#include "doofit/Builder/numerobis/numerobis_config.h"

// from project
#include "doofit/utils/MsgStream.h"

namespace doofit {
namespace builder {
namespace numerobis {

using namespace std;
namespace po = boost::program_options;
using namespace doofit::utils;

NumerobisConfig::NumerobisConfig(const std::string& name) :
    Config::AbsConfig(name),
    ptree_filename_("")
{
}

NumerobisConfig::~NumerobisConfig() {
}

void NumerobisConfig::PrintOptions() const {
  if (ptree_filename_.size() > 0) {
    scfg << "PDF property tree config file: " << ptree_filename_ << endmsg;
  }
}

void NumerobisConfig::DefineOptions() {
  po::options_description* numerobis_desc = new po::options_description("Numerobis Builder Options");
  
  numerobis_desc->add_options()
  (GetOptionString("config-file").c_str(), po::value<std::string>(&ptree_filename_), "  Path to pdf config file (property tree).");
  
  descs_visible_.push_back(numerobis_desc);
}

void NumerobisConfig::LoadOptions() {
}

} // namespace numerobis 
} // namespace builder
} // namespace doofit