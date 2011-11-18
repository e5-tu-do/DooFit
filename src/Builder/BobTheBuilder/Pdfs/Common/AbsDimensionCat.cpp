#include "Builder/BobTheBuilder/Pdfs/Common/AbsDimensionCat.h"

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
using namespace std;
using namespace boost;
using namespace boost::property_tree;

AbsDimensionCat::AbsDimensionCat() :
    AbsDimension()
  , types_("")
  , map_types_()
{
  
}

AbsDimensionCat::~AbsDimensionCat(){
  
}


void AbsDimensionCat::Initialize( const boost::property_tree::ptree::value_type &pt_head ){  
  name_    = pt_head.second.get_value<string>();
  
  ptree pt = pt_head.second;
  desc_    = pt.get<string>("desc");
  types_   = pt.get<string>("types");
  
  CreateTypes( types_ );
}

void AbsDimensionCat::CreateTypes( const std::string &type_string ){
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

