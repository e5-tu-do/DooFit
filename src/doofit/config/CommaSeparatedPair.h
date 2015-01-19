#ifndef COMMASEPARATEDPAIR_h
#define COMMASEPARATEDPAIR_h

// from STL
#include <vector>
#include <string>
#include <iostream>

// from Boost
#include <boost/lexical_cast.hpp>

// from DooFit
#include <doocore/io/MsgStream.h>


// from project
#ifndef __CINT__
#include "doofit/config/AbsTypeCommaSeparated.h"
#else
// ROOT Cint hacks...
#include "../Config/AbsTypeCommaSeparated.h"
#endif /* __CINT __ */

namespace doofit {
namespace config {
  /** @class CommaSeparatedPair
   *  @brief A simple comma separated pair of two strings.
   *
   *  This class is merely a container for Config objects to store a pair of two
   *  strings, separated by a comma. As it's inheriting from
   *  AbsTypeCommaSeparated, parsing from command line or config file
   *  is possible. 
   *
   *  It contains variables first and second whose string representation is 
   *  
   *  @verbatim first,second @endverbatim
   * 
   *  @see AbsTypeCommaSeparated
   *  @see ConfigTestAbstractType
   */
  
  template <class T>
  class CommaSeparatedPair : public AbsTypeCommaSeparated {
  public:
    /**
     *  @brief Default constructor for CommaSeparatedPair
     */
    CommaSeparatedPair() {}

    CommaSeparatedPair(const T& first, const T& second) 
     : first_(first),
       second_(second)
    {}
    
    /**
     *  @brief Destructor for CommaSeparatedPair
     */
    ~CommaSeparatedPair() {}
    
    /**
     *  @brief Parse a given string
     *
     *  The given string @a str is parsed. If parsing is not successful, an 
     *  exception will be thrown. The string is supposed to be a comma-separated 
     *  list formatted like this:
     *
     *  @verbatim first,second @endverbatim
     *
     *  @see CommaSeparatedPair::first_
     *  @see CommaSeparatedPair::second_
     *
     *  @param str string to parse
     */
    void Parse(const std::string& str) {
      using namespace doocore::io;
      std::vector<std::string> elements = DecomposeString(str);
      
      // number of elements must be two
      if (elements.size() != 2) {
        serr << "ERROR in CommaSeparatedPair::Parse(const std::string&): String '" << str << "' does not contain exactly two comma-separated strings." << endmsg;
        throw;
      }
      
      first_ = boost::lexical_cast<T>(elements[0]);
      second_ = boost::lexical_cast<T>(elements[1]);
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
      os << first_ << "->" << second_;
    }
    
    
    /** @name getter Getter
     *  Getter functions
     */
    ///@{
    /**
     *  @brief Getter for first string
     */
    const T& first() const {return first_;}
    
    /**
     *  @brief Getter for second string
     */
    const T& second() const {return second_;}
    ///@}
    
    /** @name setter Setter
     *  Setter functions
     */
    ///@{
    /**
     *  @brief Setter for first string
     */
    void set_first(const T& str) { first_ = str; }
    
    /**
     *  @brief Setter for second string
     */
    void set_second(const T& str) { second_ = str; }
    ///@}
    
    /**
     *  @brief Assignment operator for string
     *
     *  Assign a string that will be parsed to this CommaSeparatedPair.
     *
     *  Example:
     * 
     *  @code
     *  comma_separated_pair = "first,second"
     *  @endcode
     *
     *  @param str comma-separated string to be parsed
     *  @return reference to this
     */
    virtual CommaSeparatedPair& operator=(const std::string& str) { AbsTypeCommaSeparated::operator=(str); return *this; }
    
  protected:
    
  private:
    /**
     *  @brief First string
     */
    T first_;
    
    /**
     *  @brief Second string
     */
    T second_;
    
    /**
     *  @brief ClassDef statement for CINT dictionary generation
     */
    //ClassDef(CommaSeparatedPair,1);
  };
}
}

#endif // COMMASEPARATEDPAIR_h
