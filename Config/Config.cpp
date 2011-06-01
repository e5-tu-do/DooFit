#include "Config/Config.h"

using namespace std;
namespace po = boost::program_options;

Config::Config() :
  name_("Name"),
  desc_(),
  var_map_(),
  desc_visible_(),
  descs_visible_(),
  desc_hidden_(),
  descs_hidden_(),
  unrec_options_()
{
  DefineOptions();
  CombineOptions();
}

Config::Config(int argc, char* argv[]) :
  name_("Name"),
  desc_(),
  var_map_(),
  desc_visible_(),
  descs_visible_(),
  desc_hidden_(),
  descs_hidden_(),
  unrec_options_()
{
  DefineOptions();
  CombineOptions();
  
  po::parsed_options parsed = po::command_line_parser(argc, argv).options(desc_).allow_unregistered().run();
  po::store(parsed, var_map_);
  notify(var_map_);
  unrec_options_ = po::collect_unrecognized(parsed.options, po::exclude_positional);
}

Config::Config(const vector<string>& option_vector) :
  name_("Name"),
  desc_(),
  var_map_(),
  desc_visible_(),
  descs_visible_(),
  desc_hidden_(),
  descs_hidden_(),
  unrec_options_()
{
  DefineOptions();
  CombineOptions();
  
  po::parsed_options parsed = po::command_line_parser(option_vector).options(desc_).allow_unregistered().run();
  po::store(parsed, var_map_);
  notify(var_map_);
  unrec_options_ = po::collect_unrecognized(parsed.options, po::exclude_positional);
}

void Config::CombineOptions() {
  for (vector<po::options_description>::const_iterator it = descs_visible_.begin(); it < descs_visible_.end(); ++it) {
    desc_visible_.add(*it);
  }
  
  for (vector<po::options_description>::const_iterator it = descs_hidden_.begin(); it < descs_hidden_.end(); ++it) {
    desc_hidden_.add(*it);
  }
  
  desc_.add(desc_visible_).add(desc_hidden_);
}
