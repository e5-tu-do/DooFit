#include "doofit/builder/bobthebuilder/Pdfs/Tag/DimTag.h"


// from project
#include "doocore/io/MsgStream.h"

// from project Pdf related classes
#include "doofit/builder/bobthebuilder/Pdfs/Common/AbsPdf.h"


namespace doofit {
namespace builder{ 
namespace bobthebuilder{

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
