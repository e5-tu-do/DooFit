#include "Builder/BobTheBuilder/Pdfs/Common/PdfFull.h"

// from STL
#include <iostream>

// from boost
#include <boost/foreach.hpp>

// from project
#include "Builder/BobTheBuilder/Pdfs/Common/AbsDimension.h"
#include "Builder/BobTheBuilder/Pdfs/Common/CategoryBasic.h"
#include "Builder/BobTheBuilder/Pdfs/Common/CategorySuper.h"
#include "Builder/BobTheBuilder/Pdfs/Common/SubPdfFull.h"
#include "Utils/MsgStream.h"

using namespace Builder::BobTheBuilder;

using namespace std;
using namespace boost;
using namespace boost::property_tree;

Builder::BobTheBuilder::PdfFull::PdfFull() :
   name_("DummyPdfFullName")
 , desc_("A dummy description for a PdfFull.")
 , extended_(false)
 , simultaneous_(false)
 , simcat_name_("None")
 , map_subpdfs_()
{
  
}

Builder::BobTheBuilder::PdfFull::~PdfFull(){
  map_subpdfs_.clear();
}
  
void Builder::BobTheBuilder::PdfFull::Initialize( const boost::property_tree::ptree& tree_pdffull,
                          const map< string, shared_ptr<AbsDimension> >& map_dimensions,
                          const map< string, shared_ptr<CategoryBasic> >& map_cats_basic,
                          const map< string, shared_ptr<CategorySuper> >& map_cats_super
                         ){
  try{
    // Set name, description, extended of PdfFull.
    name_    = tree_pdffull.get_value<string>();
    if (name_ == "") name_ = "pdfFull";
    
    sinfo << "PdfFull \"" << name_ << "\"" << endmsg;
    sinfo << "{" << endmsg;
    sinfo.increment_indent(+2);
    
    desc_     = tree_pdffull.get<string>("desc",name_);
    extended_ = tree_pdffull.get<bool>("Extended",false);
  
    sinfo << "desc          \"" << desc_ << "\"" << endmsg;
    sinfo << "Extended      "   << extended_ << endmsg;

    // Check for simultaneous category. Set simultaneous boolean.
    simultaneous_ = tree_pdffull.get<bool>("Simultaneous",false);
    sinfo << "Simultaneous  " << simultaneous_ << endmsg;
  }
  catch(const ptree_error& ptree_err){
    serr << "PdfFull: Could not initialize. " << ptree_err.what() << endmsg;
  }
  
  if (simultaneous_){
    ptree tree_simcatsubpdfs = tree_pdffull.get_child("SimCatSubPdfs");
    simcat_name_ = tree_simcatsubpdfs.get_value<string>();
    
    InitializeSimCatSubPdfs(tree_simcatsubpdfs, map_dimensions, map_cats_basic, map_cats_super);
  }
  else{
    
  }
  
  sinfo.increment_indent(-2);
  sinfo << "}" << endmsg;
}

void Builder::BobTheBuilder::PdfFull::InitializeSimCatSubPdfs(const boost::property_tree::ptree& tree_simcatsubpdfs,
                                      const map< string, shared_ptr<AbsDimension> >& map_dimensions,
                                      const map< string, shared_ptr<CategoryBasic> >& map_cats_basic,
                                      const map< string, shared_ptr<CategorySuper> >& map_cats_super)
{
  sinfo << "SimCatSubPdfs \"" << simcat_name_ << "\"" << endmsg;
  sinfo << "{" << endmsg;
  sinfo.increment_indent(+2);
  
  // Find Category in maps and get vector of types
  // a) Look in CategoryBasic map
  // b) Look in CategorySuper map
  vector<string> simcat_types;
  if (map_cats_basic.find(simcat_name_) != map_cats_basic.end()){
    simcat_types = map_cats_basic.find(simcat_name_)->second->GetTypesVector();
  }
  else if (map_cats_super.find(simcat_name_) != map_cats_super.end()){
    simcat_types = map_cats_super.find(simcat_name_)->second->GetTypesVector();
  }
  else{
    serr << "PdfFull: Could not find category with name '" << simcat_name_ << "' for simultaneous pdf." << endmsg;
    throw;
  }
  
  // Loop over all category types. Delegate pdf building to SubPdfFull.
  BOOST_FOREACH( string simcat_type, simcat_types){
    ptree tree_subpdf;
    try {
      tree_subpdf = tree_simcatsubpdfs.get_child(simcat_type);
    }
    catch(const ptree_error& ptree_err){
      serr << "PdfFull: Could not find SubPdfFull definition for the category type '" << simcat_type << "'. " << ptree_err.what() << endmsg;
      throw;
    }
    
    // Try to add new SubPdfFull to map_subpdfs_
    pair<map<string,shared_ptr<SubPdfFull> >::iterator,bool> ret;
    ret = map_subpdfs_.insert( pair< string, shared_ptr<SubPdfFull> >( simcat_type, shared_ptr<SubPdfFull>( new SubPdfFull() ) ) ) ;
    // Check if key exists twice in map_subpdfs_
    if ( ret.second == false ){
      cout << "PdfFull: Tried to insert SubPdfFull for category type '" << simcat_type << "' to map_subpdfs_ twice." << endl;
      throw;
    }
    else{
      // Everything seems fine. Initialize the SubFullPdf
      map_subpdfs_[simcat_type]->Initialize(tree_subpdf, map_dimensions, simcat_type);
    }
    
  }
  
  
  
  sinfo.increment_indent(-2);
  sinfo << "}" << endmsg;
}

void Builder::BobTheBuilder::PdfFull::AddToWorkspace( RooWorkspace& ws ){
  
}



