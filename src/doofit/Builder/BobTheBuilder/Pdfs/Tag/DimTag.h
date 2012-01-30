#ifndef BUILDER_BOBTHEBUILDER_DIMTAG_H
#define BUILDER_BOBTHEBUILDER_DIMTAG_H

// from project
#include "doofit/Builder/BobTheBuilder/Pdfs/Common/AbsDimensionCat.h"

namespace doofit {
namespace Builder { namespace BobTheBuilder {

class DimTag : public AbsDimensionCat{
 public:
  DimTag();
  ~DimTag();
  
  boost::shared_ptr<AbsPdf> CreatePdf( const std::string& pdf_type );
 protected:
   
 private:
  
};

}}
}

#endif // BUILDER_BOBTHEBUILDER_DIMTAG_H
