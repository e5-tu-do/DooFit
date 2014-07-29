#ifndef DOOFIT_ROOFIT_FUNCTIONS_BDECAY_SIMPLECOEFFICIENT
#define DOOFIT_ROOFIT_FUNCTIONS_BDECAY_SIMPLECOEFFICIENT

#include "RooAbsReal.h"
#include "RooRealProxy.h"
#include "RooAbsCategory.h"
#include "RooCategoryProxy.h"

namespace doofit {
namespace roofit {
namespace functions {
namespace bdecay {

class SimpleCoefficient : public RooAbsReal {
public:
  enum CoeffType {
    kSin  = 3,
    kCos  = 2,
    kSinh = 1,
    kCosh = 0
  };

  SimpleCoefficient(const std::string& name,
	      RooAbsReal& _cp_coeff_,
        CoeffType   _coeff_type_,
	      RooAbsCategory& _tag_,
        RooAbsReal& _eta_,
	      RooAbsReal& _avg_eta_,
	      RooAbsReal& _p0_,
	      RooAbsReal& _p1_
	      );
  SimpleCoefficient(const SimpleCoefficient& other, const char* name=0) ;
  virtual TObject* clone(const char* newname) const { return new SimpleCoefficient(*this,newname); }
  inline virtual ~SimpleCoefficient() { }

  Int_t getAnalyticalIntegral(RooArgSet& allVars, RooArgSet& analVars, const char* rangeName=0) const ;
  Double_t analyticalIntegral(Int_t code, const char* rangeName=0) const ;

protected:

  RooRealProxy cp_coeff_ ;
  CoeffType    coeff_type_ ;

  RooCategoryProxy tag_ ;
  RooRealProxy eta_ ;
  
  RooRealProxy avg_eta_ ;
  RooRealProxy p0_ ;
  RooRealProxy p1_ ;
  
  // const RooCategory& internal_tag_ ;

  Double_t evaluate() const ;

private:

  ClassDef(SimpleCoefficient,1) // CP coefficient for RooBDecay PDF
};


} // namespace bdecay
} // namespace functions
} // namespace roofit
} // namespace doofit

#endif
