
// STL
#include <string>
#include <vector>
#include <iostream>

// Boost
#include <boost/program_options.hpp>

// ROOT

// RooFit

// from project
#include "ConfigTest.h"
#include "Utils/MsgStream.h"

using namespace std;
namespace po = boost::program_options;

ConfigTest::ConfigTest(const std::string& name) :
Config(name),
my_test_switch_(false),
my_test_int_(0)
{
}

ConfigTest::~ConfigTest() {}

void ConfigTest::PrintOptions() const {
  scfg << "my test switch (internal): " << my_test_switch_ << endmsg;
  scfg << "my test integer (internal): " << my_test_int_ << endmsg;
}

void ConfigTest::DefineOptions() {
  po::options_description* generic = new po::options_description("Generic options");
  generic->add_options()
  ("help", "produce help message")
  ("config-file", po::value<string>(&config_file_)->default_value(""),
   "config file to parse")
  ("my-test-switch,s", po::value<bool>(&my_test_switch_)->default_value(false),
   "my test bool switch")
  ("my-test-int,i", po::value<int>()->default_value(0),
   "my test integer");
  
  descs_visible_.push_back(generic);
}

void ConfigTest::LoadOptions() {
  set_my_test_int(var_map_["my-test-int"].as<int>());
  
  if (var_map_.count("help")) help_flag_ = true;
}

ConfigTestSecond::ConfigTestSecond(const std::string& name) :
Config(name),
my_test_switch_(false),
my_test_int_(0)
{
}

ConfigTestSecond::~ConfigTestSecond() {}

void ConfigTestSecond::PrintOptions() const {
  scfg << "my second test switch (internal): " << my_test_switch_ << endmsg;
  scfg << "my second test integer (internal): " << my_test_int_ << endmsg;
  scfg << "my second test vector (internal): " << my_test_vector_.size() << endmsg;
}

void ConfigTestSecond::DefineOptions() {
  po::options_description* generic = new po::options_description("Second options");
  generic->add_options()
  ("my-second-test-switch", po::value<bool>()->default_value(false),
   "my second test bool switch")
  ("my-second-test-int", po::value<int>()->default_value(0),
   "my second test integer")
  ("my-second-test-vector", po::value<vector<string> >(&my_test_vector_)->composing());
  
  descs_visible_.push_back(generic);
}

void ConfigTestSecond::LoadOptions() {
  set_my_test_switch(var_map_["my-second-test-switch"].as<bool>());
  set_my_test_int(var_map_["my-second-test-int"].as<int>());
}