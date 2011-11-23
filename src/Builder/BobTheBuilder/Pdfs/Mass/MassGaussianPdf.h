#ifndef BUILDER_BOBTHEBUILDER_MASSGAUSSIANPDF_H
#define BUILDER_BOBTHEBUILDER_MASSGAUSSIANPDF_H

// from project
#include "Builder/BobTheBuilder/Pdfs/Common/AbsPdf.h"

namespace Builder{ namespace BobTheBuilder{
  
  // forward declarations from project
  
class MassGaussianPdf : public AbsPdf{
 public:
   MassGaussianPdf();
   ~MassGaussianPdf();
    
    void Initialize( const boost::property_tree::ptree::value_type& tree_pdf_head );
    void AddToWorkspace( RooWorkspace* ws );

 protected:

 private:
  std::string dim_id_;
  std::string dim_name_;
  
  
  
};
  
}}

#endif // BUILDER_BOBTHEBUILDER_MASSGAUSSIANPDF_H
