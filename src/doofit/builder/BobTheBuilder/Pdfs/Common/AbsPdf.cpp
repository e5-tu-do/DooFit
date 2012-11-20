#include "doofit/builder/BobTheBuilder/Pdfs/Common/AbsPdf.h"

// from boost
#include <boost/algorithm/string.hpp>

namespace doofit {
using namespace std;

namespace builder{ 
namespace BobTheBuilder{

AbsPdf::AbsPdf() :
    name_("pdfDummyName")
  , desc_("Dummy description for a pdf.")
  , dim_id_("DimensionID")
  , dim_name_("DimensionName")
  , subpdffull_id_("DummySimCatID")
  , comp_id_("DummyComponentID")
  , tree_pdf_()
  , map_params_()
{
  
}

AbsPdf::~AbsPdf(){
  
}

}}

}