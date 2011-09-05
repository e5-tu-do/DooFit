/** \class Config
 *  \brief DooFit Config base class.
 *
 *  This is the abstract base class for all DooFit config objects. It offers
 *  configuration via the boost::program_options package. All derived classes 
 *  \a must define Config::DefineOptions() and Config::LoadOptions() as well as
 *  the Config::PrintOptions() function.
 *
 *  Furthermore, the Config::help_flag_ option and Config::config_file_ should 
 *  be set by the derived class accordingly. Config::CheckHelpFlagAndPrintHelp()
 *  can be used to capture the set help flag and print a help message. Both \a 
 *  must be defined by the first Config object to be initialized.
 *
 *  Usage examples can be found in ConfigTest and ConfigTestSecond as well as in
 *  file ConfigTestMain.cpp.
 *
 *  \section parsing Parsing command line and config file
 *
 *  Parsing command line and an optional config file (see Config::config_file_)
 *  is very simple through Config::InitializeOptions(const Config&) and 
 *  Config::InitializeOptions(int, char* []). 
 *
 *  Usage example:
 *
 *  \code
 *  // instantiate first config object
 *  CommonConfig config("CommonConfig");
 *  
 *  // initialize config object via argc and argv
 *  config.InitializeOptions(argc, argv);
 *  
 *  // instantiate second config object
 *  ConfigTestSecond config2("SecondConfig");
 *  
 *  // Initialize config object via previously initialized Config object.
 *  // See Config::InitializeOptions() why you should avoid initializing via 
 *  // argc and argv again.
 *  config2.InitializeOptions(config);
 *  
 *  // More or less mandatory step after initialization of all Config objects.
 *  // Check if the help flag is set and (if so) print a help message.
 *  config.CheckHelpFlagAndPrintHelp();
 *  
 *  // Print all set options for the user (optional).
 *  Config::PrintAll();
 *  \endcode
 *
 *  \author Florian Kruse
 *  \author Julian Wishahi
 *
 *  \see ConfigTest
 *  \see ConfigTestSecond
 *  \see ConfigTestMain.cpp
 */

#ifndef CONFIG_h
#define CONFIG_h

// STL
#include <string>
#include <vector>

// Boost
#include <boost/program_options.hpp>

// ROOT

// RooFit

// forward declarations
class Config;

/// Map of Config pointers accessible via std::string
typedef std::map<std::string,Config*> ConfigMap;

class Config {
  
 public:
  
  /**
   *  \brief Standard constructor.
   *
   *  Sets all memebers to default values.
   *
   *  @param name Name of this Config object.
   */
  Config(const std::string& name);
  
  /**
   *  \brief Destructor.
   */
  virtual ~Config();
  
  /** @name Initializer functions
   *  Functions to initialize Config objects through boost::program_options.
   */
  ///@{
  /**
   *  \brief Initialize options with input from command line.
   *
   *  Using argc and argv this function initializes all options using 
   *  boost::program_options. This should only be used once and for the first 
   *  and most general Config object. Also the Config object using this must 
   *  define options for Config::help_flag_ and Config::config_file_ if a Config
   *  file is to be supported.
   *  
   *  Further Config objects should use Config::InitializeOptions(const Config&)
   *  instead to initialize themself.
   *
   *  @param argc Number of arguments.
   *  @param argv char* array with arguments.
   */
  void InitializeOptions(int argc, char* argv[]);

  /**
   *  \brief Initialize options with with previously initialized Config object.
   *
   *  Using previous_config this function initializes all options using 
   *  unrecognized boost::program_options from a previously initialized Config
   *  object. This includes Config::config_file_ of previous_config. Therefore, 
   *  the config file (if set in previous_config) will be parsed again. 
   *  Unfortunately, there is no smarter way to handle this with 
   *  boost::program_options.
   *
   *  @param previous_config previous Config object for which its unrecognized 
   *                         options are to be used for this Config object. 
   */
  void InitializeOptions(const Config& previous_config);
  ///@}

  /** @name Printing
   *  Functions for printing of currently set options and help messages.
   */
  ///@{
  /**
   *  \brief Print object summary (name, options, values).
   *
   *  This function will print a complete summary of the Config object, incl.
   *  name, options, values etc.
   *
   *  \see Config::PrintOptions()
   */
  void Print() const;
  
  /**
   *  \brief Print all summaries for all Config objects.
   *
   *  Iterate through all Config objects and invoke their Config::Print() 
   *  function.
   */
  static void PrintAll();
  
  /**
   *  \brief Print help message for program_options
   *
   *  Print help for all available visible program_options. Due to static
   *  collection of all options any call of Config::PrintHelp() in any derived
   *  class will print the help message for all options throughout all Config 
   *  objects.
   */
  void PrintHelp() const;
  
  /**
   *  \brief Check for help flag and if necessary print help message
   *
   *  Depending on the setting of Config::help_flag_ print the help message and
   *  exit the program afterwards. 
   */
  void CheckHelpFlagAndPrintHelp() const {
    if (help_flag_) {
      PrintHelp();
      exit(0);
    }
  }
  ///@}
 
 protected:
  /** @name Virtual functions
   *  Pure virtual functions to be implemented in derived classes.
   */
  ///@{   
  /**
   *  \brief Define all program options.
   *
   *  Virtual function that will define all program options for this config 
   *  object. It is automatically called by constructors. All option 
   *  descriptions need to be added to Config::descs_ and Config::descs_hidden_
   *  (depending on if sections are to be visible or not). Individual entries
   *  in these vectors are for individual option categories/sections.
   */
  virtual void DefineOptions() = 0;
  
  /**
   *  \brief Load program options into own member variables.
   *
   *  Virtual function that will load all program options for this config 
   *  object from the internal variables map into own member variables. It is 
   *  automatically called by Config::InitializeOptions(). 
   */
  virtual void LoadOptions() = 0;
  
  /**
   *  \brief Print all options for this object.
   *
   *  Virtual function that will print all options for this config object.
   */
  virtual void PrintOptions() const = 0;
  ///@}
  
  /** @name Helper functions
   *  Helper functions for printing and other needs.
   */
  ///@{ 
  
  ///@}
  
  /**
   *  \brief Name of the configuration object.
   */
  std::string name_;
  
  /** @name program_options members
   *  All program_options related member variables.
   */
  ///@{
  /** 
   *  \brief Program options description for all options.
   */
  boost::program_options::options_description desc_;
  /** 
   *  \brief Program options variables map for all options.
   */
  boost::program_options::variables_map var_map_;
  
  /** 
   *  \brief Program options description for non-hidden options.
   */
  boost::program_options::options_description desc_visible_;
  /** 
   *  \brief Container for all non-hidden program option descriptions.
   *
   *  Each entry is for one group/category of options to be set. All entries 
   *  will be merged into Config::desc_. As the boost developers were so 
   *  prospective to build boost::program_options::options_description
   *  in such a way that they cannot be copied or assigned, we need to keep a 
   *  vector of pointers instead of a vector of real objects.
   */
  std::vector <boost::program_options::options_description*> descs_visible_;
  
  /** 
   *  \brief Program options description for hidden options.
   */
  boost::program_options::options_description desc_hidden_;
  /** 
   *  \brief Container for all hidden program option descriptions.
   *
   *  Each entry is for one group/category of options to be set. All entries 
   *  will be merged into \link Config::desc_hidden_. As the boost developers
   *  were so prospective to build boost::program_options::options_description
   *  in such a way that they cannot be copied or assigned, we need to keep a 
   *  vector of pointers instead of a vector of real objects.
   */
  std::vector <boost::program_options::options_description*> descs_hidden_;
  
  /** 
   *  \brief Vector of unrecognized command line program options.
   */
  std::vector<std::string> unrec_options_;
  ///@}
  
  /**
   *  \brief Config file to parse
   *
   *  Name of config file to parse for options. This is a static member as 
   *  parsing the option file only once and handling over unrecognized options
   *  is more complicated if it needs to be done for config file. 
   *  This member must be set through 
   *  boost::program_options::value<string>(&config_file_) in 
   *  Config::DefineOptions().
   *
   *  Example:
   *  \code 
   *  void MyConfig::DefineOptions() {
   *    po::options_description* generic = new po::options_description("Generic options");
   *    generic->add_options()
   *    ("config-file", po::value<string>(&config_file_)->default_value(""), "config file to parse");
   *  }
   *  \endcode
   */
  std::string config_file_;
  
  /**
   *  \brief Print help flag
   *
   *  Flag specifying whether the help message with available options should be 
   *  printed. The derived class needs to set this by hand in 
   *  Config::LoadOptions().
   *  
   *  Example:
   *  \code 
   *  void MyConfig::LoadOptions() {
   *    if (var_map_.count("help")) help_flag_ = true;
   *  }
   *  \endcode
   */
  bool help_flag_;
  
 private: 
  /**
   *  \brief Combine all previously defined option descriptions.
   *
   *  Iteratre through Config::descs_ and Config::descs_hidden_ and combine them
   *  into Config::desc_ and Config::desc_hidden_. This function will be called
   *  automatically through the constructors.
   */
  void CombineOptions();
  
  /**
   *  \brief Parse options, store them and (if necessary) parse config file.
   *
   *  Parse using the given command line parser, store parsed options in 
   *  Config::var_map_, notify map and (if Config::config_file_ is set) parse 
   *  the config file as well (and notify, store...).
   *
   *  @param parser command line parser to use
   */
  void ParseOptionsAndConfigFile(boost::program_options::command_line_parser parser);
  
  /** @name static members
   *  All static member variables for functionality across Config objects.
   */
  ///@{
  /**
   *  \brief Static collection of all visible program_options.
   *
   *  This options_description object contains all visible options that any 
   *  derived Config object ever created. Will be used by Config::PrintHelp()
   *  to print a help message for all options through any Config object. 
   */
  static boost::program_options::options_description desc_visible_all_;
  /**
   *  \brief Static collection of all created Config objects.
   *
   *  This map contains pointers to all created Config objects. This is 
   *  necessary for all functionality that has to work on all Config objects 
   *  like printing of set options via Config::PrintAll().
   */
  static ConfigMap config_container_;
  ///@}
};

/// boost error_info for the name of the config file (see boost::exception for reference)
typedef boost::error_info<struct tag_my_info,std::string> ConfigName;

/** \struct ConfigNameDuplicationException
 *  \brief Exception for name duplication in Config objects.
 */
struct ConfigNameDuplicationException: public virtual boost::exception, public virtual std::exception { 
  virtual const char* what() const throw() { return "Name duplication"; }
};

/** \struct ConfigCmdArgsUsedTwiceException
 *  \brief Exception for second (and therefore unallowed) use of argc and argv 
 *         for initialization of Config object.
 */
struct ConfigCmdArgsUsedTwiceException: public virtual boost::exception, public virtual std::exception { 
  virtual const char* what() const throw() { return "Command line arguments argc and argv used multiple times."; }
};


#endif //CONFIG_h
