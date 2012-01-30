#include "Builder/BobTheBuilder/Pdfs/Tag/DimTag.h"


// from project
#include "Utils/MsgStream.h"

// from project Pdf related classes
#include "Builder/BobTheBuilder/Pdfs/Common/AbsPdf.h"



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