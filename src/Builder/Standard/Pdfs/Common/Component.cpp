#include "Builder/Standard/Pdfs/Common/Component.h"

// from STL
#include <iostream>
#include <string>

// from boost
#include <boost/shared_ptr.hpp>

// from project
#include "Builder/Standard/Pdfs/Common/AbsDimension.h"

using namespace std;
using namespace boost;
using namespace boost::property_tree;

using namespace Builder::Standard;

Component::Component() :
    initialized_(false)
  , name_("DummyCompName")
  , dimensions_(NULL)
{
  
}

Component::~Component(){
  
}

void Component::Initialize( const ptree::value_type &pt_head, map< string, shared_ptr<AbsDimension> >* map_dimensions )
{
  // check if already initialized
  if (initialized_){
    cout << "You tried to initialize Dimension '" << name_ << "' twice. ABORT." << endl;
    throw;
  }
  else{
    initialized_ = true;
  }
  
  
  name_       = pt_head.second.get_value<string>();
  dimensions_ = map_dimensions;
  
  ptree pt = pt_head.second;
  
}