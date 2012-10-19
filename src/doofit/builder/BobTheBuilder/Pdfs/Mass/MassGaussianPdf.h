#ifndef BUILDER_BOBTHEBUILDER_MASSGAUSSIANPDF_H
#define BUILDER_BOBTHEBUILDER_MASSGAUSSIANPDF_H

// from project
#include "doofit/builder/BobTheBuilder/Pdfs/Common/AbsPdf.h"

namespace doofit {
namespace Builder{ namespace BobTheBuilder{
  
  // forward declarations from project
  
class MassGaussianPdf : public AbsPdf{
 public:
   MassGaussianPdf();
   ~MassGaussianPdf();
    
  void Initialize( const boost::property_tree::ptree::value_type& tree_pdf_head,
                   const std::string& dim_id, const std::string dim_name, 
                   const std::string& simcat_id = "", const std::string comp_id = "");
  
  void InitializeParameters();
  
  void AddToWorkspace( RooWorkspace* ws );

 protected:
  
 private:
  
  
  
};
  
}}
}
#endif // BUILDER_BOBTHEBUILDER_MASSGAUSSIANPDF_H
