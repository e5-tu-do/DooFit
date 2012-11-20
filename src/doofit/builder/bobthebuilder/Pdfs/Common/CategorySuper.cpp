#include "doofit/builder/bobthebuilder/Pdfs/Common/CategorySuper.h"

// from STL

// from boost
#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>

// from RooFit
#include "RooArgSet.h"
#include "RooCategory.h"
#include "RooSuperCategory.h"
#include "RooWorkspace.h"

// from project
#include "doocore/io/MsgStream.h"

// from project Builder BobTheBuilder
#include "doofit/builder/bobthebuilder/Pdfs/Common/CategoryBasic.h"

namespace doofit {
using namespace std;
using namespace boost;
using namespace boost::property_tree;
using namespace doocore::io;

namespace builder{ 
namespace bobthebuilder{

CategorySuper::CategorySuper() :
   name_("DummyNameCategorySuper")
 , desc_("Dummy description for a CategorySuper.")
 , subcats_("DummySubCat1,DummySubCat2")
 , vector_subcats_()
 , vector_types_()
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
  
  sinfo << "desc    \"" << desc_    << "\"" << endmsg;
  sinfo << "subcats \"" << subcats_ << "\"" << endmsg;
}

void CategorySuper::CreateTypes( const string &subcats, const map< string, shared_ptr< CategoryBasic > > &map_category_basic ){
  // split subcats
  split(vector_subcats_, subcats, is_any_of(","), token_compress_on);
  
  vector< vector< string > > subcat_type_vectors_vector;
  BOOST_FOREACH(string subcat_name, vector_subcats_){
    // find subcategory    
    map<std::string, shared_ptr< CategoryBasic > >::const_iterator subcat_it;
    subcat_it = map_category_basic.find(subcat_name);
    
    if (  subcat_it == map_category_basic.end()){
      serr << "CategorySuper: Could not find requested BasicCategory '" << subcat_name << "' for CategorySuper '" << name_ << "'. ABORTING!" << endmsg;
      throw;
    }
    
    // Get all types of this sub_cat and add it to the vector
    vector< string > subcat_type_vector;
    typedef const pair< string, int > basecattypesmap;
    BOOST_FOREACH(basecattypesmap subcat_entry, (*subcat_it).second->map_types()){
      subcat_type_vector.push_back(subcat_entry.first);
    }
    subcat_type_vectors_vector.push_back(subcat_type_vector);
  }
  
  for( vector< vector< string > >::const_iterator it_subcat_type_vector = subcat_type_vectors_vector.begin(); it_subcat_type_vector != subcat_type_vectors_vector.end(); it_subcat_type_vector++ ){
    if ( it_subcat_type_vector == subcat_type_vectors_vector.begin()){
      vector_types_ = (*it_subcat_type_vector);
    }
    else {
      vector_types_ = CombineTypes(vector_types_,*it_subcat_type_vector);
    }
  }
  // put braces, iterating by reference to manipulate entries
  BOOST_FOREACH(string& entry, vector_types_){
    entry.insert(0,"{");
    entry.append("}");
  }
}

vector< string > CategorySuper::CombineTypes( const vector< string > &subcat_one_types, const vector< string > &subcat_two_types ){
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

bool CategorySuper::AddToWorkspace( RooWorkspace& ws ){
  // create RooArgSet of subcategories
  RooArgSet cat_set;
  BOOST_FOREACH( string subcat_name, vector_subcats_ ){
    RooCategory* subcat = ws.cat(subcat_name.c_str());
    if (subcat == NULL){
      serr << "CategorySuper: RooCategory '" << subcat << "' does not exist on workspace. Unable to create SuperCategory '" << name_ << "'." << endmsg;
      throw;
    }
    else{
      cat_set.add(*subcat);
    }
  }
  // create RooSuperCategory
  RooSuperCategory super_cat(name_.c_str(),desc_.c_str(),cat_set);
  
  // check if object with same name already exists on workspace
  if (ws.obj(name_.c_str()) != NULL){
    cout << "CategorySuper: Tried to add RooSuperCategory with name '" << name_ << "' to the workspace! Object with this name already exists on the workspace. FAILED." << endl;
    throw;
  }
  else{
    ws.import(super_cat);
  }
  
  return true;
}

}}
}
