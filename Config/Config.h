/** \class Config
 *  \brief DooFit Config base class.
 *
 *  This is the base class for all DooFit config objects. It offers
 *  configuration via the boost::program_options package.
 */

// STL
#include <string>
#include <vector>

// Boost
#include <boost/program_options.hpp>

// ROOT

// RooFit

// forward declarations


class Config {
  
 public:
  
  /**
   *  \brief Standard constructor.
   *
   *  Sets all memebers to default values.
   */
  Config();
  
  /**
   *  \brief Constructor for usage with input from command line.
   *
   *  @param argc Number of arguments.
   *  @param argv Char array with arguments.
   */
  Config(int argc, char* argv);
  
  /**
   *  \brief Constructor for usage with string vector command options.
   */
  Config(const vector<string>& option_vector);
  
  /**
   *  \brief Destructor.
   */
  ~Config;
      
  
  
  const boost::program_options::description&    desc();
  const boost::program_options::variables_map&  var_map();
  const vector<string>& unrec_options(); 
  
 protected:
  /**
   *  \brief Name of the configuration object.
   */
  std::string name_;
  
  /** 
   *  \brief Program options description.
   */
  boost::program_options::description   desc_;
  /** 
   *  \brief Program options variables map.
   */
  boost::program_options::variables_map var_map_;
  /** 
   *  \brief Vector of unrecognized program options.
   */
  vector<string> unrec_options_;
  
 private: 
};
