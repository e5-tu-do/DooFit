#include "Builder/BobTheBuilder/Pdfs/Mass/MassGaussianPdf.h"

// from project
#include "Utils/MsgStream.h"

using namespace Builder::BobTheBuilder;

using namespace std;
using namespace boost;
using namespace boost::property_tree;

MassGaussianPdf::MassGaussianPdf()
{
  
}

MassGaussianPdf::~MassGaussianPdf(){
  
}

void MassGaussianPdf::Initialize( const ptree::value_type& tree_pdf_head, 
                                  const std::string& dim_id, const std::string dim_name, 
                                  const std::string& simcat_id, const std::string comp_id)
{
  
  
  sinfo << tree_pdf_head.first << " \"" << tree_pdf_head.second.get_value<string>() << "\"" << endmsg;
  sinfo << "{" << endmsg;
  sinfo.increment_indent(+2);
  
  dim_id_    = dim_id;
  dim_name_  = dim_name;
  simcat_id_ = simcat_id;
  comp_id_   = comp_id;
  
  // check entries
  ptree tree_pdf = tree_pdf_head.second;
  name_ = tree_pdf.get<string>("name",string("pdf")+simcat_id_+comp_id_+dim_id_);
  desc_ = tree_pdf.get<string>("desc",name_);
  
  
  
  sinfo << "name \"" << name_ << "\"" << endmsg;
  sinfo << "desc \"" << desc_ << "\"" << endmsg;
  
  // set parameters
  string default_mean  = string("par")+simcat_id_+comp_id_+dim_id_+"Mean|m_{"+comp_id_+","+simcat_id_+"}";
  string default_sigma = string("par")+simcat_id_+comp_id_+dim_id_+"Sigma|#sigma_{m,"+comp_id_+","+simcat_id_+"}";
  
  mean_  = ParseParameter( tree_pdf.get<string>("mean",default_mean));
  sigma_ = ParseParameter( tree_pdf.get<string>("desc",default_mean));
  
  sinfo << "mean  \"" << mean_.first  << "|" << mean_.second   << "\"" << endmsg;
  sinfo << "sigma \"" << sigma_.first << "|" << sigma_.second  << "\"" << endmsg;
  
  sinfo.increment_indent(-2);
  sinfo << "}" << endmsg;
}

void MassGaussianPdf::AddToWorkspace( RooWorkspace* ws ){
  
}
