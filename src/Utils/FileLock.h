#ifndef UTILS_FILELOCK_h
#define UTILS_FILELOCK_h

// forward declarations

namespace Utils {
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
   *  Especially shared file systems ...
   */
  class FileLock {
   public:
    /**
     *  @brief Default constructor for FileLock
     *
     *  Detailed description.
     *
     *  @param parameter description
     */
    FileLock();
    
    /**
     *  @brief Destructor for FileLock
     *
     *  Detailed description.
     *
     */
    ~FileLock();
    
   protected:
    
   private:
  };
}

#endif // UTILS_FILELOCK_h
