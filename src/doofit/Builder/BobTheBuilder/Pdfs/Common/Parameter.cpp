#include "doofit/Builder/BobTheBuilder/Pdfs/Common/Parameter.h"

// from STL

// from boost
#include <boost/algorithm/string.hpp>

// from ROOT

// from RooFit
#include "RooAbsReal.h"
#include "RooRealVar.h"
#include "RooWorkspace.h"

// from project
#include "doofit/utils//MsgStream.h"

namespace doofit {
using namespace std;
using namespace boost;

namespace Builder{ 
namespace BobTheBuilder{

Parameter::Parameter() :
   name_("parDummyName")
 , desc_("Dummy description of a parameter")
{
  
}

Parameter::Parameter( const std::string& string_to_parse ) :
   name_("parDummyName")
 , desc_("Dummy description of a parameter")
{
  Parse(string_to_parse);
}

Parameter::~Parameter(){
  
}

void Parameter::Parse( const string& string_to_parse ){
  int position_of_pipe = string_to_parse.find("|");
  if( position_of_pipe != string::npos ){
    name_  = string_to_parse.substr(0, position_of_pipe);
    desc_  = string_to_parse.substr(position_of_pipe+1);
  }
  
}

void Parameter::Print( ostream& os ) const{
  os << name_ << "|" << desc_;
}

void Parameter::AddToWorkspace( RooWorkspace& ws ){
  
  // check if object of this name already exists on the workspace. If it exists, we don't care and use it, else we create it.
  if (ws.obj(name_.c_str()) != NULL){
    return;
  }
}





std::istream& operator>>(std::istream& is, Parameter& arg){
  string string_to_parse;
  is >> string_to_parse;
  arg.Parse(string_to_parse);
  return is;
}

std::ostream& operator<<(std::ostream& os, const Parameter& arg){
  arg.Print(os);
  return os;
}

}}
}