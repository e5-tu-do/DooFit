#include "ToyFactory/ToyFactoryStd/ToyFactoryStdConfig.h"

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

ToyFactoryStdConfig::ToyFactoryStdConfig(const std::string& name) :
Config(name)
{
}

ToyFactoryStdConfig::~ToyFactoryStdConfig() {}

void ToyFactoryStdConfig::PrintOptions() const {
}

void ToyFactoryStdConfig::DefineOptions() {
}

void ToyFactoryStdConfig::LoadOptions() {
}
