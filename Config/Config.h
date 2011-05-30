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
      
  
  /**
   *  \brief Getter for program options description.
   */
  const boost::program_options::description&    desc(){
    return desc_;
  }

  /**
   *  \brief Getter for program options variables map.
   */
  const boost::program_options::variables_map&  var_map(){
    return var_map_;
  }
  
  /**
   *  \brief Getter for program options that could not be parsed.
   */
  const vector<string>& unrec_options(){
    return unrec_options_;
  }
  
 protected:
  /**
   *  \brief Name of the configuration object.
   */
  std::string name_;
  
  /** 
   *  \brief Program options description for non-hidden options.
   */
  boost::program_options::description   desc_;
  /** 
   *  \brief Program options variables map for non-hidden options.
   */
  boost::program_options::variables_map var_map_;
  /** 
   *  \brief Container for all non-hidden program option descriptions.
   *
   *  Each entry is for one group/category of options to be set. All entries 
   *  will be merged into desc_
   */
  vector <boost::program_options::description> descs_;
  
  /** 
   *  \brief Program options description for hidden options.
   */
  boost::program_options::description   desc_hidden_;
  /** 
   *  \brief Program options variables map for hidden options.
   */
  boost::program_options::variables_map var_map_hidden_;
  /** 
   *  \brief Container for all hidden program option descriptions.
   *
   *  Each entry is for one group/category of options to be set. All entries 
   *  will be merged into desc_hidden_
   */
  vector <boost::program_options::description> descs_hidden_;
  
  /** 
   *  \brief Vector of unrecognized program options.
   */
  vector<string> unrec_options_;
  
 private: 
};
