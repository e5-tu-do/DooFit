// STL
#include <string>
#include <vector>
#include <iostream>

// Boost
#include <boost/program_options.hpp>

// ROOT

// RooFit

// from project
#include "Utils/MsgStream.h"
#include "CommonConfig.h"

using namespace std;
namespace po = boost::program_options;

CommonConfig::CommonConfig(const std::string& name) :
Config(name)
{
  PrintOptions();
}

CommonConfig::~CommonConfig() {}

void CommonConfig::PrintOptions() const {
  scfg << "nothing to see here" << endmsg;
}

void CommonConfig::DefineOptions() {
  
}

void CommonConfig::LoadOptions() {
  
}
