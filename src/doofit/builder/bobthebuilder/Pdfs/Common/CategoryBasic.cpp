#include "doofit/builder/bobthebuilder/Pdfs/Common/CategoryBasic.h"

// from STL
#include <iostream>
#include <string>
#include <vector>


// from boost
#include <boost/foreach.hpp>
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string.hpp>

// from RooFit
#include "RooCategory.h"
#include "RooWorkspace.h"


// from project
#include "doocore/io/MsgStream.h"

namespace doofit {
using namespace boost;
using namespace boost::property_tree;
using namespace std;
using namespace doocore::io;
  
namespace builder{ 
namespace bobthebuilder{

CategoryBasic::CategoryBasic() :
   name_("DummyName")
 , desc_("Dummy description for a CategoryBasic.")
 , types_("NoType,0")
 , map_types_()
{  
  
}

CategoryBasic::~CategoryBasic(){
  
}


void CategoryBasic::Initialize( const boost::property_tree::ptree::value_type& pt_head ){
  
  name_    = pt_head.second.get_value<string>();
  
  ptree pt = pt_head.second;
  desc_    = pt.get<string>("desc");
  types_   = pt.get<string>("types");

  CreateTypes( types_ ); 
  
  sinfo << "desc  \"" << desc_    << "\"" << endmsg;
  sinfo << "types \"" << types_   << "\"" << endmsg;
}

void CategoryBasic::CreateTypes( const std::string &type_string ){
  char_separator<char> sep_semicolon(";");
  
  tokenizer< char_separator<char> > tokens_types(type_string, sep_semicolon);
  
  BOOST_FOREACH( string types, tokens_types ){
    vector<string> token_list;
    split(token_list, types, is_any_of(","), token_compress_on);
        
    pair<map<string, int >::iterator,bool> ret;
    ret = map_types_.insert(pair<string,int>( token_list[0], atoi(token_list[1].c_str()) )) ;
    if ( ret.second == false ){
      serr << "AbsDimensionCat: Tried to insert type '" << token_list[0] << " twice in Dimension '" << name_ << "'. ABORTING." << endmsg;
      throw;
    }
  }
}

bool CategoryBasic::AddToWorkspace( RooWorkspace& ws){
  RooCategory cat_temp(name_.c_str(), desc_.c_str());
  
  typedef pair<string,int> type_pair;
  BOOST_FOREACH( type_pair type , map_types_ ){
    cat_temp.defineType(type.first.c_str(), type.second);
  }
  
  // Check if object with this name exists on workspace, else create one.
  if (ws.obj(name_.c_str()) != NULL){
    cout << "CategoryBasic: Tried to add RooCategory with name '" << name_ << "' to the workspace! Object with this name already exists on the workspace. FAILED." << endl;
    throw;
  }
  else{
    ws.import(cat_temp);
  }
  
  return true;
}

std::vector< std::string > CategoryBasic::GetTypesVector(){
  vector<string> vector_temp;
  typedef pair<string,int> type_pair;
  BOOST_FOREACH( type_pair type , map_types_ ){
    vector_temp.push_back(type.first);
  }
  return vector_temp;
}

}}
}
