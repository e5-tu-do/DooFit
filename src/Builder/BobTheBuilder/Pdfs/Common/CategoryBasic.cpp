#include "Builder/BobTheBuilder/Pdfs/Common/CategoryBasic.h"

// from STL
#include <iostream>
#include <string>
#include <vector>


// from boost
#include <boost/foreach.hpp>
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string.hpp>

// from project
#include "Utils/MsgStream.h"

using namespace Builder::BobTheBuilder;
using namespace boost;
using namespace boost::property_tree;

using namespace std;



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
