#ifndef DISCRETEPROBABILTYDISTRIBUTION_h
#define DISCRETEPROBABILTYDISTRIBUTION_h

// from project
#ifndef __CINT__
#include "Config/ConfigAbstractTypeCommaSeparated.h"
#else
// ROOT Cint hacks...
#include "../Config/ConfigAbstractTypeCommaSeparated.h"
#endif /* __CINT __ */

namespace Config {
  /** @class DiscreteProbabilityDistribution
   *  @brief Distribution for discrete probabilities of variables.
   *
   *  This class is merely a container for Config objects to store the discrete
   *  probability distribution of a certain variable. As it's inheriting from 
   *  ConfigAbstractTypeCommaSeparated, parsing from command line or config file
   *  is possible. 
   *
   *  It contains a variable name and a list of value and corresponding 
   *  probability pairs. String representation is 
   *  
   *  @verbatim var_name,value1,prob1,value2,prob2,...,valueN,probN @endverbatim
   * 
   *  @see ConfigAbstractTypeCommaSeparated
   *  @see ConfigTestAbstractType
   */
  
  class DiscreteProbabilityDistribution : public ConfigAbstractTypeCommaSeparated {
  public:
    /**
     *  @brief Default constructor for DiscreteProbabilityDistribution
     */
    DiscreteProbabilityDistribution() {}
    
    /**
     *  @brief Destructor for DiscreteProbabilityDistribution
     */
    ~DiscreteProbabilityDistribution() {}
    
    /**
     *  @brief Parse a given string
     *
     *  The given string @a str is parsed. If parsing is not successful, an 
     *  exception will be thrown. The string is supposed to be a comma-separated 
     *  list formatted like this:
     *
     *  @verbatim var_name,value1,prob1,value2,prob2,...,valueN,probN @endverbatim
     *
     *  @see DiscreteProbabilityDistribution::var_name_
     *  @see DiscreteProbabilityDistribution::probabilities_map_
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
     *         DiscreteProbabilityDistribution::probabilities_map_
     * 
     *  @return abstract member map
     */
    const std::vector<std::pair<double,double> >& probabilities() const {return probabilities_;}
    
  protected:
    
  private:
    /**
     *  @brief Name of variable to store probabilities for
     */
    std::string var_name_;
    
    /**
     *  @brief Vector for values and corresponding cumulative discrete probabilities 
     * 
     *  This vector contains pair<double, double> value pairs which are 
     *  supposed to be filled via a string. The pair contains the value and 
     *  corresponding cumulative probability.
     *
     *  @see DiscreteProbabilityDistribution::Parse(std::string)
     */
    std::vector<std::pair<double,double> > probabilities_;
    
    /**
     *  @brief ClassDef statement for CINT dictionary generation
     */
    ClassDef(DiscreteProbabilityDistribution,1);
  };
}

#endif // DISCRETEPROBABILTYDISTRIBUTION_h
