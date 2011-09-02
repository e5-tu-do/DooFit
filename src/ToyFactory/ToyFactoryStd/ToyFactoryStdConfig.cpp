#include "ToyFactory/ToyFactoryStd/ToyFactoryStdConfig.h"

// STL
#include <string>
#include <vector>
#include <iostream>

// Boost

// ROOT
#include "TIterator.h"

// RooFit
#include "RooAbsPdf.h"
#include "RooAbsArg.h"

// from project
#include "Utils/MsgStream.h"

using namespace std;
namespace po = boost::program_options;

ToyFactoryStdConfig::ToyFactoryStdConfig(const std::string& name) :
Config(name),
generation_pdf_(NULL),
expected_yield_(0),
argset_generation_observables_(NULL)
{
}

ToyFactoryStdConfig::~ToyFactoryStdConfig() {}

void ToyFactoryStdConfig::PrintOptions() const {
  if (generation_pdf_ != NULL) {
    scfg << "Generation PDF:    " << generation_pdf_->IsA()->GetName() << ":"
         << generation_pdf_->GetName() << " (" << generation_pdf_->GetTitle()
         << ")" << endmsg;
  } else {
    scfg << "Generation PDF not set." << endmsg; 
  }
  
  scfg << "Expected yield:    ";
  if (expected_yield_ == 0) {
    scfg << "(determined by PDF itself)" << endmsg;
  } else {
    scfg << expected_yield_ << endmsg;
  }
  
  scfg << "Observables to generate directly: ";
  if (argset_generation_observables_ != NULL) {
    TIterator* arg_it = argset_generation_observables_->createIterator();
    RooAbsArg* arg = NULL;
    bool first = true;
    
    while (arg = (RooAbsArg*)arg_it->Next()) {
      if (!first) {
        scfg << ", ";
      } else {
        first = false;
      }
      
      scfg << arg->GetName();
    }
    scfg << endmsg;
  } else {
    scfg << "(not set)" << endmsg;
  }
}

void ToyFactoryStdConfig::DefineOptions() {
}

void ToyFactoryStdConfig::LoadOptions() {
}
