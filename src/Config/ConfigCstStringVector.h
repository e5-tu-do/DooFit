/** @class ConfigCstStringVector
 *  @brief I HAVE NO DESCRIPTION. SHAME ON ME.
 *
 *  This class is merely a container for Config objects to store components
 *  names that are needed for the construction of p.d.f.
 * 
 *  @see ConfigAbstractTypeCommaSeparated
 */

#ifndef CONFIGCSTSTRINGVECTOR_H
#define CONFIGCSTSTRINGVECTOR_H


// from STL
#include <string>

// from project
#ifndef __CINT__
#include "Config/ConfigAbstractTypeCommaSeparated.h"
#else
// ROOT Cint hacks...
#include "../Config/ConfigAbstractTypeCommaSeparated.h"
#endif /* __CINT __ */


class ConfigCstStringVector : public ConfigAbstractTypeCommaSeparated {
 public:
  /**
   *  @brief Default constructor for ConfigCstStringVector
   */
  ConfigCstStringVector(){}
  
  /**
   *  @brief Destructor for ConfigCstStringVector
   */
  ~ConfigCstStringVector(){}
  
  /**
   *  @brief Parse a given string
   *
   *  The given string @a str is parsed. If parsing is not successful, an 
   *  exception will be thrown. The string is supposed to be a comma-separated 
   *  list formatted like this:
   *
   *  @verbatim comp1,comp2,comp3,...,compN @endverbatim
   *
   *  @see ConfigCstStringVector::var_name_
   *
   *  @param str string to parse
   */
  void Parse(std::string str);
  
  /**
   *  @brief Print this object to an std::ostream
   *
   *  This function is used to print the object. Calling this function directly
   *  is not necessary as it can be streamed directly to any ostream via 
   *  operator<<(std::ostream&, const ConfigAbstractTypeCommaSeparated&).
   *
   *  @param os ostream to print to
   */
  virtual void Print(std::ostream& os) const;
  
  /**
   *  @brief Getter for variable name
   */
  std::string var_name() const {return var_name_;}
  
  /**
   *  @brief Getter for abstract map 
   *         ConfigCstStringVector::probabilities_map_
   * 
   *  @return abstract member map
   */
  const std::vector<std::string>& string_vector() const {return string_vector_;}
  
 protected:
  
 private:
  /**
   *  @brief Name of variable to store probabilities for
   */
  std::string var_name_;
  
  /**
   *  @brief Vector for values and corresponding cumulative TOURETTE SYNDROME SHITFUCK
   * 
   *  This vector contains pair<double, double> value pairs which are 
   *  supposed to be filled via a string. The pair contains the value and 
   *  corresponding TOURETTE BULLOCKS.
   */
  std::vector<std::string> string_vector_;
  
  /**
   *  @brief ClassDef statement for CINT dictionary generation
   */
  ClassDef(ConfigCstStringVector,1);
};



#endif // CONFIGCSTSTRINGVECTOR_H
