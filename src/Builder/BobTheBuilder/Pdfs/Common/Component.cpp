#include "Builder/BobTheBuilder/Pdfs/Common/Component.h"

// from STL
#include <iostream>
#include <string>

// from boost
#include <boost/foreach.hpp>
#include <boost/shared_ptr.hpp>

// from project
#include "Builder/BobTheBuilder/Pdfs/Common/AbsDimension.h"
#include "Builder/BobTheBuilder/Pdfs/Common/AbsPdf.h"
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
  , map_dimpdfs_()
  , map_dimensions_()
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
  sinfo << "\"" << comp_id_ << "\" \"" << comp_id_full_ << "\"" << endmsg;
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
  map_dimensions_ = map_dimensions;
  
  ptree tree_compdimensions;
  try{
    tree_compdimensions = tree_component.get_child("Dimensions");
  }
  catch( const ptree_error& ptree_err){
    serr << "Components: Could not find 'Dimensions'. " << ptree_err.what() << endmsg;
    throw;
  }
  
  sinfo << "Dimensions" << endmsg;
  sinfo << "{" << endmsg;
  sinfo.increment_indent(+2);
  CreateDimPdfs(tree_compdimensions);
  sinfo.increment_indent(-2);
  sinfo << "}" << endmsg;
  
  sinfo.increment_indent(-2);
  sinfo << "}" << endmsg;
}

void Component::CreateDimPdfs(const ptree& tree_compdimensions){
  // loop over all known dimensions, search for entry in property_tree, initialize pdfs.
  map< string, shared_ptr<AbsDimension> >::const_iterator dim_it;
  
  BOOST_FOREACH( ptree::value_type tree_compdim_head, tree_compdimensions){
    string dim_name = tree_compdim_head.first;
    string pdf_type = tree_compdim_head.second.get_value<string>();
    
    
    dim_it = map_dimensions_.find(dim_name);
    // if dimension does not exist in map_dimensions_, ignore it
    if (dim_it == map_dimensions_.end()) continue;
    
    
    
    // if not, go on and delegate the pdf building
    shared_ptr<AbsPdf> pdf_temp = (*dim_it).second->CreatePdf(pdf_type);
    
    pair<map<string,shared_ptr<AbsPdf> >::iterator,bool> ret;
    ret = map_dimpdfs_.insert( pair< string, shared_ptr<AbsPdf> >( dim_name, pdf_temp ) ) ;
    // Check if key exists twice in map_subpdfs_
    if ( ret.second == false ){
      cout << "Component: Tried to insert pdf for dimension '" << dim_name << "' to map_dimpdfs_ twice." << endl;
      throw;
    }
    else{
      // Everything seems fine. Initialize the SubFullPdf
      map_dimpdfs_[dim_name]->Initialize(tree_compdim_head);
    }
    
    
    
    
    
  }
}

