#include "Toy/ToyStudyStd/ToyStudyStdConfig.h"

// STL

// Boost

// ROOT

// RooFit

// from project
#include "Utils/MsgStream.h"

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
  }
  
  void ToyStudyStdConfig::DefineOptions() {
    po::options_description* generation = new po::options_description("Toy study options");
    
//    generation->add_options()
//    (GetOptionString("parameter_save_file").c_str(), po::value<string>(&parameter_save_file_),"Filename to save parameters to after generation");
    
    descs_visible_.push_back(generation);
  }
  
  void ToyStudyStdConfig::LoadOptions() {
  }

}