#include "Builder/BuilderStd/BuilderStdConfig.h"

// STL
#include <string>
#include <vector>
#include <iostream>

// Boost

// ROOT

// RooFit

// from project
#include "Utils/MsgStream.h"

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
