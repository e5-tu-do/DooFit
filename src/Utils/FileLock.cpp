#include "FileLock.h"

// STL
#include <string>
#include <unistd.h>
#include <vector>
#include <fstream>

// boost
#include "boost/filesystem.hpp"
#include "boost/lexical_cast.hpp"
#include "boost/random/random_device.hpp"

// ROOT

// from RooFit

// from Project
#include "Utils/MsgStream.h"

namespace Utils {
  using namespace std;
  namespace fs = boost::filesystem;
  
  FileLock::FileLock(const std::string& filename) :
  is_locked_by_us_(false)
  {
    if (fs::exists(file_)) {
      file_ = fs::canonical(filename);
    } else {
      file_ = fs::absolute(filename);
    }
        
    if (!fs::is_regular_file(file_) and fs::exists(file_)) {
      serr << "File lock error. File " << file_.string() << " is regular: " << fs::is_regular_file(file_) << " and exists: " << fs::exists(file_) << endmsg;
      throw ExceptionFileLockError();
    } 
        
    string lockfile = file_.string() + ".lock." + Hostname() + "." + boost::lexical_cast<std::string>(Pid());
    lockfile_ = fs::path(lockfile);
  }
  
  FileLock::~FileLock() {
    Unlock();
  }
  
  bool FileLock::Lock() {
    if (is_locked_by_us_) return true;
    if (IsLocked()) return false;
    
    ofstream touchfile(lockfile_.string().c_str());
    touchfile.close();
    
    if (!fs::is_regular_file(lockfile_)) {
      throw ExceptionFileLockError();
    }
    
    Sleep(1);
    
    int number_lockfiles_postlock = NumberOfLockfiles();
    if (number_lockfiles_postlock == 1) {
      is_locked_by_us_ = true;
      return true;
    } else {
      fs::remove(lockfile_);
      
      boost::random::random_device rnd;
      double wait_time = (static_cast<double>(rnd())/(rnd.max()-rnd.min())-rnd.min())*2.0;
      Sleep(wait_time);
      
      return false;
    }
    
    return false;
  }
  
  bool FileLock::Unlock() {
    if (is_locked_by_us_) {
      if (fs::remove(lockfile_)) {
        is_locked_by_us_ = false;
        return true;
      } else {
        return false;
      }
    } else {
      return false;
    }
  }
  
  bool FileLock::IsLocked() const {
    if (NumberOfLockfiles() > 0) {
      return true;
    } else {
      return false;
    }
  }
  
  std::string FileLock::Hostname() const {
    char szHostName[128];
    int i = gethostname(szHostName,128);
    if (i == 0) {
      return szHostName;
    } else {
      return "";
    }
  }
  
  int FileLock::Pid() const {
    return getpid();
  }
  
  int FileLock::NumberOfLockfiles() const {
    string filename_lock(file_.filename().string() + ".lock");
    int num_lockfiles = 0;
    
    // get all files in file_'s directory
    vector<fs::path> elements;
    copy(fs::directory_iterator(file_.parent_path()), fs::directory_iterator(), back_inserter(elements));
    for (vector<fs::path>::const_iterator it (elements.begin()); it != elements.end(); ++it) {
      string element_name((*it).filename().string());
      if (element_name.length() > filename_lock.length() && 
          element_name.substr(0,filename_lock.length()).compare(filename_lock) == 0) {
        num_lockfiles++;
      }
    }
    return num_lockfiles;
  }
}