#include "doofit/Builder/BobTheBuilder/Pdfs/Mass/DimMass.h"

// from STL
#include <iostream>

// from boost

// from project
#include "doofit/utils//MsgStream.h"

// from project Pdf related classes
#include "doofit/Builder/BobTheBuilder/Pdfs/Common/AbsPdf.h"
#include "doofit/Builder/BobTheBuilder/Pdfs/Mass/MassGaussianPdf.h"


using namespace std;
using namespace boost;
using namespace boost::property_tree;

namespace doofit {
namespace Builder{ 
namespace BobTheBuilder{

DimMass::DimMass() :
   AbsDimensionReal()
{
}

DimMass::~DimMass(){
}

boost::shared_ptr<AbsPdf> DimMass::CreatePdf( const std::string& pdf_type ){
  if ( pdf_type == "Gaussian" ){
    return shared_ptr<AbsPdf>( new MassGaussianPdf() );
  }
  else{
    serr << "DimMass: Could not find pdf of type '" << pdf_type << "'." << endmsg;
    throw;
  }
}

}}
}