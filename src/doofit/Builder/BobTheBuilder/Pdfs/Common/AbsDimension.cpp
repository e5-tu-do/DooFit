#include "DooFit/Builder/BobTheBuilder/Pdfs/Common/AbsDimension.h"

// from STL
#include <iostream>

using namespace std;
using namespace boost::property_tree;

namespace DooFit {
namespace Builder{ 
namespace BobTheBuilder{
  
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