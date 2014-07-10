#ifndef DOOFIT_ROOFIT_FUNCTIONS_BDECAY_CALIBRATEDMISTAG
#define DOOFIT_ROOFIT_FUNCTIONS_BDECAY_CALIBRATEDMISTAG

#include "RooAbsPdf.h"
#include "RooRealProxy.h"
#include "RooCategoryProxy.h"
#include "RooAbsReal.h"
#include "RooAbsCategory.h"

namespace doofit {
namespace roofit {
namespace functions {
namespace bdecay {

class CalibratedMistag : public RooAbsPdf {
public:
  enum MistagType {
    kB  = +1,
    kBbar = -1,
    kNoAsymmetry = 0
  };

  CalibratedMistag(const std::string& name,
	      RooAbsReal& _eta_,
	      RooAbsReal& _avg_eta_,
	      RooAbsReal& _p0_,
	      RooAbsReal& _p1_,
	      RooAbsReal& _delta_p0_,
	      RooAbsReal& _delta_p1_,
	      RooAbsCategory& _tag_,
        const MistagType& _mistag_type_=kNoAsymmetry);
  CalibratedMistag(const CalibratedMistag& other, const char* name=0) ;
  virtual TObject* clone(const char* newname) const { return new CalibratedMistag(*this,newname); }
  inline virtual ~CalibratedMistag() { }

  Int_t getAnalyticalIntegral(RooArgSet& allVars, RooArgSet& analVars, const char* rangeName=0) const ;
  Double_t analyticalIntegral(Int_t code, const char* rangeName=0) const ;

protected:

  RooRealProxy eta_ ;
  RooRealProxy avg_eta_ ;
  RooRealProxy p0_ ;
  RooRealProxy p1_ ;
  RooRealProxy delta_p0_ ;
  RooRealProxy delta_p1_ ;
  RooCategoryProxy tag_ ;
  
  const MistagType mistag_type_ ;

  Double_t evaluate() const ;

private:

  ClassDef(CalibratedMistag,1) // Calibration of single mistag
};

} // namespace bdecay
} // namespace functions
} // namespace roofit
} // namespace doofit

#endif
