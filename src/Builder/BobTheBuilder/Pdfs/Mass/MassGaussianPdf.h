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
    
  void Initialize( const boost::property_tree::ptree::value_type& tree_pdf_head,
                   const std::string& dim_id, const std::string dim_name, 
                   const std::string& simcat_id = "", const std::string comp_id = "");
    void AddToWorkspace( RooWorkspace* ws );

 protected:

 private:
  std::pair< std::string, std::string> mean_;
  std::pair< std::string, std::string> sigma_;
  
  
  
};
  
}}

#endif // BUILDER_BOBTHEBUILDER_MASSGAUSSIANPDF_H
