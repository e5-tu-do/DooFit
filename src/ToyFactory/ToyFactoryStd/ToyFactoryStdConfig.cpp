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
argset_generation_observables_(NULL),
random_seed_(0)
{
}

ToyFactoryStdConfig::~ToyFactoryStdConfig() {}

void ToyFactoryStdConfig::PrintOptions() const {
  scfg << "Random seed:       " << random_seed() << endmsg;
  
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
    
    while ((arg = (RooAbsArg*)arg_it->Next())) {
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
  po::options_description* generation = new po::options_description("Toy generation options");
  
  generation->add_options()
  (GetOptionString("random-seed").c_str(), po::value<int>(&random_seed_)->default_value(0),
   "Random seed to use for the toy generation")
  ("expected-yield", po::value<int>(&expected_yield_)->default_value(0),
   "Expected yield for toy sample (set to 0 for usage of expected yield in extended likelihood PDF)");
  
  descs_visible_.push_back(generation);
}

void ToyFactoryStdConfig::LoadOptions() {
}
