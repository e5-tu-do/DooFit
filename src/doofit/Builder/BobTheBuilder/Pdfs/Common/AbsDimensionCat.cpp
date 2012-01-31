#include "doofit/Builder/BobTheBuilder/Pdfs/Common/AbsDimensionCat.h"

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
#include "doofit/utils//MsgStream.h"

namespace doofit {
using namespace std;
using namespace boost;
using namespace boost::property_tree;

namespace Builder{ 
namespace BobTheBuilder{

AbsDimensionCat::AbsDimensionCat() :
    AbsDimension()
  , types_("")
  , map_types_()
{
  
}

AbsDimensionCat::~AbsDimensionCat(){
  
}


void AbsDimensionCat::Initialize( const boost::property_tree::ptree::value_type &pt_head ){  
  dim_id_ = pt_head.second.get_value<string>();
  
  sinfo << pt_head.first << " \"" << dim_id_ << "\"" << endmsg;
  sinfo << "{" << endmsg;
  sinfo.increment_indent(+2);
  
  ptree pt = pt_head.second;
  name_    = pt.get<string>("name",string("var")+dim_id_);
  desc_    = pt.get<string>("desc");
  types_   = pt.get<string>("types");
  
  CreateTypes( types_ );
  sinfo << "name  \"" << name_    << "\"" << endmsg;
  sinfo << "desc  \"" << desc_    << "\"" << endmsg;
  sinfo << "types \"" << types_   << "\"" << endmsg;
  
  sinfo.increment_indent(-2);
  sinfo << "}" << endmsg;
}

bool AbsDimensionCat::AddToWorkspace( RooWorkspace& ws ){
  RooCategory dim_temp(name_.c_str(), desc_.c_str());
  
  typedef pair<string,int> type_pair;
  BOOST_FOREACH( type_pair type , map_types_ ){
    dim_temp.defineType(type.first.c_str(), type.second);
  }
  
  // Check if object with this name exists on workspace, else create one.
  if (ws.obj(name_.c_str()) != NULL){
    cout << "AbsDimensionCat: Tried to add already existing dimension variable with name '" << name_ << "' to the workspace! FAILED." << endl;
    throw;
  }
  else{
    ws.import(dim_temp);
  }
  
  return true;
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

}}
}