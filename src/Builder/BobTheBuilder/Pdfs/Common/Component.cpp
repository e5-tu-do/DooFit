#include "Builder/BobTheBuilder/Pdfs/Common/Component.h"

// from STL
#include <iostream>
#include <string>

// from boost
#include <boost/shared_ptr.hpp>

// from project
#include "Builder/BobTheBuilder/Pdfs/Common/AbsDimension.h"
#include "Utils/MsgStream.h"

using namespace std;
using namespace boost;
using namespace boost::property_tree;

using namespace Builder::BobTheBuilder;

Component::Component() :
    comp_id_("DummyCompId")
  , comp_id_full_("DummyCompIdFull")
  , name_("DummyCompName")
  , desc_("Dummy description for Component.")
  , extended_(false)
  , dimensions_()
{
  
}

Component::~Component(){
  
}

void Component::Initialize( const ptree::value_type& tree_component_head, 
                            const map< string, shared_ptr<AbsDimension> >& map_dimensions, 
                            const string& subpdffull_id)
{
  comp_id_      = tree_component_head.first;
  comp_id_full_ = tree_component_head.second.get_value<string>();
  if (comp_id_full_ == ""){
    comp_id_full_ = subpdffull_id+comp_id_; 
  }
  sinfo << comp_id_ << " \"" << comp_id_full_ << "\"" << endmsg;
  sinfo << "{" << endmsg;
  sinfo.increment_indent(+2);
  
  // set name, description and extended flag
  ptree tree_component = tree_component_head.second;
  name_     = tree_component.get<string>("name",string("pdf")+comp_id_full_);
  desc_     = tree_component.get<string>("desc",name_);
  extended_ = tree_component.get<bool>("Extended",false);
  
  sinfo << "name      \"" << name_ << "\"" << endmsg; 
  sinfo << "desc      \"" << desc_ << "\"" << endmsg;
  sinfo << "Extended  "   << extended_     << endmsg;
  
  
  // set dimensions map. Then create and add dimension's pdfs.
  dimensions_ = map_dimensions;
  CreateDimPdfs();
  
  sinfo.increment_indent(-2);
  sinfo << "}" << endmsg;
}

void Component::CreateDimPdfs(){
  
}