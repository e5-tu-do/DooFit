#include "doofit/builder/BobTheBuilder/Pdfs/Mass/DimMass.h"

// from STL
#include <iostream>

// from boost

// from project
#include "doocore/io/MsgStream.h"

// from project Pdf related classes
#include "doofit/builder/BobTheBuilder/Pdfs/Common/AbsPdf.h"
#include "doofit/builder/BobTheBuilder/Pdfs/Mass/MassGaussianPdf.h"


using namespace std;
using namespace boost;
using namespace boost::property_tree;
using namespace doocore::io;

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