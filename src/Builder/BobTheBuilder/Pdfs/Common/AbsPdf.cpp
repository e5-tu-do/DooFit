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
  , simcat_id_("DummySimCatID")
  , comp_id_("DummyComponentID")
{
  
}

AbsPdf::~AbsPdf(){
  
}

pair<string,string> AbsPdf::ParseParameter( string parameter_string ){
  pair<string,string> temp_param;
  
  int position_of_pipe = parameter_string.find("|");
  if( position_of_pipe != string::npos ){
    temp_param.first  = parameter_string.substr(0, position_of_pipe);
    temp_param.second = parameter_string.substr(position_of_pipe+1);
  }
                  
  return temp_param;
}



