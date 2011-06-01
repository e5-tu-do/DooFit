
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

void ConfigTest::Print() {
  if (var_map_.count("help")) {
    cout << desc_visible_ << "\n";
  }
  if (var_map_.count("my-test-switch")) {
    cout << "my test switch: " << var_map_["my-test-switch"].as<bool>() << "\n";
  }
  if (var_map_.count("my-test-int")) {
    cout << "my test integer: " << var_map_["my-test-int"].as<int>() << "\n";
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