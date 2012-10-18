#include "doofit/builder/BobTheBuilder/Pdfs/Common/SubPdfFull.h"

// from boost
#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>

// from project
#include "doofit/builder/BobTheBuilder/Pdfs/Common/AbsDimension.h"
#include "doofit/builder/BobTheBuilder/Pdfs/Common/Component.h"
#include "doocore/io/MsgStream.h"

namespace doofit {
using namespace std;
using namespace boost;
using namespace boost::property_tree;
using namespace doocore::lutils; using namespace doocore::io;
  
namespace Builder{ 
namespace BobTheBuilder{
  
SubPdfFull::SubPdfFull() :
   subpdffull_id_("")
 , simcat_type_("")
 , name_("DummySubPdfFullName")
 , desc_("Dummy description for a SubPdfFull")
 , extended_(false)
 , map_components_()
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
    subpdffull_id_ = tree_subpdffull.get_value<string>(); 
    if (subpdffull_id_ == ""){
      subpdffull_id_ = simcat_type_;
      erase_all(subpdffull_id_,"{");
      erase_all(subpdffull_id_,";");
      erase_all(subpdffull_id_,"}");
    }
    
    sinfo << simcat_type_ << " \"" << subpdffull_id_ << "\"" << endmsg;
    sinfo << "{" << endmsg;
    sinfo.increment_indent(+2);
    
    
    // Set name, description, extended of PdfFull.  
    name_     = tree_subpdffull.get<string>("name","pdf"+subpdffull_id_+"Full");
    desc_     = tree_subpdffull.get<string>("desc",name_);
    extended_ = tree_subpdffull.get<bool>("Extended",false);    
    
    sinfo << "desc         \"" << desc_ << "\"" << endmsg;
    sinfo << "Extended     "   << extended_ << endmsg;
    
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
    string component_name = tree_component_head.first;
    // insert to map and check for duplicates
    pair<map<string,shared_ptr<Component> >::iterator,bool> ret;
    ret = map_components_.insert(pair<string,shared_ptr<Component> >( component_name, shared_ptr<Component>( new Component() ) )) ;
    if ( ret.second == false ){
      cout << "SubPdfFull: Tried to insert category '" << component_name << "' in SubPdfFull '" << name_ << "' to map_componentss_ twice." << endl;
      throw;
    }
    map_components_[component_name]->Initialize(tree_component_head, map_dimensions, subpdffull_id_);
  }
  
  sinfo.increment_indent(-2);
  sinfo << "}" << endmsg;
}

void SubPdfFull::AddToWorkspace( RooWorkspace& ws ){
  
}

}}
}
