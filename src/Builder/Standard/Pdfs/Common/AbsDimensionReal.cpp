// from project
#include "Builder/Standard/Pdfs/Common/AbsDimensionReal.h"

// from STL
#include <iostream>
#include <string>

// from RooFit
#include "RooRealVar.h"
#include "RooWorkspace.h"


using namespace std;
using namespace boost::property_tree;
using namespace RooFit;

using namespace Builder::Standard;


AbsDimensionReal::AbsDimensionReal() :
   AbsDimension()
 , val_min_(0)
 , val_max_(0)
 , unit_("DummyUnit")
{
  
}

AbsDimensionReal::~AbsDimensionReal(){}

void AbsDimensionReal::Initialize( const boost::property_tree::ptree::value_type &pt_head ){  
  name_    = pt_head.second.get_value<string>();
  
  ptree pt = pt_head.second;
  desc_    = pt.get<string>("desc");
  val_min_ = pt.get<double>("val_min");
  val_max_ = pt.get<double>("val_max");
  unit_    = pt.get<string>("unit");
  
  cout << name_ << " / " << desc_ << " / " << val_min_ << " / " << val_max_ <<" / " << unit_ << endl;
}


RooRealVar* const AbsDimensionReal::add_to_workspace( RooWorkspace* const ws ){
  RooRealVar dim_temp(name_.c_str(), desc_.c_str(), val_min_, val_max_, unit_.c_str());
  
  // Check if object with this name exists on workspace, else create one.
  if (ws->obj(name_.c_str()) != NULL){
    cout << "AbsDimensionReal tried to add already existing dimension variable with name '" << name_ << "' to the workspace! FAILED." << endl;
    throw;
  }
  else{
    ws->import(dim_temp);
  }

  return ws->var(name_.c_str());
}