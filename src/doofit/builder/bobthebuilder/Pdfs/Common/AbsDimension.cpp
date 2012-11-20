#include "doofit/builder/bobthebuilder/Pdfs/Common/AbsDimension.h"

// from STL
#include <iostream>

using namespace std;
using namespace boost::property_tree;

namespace doofit {
namespace builder{ 
namespace bobthebuilder{
  
AbsDimension::AbsDimension() :
    dim_id_("DummyDimID")
  , name_("varDummyName")
  , desc_("Dummy description for a dimension variable.")
{
  
}

AbsDimension::~AbsDimension(){
  
};

}}
}