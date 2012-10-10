#include "doofit/Builder/BobTheBuilder/Pdfs/Mass/MassGaussianPdf.h"

// from project
#include "doofit/Builder/BobTheBuilder/Pdfs/Common/Parameter.h"
#include "doocore/io/MsgStream.h"

using namespace std;
using namespace boost;
using namespace boost::property_tree;

namespace doofit {
namespace Builder{ 
namespace BobTheBuilder{
using namespace doocore::lutils; using namespace doocore::io;
  
MassGaussianPdf::MassGaussianPdf() : 
   AbsPdf()
{
  
}

MassGaussianPdf::~MassGaussianPdf(){
  
}

void MassGaussianPdf::Initialize( const ptree::value_type& tree_pdf_head, 
                                  const std::string& dim_id, const std::string dim_name, 
                                  const std::string& subpdffull_id, const std::string comp_id)
{
  
  
  sinfo << tree_pdf_head.first << " \"" << tree_pdf_head.second.get_value<string>() << "\"" << endmsg;
  sinfo << "{" << endmsg;
  sinfo.increment_indent(+2);
  
  dim_id_        = dim_id;
  dim_name_      = dim_name;
  subpdffull_id_ = subpdffull_id;
  comp_id_       = comp_id;
  tree_pdf_      = tree_pdf_head.second;
  
  // check entries
  name_ = tree_pdf_.get<string>("name",string("pdf")+subpdffull_id_+comp_id_+dim_id_);
  desc_ = tree_pdf_.get<string>("desc",name_);
  
  sinfo << "name \"" << name_ << "\"" << endmsg;
  sinfo << "desc \"" << desc_ << "\"" << endmsg;
  
  // set parameters
  InitializeParameters();
  
  sinfo.increment_indent(-2);
  sinfo << "}" << endmsg;
}

void MassGaussianPdf::InitializeParameters(){
  sinfo << "Parameters" << endmsg;
  sinfo << "{" << endmsg;
  sinfo.increment_indent(+2);
  
  if ( tree_pdf_.find("Parameters") == tree_pdf_.not_found() ){
    // Standard initialization
    string default_mean  = string("par")+subpdffull_id_+comp_id_+dim_id_+"Mean|m_{"+comp_id_+","+subpdffull_id_+"}";
    string default_sigma = string("par")+subpdffull_id_+comp_id_+dim_id_+"Sigma|#sigma_{m,"+comp_id_+","+subpdffull_id_+"}";
    
    map_params_["mean"]  = shared_ptr<Parameter>( new Parameter(default_mean) );
    map_params_["sigma"] = shared_ptr<Parameter>( new Parameter(default_sigma) );
  }
  else{
    ptree tree_param = tree_pdf_.get_child("Parameters");
    map_params_["mean"]  = shared_ptr<Parameter>( new Parameter(tree_param.get<Parameter>("mean")));
    map_params_["sigma"] = shared_ptr<Parameter>( new Parameter(tree_param.get<Parameter>("sigma")));
  }
  sinfo.increment_indent(-2);
  sinfo << "}" << endmsg;
  
  
  
  
  

//  
//  mean_  = tree_pdf.get<Parameter>("mean",Parameter(default_mean));
//  sigma_ = tree_pdf.get<Parameter>("sigma",Parameter(default_sigma));
//  
//  sinfo << "mean  \"" << mean_  << "\"" << endmsg;
//  sinfo << "sigma \"" << sigma_ << "\"" << endmsg;
}


void MassGaussianPdf::AddToWorkspace( RooWorkspace* ws ){
  
}
}}
}