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
  
}

Config::Config(int argc, char* argv){
  // 
}

Config::Config(const vector<string>& option_vector){
  
}