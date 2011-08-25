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
#include "BuilderStdConfig.h"

using namespace std;
namespace po = boost::program_options;

BuilderStdConfig::BuilderStdConfig(const std::string& name) :
Config(name)
{
}

BuilderStdConfig::~BuilderStdConfig() {}

void BuilderStdConfig::PrintOptions() const {
  scfg << "nothing to see here" << endmsg;
}

void BuilderStdConfig::DefineOptions() {
  
}

void BuilderStdConfig::LoadOptions() {
  
}
