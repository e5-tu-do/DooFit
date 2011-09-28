#include "Config/Config.h"

// from STL
#include <string>
#include <vector>
#include <iostream>
#include <fstream>

// from boost
#include <boost/exception/all.hpp>
#include <boost/tokenizer.hpp>

// from Project
#include "Utils/MsgStream.h"


using namespace std;
namespace po = boost::program_options;

ConfigMap Config::config_container_;
unsigned int Config::id_counter_ = 0;

Config::Config(const std::string& name) :
  name_(name),
  var_map_(),
  descs_visible_(),
  descs_hidden_(),
  unrec_options_(),
  config_file_(),
  help_flag_(false),
  id_(id_counter_++),
  initialized_(false)
{
  if (config_container_.count(id_) > 0) {
    serr << "ERROR in Config::Config(const std::string&): Config object with ID " << id_ << " already existing." << endmsg;
    throw ConfigNameDuplicationException() << ConfigName(name_); 
  }
  
  config_container_[id_] = this;
}

Config::Config(const Config& other) :
  name_(other.name_),
  var_map_(other.var_map_),
  descs_visible_(),
  descs_hidden_(),
  unrec_options_(other.unrec_options_),
  config_file_(other.config_file_),
  help_flag_(other.help_flag_),
  id_(id_counter_++),
  initialized_(other.initialized_)
{
  // copy dynamically allocated objects
  for (vector<po::options_description*>::const_iterator it = other.descs_visible_.begin(); it < other.descs_visible_.end(); ++it) {
    descs_visible_.push_back(new boost::program_options::options_description(**it));
  }
  
  for (vector<po::options_description*>::const_iterator it = other.descs_hidden_.begin(); it < other.descs_hidden_.end(); ++it) {
    descs_hidden_.push_back(new boost::program_options::options_description(**it));
  }
  
  config_container_[id_] = this;
}

Config::~Config() {
  config_container_.erase(id_);
  
  // delete all dynamically allocated objects
  for (vector<po::options_description*>::const_iterator it = descs_visible_.begin(); it < descs_visible_.end(); ++it) {
    delete *it;
  }
  
  for (vector<po::options_description*>::const_iterator it = descs_hidden_.begin(); it < descs_hidden_.end(); ++it) {
    delete *it;
  }
}

Config& Config::operator=(const Config& other) {
  if (this != &other) {
    name_ = other.name_;
    var_map_ = other.var_map_;
    //descs_visible_ = other.descs_visible_;
    //descs_hidden_ = other.descs_hidden_;
    unrec_options_ = other.unrec_options_;
    config_file_ = other.config_file_;
    help_flag_ = other.help_flag_;
    // id should stay the same
    
    // delete our dynamically allocated objects...
    for (vector<po::options_description*>::const_iterator it = descs_visible_.begin(); it < descs_visible_.end(); ++it) {
      delete *it;
    }
    
    for (vector<po::options_description*>::const_iterator it = descs_hidden_.begin(); it < descs_hidden_.end(); ++it) {
      delete *it;
    }
    
    // ...clear vectors holding them...
    descs_visible_.clear();
    descs_hidden_.clear();
    
    // ...and copy dynamically allocated objects from other
    for (vector<po::options_description*>::const_iterator it = other.descs_visible_.begin(); it < other.descs_visible_.end(); ++it) {
      descs_visible_.push_back(new boost::program_options::options_description(**it));
    }
    
    for (vector<po::options_description*>::const_iterator it = other.descs_hidden_.begin(); it < other.descs_hidden_.end(); ++it) {
      descs_hidden_.push_back(new boost::program_options::options_description(**it));
    }
  }
  return *this;
}

void Config::InitializeOptions(int argc, char* argv[]) {
  if (!initialized_) {
    DefineOptions();
    
    ParseOptionsAndConfigFile(po::command_line_parser(argc, argv));
    
    LoadOptions();
    
    initialized_ = true;
  } else {
    throw ConfigAlreadyInitializedException() << ConfigName(name_);
  }
}

void Config::InitializeOptions(const Config& previous_config) {
  if (!initialized_) {
    DefineOptions();
    
    config_file_ = previous_config.config_file_;
    ParseOptionsAndConfigFile(po::command_line_parser(previous_config.unrec_options_));
    
    LoadOptions();
    
    initialized_ = true;
  } else {
    throw ConfigAlreadyInitializedException() << ConfigName(name_);
  }
}

void Config::Print() const {
  scfg.Ruler();
  scfg << "Config object " << name_ << ":" << endmsg;
  scfg.set_indent(2);
  PrintOptions();
  scfg.set_indent(0);
}

void Config::PrintAll() {
  for (ConfigMap::const_iterator it = config_container_.begin(); it != config_container_.end(); ++it) {
    scfg << endmsg;
    (*it).second->Print();
  }
}

void Config::PrintHelp() const {
  boost::program_options::options_description desc_visible_all;
  
  for (ConfigMap::const_iterator it = config_container_.begin(); it != config_container_.end(); ++it) {
    
    // @todo finish
    
    desc_visible_all.add((*it).second->GetAllVisibleOptionsDescriptions());
  }
  
  cout << desc_visible_all << endl;
}

std::string Config::GetOptionString(std::string option_name, std::string short_option) const {
  std::string option;
  
  option = name_ + "." + option_name;
  
  if (short_option.length() > 0) {
    option = option + "," + short_option;
  }
  
  return option;
}

boost::program_options::options_description Config::GetAllVisibleOptionsDescriptions() const {
  boost::program_options::options_description desc_visible;
  
  for (vector<po::options_description*>::const_iterator it = descs_visible_.begin(); it < descs_visible_.end(); ++it) {
    desc_visible.add(**it);
  }
  
  return desc_visible;
}

boost::program_options::options_description Config::GetAllOptionsDescriptions() const {
  boost::program_options::options_description desc;
  boost::program_options::options_description desc_hidden;
  
  for (vector<po::options_description*>::const_iterator it = descs_hidden_.begin(); it < descs_hidden_.end(); ++it) {
    desc_hidden.add(**it);
  }
  
  desc.add(GetAllVisibleOptionsDescriptions()).add(desc_hidden);
  
  return desc;
}

void Config::ParseOptionsAndConfigFile(boost::program_options::command_line_parser parser) {
  try {
    boost::program_options::options_description desc;
    
    desc.add(GetAllOptionsDescriptions());
    
    po::parsed_options parsed = parser.options(desc).allow_unregistered().run();
    po::store(parsed, var_map_);
    po::notify(var_map_);
    unrec_options_ = po::collect_unrecognized(parsed.options, po::exclude_positional);
    
    if (config_file_.length() > 0) {
      ifstream ifs(config_file_.c_str());
      if (!ifs) {
        serr << "ERROR in Config::ParseOptionsAndConfigFile(boost::program_options::command_line_parser): Can not open config file: " << config_file_ << endmsg;
      } else {
        po::parsed_options parsed = po::parse_config_file(ifs, desc, true);
        po::store(parsed, var_map_);
        po::notify(var_map_);
      }
    }
  } catch (exception& e) {
    // in case of parsing problems give an error message and throw exception further
    serr << "ERROR in Config::ParseOptionsAndConfigFile(boost::program_options::command_line_parser): Parsing problem in Config " << name_ << ": " << e.what() << endmsg;
    throw e;
  }
}



