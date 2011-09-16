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

// from project
#include "Utils/MsgStream.h"

using namespace std;
namespace po = boost::program_options;

ToyFactoryStdConfig::ToyFactoryStdConfig() : 
Config("empty_name"),
generation_pdf_(NULL),
expected_yield_(0),
argset_generation_observables_(NULL),
random_seed_(0)
{
  swarn << "Usage of ToyFactoryStdConfig::ToyFactoryStdConfig() is not recommended!" <<endmsg;
}

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
  (GetOptionString("discrete_probabilities").c_str(), po::value<vector<DiscreteProbabilityDistribution> >(&discrete_probabilities_)->composing(), "Discrete probability distribution for variables (can be multiply defined). The string representation is var_name,value1,prob1,value2,prob2,...,valueN,probN");
  
  descs_visible_.push_back(generation);
}

void ToyFactoryStdConfig::LoadOptions() {
}

void DiscreteProbabilityDistribution::Parse(string str) {
  vector<string> elements = DecomposeString(str);
  
  // number of elements must be odd (one simple and several pairs)
  if (elements.size()%2 != 1) {
    serr << "ERROR in DiscreteProbabilityDistribution::Parse(std::string): String '" << str << "' ends prematurely." << endmsg;
    throw;
  }
  
  vector<string>::iterator it=elements.begin();
  
  var_name_ = *(it);
  ++it;
  
  double cumuluated_prob = 0.0;
  
  while (it!=elements.end()) {
    double key = atof((*it).c_str());
    double value = atof((*(++it)).c_str());
    cumuluated_prob += value;    
    
    probabilities_.push_back(pair<double,double>(key,cumuluated_prob));
    ++it;
  }
  
  if (TMath::Abs((cumuluated_prob - 1.0)) > 1e-14) {
    serr << "ERROR in DiscreteProbabilityDistribution::Parse(string): Cumulative probability is not 1.0 (is " << cumuluated_prob << " - 1 = " << cumuluated_prob-1.0 << ")." << endmsg;
    throw;
  }
}

void DiscreteProbabilityDistribution::Print(std::ostream& os) const {
  os << "Variable: " << var_name_;
  double cumuluated_prob = 0.0;
  for (std::vector<pair<double,double> >::const_iterator itmap=probabilities_.begin(); itmap!=probabilities_.end(); ++itmap) {
    
    os << "; P(" << (*itmap).first << ") = " << (*itmap).second-cumuluated_prob;
    cumuluated_prob = (*itmap).second;
  }
}

