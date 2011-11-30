#include "Toy/ToyStudyStd/ToyStudyStd.h"

// STL

// boost
#include "boost/interprocess/sync/file_lock.hpp"
#include "boost/filesystem.hpp"

// ROOT
#include "TTree.h"
#include "TFile.h"

// from RooFit
#include "RooFitResult.h"

// from Project
#include "Config/CommonConfig.h"
#include "Toy/ToyStudyStd/ToyStudyStdConfig.h"
#include "Utils/MsgStream.h"
#include "Utils/Utils.h"
#include "Utils/FileLock.h"

using namespace ROOT;
using namespace RooFit;

namespace Toy {
  namespace fs = boost::filesystem;
  
  ToyStudyStd::ToyStudyStd(const CommonConfig& cfg_com, const ToyStudyStdConfig& cfg_tstudy) :
  config_common_(cfg_com),
  config_toystudy_(cfg_tstudy)
  {
    
  }
  
  ToyStudyStd::~ToyStudyStd() {
    
  }
  
  void ToyStudyStd::StoreFitResult(const RooFitResult* fit_result) const {
    const string& filename = config_toystudy_.store_result_filename_treename().first();
    const string& treename = config_toystudy_.store_result_filename_treename().second();
    Utils::FileLock flock(filename);
    
    if (!flock.Lock()) {
      swarn << "File to save fit result to " << filename << " is locked. Waiting for unlock." << endmsg;
    }
    while (!flock.Lock()) {
      Utils::Sleep(2);
    }

    sinfo << "Saving fit result to file " << filename << endmsg;
    bool file_existing = fs::exists(filename);
    TFile f(fs::absolute(filename).string().c_str(),"update");
    if (f.IsZombie() || !f.IsOpen()) {
      serr << "Cannot open file which may be corrupted." << endmsg;
      flock.Unlock();
      throw ExceptionCannotStoreFitResult();
    } else {
      TTree* tree_results = NULL;
      if (file_existing) {      
        tree_results = (TTree*)f.Get(treename.c_str());
      } 
      
      if (tree_results == NULL) {
        tree_results = new TTree(treename.c_str(), "Tree for toy study fit results");
        tree_results->Branch("fit_results", "RooFitResult", &fit_result, 64000, 0);
      } else {      
        tree_results->SetBranchAddress("fit_results", &fit_result);
      }
      
      tree_results->Fill();
      tree_results->Write("",TObject::kOverwrite);
      f.Close();
      
      flock.Unlock();
    }
  }
}
