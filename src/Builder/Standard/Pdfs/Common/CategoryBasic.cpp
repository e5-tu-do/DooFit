#include "Builder/Standard/Pdfs/Common/CategoryBasic.h"

// from STL
#include <iostream>




using namespace Builder::Standard;

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
  
  cout << "I hate you" << endl;
  
}