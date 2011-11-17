#include "Builder/Standard/Pdfs/Common/CategorySuper.h"

// from STL

// from boost
#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>

// from project
#include "Utils/MsgStream.h"

// from project Builder Standard
#include "Builder/Standard/Pdfs/Common/CategoryBasic.h"

using namespace Builder::Standard;

using namespace std;
using namespace boost;
using namespace boost::property_tree;




CategorySuper::CategorySuper() :
   name_("DummyNameCategorySuper")
 , desc_("Dummy description for a CategorySuper.")
 , subcats_("DummySubCat1,DummySubCat2")
 , list_subcats_()
 , list_types_()
{
  
}

CategorySuper::~CategorySuper(){
  
}

void CategorySuper::Initialize( const ptree::value_type& pt_head, const map< string, shared_ptr< CategoryBasic > > &map_category_basic ){
  name_    = pt_head.second.get_value<string>();
  
  ptree pt = pt_head.second;
  desc_    = pt.get<string>("desc");
  subcats_ = pt.get<string>("subcats");

  CreateTypes( subcats_, map_category_basic );
}

void CategorySuper::CreateTypes( const string &subcats, const map< string, shared_ptr< CategoryBasic > > &map_category_basic ){
  // split subcats
  split(list_subcats_, subcats, is_any_of(","), token_compress_on);
  
  vector< vector< string > > subcat_type_lists_list;
  BOOST_FOREACH(string subcat_name, list_subcats_){
    // find subcategory    
    map<std::string, shared_ptr< CategoryBasic > >::const_iterator subcat_it;
    subcat_it = map_category_basic.find(subcat_name);
    
    if (  subcat_it == map_category_basic.end()){
      serr << "CategorySuper: Could not find requested BasicCategory '" << subcat_name << "' for CategorySuper '" << name_ << "'. ABORTING!" << endmsg;
      throw;
    }
    
    // Get all types of this sub_cat and add it to the list
    vector< string > subcat_type_list;
    typedef const pair< string, int > basecattypesmap;
    BOOST_FOREACH(basecattypesmap subcat_entry, (*subcat_it).second->map_types()){
      subcat_type_list.push_back(subcat_entry.first);
    }
    subcat_type_lists_list.push_back(subcat_type_list);
  }
  
  for( vector< vector< string > >::const_iterator it_subcat_type_list = subcat_type_lists_list.begin(); it_subcat_type_list != subcat_type_lists_list.end(); it_subcat_type_list++ ){
    if ( it_subcat_type_list == subcat_type_lists_list.begin()){
      list_types_ = (*it_subcat_type_list);
    }
    else {
      list_types_ = CombineTypes(list_types_,*it_subcat_type_list);
    }
  }
  // put braces, iterating by reference to manipulate entries
  BOOST_FOREACH(string& entry, list_types_){
    entry.insert(0,"{");
    entry.append("}");
  }
}

vector< string > CategorySuper::CombineTypes( const vector< string > &subcat_one_types, const vector< string > &subcat_two_types){
  string sep   = ";";
  vector< string > temp_comb_types;
  BOOST_FOREACH(string type_one, subcat_one_types){
    BOOST_FOREACH(string type_two, subcat_two_types){
      string temp_string = type_one;
      temp_string.append(sep);
      temp_string.append(type_two);
      temp_comb_types.push_back(temp_string);
    }
  }
  return temp_comb_types;  
}


