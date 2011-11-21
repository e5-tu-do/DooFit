#include "Builder/BobTheBuilder/Pdfs/Common/PdfFull.h"

// from STL
#include <iostream>

// from boost


// from project
#include "Builder/BobTheBuilder/Pdfs/Common/Absdimension.h"
#include "Builder/BobTheBuilder/Pdfs/Common/CategoryBasic.h"
#include "Builder/BobTheBuilder/Pdfs/Common/CategorySuper.h"
#include "Builder/BobTheBuilder/Pdfs/Common/SubPdfFull.h"
#include "Utils/MsgStream.h"

using namespace Builder::BobTheBuilder;

using namespace std;
using namespace boost;
using namespace boost::property_tree;

PdfFull::PdfFull() :
   name_("DummyPdfFullName")
 , desc_("A dummy description for a PdfFull.")
 , extended_(false)
 , map_subpdfs_()
{
  
}

PdfFull::~PdfFull(){
  map_subpdfs_.clear();
}
  
void PdfFull::Initialize( const boost::property_tree::ptree& tree_pdffull,
                          const map< string, shared_ptr<AbsDimension> >& map_dimensions,
                          const map< string, shared_ptr<CategoryBasic> >& map_cats_basic,
                          const map< string, shared_ptr<CategorySuper> >& map_cats_super
                         ){
  // Set name, description, extended of PdfFull.
  name_    = tree_pdffull.get_value<string>();
  
  sinfo << "PdfFull \"" << name_ << "\"" << endmsg;
  sinfo << "{" << endmsg;
  sinfo.increment_indent(+2);
  
  //ptree pt  = pt_head.front().second;
  desc_     = tree_pdffull.get<string>("desc");
  extended_ = tree_pdffull.get<bool>("Extended",false);
  
  sinfo << "desc         \"" << desc_ << "\"" << endmsg;
  sinfo << "Extended     " << extended_ << endmsg;
  
  
  // Check for simultaneous category. Set simultaneous boolean.
  simultaneous_ = tree_pdffull.get<bool>("Simultaneous",false);
  sinfo << "Simultaneous " << simultaneous_ << endmsg;
  
  
  if (simultaneous_){
    ptree tree_simcatsubpdfs = tree_pdffull.get_child("SimCatSubPdfs");
    InitializeSimCatSubPdfs(tree_simcatsubpdfs, map_dimensions, map_cats_basic, map_cats_super);
  }
  else{
    
  }
  
  sinfo.increment_indent(-2);
  sinfo << "}" << endmsg;
}

void PdfFull::InitializeSimCatSubPdfs(const boost::property_tree::ptree& tree_simcatsubpdfs,
                                      const map< string, shared_ptr<AbsDimension> >& map_dimensions,
                                      const map< string, shared_ptr<CategoryBasic> >& map_cats_basic,
                                      const map< string, shared_ptr<CategorySuper> >& map_cats_super)
{
  sinfo << "SimCatSubPdfs \"" << tree_simcatsubpdfs.get_value<string>() << "\"" << endmsg;
  sinfo << "{" << endmsg;
  sinfo.increment_indent(+2);
  
  // Find Category in maps and get vector of types
  vector<string> simcat_types();
  // a) Look in CategoryBasic map
  
  // b) Look in CategorySuper map
  
  
  // Check SubPdf descriptions:
  // 1. Are there SubPdf definitions for all simulateneous category types?
  // 2. Is the extended flag set consistently? How to interpret that?
  
  
  
  // Delegate to SubPdfFull.
  
  
  sinfo.increment_indent(-2);
  sinfo << "}" << endmsg;
}
