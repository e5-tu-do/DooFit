#include "DooFit/Builder/BobTheBuilder/Pdfs/Tag/DimTag.h"


// from project
#include "DooFit/Utils//MsgStream.h"

// from project Pdf related classes
#include "DooFit/Builder/BobTheBuilder/Pdfs/Common/AbsPdf.h"


namespace DooFit {
namespace Builder{ 
namespace BobTheBuilder{

DimTag::DimTag() :
   AbsDimensionCat()
{
  
}

DimTag::~DimTag(){
  
}

boost::shared_ptr<AbsPdf> DimTag::CreatePdf( const std::string& pdf_type ){
  
}

}}
}
