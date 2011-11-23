#include "Builder/BobTheBuilder/Pdfs/Mass/MassGaussianPdf.h"

// from project
#include "Utils/MsgStream.h"

using namespace Builder::BobTheBuilder;

using namespace std;
using namespace boost;
using namespace boost::property_tree;

MassGaussianPdf::MassGaussianPdf() :
    dim_id_("DummyMassId")
  , dim_name_("DummyMassName")
{
  
}

MassGaussianPdf::~MassGaussianPdf(){
  
}

void MassGaussianPdf::Initialize( const ptree::value_type& tree_pdf_head ){
  
  
  sinfo << tree_pdf_head.first << " \"" << tree_pdf_head.second.get_value<string>() << "\"" << endmsg;
  sinfo << "{" << endmsg;
  
  
  sinfo << "}" << endmsg;
}

void MassGaussianPdf::AddToWorkspace( RooWorkspace* ws ){
  
}