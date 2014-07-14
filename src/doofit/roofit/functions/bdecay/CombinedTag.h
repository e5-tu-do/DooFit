#ifndef DOOFIT_ROOFIT_FUNCTIONS_BDECAY_COMBINEDTAG
#define DOOFIT_ROOFIT_FUNCTIONS_BDECAY_COMBINEDTAG

#include "RooAbsReal.h"
#include "RooRealProxy.h"
#include "RooCategoryProxy.h"
#include "RooAbsReal.h"
#include "RooAbsCategory.h"

namespace doofit {
namespace roofit {
namespace functions {
namespace bdecay {

class CombinedTag : public RooAbsReal {
public:
  CombinedTag(const std::string& name,
	      RooAbsReal& _eta_one_,
	      RooAbsReal& _eta_two_,
	      RooAbsCategory& _tag_one_,
	      RooAbsCategory& _tag_two_);
  CombinedTag(const CombinedTag& other, const char* name=0) ;
  virtual TObject* clone(const char* newname) const { return new CombinedTag(*this,newname); }
  inline virtual ~CombinedTag() { }

protected:

  RooRealProxy eta_one_ ;
  RooRealProxy eta_two_ ;
  RooCategoryProxy tag_one_ ;
  RooCategoryProxy tag_two_ ;
  
  Double_t evaluate() const ;

private:

  ClassDef(CombinedTag,1) // Combination of the tag decision of to taggers
};

} // namespace bdecay
} // namespace functions
} // namespace roofit
} // namespace doofit

#endif
