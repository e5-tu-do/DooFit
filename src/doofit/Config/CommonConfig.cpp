// STL
#include <string>
#include <vector>
#include <iostream>

// Boost
#include <boost/program_options.hpp>

// ROOT

// RooFit

// from project
#include "doocore/io/MsgStream.h"
#include "CommonConfig.h"

namespace doofit {
namespace Config {
using namespace std;
namespace po = boost::program_options;
using namespace doocore::lutils; using namespace doocore::io;

CommonConfig::CommonConfig() :
Config::AbsConfig("empty_name")
{
}

CommonConfig::CommonConfig(const std::string& name) :
Config::AbsConfig(name)
{
}

CommonConfig::~CommonConfig() {}

void CommonConfig::PrintOptions() const {
  scfg << "nothing to see here" << endmsg;
}

void CommonConfig::DefineOptions() {
  po::options_description* generic = new po::options_description("Generic options");
  generic->add_options()
  ("help", "produce help message")
  ("config-file,c", po::value<string>(&config_file_)->default_value(""),
   "config file to parse");
  
  descs_visible_.push_back(generic);
}

void CommonConfig::LoadOptions() {
  if (var_map_.count("help")) help_flag_ = true;
}
} // namespace Config
} // namespace doofit