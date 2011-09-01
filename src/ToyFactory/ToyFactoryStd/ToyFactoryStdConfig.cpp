#include "ToyFactory/ToyFactoryStd/ToyFactoryStdConfig.h"

// STL
#include <string>
#include <vector>
#include <iostream>

// Boost

// ROOT

// RooFit
#include "RooAbsPdf.h"

// from project
#include "Utils/MsgStream.h"

using namespace std;
namespace po = boost::program_options;

ToyFactoryStdConfig::ToyFactoryStdConfig(const std::string& name) :
Config(name),
generation_pdf_(NULL)
{
}

ToyFactoryStdConfig::~ToyFactoryStdConfig() {}

void ToyFactoryStdConfig::PrintOptions() const {
  if (generation_pdf_ != NULL) {
    scfg << "Generation PDF: " << generation_pdf_->IsA()->GetName() << ":"
         << generation_pdf_->GetName() << " (" << generation_pdf_->GetTitle()
         << ")" << endmsg;
  } else {
    scfg << "Generation PDF not set." << endmsg; 
  }
}

void ToyFactoryStdConfig::DefineOptions() {
}

void ToyFactoryStdConfig::LoadOptions() {
}
