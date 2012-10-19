/** @namespace doofit::Builder::BobTheBuilder
 *
 *  @brief DooFit Builder::BobTheBuilder namespace for all classes related to the Builder
 *  BobTheBuilder implementation.
 *  @see Builder::BobTheBuilder::Bob
 *
 */


#ifndef BUILDER_BOBTHEBUILDER_BOB_H
#define BUILDER_BOBTHEBUILDER_BOB_H

// from STL
#include <map>
#include <string>

// from boost
#include <boost/property_tree/ptree.hpp>
#include <boost/shared_ptr.hpp>

// from project
#include "doofit/builder/BobTheBuilder/Pdfs/Common/PdfFull.h"

// from RooFit

// forward declarations outside project
class RooWorkspace;

namespace doofit {
namespace Builder { namespace BobTheBuilder {
// forward declarations inside project
class AbsDimension;
class CategoryBasic;
class CategorySuper;


/** @class Bob
 *  @brief A standard builder. Adds all necessary objects to a RooWorkspace.
 *
 *  This Builder class is meant as standard implementation. The configuration of
 *  the Bob is based on a combination of a @Config and a 
 *  boost::property_tree.
 * 
 */
class Bob{
 public:
  /** 
   *  @brief The standard constructor.
   *  
   *  Takes no arguments.
   *  @todo Add Bob constuctor with CommonConfig and BobConfig 
   *  as parameters.
   */
  Bob();
  
  /** 
   *  @brief The destructor for Bob.
   */
  ~Bob();
  
  /** 
   *  @brief Loads configuration from a INFO/XML/JSON file.
   *
   *  Prepares configuration from a boost::property_tree saved in INFO, XML, or
   *  JSON format.
   *  
   *  @param file_input Input file that can be parsed with boost::property_tree.
   */
  void load( const std::string& file_input );
  
  /** @brief Saves configuration to an INFO/XML/JSON file.
   *
   *  Saves configuration to a boost::property_tree in INFO, XML, or
   *  JSON format.
   *  
   *  @param file_output Output file for saving configuration.
   */
  void save( const std::string& file_output );
  
  /** 
   *  @brief Creates a configuration skeleton.
   *  
   *  Can be used to create a basic configuration that can be easily saved to
   *  a file.
   */
  void BuildSkeleton();
  
  /**
   *  @brief Set the main property_tree @see tree_main_ manually.
   */
  void set_tree_main( const boost::property_tree::ptree& tree_main ){ tree_main_ = tree_main; };
  
  /**
   *  @brief Get the main property_tree @see tree_main_.
   */
  const boost::property_tree::ptree& tree_main();
  
  /**
   *  @brief Fill workspace based on configuration of the Bob.
   */
  void DumpToRooWorkspace( RooWorkspace& ws);
  
 protected:
  
 private:
  /**
   *  @brief The main property_tree used for the configuration.
   */
  boost::property_tree::ptree tree_main_;
  
  /**
   *  @brief A map of AbsDimension for configuration of dimension variables.
   */
  std::map < std::string, boost::shared_ptr<AbsDimension> >  map_dimensions_;
  
  /**
   *  @brief A map of CategoryBasic for configuration of RooCategory.
   */  
  std::map < std::string, boost::shared_ptr<CategoryBasic> > map_categories_basic_;
  
  /**
   *  @brief A map of CategorySuper for configuration of RooSuperCategory.
   */
  std::map < std::string, boost::shared_ptr<CategorySuper> > map_categories_super_;
  
  PdfFull pdf_full_;
  
  /**
   *  @brief Parses the file_input.
   *
   *  Tries to parse files in INFO/XML/JSON format to property_tree. Called by
   *  load @see load.
   */
  void ParseFileInput( const std::string& file_input );
  
  /**
   *  @brief Adds all dimensions defined in property_tree to map_dimensions_.
   */
  void CreateDimensions();
  
  /**
   *  @brief Central function for association of dimension key to dimensions.
   *
   *  This function calls dimension constructors based on the dimension keys 
   *  used in the property_tree. It returns a shared pointer for the map
   *  insertion in CreateDimensions() @see CreateDimensions.
   */
  boost::shared_ptr<AbsDimension> const CreateDimension( std::string dim_type );
  
  /**
   *  @brief Creates CategoryBasic and CategorySuper objects.
   */
  void CreateCategories();  
  
  /**
   *  @brief Creates the actual full pdf based on dimensions and categories.
   */
  void CreatePdfFull();  
};
 
}}
}

#endif // BUILDER_BOBTHEBUILDER_BOB_H
