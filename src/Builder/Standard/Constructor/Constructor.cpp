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
#include "Builder/Standard/Pdfs/Common/Component.h"
#include "Builder/Standard/Pdfs/Common/SimCategory.h"
#include "Builder/Standard/Pdfs/Mass/DimMass.h"

using namespace std;
using namespace boost;
namespace bpt = property_tree;

using namespace Builder::Standard;

Constructor::Constructor() :
   tree_main_()
 , map_dimensions_()
{
  
}

Constructor::~Constructor(){
  
  map_dimensions_.clear();
}

void Constructor::load( const string& file_input ){
  
  // parse file input.
  ParseFileInput( file_input );
  CreateDimensions();
  
  // Testing ground, to be put into a function
  bpt::ptree tree_simcategories = tree_main_.get_child("SimCategories");
  BOOST_FOREACH(bpt::ptree::value_type &tree_simcat, tree_simcategories){
    cout << tree_simcat.first << " " << endl;
  }  
  
  
} 

void Constructor::ParseFileInput( const string& file_input ){
  // try to parse file
  try {
    cout << "Trying to parse " << file_input << " as INFO file. ";
    read_info(file_input, tree_main_);
  }
  catch (std::exception const& exc_info ) {
    try {
      cout << "Failed!" << endl;
      cout << "Trying to parse " << file_input << " as JSON file. ";
      read_json(file_input, tree_main_);
    }
    catch (std::exception const& exc_json ) {
      try {
        cout << "Failed!" << endl;
        cout << "Trying to parse " << file_input << " as XML  file. ";
        read_xml(file_input, tree_main_);
      }
      catch (std::exception const& exc_xml ) {
        cout << "Failed!" << endl;
        throw;
      }
    }
  }
  cout << "Success!" << endl;
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
      cout << "Constructor: Tried to insert dimension of type '" << dim_type << "' and name '" << dim_name << "' to map twice. ABORTING." << endl;
      throw;
    }
    map_dimensions_[dim_name]->Initialize(tree_dim);
  }
}

shared_ptr<AbsDimension> const Constructor::CreateDimension( std::string dim_type ){
  if ( dim_type == "Mass" )       return shared_ptr<AbsDimension>(new DimMass);       
  //if ( dim_type == "Propertime" ) return shared_ptr<AbsDimension>(new DimPropertime);

  else{
    cout << "Could not identify dimension type '" << dim_type << "'. ABORT." << endl;
    throw;    
  }

}