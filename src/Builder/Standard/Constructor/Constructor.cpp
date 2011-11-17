#include "Builder/Standard/Constructor/Constructor.h"

// from STL
#include <exception>
#include <iostream>
#include <string>

// from boost
#include <boost/foreach.hpp>
#include <boost/property_tree/info_parser.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/json_parser.hpp>

// from project
#include "Utils/MsgStream.h"

// from Builder/Standard subproject
#include "Builder/Standard/Pdfs/Common/Component.h"
#include "Builder/Standard/Pdfs/Common/CategoryBasic.h"
#include "Builder/Standard/Pdfs/Common/CategorySuper.h"
#include "Builder/Standard/Pdfs/Common/SimCategory.h"
#include "Builder/Standard/Pdfs/Mass/DimMass.h"
#include "Builder/Standard/Pdfs/Tag/DimTag.h"



using namespace std;
using namespace boost;
namespace bpt = property_tree;

using namespace Builder::Standard;

Constructor::Constructor() :
   tree_main_()
 , map_dimensions_()
 , map_categories_basic_()
 , map_categories_super_()
{
  
}

Constructor::~Constructor(){
  map_dimensions_.clear();
  map_dimensions_.clear();
}

void Constructor::load( const string& file_input ){
  
  // parse file input.
  ParseFileInput( file_input ); 
  CreateDimensions();
  CreateCategories();
//  CreateSuperCategories();
  
  // Testing ground, to be put into a function
  // bpt::ptree tree_simcategories = tree_main_.get_child("SimCategories");
  //   BOOST_FOREACH(bpt::ptree::value_type &tree_simcat, tree_simcategories){
  //     cout << tree_simcat.first << " " << endl;
  //   }  
  
  
} 

void Constructor::ParseFileInput( const string& file_input ){
  // try to parse file
  try {
    sinfo << "Trying to parse " << file_input << " as INFO file. ";
    read_info(file_input, tree_main_);
  }
  catch (std::exception const& exc_info ) {
    try {
      sinfo << "Failed!" << endmsg;
      sinfo << "Trying to parse " << file_input << " as JSON file. ";
      read_json(file_input, tree_main_);
    }
    catch (std::exception const& exc_json ) {
      try {
        sinfo << "Failed!" << endmsg;
        sinfo << "Trying to parse " << file_input << " as XML  file. ";
        read_xml(file_input, tree_main_);
      }
      catch (std::exception const& exc_xml ) {
        sinfo << "Failed!" << endmsg;
        serr  << "Failed to parse " << file_input << "." << endmsg;
        throw;
      }
    }
  }
  sinfo << "Success!" << endmsg;
}

void Constructor::CreateDimensions(){
  bpt::ptree tree_dimensions = tree_main_.get_child("Dimensions");
  
  // loop over dimensions tree and add dimensions to map
  BOOST_FOREACH(bpt::ptree::value_type &tree_dim, tree_dimensions){
    string dim_type = tree_dim.first;
    string dim_name = tree_dim.second.get_value<string>();
    
    // insert to map and check for duplicates
    pair<map<string,shared_ptr<AbsDimension> >::iterator,bool> ret;
    ret = map_dimensions_.insert(pair<string,shared_ptr<AbsDimension> >( dim_name, CreateDimension(dim_type))) ;
    if ( ret.second == false ){
      cout << "Constructor: Tried to insert dimension of type '" << dim_type << "' and name '" << dim_name << "' to map_dimensions_ twice. ABORTING." << endl;
      throw;
    }
    map_dimensions_[dim_name]->Initialize(tree_dim);
  }
}

shared_ptr<AbsDimension> const Constructor::CreateDimension( std::string dim_type ){
  // Real dimensions
  if ( dim_type == "Mass" )       return shared_ptr<AbsDimension>(new DimMass);       
  //if ( dim_type == "Propertime" ) return shared_ptr<AbsDimension>(new DimPropertime);
  
  // Category dimensions
  if ( dim_type == "Tag" ) return shared_ptr<AbsDimension>(new DimTag);
  
  
  else{
    cout << "Could not identify dimension type '" << dim_type << "'. ABORT." << endl;
    throw;    
  }
}

void Constructor::CreateCategories(){
  // Test if categories were specified
  bpt::ptree tree_categories;
  try {
    tree_categories = tree_main_.get_child("Categories");
  }
  catch ( const std::exception &cat_ex ) {
      swarn << "Constructor: Could not find 'Categories' in property tree. Assuming that no categories were defined." << endmsg;
      return;     
  }
  
  try {
    BOOST_FOREACH(bpt::ptree::value_type &tree_cat, tree_categories){    
      string cat_type = tree_cat.first;
      string cat_name = tree_cat.second.get_value<string>();
      
      if ( cat_type == "Basic"){
        shared_ptr<CategoryBasic> cat_basic_obj_temp(new CategoryBasic());
        pair< map< string, shared_ptr< CategoryBasic > >::iterator, bool > ret; 
        ret = map_categories_basic_.insert(pair< string, shared_ptr<CategoryBasic> >( cat_name, cat_basic_obj_temp ));
        if ( ret.second == false ){
          serr << "Constructor: Tried to insert category of type '" << cat_type << "' and name '" << cat_name << "' to categories' map twice." << endmsg;
          throw;
        }

        map_categories_basic_[cat_name]->Initialize(tree_cat);
      }
      else if ( cat_type == "Super" ){
        shared_ptr<CategorySuper> cat_super_obj_temp(new CategorySuper());
        pair< map< string, shared_ptr< CategorySuper > >::iterator, bool > ret; 
        ret = map_categories_super_.insert(pair< string, shared_ptr<CategorySuper> >( cat_name, cat_super_obj_temp ));
        if ( ret.second == false ){
          serr << "Constructor: Tried to insert category of type '" << cat_type << "' and name '" << cat_name << "' to map_categories_ twice." << endmsg;
          throw;
        }

        map_categories_super_[cat_name]->Initialize(tree_cat, map_categories_basic_);
      }
    }  
  }
  catch(...){
    serr << "Constructor: Something went wrong in the parsing of categories." << endmsg;
    throw;
  }

  
  // Create normal categories
  
  
  // Create SuperCategories
  
}

