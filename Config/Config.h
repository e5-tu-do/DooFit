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

class Config {
  
 public:
   
  /**
   *  \brief The standard constructor.
   *
   *  Sets all memebers to default values.
  **/

  Config();
  Config(int argc, char* argv);
  Config(const vector<string>& option_vector);
  
  ~Config;
  
  const boost::program_options::description&    desc();
  const boost::program_options::variables_map&  var_map();
  const vector<string>& unrec_options(); 
  
 protected:
  std::string name_;
  
  boost::program_options::description   desc_;
  boost::program_options::variables_map var_map_;
  vector<string> unrec_options_;
  
 private: 
};
