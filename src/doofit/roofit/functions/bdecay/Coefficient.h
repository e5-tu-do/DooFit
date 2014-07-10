#ifndef DOOFIT_ROOFIT_FUNCTIONS_BDECAY_COEFFICIENT
#define DOOFIT_ROOFIT_FUNCTIONS_BDECAY_COEFFICIENT

#include "RooAbsPdf.h"
#include "RooRealProxy.h"
#include "RooCategoryProxy.h"
#include "RooAbsReal.h"
#include "RooAbsCategory.h"

namespace doofit {
namespace roofit {
namespace functions {
namespace bdecay {

class Coefficient : public RooAbsPdf {
public:
  enum CoeffType {
    Sin  = 3,
    Cos  = 2,
    Sinh = 1,
    Cosh = 0
  };

  Coefficient(const std::string& name,
        RooAbsReal& _cp_coeff_,
        CoeffType&  _coeff_type_,
        RooAbsCategory& _tag_,
	      RooAbsReal& _mistag_b_,
	      RooAbsReal& _mistag_bbar_,
	      RooAbsReal& _production_asym_); // TODO: make default value = 0
  Coefficient(const Coefficient& other, const char* name=0) ;
  virtual TObject* clone(const char* newname) const { return new Coefficient(*this,newname); }
  inline virtual ~Coefficient() { }

  Int_t getAnalyticalIntegral(RooArgSet& allVars, RooArgSet& analVars, const char* rangeName=0) const ;
  Double_t analyticalIntegral(Int_t code, const char* rangeName=0) const ;

protected:

  RooRealProxy cp_coeff_ ;
  CoeffType coeff_type_ ;
  RooCategoryProxy tag_ ;
  RooRealProxy mistag_b_ ;
  RooRealProxy mistag_bbar_ ;
  RooRealProxy production_asym_ ;
    

  Double_t evaluate() const ;

private:

  ClassDef(Coefficient,1) // CP coefficient for RooBDecay PDF
};

} // namespace bdecay
} // namespace functions
} // namespace roofit
} // namespace doofit

#endif
