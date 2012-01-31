#ifndef UTILS_FILELOCK_h
#define UTILS_FILELOCK_h

// STL
#include <string>

// boost
#include "boost/filesystem.hpp"

// forward declarations

namespace doofit {
namespace utils {
  /** @class FileLock
   *  @brief Helper class for safe file locking among shared file systems without locking specific support
   *
   *  This is a helper class to get locking functionality that's independent of 
   *  any filesystem locking capabilities and also works on shared filesystems
   *  without specific lock support. Locks are advisory, i.e. access to the file
   *  is never actually locked for any file access attempt not using FileLock.
   *  The locks are implemented via lock files where the lock file name is 
   *  unique in a way that it contains host name and process ID information.
   *
   *  Especially shared file systems have no or little support for atomic 
   *  operations and so race conditions can occur. Therefore, multiple accesses
   *  cannot be excluded completely. Use with caution.
   *
   *  Also locking a file requires write access to the directory the file is 
   *  stored in (the lock file needs to be seen by anyone who can see the locked
   *  file, thus the only safe place is the same directory).
   *
   *  @todo: Resolve links, if file is a link
   */
  class FileLock {
   public:
    /**
     *  @brief Default constructor for FileLock
     *
     *  @param filename file to lock
     */
    FileLock(const std::string& filename);
    
    /**
     *  @brief Destructor for FileLock
     */
    ~FileLock();
    
    /**
     *  @brief Lock the file
     *
     *  An attempt is made to lock the file via a lockfile. If successful, true
     *  will be returned. If the file is already locked, false will be returned.
     *  For locking a lock file will be created. Afterwards, a grace time of 1 s
     *  will be waited and the lock file count will be checked again. If there 
     *  is only the just created lock file, the lock is treated as successful.
     *  If there will be more than one lock file after the grace time, the race
     *  condition has occured.
     *
     *  In case of the race condition, the lock file will be deleted and a 
     *  random amount of time between 0 and 2 seconds will be waited before 
     *  false is returned.
     *
     *  @todo: throw ExceptionFileLockRaceCondition if race condition?
     *
     *  @return whether the lock was successful or not
     */
    bool Lock();
    
    /**
     *  @brief Unlock the file
     *
     *  @return whether the unlock was successful or not
     */
    bool Unlock();
    
    /**
     *  @brief Check if file is already locked
     *
     *  @return whether the file is locked or not
     */
    bool IsLocked() const;
    
   protected:
    
   private:
    /**
     *  @brief Get this machine's hostname
     *
     *  @return the hostname
     */
    std::string Hostname() const;
    
    /**
     *  @brief Get this process' PID
     *
     *  @return the hostname
     */
    int Pid() const;
    
    /**
     *  @brief Count number of lock files
     *
     *  @return number of lock files
     */
    int NumberOfLockfiles() const;
    
    /**
     *  @brief boost path member for file to lock
     **/
    boost::filesystem::path file_;
    
    /**
     *  @brief boost path member for lock file
     **/
    boost::filesystem::path lockfile_;
    
    /**
     *  @brief member if file is locked by this FileLock
     **/
    bool is_locked_by_us_;
  };
  
  /** \struct ExceptionFileLockError
   *  \brief Exception for problem in locking file
   */
  struct ExceptionFileLockError: public virtual boost::exception, public virtual std::exception { 
    virtual const char* what() const throw() { return "FileLock error"; }
  };
  
  /** \struct ExceptionFileLockRaceCondition
   *  \brief Exception for race condition in locking file
   */
  struct ExceptionFileLockRaceCondition: public virtual boost::exception, public virtual std::exception { 
    virtual const char* what() const throw() { return "FileLock race condition"; }
  };
}
}
#endif // UTILS_FILELOCK_h
