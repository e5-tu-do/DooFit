#include "ToyFactory/ToyFactoryStd/ToyFactoryStdConfig.h"

// STL
#include <string>
#include <vector>
#include <iostream>
#include <cstdlib>
#include <map>
#include <utility>

// Boost

// ROOT
#include "TIterator.h"
#include "TMath.h"

// RooFit
#include "RooAbsPdf.h"
#include "RooAbsArg.h"
#include "RooWorkspace.h"

// from project
#include "Utils/MsgStream.h"

using namespace std;
namespace po = boost::program_options;

ToyFactoryStdConfig::ToyFactoryStdConfig() : 
Config("empty_name"),
generation_pdf_(NULL),
expected_yield_(0),
argset_generation_observables_(NULL),
workspace_(NULL),
generation_pdf_workspace_(""),
argset_generation_observables_workspace_(""),
random_seed_(0)
{
  swarn << "Usage of ToyFactoryStdConfig::ToyFactoryStdConfig() is not recommended!" <<endmsg;
}

ToyFactoryStdConfig::ToyFactoryStdConfig(const std::string& name) :
Config(name),
generation_pdf_(NULL),
expected_yield_(0),
argset_generation_observables_(NULL),
workspace_(NULL),
generation_pdf_workspace_(""),
argset_generation_observables_workspace_(""),
random_seed_(0)
{
}

ToyFactoryStdConfig::~ToyFactoryStdConfig() {}

RooAbsPdf* ToyFactoryStdConfig::generation_pdf() const {
  if (generation_pdf_) { 
    return generation_pdf_;
  } else if (generation_pdf_workspace_.length() > 0 && workspace_ && workspace_->pdf(generation_pdf_workspace_.c_str())) {
    return workspace_->pdf(generation_pdf_workspace_.c_str());
  } else {
    serr << "ERROR: PDF to generate not set and attempt to load from workspace not successful." << endmsg;
    throw;
  }  
}

const RooArgSet* ToyFactoryStdConfig::argset_generation_observables() const {
  if (argset_generation_observables_) { 
    return argset_generation_observables_;
  } else if (argset_generation_observables_workspace_.length() > 0 && workspace_ && workspace_->set(argset_generation_observables_workspace_.c_str())) {
    return workspace_->set(argset_generation_observables_workspace_.c_str());
  } else {
    serr << "ERROR: Obervables argument set to generate not set and attempt to load from workspace not successful." << endmsg;
    throw;
  }  
}

void ToyFactoryStdConfig::set_generation_pdf_workspace(const std::string& name) {
  generation_pdf_workspace_ = name;
  
  if (generation_pdf_) {
    swarn << "WARNING: Setting PDF to be loaded from workspace although it is already directly specified. Will ignore workspace." << endmsg;
  }
}

void ToyFactoryStdConfig::set_argset_generation_observables_workspace(const std::string& name) {
  argset_generation_observables_workspace_ = name;
  
  if (argset_generation_observables_) {
    swarn << "WARNING: Setting argument set to be loaded from workspace although it is already directly specified. Will ignore workspace." << endmsg;
  }
}

void ToyFactoryStdConfig::PrintOptions() const {
  scfg << "Random seed:               " << random_seed() << endmsg;
  
  try {
    scfg << "Generation PDF:            " << generation_pdf()->IsA()->GetName() << ":"
    << generation_pdf()->GetName() << " (" << generation_pdf()->GetTitle()
    << ")" << endmsg;
  } catch (...) {
     scfg << "Generation PDF not set." << endmsg;
  }
  
  scfg << "Expected yield:            ";
  if (expected_yield_ == 0) {
    scfg << "(determined by PDF itself)" << endmsg;
  } else {
    scfg << expected_yield_ << endmsg;
  }
  
  scfg << "Observables to generate directly: ";
  if (argset_generation_observables() != NULL) {
    TIterator* arg_it = argset_generation_observables()->createIterator();
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
  
  scfg << "Workspace:                 ";
  if (workspace_) {
    scfg << "(is set)" << endmsg;
    
    scfg << " PDF name (workspace):     " << generation_pdf_workspace_ << endmsg;
    scfg << " Argset name (workspace):  " << argset_generation_observables_workspace_ << endmsg;
  } else {
    scfg << "(is not set)" << endmsg;
  }
  
  
  
  for (vector<DiscreteProbabilityDistribution>::const_iterator it = discrete_probabilities_.begin(); it != discrete_probabilities_.end(); ++it) {
    scfg << "Discrete probability: " << *it << endmsg;
  }
}

void ToyFactoryStdConfig::DefineOptions() {
  po::options_description* generation = new po::options_description("Toy generation options");
  
  generation->add_options()
  (GetOptionString("random_seed").c_str(), po::value<int>(&random_seed_)->default_value(0),
   "Random seed to use for the toy generation (set to 0 for random seed)")
  (GetOptionString("expected_yield").c_str(), po::value<double>(&expected_yield_)->default_value(0),
   "Expected yield for toy sample (set to 0 for usage of expected yield in extended likelihood PDF)")
  (GetOptionString("pdf_name_ws").c_str(), po::value<string>(&generation_pdf_workspace_),
   "Name of PDF to generate for on linked workspace")
  (GetOptionString("argset_name_ws").c_str(), po::value<string>(&argset_generation_observables_workspace_),
   "Name of variables argset to generate for on linked workspace")
  (GetOptionString("discrete_probabilities").c_str(), po::value<vector<DiscreteProbabilityDistribution> >(&discrete_probabilities_)->composing(), "Discrete probability distribution for variables (can be multiply defined). The string representation is var_name,value1,prob1,value2,prob2,...,valueN,probN");
  
  descs_visible_.push_back(generation);
}

void ToyFactoryStdConfig::LoadOptions() {
}


