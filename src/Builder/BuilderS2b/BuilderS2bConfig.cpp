#include "Builder/BuilderS2b/BuilderS2bConfig.h"

// STL
#include <string>
#include <vector>
#include <iostream>

// Boost
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/info_parser.hpp>


// ROOT

// RooFit

// from project
#include "Utils/MsgStream.h"

using namespace std;
namespace po = boost::program_options;
namespace pt = boost::property_tree;

BuilderS2bConfig::BuilderS2bConfig(const std::string& name) :
Config::AbsConfig(name)
{
}

BuilderS2bConfig::~BuilderS2bConfig() {}

void BuilderS2bConfig::PrintOptions() const {
  scfg << "nothing to see here" << endmsg;
}

void BuilderS2bConfig::DefineOptions() {
  
}

void BuilderS2bConfig::LoadOptions() {
  
}

/** BAUSTELLE */
void BuilderS2bConfig::load(const std::string &filename){
  pt::ptree pt;
}
