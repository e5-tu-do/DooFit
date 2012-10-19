#include "doofit/builder/BuilderStd/BuilderStdConfig.h"

// STL
#include <string>
#include <vector>
#include <iostream>

// Boost
#include <boost/foreach.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/info_parser.hpp>


// ROOT

// RooFit

// from project
#include "doocore/io/MsgStream.h"

namespace doofit {
using namespace std;
namespace po = boost::program_options;
namespace pt = boost::property_tree;
using namespace doocore::io;

BuilderStdConfig::BuilderStdConfig(const std::string& name) :
Config::AbsConfig(name)
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

/** BAUSTELLE */
void BuilderStdConfig::load(const std::string &filename){
  pt::ptree pt;
//  pt::read_info(filename, pt);
//  
//  categories_ = to_array<std::string>(pt.get<std::string>("Components"));
//  BOOST_FOREACH(string category, categories_){
//    cout << category << endl;
//  }
}
}
