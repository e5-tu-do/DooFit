#include "Config/Config.h"

using namespace std;
namespace po = boost::program_options;

Config::Config() :
  name_("Name"),
  desc_(),
  var_map_(),
  descs_(),
  desc_hidden_(),
  var_map_hidden(),
  descs_hidden_(),
  unrec_options_()
{
  DefineOptions();
  CombineOptions();
}

Config::Config(int argc, char* argv){
  DefineOptions();
  CombineOptions();
  
  /// \todo Add a clever way to parse the config file. How to specify what the
  ///       config file is without messing with the program_options objects 
  ///       itself too much or requiring anything...
}

Config::Config(const vector<string>& option_vector){
  DefineOptions();
  CombineOptions();
}

Config::CombineOptions() {
  for (vector<po::options_description>::const_iterator it = descs_.begin(); it < descs_.end(); ++it) {
    desc_.add(*it);
  }
  
  for (vector<po::options_description>::const_iterator it = descs_hidden_.begin(); it < descs_hidden_.end(); ++it) {
    desc_hidden_.add(*it);
  }
}