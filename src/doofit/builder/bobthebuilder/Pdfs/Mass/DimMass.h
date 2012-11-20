#ifndef BUILDER_BOBTHEBUILDER_DIMMASS_H
#define BUILDER_BOBTHEBUILDER_DIMMASS_H

// from Project
#include "doofit/builder/bobthebuilder/Pdfs/Common/AbsDimensionReal.h"

namespace doofit {
namespace builder { namespace bobthebuilder {

class DimMass : public AbsDimensionReal{
 public:
  DimMass();
  ~DimMass();
  
  boost::shared_ptr<AbsPdf> CreatePdf( const std::string& pdf_type );
};

}}
}
#endif // BUILDER_BOBTHEBUILDER_DIMMASS_H
