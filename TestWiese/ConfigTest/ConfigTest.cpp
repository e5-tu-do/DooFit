
// STL
#include <string>
#include <vector>
#include <iostream>

// Boost
#include <boost/program_options.hpp>

// ROOT

// RooFit

// from project
#include "TestWiese/ConfigTest/ConfigTest.h"

using namespace std;
namespace po = boost::program_options;

ConfigTest::ConfigTest() :
Config(),
my_test_switch_(false),
my_test_int_(0)
{
}

ConfigTest::~ConfigTest() {}

void ConfigTest::Print() {
  if (var_map_.count("help")) {
    cout << desc_visible_ << "\n";
  }
   
  cout << "my test switch (internal): ";
  if (my_test_switch_) {
    cout << "true";
  } else {
    cout << "false";
  }
  cout << endl;
  
  cout << "my test integer (internal): " << my_test_int_ << endl;
}

void ConfigTest::DefineOptions() {
  po::options_description* generic = new po::options_description("Generic options");
  generic->add_options()
  ("help", "produce help message")
  ("my-test-switch,s", po::value<bool>()->default_value(false),
   "my test bool switch")
  ("my-test-int,i", po::value<int>()->default_value(0),
   "my test integer");
  
  descs_visible_.push_back(generic);
}

void ConfigTest::LoadOptions() {
  set_my_test_switch(var_map_["my-test-switch"].as<bool>());
  set_my_test_int(var_map_["my-test-int"].as<int>());
}