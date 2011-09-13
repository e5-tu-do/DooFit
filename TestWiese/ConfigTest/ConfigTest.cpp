
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
  (GetOptionString("my-test-switch","s").c_str(), po::value<bool>(&my_test_switch_)->default_value(false),
   "my test bool switch")
  (GetOptionString("my-test-int","i").c_str(), po::value<int>()->default_value(0),
   "my test integer");
  
  descs_visible_.push_back(generic);
}

void ConfigTest::LoadOptions() {
  set_my_test_int(var_map_[GetOptionString("my-test-int")].as<int>());
  
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
  
  for (vector<string>::const_iterator it = my_test_vector_.begin(); it != my_test_vector_.end(); ++it) {
    scfg << " my second test vector: " << *it << endmsg;
  }
}

void ConfigTestSecond::DefineOptions() {
  po::options_description* generic = new po::options_description("Second options");
  generic->add_options()
  (GetOptionString("my-second-test-switch").c_str(), po::value<bool>()->default_value(false),
   "my second test bool switch")
  (GetOptionString("my-second-test-int").c_str(), po::value<int>()->default_value(0),
   "my second test integer")
  (GetOptionString("my-second-test-vector").c_str(), po::value<vector<string> >(&my_test_vector_)->composing());
  
  descs_visible_.push_back(generic);
}

void ConfigTestSecond::LoadOptions() {
  set_my_test_switch(var_map_[GetOptionString("my-second-test-switch")].as<bool>());
  set_my_test_int(var_map_[GetOptionString("my-second-test-int")].as<int>());
}