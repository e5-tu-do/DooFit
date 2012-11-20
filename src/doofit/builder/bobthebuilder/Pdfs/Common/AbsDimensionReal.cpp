// from project
#include "doofit/builder/BobTheBuilder/Pdfs/Common/AbsDimensionReal.h"
#include "doocore/io/MsgStream.h"

// from STL
#include <iostream>
#include <string>

// from RooFit
#include "RooRealVar.h"
#include "RooWorkspace.h"


namespace doofit {
using namespace std;
using namespace boost::property_tree;
using namespace RooFit;
using namespace doocore::io;

namespace builder{ 
namespace bobthebuilder{
  
AbsDimensionReal::AbsDimensionReal() :
   AbsDimension()
 , val_min_(0)
 , val_max_(0)
 , unit_("DummyUnit")
{
  
}

AbsDimensionReal::~AbsDimensionReal(){}

void AbsDimensionReal::Initialize( const boost::property_tree::ptree::value_type &pt_head ){  
  dim_id_ = pt_head.second.get_value<string>();
  
  sinfo << pt_head.first << " \"" << dim_id_ << "\"" << endmsg;
  sinfo << "{" << endmsg;
  sinfo.increment_indent(+2);
  
  ptree pt = pt_head.second;
  name_    = pt.get<string>("name",string("var")+dim_id_);
  desc_    = pt.get<string>("desc");
  val_min_ = pt.get<double>("val_min");
  val_max_ = pt.get<double>("val_max");
  unit_    = pt.get<string>("unit");
  
  sinfo << "name    \"" << name_    << "\"" << endmsg;
  sinfo << "desc    \"" << desc_    << "\"" << endmsg;
  sinfo << "val_min \"" << val_min_ << "\"" << endmsg;
  sinfo << "val_max \"" << val_max_ << "\"" << endmsg;
  sinfo << "unit    \"" << unit_    << "\"" << endmsg;
  
  sinfo.increment_indent(-2);
  sinfo << "}" << endmsg;
}


bool AbsDimensionReal::AddToWorkspace( RooWorkspace& ws ){
  RooRealVar dim_temp(name_.c_str(), desc_.c_str(), val_min_, val_max_, unit_.c_str());
  
  // Check if object with this name exists on workspace, else create one.
  if (ws.obj(name_.c_str()) != NULL){
    cout << "AbsDimensionReal tried to add already existing dimension variable with name '" << name_ << "' to the workspace! FAILED." << endl;
    throw;
  }
  else{
    ws.import(dim_temp);
  }

  return true;
}

}}
}
