#ifndef DOOFIT_ROOFIT_FUNCTIONS_BDECAY_COMBINEDMISTAG
#define DOOFIT_ROOFIT_FUNCTIONS_BDECAY_COMBINEDMISTAG

#include "RooAbsPdf.h"
#include "RooRealProxy.h"
#include "RooCategoryProxy.h"
#include "RooAbsReal.h"
#include "RooAbsCategory.h"

namespace doofit {
namespace roofit {
namespace functions {
namespace bdecay {

class CombinedMistag : public RooAbsPdf {
public:
  CombinedMistag(const std::string& name,
	      RooAbsReal& _eta_one_,
	      RooAbsReal& _eta_two_,
	      RooAbsCategory& _tag_one_,
	      RooAbsCategory& _tag_two_);
  CombinedMistag(const CombinedMistag& other, const char* name=0) ;
  virtual TObject* clone(const char* newname) const { return new CombinedMistag(*this,newname); }
  inline virtual ~CombinedMistag() { }

  Int_t getAnalyticalIntegral(RooArgSet& allVars, RooArgSet& analVars, const char* rangeName=0) const ;
  Double_t analyticalIntegral(Int_t code, const char* rangeName=0) const ;

protected:

  RooRealProxy eta_one_ ;
  RooRealProxy eta_two_ ;
  RooCategoryProxy tag_one_ ;
  RooCategoryProxy tag_two_ ;
  
  Double_t evaluate() const ;

private:

  ClassDef(CombinedMistag,1) // Combination of the mistag estimate of to taggers
};

} // namespace bdecay
} // namespace functions
} // namespace roofit
} // namespace doofit

#endif
