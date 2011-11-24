#include "Toy/ToyStudyStd/ToyStudyStdConfig.h"

// STL

// Boost

// ROOT

// RooFit

// from project
#include "Utils/MsgStream.h"
#include "../../Config/CommaSeparatedPair.h"

using namespace std;
namespace po = boost::program_options;

namespace Toy {
  ToyStudyStdConfig::ToyStudyStdConfig() : 
  Config::AbsConfig("empty_name")
  {
    swarn << "Usage of ToyStudyStdConfig::ToyStudyStdConfig() is not recommended!" <<endmsg;
  }
  
  ToyStudyStdConfig::ToyStudyStdConfig(const std::string& name) :
  Config::AbsConfig(name)
  {
  }
  
  ToyStudyStdConfig::~ToyStudyStdConfig() {
  }
  
  void ToyStudyStdConfig::PrintOptions() const {
    if (result_filename_treename_.first().size() > 0) {
      scfg << "File and tree to save result to: " << result_filename_treename_ << endmsg;
    }
  }
  
  void ToyStudyStdConfig::DefineOptions() {
    po::options_description* generation = new po::options_description("Toy study options");
    
    generation->add_options()
    (GetOptionString("result_filename_treename").c_str(), po::value<Config::CommaSeparatedPair>(&result_filename_treename_),"File name and tree name to save fit results to (set as filename,treename)");
    
    descs_visible_.push_back(generation);
  }
  
  void ToyStudyStdConfig::LoadOptions() {
  }

}