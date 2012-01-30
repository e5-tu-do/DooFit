#ifndef COMMASEPARATEDPAIR_h
#define COMMASEPARATEDPAIR_h

// from project
#ifndef __CINT__
#include "DooFit/Config/AbsTypeCommaSeparated.h"
#else
// ROOT Cint hacks...
#include "../Config/AbsTypeCommaSeparated.h"
#endif /* __CINT __ */

namespace DooFit {
namespace Config {
  /** @class CommaSeparatedPair
   *  @brief A simple comma separated pair of two strings.
   *
   *  This class is merely a container for Config objects to store a pair of two
   *  strings, separated by a comme. As it's inheriting from 
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
  
  class CommaSeparatedPair : public AbsTypeCommaSeparated {
  public:
    /**
     *  @brief Default constructor for CommaSeparatedPair
     */
    CommaSeparatedPair() {}
    
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
    void Parse(std::string str);
    
    /**
     *  @brief Print this object to an std::ostream
     *
     *  This function is used to print the object. Calling this function directly
     *  is not necessary as it can be streamed directly to any ostream via 
     *  operator<<(std::ostream&, const AbsTypeCommaSeparated&).
     *
     *  @param os ostream to print to
     */
    virtual void Print(std::ostream& os) const;
    
    /**
     *  @brief Getter for first string
     */
    const std::string& first() const {return first_;}
    
    /**
     *  @brief Getter for second string
     */
    const std::string& second() const {return second_;}
    
  protected:
    
  private:
    /**
     *  @brief First string
     */
    std::string first_;
    
    /**
     *  @brief Second string
     */
    std::string second_;
    
    /**
     *  @brief ClassDef statement for CINT dictionary generation
     */
    //ClassDef(CommaSeparatedPair,1);
  };
}
}

#endif // COMMASEPARATEDPAIR_h
