#include "Builder/Standard/Pdfs/Common/AbsDimension.h"

// from STL
#include <iostream>

using namespace std;
using namespace boost::property_tree;

using namespace Builder::Standard;


AbsDimension::AbsDimension() :
    initialized_(false)
  , name_("varDummyName")
  , desc_("Dummy description for a dimension variable.")
{
  
}

AbsDimension::~AbsDimension(){
  
};
