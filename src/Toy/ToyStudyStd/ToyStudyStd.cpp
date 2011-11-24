#include "Toy/ToyStudyStd/ToyStudyStd.h"

// STL

// boost
#include "boost/interprocess/sync/file_lock.hpp"
#include "boost/filesystem.hpp"

// ROOT

// from RooFit

// from Project
#include "Config/CommonConfig.h"
#include "Toy/ToyStudyStd/ToyStudyStdConfig.h"
#include "Utils/MsgStream.h"
#include "Utils/Utils.h"

using namespace ROOT;
using namespace RooFit;

namespace Toy {
  ToyStudyStd::ToyStudyStd(const CommonConfig& cfg_com, const ToyStudyStdConfig& cfg_tstudy) :
  config_common_(cfg_com),
  config_toystudy_(cfg_tstudy)
  {
    
  }
  
  ToyStudyStd::~ToyStudyStd() {
    
  }
  
  void ToyStudyStd::SaveFitResult(RooFitResult* fit_result) const {
    const string& filename = config_toystudy_.result_filename_treename().first();
    const string& treename = config_toystudy_.result_filename_treename().second();
    
    
    if (boost::filesystem::exists(filename)) {
      boost::interprocess::file_lock flock(filename.c_str());
      
      if (!flock.try_lock()) {
        swarn << "File to save fit result to " << filename << " is locked. Waiting for unlock." << endmsg;
      }
      while (!flock.try_lock()) {
        Utils::Sleep(2);
      } 
      
      while(1) {
        sdebug << "I GOT THE LOCK" << endmsg;
        Utils::Sleep(2);
      }
    }
  }
}
