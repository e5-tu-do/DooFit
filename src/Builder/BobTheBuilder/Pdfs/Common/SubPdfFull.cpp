#include "Builder/BobTheBuilder/Pdfs/Common/SubPdfFull.h"

// from boost
#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>

// from project
#include "Builder/BobTheBuilder/Pdfs/Common/AbsDimension.h"
#include "Builder/BobTheBuilder/Pdfs/Common/Component.h"
#include "Utils/MsgStream.h"



using namespace Builder::BobTheBuilder;

using namespace std;
using namespace boost;
using namespace boost::property_tree;

SubPdfFull::SubPdfFull() :
   simcat_type_("")
 , simcat_type_stripped_("")
 , name_("DummySubPdfFullName")
 , desc_("Dummy description for a SubPdfFull")
 , extended_(false)
 , components_()
{
}

SubPdfFull::~SubPdfFull(){
  
}

void SubPdfFull::Initialize( const ptree& tree_subpdffull, 
                             const map< string, shared_ptr<AbsDimension> >& map_dimensions,
                             const string& simcat_type)
{
  try{
    // Set category type
    simcat_type_ = simcat_type;
    
    // Strip category type from {;} for standard pdf names
    simcat_type_stripped_ = simcat_type_;
    erase_all(simcat_type_stripped_,"{");
    erase_all(simcat_type_stripped_,";");
    erase_all(simcat_type_stripped_,"}");
    
    // Set name, description, extended of PdfFull.
    name_ = tree_subpdffull.get_value<string>(); 
    if (name_ == "") name_ = "pdfFull"+simcat_type_stripped_;
    
    
    sinfo << simcat_type_ << " \"" << name_ << "\"" << endmsg;
    sinfo << "{" << endmsg;
    sinfo.increment_indent(+2);
    
    desc_     = tree_subpdffull.get<string>("desc",name_);
    extended_ = tree_subpdffull.get<bool>("Extended",false);    
    
    sinfo << "desc         \"" << desc_ << "\"" << endmsg;
    sinfo << "Extended     " << extended_ << endmsg;
    
  }
  catch(const ptree_error& ptree_err){
    serr << "PdfFull: Could not initialize. " << ptree_err.what() << endmsg;
  }
  
  // Initialize components
  ptree tree_components;
  try{
    tree_components = tree_subpdffull.get_child("Components");
  }
  catch( const ptree_error& ptree_err){
    serr << "PdfFull: Could not find 'Components'. " << ptree_err.what() << endmsg;
    throw;
  }
  InitializeComponents(tree_components, map_dimensions);
  
  sinfo.increment_indent(-2);
  sinfo << "}" << endmsg;
}

void SubPdfFull::InitializeComponents( const ptree& tree_components, 
                                       const map< string, shared_ptr<AbsDimension> >& map_dimensions){
  sinfo << "Components" << endmsg;
  sinfo << "{" << endmsg;
  sinfo.increment_indent(+2);
  
  BOOST_FOREACH( ptree::value_type tree_component_head, tree_components){
    
  }
  
  sinfo.increment_indent(-2);
  sinfo << "}" << endmsg;
}