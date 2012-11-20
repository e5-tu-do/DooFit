/** @class doofit::config::AbsTypeCommaSeparated
 *  @brief Abstract base class for complex Config members with comma-separated
 *         string representation
 *
 *  This abstract base class provides support for complex Config members that 
 *  have a comma-separated string representation for boost::program_options.
 *  The base class provides support for decomposition of the comma separated 
 *  list through AbsTypeCommaSeparated::DecomposeString(std::string).
 */

#ifndef CONFIGABSTRACTTYPECOMMASEPARATED_h
#define CONFIGABSTRACTTYPECOMMASEPARATED_h

// from STL
#include <string>
#include <vector>

// from ROOT
#include "TObject.h"

namespace doofit {
namespace config {
  class AbsTypeCommaSeparated : public TObject {
  public:
    /**
     *  \brief Destructor.
     */
    virtual ~AbsTypeCommaSeparated() {}
    
    /**
     *  @brief Function to parse the string into own members
     *
     *  This pure virtual function will be called to parse the string 
     *  representation into own member variables. Calling this function is not 
     *  necessary. It will be transparent though boost::program_options.
     *
     *  @param str string to parse
     */
    virtual void Parse(std::string str) = 0;
    
    /**
     *  @brief Print this object to an std::ostream
     *
     *  This function is used to print the object. Calling this function directly
     *  is not necessary as it can be streamed directly to any ostream via 
     *  operator<<(std::ostream&, const AbsTypeCommaSeparated&). Avoid
     *  adding unnecessary newline characters at the end.
     *
     *  @param os ostream to print to
     */
    virtual void Print(std::ostream& os) const = 0;
    
  protected:  
    /**
     *  @brief Get string elements in vector.
     *
     *  Decompose the comma-separated string @a str into vector of strings.
     *
     *  @param str String to decompose
     *  @return Decomposition of the comma-separated string as vector of strings.
     */
    std::vector<std::string> DecomposeString(std::string str) const;
    
  private:
    
    /**
     *  @brief ClassDef statement for CINT dictionary generation
     */
    //ClassDef(AbsTypeCommaSeparated,1);
  };
}
}

/**
 *  @brief Input stream operator for AbsTypeCommaSeparated
 *
 *  This function is used to pass string representations of 
 *  AbsTypeCommaSeparated via an istream into a 
 *  AbsTypeCommaSeparated object.
 *
 *  @param is the incoming input stream
 *  @param arg the AbsTypeCommaSeparated to parse the string and fill 
 *             its members accordingly
 *  @return the rest of the input stream to pass along
 */
std::istream& operator>>(std::istream& is, doofit::config::AbsTypeCommaSeparated& arg);

/**
 *  @brief Output stream operator for AbsTypeCommaSeparated
 *
 *  This function is used to pass a AbsTypeCommaSeparated object to 
 *  an ostream for printing.
 *
 *  @param os the incoming output stream
 *  @param arg the AbsTypeCommaSeparated to print
 *  @return the rest of the output stream to pass along
 */
std::ostream& operator<<(std::ostream& os, const doofit::config::AbsTypeCommaSeparated& arg);


#endif // CONFIGABSTRACTTYPECOMMASEPARATED_h

