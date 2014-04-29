/*****************************************************************************
 * Project: RooFit                                                           *
 *                                                                           *
  * This code was autogenerated by RooClassFactory                            * 
 *****************************************************************************/

#ifndef DOOFIT_ROOFIT_FUNCTIONS_POWERLAW
#define DOOFIT_ROOFIT_FUNCTIONS_POWERLAW

#include "RooAbsReal.h"
#include "RooRealProxy.h"
#include "RooCategoryProxy.h"
#include "RooAbsReal.h"
#include "RooAbsCategory.h"


namespace doofit {
namespace roofit {
namespace functions {

class PowerLaw : public RooAbsReal {
public:
  PowerLaw() {} ; 
  PowerLaw(const std::string name,
	      RooAbsReal& _x_,
	      RooAbsReal& _alpha_,
	      RooAbsReal& _gamma_);
  PowerLaw(const PowerLaw& other, const char* name=0) ;
  virtual TObject* clone(const char* newname) const { return new PowerLaw(*this,newname); }
  inline virtual ~PowerLaw() { }

  Int_t getAnalyticalIntegral(RooArgSet& allVars, RooArgSet& analVars, const char* rangeName=0) const ;
  Double_t analyticalIntegral(Int_t code, const char* rangeName=0) const ;

protected:

  RooRealProxy x_ ;
  RooRealProxy alpha_ ;
  RooRealProxy gamma_ ;
  
  Double_t evaluate() const ;

private:

  ClassDef(PowerLaw,1) // Your description goes here...
};
 
}
}
}
#endif
