#include "Builder/BobTheBuilder/Pdfs/Common/AbsPdf.h"

using namespace Builder::BobTheBuilder;


AbsPdf::AbsPdf() :
    name_("pdfDummyName")
  , desc_("Dummy description for a pdf.")
  , dim_id_("DimensionID")
  , dim_name_("DimensionName")
  , simcat_id_("DummySimCatID")
  , comp_id_("DummyComponentID")
{
  
}

AbsPdf::~AbsPdf(){
  
}