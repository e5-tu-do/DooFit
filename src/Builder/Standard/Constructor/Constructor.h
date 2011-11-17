#ifndef BUILDER_STANDARD_CONSTRUCTOR_H
#define BUILDER_STANDARD_CONSTRUCTOR_H

// from STL
#include <map>
#include <string>

// from boost
#include <boost/property_tree/ptree.hpp>
#include <boost/shared_ptr.hpp>

// from project


// from RooFit

// forward declarations outside project
class RooWorkspace;

namespace Builder { namespace Standard {
// forward declarations inside project
class AbsDimension;
class CategoryBasic;
class CategorySuper;



class Constructor{
 public:
  Constructor();
  ~Constructor();
  
  void load( const std::string& file_input );  
  void save( const std::string& file_input );
  
  void BuildSkeleton();
  
  void set_tree_main( const boost::property_tree::ptree& tree_main);
  const boost::property_tree::ptree& tree_main();
  
  void DumpToRooWorkspace( RooWorkspace& ws);
  
 protected:
  
 private:
  boost::property_tree::ptree tree_main_;
  
  std::map < std::string, boost::shared_ptr<AbsDimension> >  map_dimensions_;
  std::map < std::string, boost::shared_ptr<CategoryBasic> > map_categories_basic_;
  std::map < std::string, boost::shared_ptr<CategorySuper> > map_categories_super_;
  
  void ParseFileInput( const std::string& file_input );
  
  void CreateDimensions();
  boost::shared_ptr<AbsDimension> const CreateDimension( std::string dim_type );
  
  void CreateCategories();  
  
  void CreatePdfFull();  
};
 
}}


#endif // BUILDER_STANDARD_CONSTRUCTOR_H
