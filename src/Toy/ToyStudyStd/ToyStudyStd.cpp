#include "Toy/ToyStudyStd/ToyStudyStd.h"

// STL

// boost

// ROOT

// from RooFit

// from Project
#include "Config/CommonConfig.h"
#include "Toy/ToyStudyStd/ToyStudyStdConfig.h"
#include "Utils/MsgStream.h"

using namespace ROOT;
using namespace RooFit;

namespace Toy {
  ToyStudyStd::ToyStudyStd(const CommonConfig& cfg_com, const ToyStudyStdConfig& cfg_tstudy) :
  config_common_(cfg_com),
  config_toystudy_(cfg_tstudy)
  {
    
  }
  
  ToyStudyStd::~ToyStudyStd(){
    
  }
}