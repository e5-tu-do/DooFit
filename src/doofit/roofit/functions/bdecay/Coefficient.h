#ifndef DOOFIT_ROOFIT_FUNCTIONS_BDECAY_COEFFICIENT
#define DOOFIT_ROOFIT_FUNCTIONS_BDECAY_COEFFICIENT

#include "RooAbsReal.h"
#include "RooRealProxy.h"
#include "RooAbsReal.h"

namespace doofit {
namespace roofit {
namespace functions {
namespace bdecay {

class Coefficient : public RooAbsReal {
public:
  enum CoeffType {
    kSin  = 3,
    kCos  = 2,
    kSinh = 1,
    kCosh = 0
  };

  Coefficient(const std::string& name,
        RooAbsReal& _cp_coeff_,
        CoeffType   _coeff_type_,
        RooAbsReal& _tag_,
	      RooAbsReal& _mistag_b_,
	      RooAbsReal& _mistag_bbar_,
	      RooAbsReal& _production_asym_); // TODO: make default value = 0
  Coefficient(const Coefficient& other, const char* name=0) ;
  virtual TObject* clone(const char* newname) const { return new Coefficient(*this,newname); }
  inline virtual ~Coefficient() { }

protected:

  RooRealProxy cp_coeff_ ;
  CoeffType    coeff_type_ ;
  RooRealProxy tag_ ;
  RooRealProxy mistag_b_ ;
  RooRealProxy mistag_bbar_ ;
  RooRealProxy production_asym_ ;
    
  Double_t evaluate() const ;

  virtual Int_t getAnalyticalIntegral(RooArgSet& allVars, RooArgSet& analVars/**, const char* rangeName = 0**/) const;
  virtual Double_t analyticalIntegral(Int_t code/**, const char* rangeName = 0**/) const;

private:

  ClassDef(Coefficient,1) // CP coefficient for RooBDecay PDF
};

} // namespace bdecay
} // namespace functions
} // namespace roofit
} // namespace doofit

#endif
