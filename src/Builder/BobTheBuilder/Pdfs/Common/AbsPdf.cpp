#include "Builder/BobTheBuilder/Pdfs/Common/AbsPdf.h"

// from boost
#include <boost/algorithm/string.hpp>

using namespace Builder::BobTheBuilder;

using namespace std;

AbsPdf::AbsPdf() :
    name_("pdfDummyName")
  , desc_("Dummy description for a pdf.")
  , dim_id_("DimensionID")
  , dim_name_("DimensionName")
  , subpdffull_id_("DummySimCatID")
  , comp_id_("DummyComponentID")
{
  
}

AbsPdf::~AbsPdf(){
  
}




