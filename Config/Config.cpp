
// STL
#include <string>
#include <vector>
#include <iostream>
#include <fstream>

#include "Config/Config.h"

using namespace std;
namespace po = boost::program_options;

boost::program_options::options_description Config::desc_visible_all_;
std::vector<Config*> Config::config_container_;

Config::Config() :
  name_("Name"),
  desc_(),
  var_map_(),
  desc_visible_(),
  descs_visible_(),
  desc_hidden_(),
  descs_hidden_(),
  unrec_options_(),
  config_file_()
{
  config_container_.push_back(this);
}

Config::~Config() {}

void Config::InitializeOptions(int argc, char* argv[]) {
  DefineOptions();
  CombineOptions();
  
  ParseOptionsAndConfigFile(po::command_line_parser(argc, argv));
  
  LoadOptions();
}

void Config::InitializeOptions(const Config& previous_config) {
  DefineOptions();
  CombineOptions();
  
  config_file_ = previous_config.config_file_;
  ParseOptionsAndConfigFile(po::command_line_parser(previous_config.unrec_options()));
  
  LoadOptions();
}

void Config::PrintHelp() {
  cout << desc_visible_all_ << endl;
}

void Config::CombineOptions() {
  for (vector<po::options_description*>::const_iterator it = descs_visible_.begin(); it < descs_visible_.end(); ++it) {
    desc_visible_.add(**it);
  }
  
  for (vector<po::options_description*>::const_iterator it = descs_hidden_.begin(); it < descs_hidden_.end(); ++it) {
    desc_hidden_.add(**it);
  }
  
  desc_.add(desc_visible_).add(desc_hidden_);
  desc_visible_all_.add(desc_visible_);
}

void Config::ParseOptionsAndConfigFile(boost::program_options::command_line_parser parser) {
  po::parsed_options parsed = parser.options(desc_).allow_unregistered().run();
  po::store(parsed, var_map_);
  po::notify(var_map_);
  unrec_options_ = po::collect_unrecognized(parsed.options, po::exclude_positional);
  
  if (config_file_.length() > 0) {
    ifstream ifs(config_file_.c_str());
    if (!ifs) {
      cout << "can not open config file: " << config_file_ << "\n";
    } else {
      po::parsed_options parsed = po::parse_config_file(ifs, desc_, true);
      po::store(parsed, var_map_);
      po::notify(var_map_);
    }
  }
}