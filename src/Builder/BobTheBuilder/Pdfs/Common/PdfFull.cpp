#include "Builder/BobTheBuilder/Pdfs/Common/PdfFull.h"

// from boost

// from project
#include "Builder/BobTheBuilder/Pdfs/Common/Absdimension.h"
#include "Builder/BobTheBuilder/Pdfs/Common/CategoryBasic.h"
#include "Builder/BobTheBuilder/Pdfs/Common/CategorySuper.h"
#include "Builder/BobTheBuilder/Pdfs/Common/SubPdfFull.h"

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
  
void PdfFull::Initialize( const ptree::value_type& pt_head,
                          const map< string, shared_ptr<AbsDimension> >& map_dimensions,
                          const map< string, shared_ptr<CategoryBasic> >& map_cats_basic,
                          const map< string, shared_ptr<CategorySuper> >& map_cats_super
                         ){
  // Set name, description, extended of PdfFull.
  
  
  // Check for simultaneous category. Set simultaneous boolean.
  
  // Check SubPdf descriptions:
  // 1. Are there SubPdf definitions for all simulateneous category types?
  // 2. Is the extended flag set consistently? How to interpret that?
  
  // Delegate to SubPdfFull.
}
