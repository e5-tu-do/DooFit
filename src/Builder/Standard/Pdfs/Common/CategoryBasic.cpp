#include "Builder/Standard/Pdfs/Common/CategoryBasic.h"

// from STL
#include <iostream>




using namespace Builder::Standard;
using namespace boost::property_tree;

using namespace std;



CategoryBasic::CategoryBasic() :
   name_("DummyName")
 , desc_("Dummy description for a CategoryBasic.")
 , map_types_()
{  
  
}

CategoryBasic::~CategoryBasic(){
  
}


void CategoryBasic::Initialize( const boost::property_tree::ptree::value_type& pt_head ){
  
  name_    = pt_head.second.get_value<string>();
  
  ptree pt = pt_head.second;
  desc_    = pt.get<string>("desc");
  
  cout << name_ << " / " << desc_ << endl;  
}