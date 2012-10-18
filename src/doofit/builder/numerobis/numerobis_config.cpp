#include "doofit/builder/numerobis/numerobis_config.h"

// from project
#include "doocore/io/MsgStream.h"

namespace doofit {
namespace builder {
namespace numerobis {

using namespace std;
namespace po = boost::program_options;
using namespace doocore::lutils; using namespace doocore::io;

NumerobisConfig::NumerobisConfig(const std::string& name) :
    Config::AbsConfig(name),
    pdf_config_file_("")
{
}

NumerobisConfig::~NumerobisConfig() {
}

void NumerobisConfig::PrintOptions() const {
  if (pdf_config_file_.size() > 0) {
    scfg << "PDF property tree config file: " << pdf_config_file_ << endmsg;
  }
}

void NumerobisConfig::DefineOptions() {
  po::options_description* numerobis_desc = new po::options_description("Numerobis Builder Options");
  
  numerobis_desc->add_options()
  (GetOptionString("pdf_config_file").c_str(), po::value<std::string>(&pdf_config_file_), "  Path to pdf config file (property tree).");
  
  descs_visible_.push_back(numerobis_desc);
}

void NumerobisConfig::LoadOptions() {
}

} // namespace numerobis 
} // namespace builder
} // namespace doofit