
// STL
#include <string>
#include <vector>
#include <map>
#include <iostream>

// Boost
#include <boost/program_options.hpp>

// ROOT

// RooFit

// from project
#include "ConfigTest.h"
#include "DooFit/Utils//MsgStream.h"

using namespace std;
namespace po = boost::program_options;

namespace DooFit {
ConfigTest::ConfigTest(const std::string& name) :
Config::AbsConfig(name),
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
Config::AbsConfig(name),
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
  
  for (vector<ConfigTestAbstractType>::const_iterator it = my_abstract_type_.begin(); it != my_abstract_type_.end(); ++it) {
    scfg << "my abstract: " << *it << endmsg;
  }
}

void ConfigTestSecond::DefineOptions() {
  po::options_description* generic = new po::options_description("Second options");
  generic->add_options()
  (GetOptionString("my-second-test-switch").c_str(), po::value<bool>()->default_value(false),
   "my second test bool switch")
  (GetOptionString("my-second-test-int").c_str(), po::value<int>()->default_value(0),
   "my second test integer")
  (GetOptionString("my-abstract").c_str(), po::value<vector<ConfigTestAbstractType> >(&my_abstract_type_)->composing(), "Abstract test object with a simple string and a map of keys and values (both string). The string representation of this object is string,key,value,key,value,...")
  (GetOptionString("my-second-test-vector").c_str(), po::value<vector<string> >(&my_test_vector_)->composing(), "some simple string vector");
  
  descs_visible_.push_back(generic);
}

void ConfigTestSecond::LoadOptions() {
  set_my_test_switch(var_map_[GetOptionString("my-second-test-switch")].as<bool>());
  set_my_test_int(var_map_[GetOptionString("my-second-test-int")].as<int>());
}

void ConfigTestAbstractType::Parse(string str) {
  vector<string> elements = DecomposeString(str);
  
  // number of elements must be odd (one simple and several pairs)
  if (elements.size()%2 != 1) {
    serr << "ERROR in ConfigTestAbstractType::Parse(std::string): String '" << str << "' ends prematurely." << endmsg;
    throw;
  }
  
  vector<string>::iterator it=elements.begin();
  
  simple_ = *(it);
  ++it;
  
  while (it!=elements.end()) {
    map_[*it] = *(++it);
    ++it;
  }
}

void ConfigTestAbstractType::Print(std::ostream& os) const {
  os << "simple: " << simple_;
  for (std::map<string,string>::const_iterator itmap=map_.begin(); itmap!=map_.end(); ++itmap) {
    os << "; k: " << (*itmap).first << ", v: " << (*itmap).second;
  }
}
} // namspace DooFit
