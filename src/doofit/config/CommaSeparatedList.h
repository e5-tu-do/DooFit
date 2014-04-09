#ifndef DOOFIT_CONFIG_COMMASEPARATEDLIST_H
#define DOOFIT_CONFIG_COMMASEPARATEDLIST_H

// from STL
#include <vector>
#include <string>
#include <iostream>

// from Boost
#include <boost/lexical_cast.hpp>

// from project
#ifndef __CINT__
#include "doofit/config/AbsTypeCommaSeparated.h"
#else
// ROOT Cint hacks...
#include "../Config/AbsTypeCommaSeparated.h"
#endif /* __CINT __ */

namespace doofit {
namespace config {
  /** @class CommaSeparatedList
   *  @brief A simple comma separated pair of two strings.
   *
   *  This class is merely a container for Config objects to store a list of 
   *  same type objects separated by a comma. As it's inheriting from
   *  AbsTypeCommaSeparated, parsing from command line or config file
   *  is possible. 
   *
   *  Its string representation is
   *  
   *  @verbatim a,b,c,d,e,f,g,h,... @endverbatim
   * 
   *  @see AbsTypeCommaSeparated
   *  @see ConfigTestAbstractType
   */
  template <class T>
  class CommaSeparatedList : public AbsTypeCommaSeparated {
  public:
    /**
     *  @brief Default constructor for CommaSeparatedList
     */
    CommaSeparatedList() {}
    
    /**
     *  @brief Destructor for CommaSeparatedList
     */
    ~CommaSeparatedList() {}
    
    /**
     *  @brief Parse a given string
     *
     *  The given string @a str is parsed. If parsing is not successful, an 
     *  exception will be thrown. The string is supposed to be a comma-separated 
     *  list formatted like this:
     *
     *  @verbatim first,second @endverbatim
     *
     *  @see CommaSeparatedList::first_
     *  @see CommaSeparatedList::second_
     *
     *  @param str string to parse
     */
    void Parse(const std::string& str) {
      std::vector<std::string> elements = DecomposeString(str);
      
      for (std::vector<std::string>::const_iterator it=elements.begin();
           it != elements.end(); ++it) {
        vec_.push_back(boost::lexical_cast<T>(*it));
      }
    }
    
    /**
     *  @brief Print this object to an std::ostream
     *
     *  This function is used to print the object. Calling this function directly
     *  is not necessary as it can be streamed directly to any ostream via 
     *  operator<<(std::ostream&, const AbsTypeCommaSeparated&).
     *
     *  @param os ostream to print to
     */
    virtual void Print(std::ostream& os) const {
      for (typename std::vector<T>::const_iterator it=vec_.begin(); it != vec_.end()-1; ++it) {
        os << *it << ",";
      }
      os << vec_.back();
    }
    
    /**
     *  @brief Getter for elements
     */
    const T& at(int i) const {return vec_.at(i);}

    /**
     *  @brief List size
     */
    int size() const {return vec_.size();}
    
    /**
     *  @brief operator[] element getter
     */
    const T& operator[](int i) const {return at(i);}
    
  protected:
    
  private:
    /**
     *  @brief Internal vector for elements
     */
    std::vector<T> vec_;
  };
  
}
}

#endif // DOOFIT_CONFIG_COMMASEPARATEDLIST_H
