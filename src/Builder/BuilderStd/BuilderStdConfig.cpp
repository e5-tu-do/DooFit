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
#include "Pdf2WsStdConfig.h"

using namespace std;
namespace po = boost::program_options;

Pdf2WsStdConfig::Pdf2WsStdConfig(const std::string& name) :
Config(name)
{
}

Pdf2WsStdConfig::~Pdf2WsStdConfig() {}

void Pdf2WsStdConfig::PrintOptions() const {
  scfg << "nothing to see here" << endmsg;
}

void Pdf2WsStdConfig::DefineOptions() {
  
}

void Pdf2WsStdConfig::LoadOptions() {
  
}
