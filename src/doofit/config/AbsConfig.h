
/** @namespace doofit::config
 *
 *  @brief Config subsystem namespace.
 *
 *  This namespace contains all objects relevant for the Config system of 
 *  DooFit, but not actual Config objects for other modules. These are to 
 *  remain in the namespace of the module itself.
 *
 */

#ifndef CONFIG_h
#define CONFIG_h

// from STL
#include <string>
#include <vector>
#include <map>

// from Boost
#ifndef __CINT__
#include <boost/program_options.hpp>
#else
// ROOT Cint hacks...
namespace boost { namespace program_options {
  class options_description;
  class variables_map;
  class command_line_parser;
};};
#endif /* __CINT __ */

// from ROOT
#include "TObject.h"

// from RooFit

namespace doofit {
  
namespace config {
  /** \class doofit::config::AbsConfig
   *  \brief DooFit abstract Config base class.
   *
   *  This is the abstract base class for all DooFit config objects. It offers
   *  configuration via the boost::program_options package. All derived classes 
   *  \a must define AbsConfig::DefineOptions() and AbsConfig::LoadOptions() as well as
   *  the AbsConfig::PrintOptions() function.
   *
   *  Furthermore, the AbsConfig::help_flag_ option and AbsConfig::config_file_ should 
   *  be set by the derived class accordingly. AbsConfig::CheckHelpFlagAndPrintHelp()
   *  can be used to capture the set help flag and print a help message. Both \a 
   *  must be defined by the first AbsConfig object to be initialized.
   *
   *  Also check if you need to implement a copy constructor inside your derived
   *  class (will be generated automatically by C++, but this might behave 
   *  incorrectly). A default (i.e. argumentless) constructor is not wanted in 
   *  general, but might be needed for ROOT CINT and .root file streaming (see 
   *  below).
   *
   *  Usage examples can be found in ConfigTest and ConfigTestSecond as well as in
   *  file ConfigTestMain.cpp.
   *
   *  @section defining Defining options to be parsed
   *
   *  Options for parsing are defined in AbsConfig::DefineOptions() using 
   *  boost::program_options::options_description. See boost documentation for 
   *  more details on that or ConfigTest and ConfigTestSecond for an example. The
   *  usage of AbsConfig::GetOptionString(std::string, std::string) is also 
   *  recommended to group options into sections defined by the config object's
   *  name.
   *
   *  @see config::AbsConfig::desc_visible_
   *  @see config::AbsConfig::desc_hidden_
   *
   *  Usage example:
   *
   *  @code
   *  void MyConfig::DefineOptions() {
   *    po::options_description* generic = new po::options_description("Generic options");
   *    generic->add_options()
   *    ("help", "produce help message")
   *    ("config-file", po::value<string>(&config_file_)->default_value(""), "config file to parse")
   *    (GetOptionString("my-test-switch","s").c_str(), po::value<bool>(&my_test_switch_)->default_value(false), "my test bool switch")
   *    (GetOptionString("my-test-int","i").c_str(), po::value<int>()->default_value(0), "my test integer");
   *  
   *    descs_visible_.push_back(generic);
   *  }
   *  @endcode
   *
   *  @section parsing Parsing command line and config file
   *
   *  Parsing command line and an optional config file (see config::AbsConfig::config_file_)
   *  is very simple through config::AbsConfig::InitializeOptions(const AbsConfig&) and 
   *  config::AbsConfig::InitializeOptions(int, char* []). 
   *
   *  Usage example:
   *
   *  @code
   *  // instantiate first config object
   *  MyConfig1 config1("MyConfig1");
   *  
   *  // initialize config object via argc and argv
   *  config1.InitializeOptions(argc, argv);
   *  
   *  // instantiate second config object
   *  MyConfig2 config2("MyConfig2");
   *  
   *  // Initialize config object via previously initialized config object.
   *  // See config::AbsConfig::InitializeOptions() why you should avoid initializing via 
   *  // argc and argv again.
   *  config2.InitializeOptions(config);
   *
   *  // instantiate third config object
   *  MyConfig3 config3("MyConfig3");
   *  
   *  // Initialize config object via previously initialized config object.
   *  // See config::AbsConfig::InitializeOptions() why you should avoid initializing via 
   *  // argc and argv again.
   *  config3.InitializeOptions(config2);  
   *  
   *  // More or less mandatory step after initialization of all config objects.
   *  // Check if the help flag is set and (if so) print a help message.
   *  config1.CheckHelpFlagAndPrintHelp();
   *  
   *  // Print all set options for the user (optional).
   *  config::AbsConfig::PrintAll();
   *  @endcode
   *
   *  @section loading Loading parsed options
   *
   *  Parsed options can be loaded into specific member variables via 
   *  config::AbsConfig::LoadOptions(). Again, see boost documentation on how to get the 
   *  parsed options.
   *
   *  Usage example:
   *
   *  @code
   *  void MyConfig::LoadOptions() {
   *    set_my_test_int(var_map_[GetOptionString("my-test-int")].as<int>());
   *    
   *    if (var_map_.count("help")) help_flag_ = true;
   *  }
   *  @endcode
   *
   *  @section root-files ROOT file streaming
   *
   *  To be able to stream config objects into ROOT files and read them afterwards
   *  a dictionary for the derived class needs to be created. See the ROOT 
   *  documentation for details. In general, adding a ClassDef statement to the 
   *  class definition and generating the dictionary via rootcint (can be 
   *  automated via CMake through the root_generate_dictionaries macro in 
   *  FindROOT.cmake) should be enough.
   *
   *  Some hacks with #ifndef macros might be required for rootcint to find 
   *  certain headers and not see certain things it cannot cope with.
   *  
   *  AbsConfig itself is prepared for dictionary building. However, all boost 
   *  related member variables for program options had to be masked from rootcint.
   *  This means that these members will not be saved to and read from ROOT files!
   *
   *  Example:
   *
   *  @code
   *  // RooFit
   *  // forward declaration not enough as rootcint dictionary will fail compiling if 
   *  // not included
   *  #include "RooAbsPdf.h"
   *  #include "RooArgSet.h"
   *  
   *  // from project
   *  #ifndef __CINT__
   *  #include "doofit/config/AbsConfig.h"
   *  #else
   *  // ROOT Cint hacks...
   *  #include "../../Config/AbsConfig.h"
   *  #endif // __CINT __
   *  
   *  class MyConfig : public config::AbsConfig {
   *  ...
   *    ClassDef(MyConfig, 42);
   *  };
   *  @endcode
   *
   *  @author Florian Kruse
   *  @author Julian Wishahi
   *
   *  @see ConfigTest
   *  @see ConfigTestSecond
   *  @see ConfigTestMain.cpp
   */
  
  // forward declarations
  class AbsConfig;
  
  /// Map of AbsConfig pointers accessible via std::string
  typedef std::map<unsigned int,AbsConfig*> ConfigMap;
  
  class AbsConfig : public TObject {
    
  public:
    
    /**
     *  @brief Standard constructor.
     *
     *  Sets all memebers to default values.
     *
     *  @param name Name of this config object.
     */
    AbsConfig(const std::string& name);
    
    /**
     *  @brief Copy constructor
     * 
     *  @param other config object to copy
     */
    AbsConfig(const AbsConfig& other);
    
    /**
     *  \brief Destructor.
     */
    virtual ~AbsConfig();
    
    /**
     *  @brief Standard assignment operator
     *
     *  This operator is needed as the default auto-build version will not work as
     *  boost::program_options::options_description are not assignable.
     *
     *  @param other other AbsConfig object to assign
     *  @return reference to this AbsConfig object
     */
    AbsConfig& operator=(const AbsConfig& other);
    
    /** @name Initializer functions
     *  Functions to initialize config objects through boost::program_options.
     */
    ///@{
    /**
     *  \brief Initialize options with input from command line.
     *
     *  Using argc and argv this function initializes all options using 
     *  boost::program_options. This should only be used once and for the first 
     *  and most general config object. Also the config object using this must 
     *  define options for AbsConfig::help_flag_ and AbsConfig::config_file_ if a config
     *  file is to be supported.
     *  
     *  Further config objects should use AbsConfig::InitializeOptions(const AbsConfig&)
     *  instead to initialize themself.
     *
     *  @param argc Number of arguments.
     *  @param argv char* array with arguments.
     */
    void InitializeOptions(int argc, char* argv[]);
    
    /**
     *  \brief Initialize options with with previously initialized AbsConfig object.
     *
     *  Using previous_config this function initializes all options using 
     *  unrecognized boost::program_options from a previously initialized AbsConfig
     *  object. This includes AbsConfig::config_file_ of previous_config. Therefore, 
     *  the config file (if set in previous_config) will be parsed again. 
     *  Unfortunately, there is no smarter way to handle this with 
     *  boost::program_options.
     *
     *  @param previous_config previous AbsConfig object for which its unrecognized 
     *                         options are to be used for this AbsConfig object. 
     */
    void InitializeOptions(const AbsConfig& previous_config);
    ///@}
    
    /** @name Printing
     *  Functions for printing of currently set options and help messages.
     */
    ///@{
    /**
     *  \brief Print object summary (name, options, values).
     *
     *  This function will print a complete summary of the config object, incl.
     *  name, options, values etc.
     *
     *  \see AbsConfig::PrintOptions()
     */
    void Print() const;
    
    /**
     *  \brief Print all summaries for all AbsConfig objects.
     *
     *  Iterate through all AbsConfig objects and invoke their AbsConfig::Print() 
     *  function.
     */
    static void PrintAll();
    
    /**
     *  \brief Print help message for program_options
     *
     *  Print help for all available visible program_options. Due to static
     *  collection of all options any call of AbsConfig::PrintHelp() in any derived
     *  class will print the help message for all options throughout all AbsConfig 
     *  objects.
     */
    void PrintHelp() const;
    
    /**
     *  \brief Check for help flag and if necessary print help message
     *
     *  Depending on the setting of AbsConfig::help_flag_ print the help message and
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
     *  descriptions need to be added to AbsConfig::descs_ and AbsConfig::descs_hidden_
     *  (depending on if sections are to be visible or not). Individual entries
     *  in these vectors are for individual option categories/sections.
     */
    virtual void DefineOptions() = 0;
    
    /**
     *  \brief Load program options into own member variables.
     *
     *  Virtual function that will load all program options for this config 
     *  object from the internal variables map into own member variables. It is 
     *  automatically called by AbsConfig::InitializeOptions().
     * 
     *  Only in case of non-standard options that cannot be directly transfered
     *  via Boost's program_options, this function is actually needed.
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
    /**
     *  @brief Get option string with prefixed AbsConfig name
     *
     *  This function will return the option name @a option_name prefixed with 
     *  this config object's name. A short option @a short_option can also be 
     *  specified which will (of course) not be prefixed. Caution should be used
     *  for short options to avoid option clashes.
     *
     *  @param option_name the option name to be used (e.g. my-option for 
     *                     --configname.my-option on command line)
     *  @param short_option the short option name to be used (e.g. i for -i on 
     *                      command line)
     *  @return option string as used by boost::program_options
     */
    std::string GetOptionString(std::string option_name, std::string short_option="") const;
    ///@}
    
    /**
     *  @brief Name of the configuration object.
     */
    std::string name_;
    
#ifndef __CINT__
    /** @name program_options members
     *  All program_options related member variables.
     */
    ///@{
    /** 
     *  \brief Program options variables map for all options.
     */
    boost::program_options::variables_map var_map_;
    
    /** 
     *  \brief Container for all non-hidden program option descriptions.
     *
     *  Each entry is for one group/category of options to be set. All entries 
     *  will be merged into AbsConfig::desc_. As the boost developers were so 
     *  prospective to build boost::program_options::options_description
     *  in such a way that they cannot be copied or assigned, we need to keep a 
     *  vector of pointers instead of a vector of real objects.
     */
    std::vector <boost::program_options::options_description*> descs_visible_;
    
    /** 
     *  \brief Container for all hidden program option descriptions.
     *
     *  Each entry is for one group/category of options to be set. All entries 
     *  will be merged into \link AbsConfig::desc_hidden_. As the boost developers
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
#endif /* __CINT __ */
    
    /**
     *  \brief Config file to parse
     *
     *  Name of config file to parse for options. This is a static member as 
     *  parsing the option file only once and handling over unrecognized options
     *  is more complicated if it needs to be done for config file. 
     *  This member must be set through 
     *  boost::program_options::value<string>(&config_file_) in 
     *  AbsConfig::DefineOptions().
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
     *  AbsConfig::LoadOptions().
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
     *  @brief Get all visible program options descriptions
     *
     *  @return boost::program_options::options_description with all visible 
     *          options descriptions
     */
    boost::program_options::options_description GetAllVisibleOptionsDescriptions() const;
    
    /**
     *  @brief Get all program options descriptions (visible and hidden)
     *
     *  @return boost::program_options::options_description with all options 
     *          descriptions
     */
    boost::program_options::options_description GetAllOptionsDescriptions() const;
    
    /**
     *  \brief Parse options, store them and (if necessary) parse config file.
     *
     *  Parse using the given command line parser, store parsed options in 
     *  AbsConfig::var_map_, notify map and (if AbsConfig::config_file_ is set) parse 
     *  the config file as well (and notify, store...).
     *
     *  @param parser command line parser to use
     */
    void ParseOptionsAndConfigFile(boost::program_options::command_line_parser parser);
    
    /**
     *  @brief Unique ID of this AbsConfig object
     *
     *  For internal use in bookkeeping.
     */
    unsigned int id_;
    
    /**
     *  @brief Initialization state of this object
     *
     *  Used to prevent multiple initialization.
     */
    bool initialized_;
    
    /** @name static members
     *  All static member variables for functionality across AbsConfig objects.
     */
    ///@{
    /**
     *  \brief Static collection of all created AbsConfig objects.
     *
     *  This map contains pointers to all created AbsConfig objects. This is 
     *  necessary for all functionality that has to work on all AbsConfig objects 
     *  like printing of set options via AbsConfig::PrintAll().
     */
    static ConfigMap config_container_;
    /**
     *  @brief Unique ID counter
     *
     *  Internal counter for unique IDs. Bookkeeping is handled by AbsConfig base 
     *  class itself.
     */
    static unsigned int id_counter_;
    ///@}
    
    /**
     *  @brief ClassDef statement for CINT dictionary generation
     */
    //ClassDef(AbsConfig,1);
  };
  
  
  // let ROOT Cint not bother about this
#ifndef __CINT__
  /// boost error_info for the name of the config file (see boost::exception for reference)
  typedef boost::error_info<struct tag_my_info,std::string> ConfigName;
  
  /** \struct NameDuplicationException
   *  \brief Exception for name duplication in Config objects.
   */
  struct NameDuplicationException: public virtual boost::exception, public virtual std::exception { 
    virtual const char* what() const throw() { return "Name duplication"; }
  };
  
  /** \struct CmdArgsUsedTwiceException
   *  \brief Exception for second (and therefore unallowed) use of argc and argv 
   *         for initialization of AbsConfig object.
   */
  struct CmdArgsUsedTwiceException: public virtual boost::exception, public virtual std::exception { 
    virtual const char* what() const throw() { return "Command line arguments argc and argv used multiple times."; }
  };
  
  /** \struct AlreadyInitializedException
   *  \brief Exception for initialization attempt of already initialized AbsConfig 
   *         object
   */
  struct AlreadyInitializedException: public virtual boost::exception, public virtual std::exception { 
    virtual const char* what() const throw() { return "Object already initialized."; }
  };
#endif /* __CINT __ */
}
}
#endif //CONFIG_h
