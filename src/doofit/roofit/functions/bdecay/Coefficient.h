#ifndef DOOFIT_ROOFIT_FUNCTIONS_BDECAY_COEFFICIENT
#define DOOFIT_ROOFIT_FUNCTIONS_BDECAY_COEFFICIENT

#include "RooAbsReal.h"
#include "RooRealProxy.h"
#include "RooAbsCategory.h"
#include "RooCategoryProxy.h"

#include "Riostream.h" 

#include "Coefficient.h" 
#include "RooAbsReal.h" 
#include "RooAbsCategory.h"
#include "RooCategory.h" 
#include <math.h> 
#include "TMath.h" 

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
        RooAbsCategory& _tag_,
        RooAbsReal& _eta_,
        RooAbsReal& _avg_eta_,
        RooAbsReal& _p0_,
        RooAbsReal& _p1_,
        RooAbsReal& _delta_p0_,
        RooAbsReal& _delta_p1_,
        RooAbsReal& _production_asym_,
        int         _tag_sign_=+1);
  Coefficient(const Coefficient& other, const char* name=0) ;
  virtual TObject* clone(const char* newname) const { return new Coefficient(*this,newname); }
  inline virtual ~Coefficient() { }

  Int_t getAnalyticalIntegral(RooArgSet& allVars, RooArgSet& analVars, const char* rangeName=0) const ;
  Int_t getAnalyticalIntegralWN(RooArgSet& allVars, RooArgSet& analVars, const RooArgSet* normSet, const char* rangeName=0) const ;
  Double_t analyticalIntegral(Int_t code, const char* rangeName=0) const;

protected:

  RooRealProxy cp_coeff_ ;
  CoeffType    coeff_type_ ;
  
  RooCategoryProxy tag_ ;
  RooRealProxy eta_ ;
  RooRealProxy avg_eta_ ;
  RooRealProxy p0_ ;
  RooRealProxy p1_ ;
  RooRealProxy delta_p0_ ;
  RooRealProxy delta_p1_ ;

  RooRealProxy production_asym_ ;

  const RooCategory* tag_cat_;

  int tag_sign_ ;

  inline Double_t evaluate() const { 
   return evaluate(cp_coeff_, coeff_type_, tag_, eta_, avg_eta_, p0_, p1_, delta_p0_, delta_p1_, production_asym_, tag_sign_);
  }

private:

  std::pair<double, double> calibrate(double eta, double avg_eta, double p0, double p1, double delta_p0, double delta_p1) const;

  Double_t evaluate(double cp_coeff,
                    CoeffType coeff_type,
                    int    tag,
                    double eta,
                    double avg_eta,
                    double p0,
                    double p1,
                    double delta_p0,
                    double delta_p1,
                    double production_asym,
                    int    tag_sign) const; 

  inline bool isTagInRange(const RooCategoryProxy& tag, int tag_state, const char* rangeName) const
  {
   return dynamic_cast<const RooCategory&>(tag.arg()).isStateInRange(rangeName, tag.arg().lookupType(tag_state)->GetName());
  }
  inline bool hasTagState(const RooCategoryProxy& tag, int tag_state) const
  { 
    return dynamic_cast<const RooCategory&>(tag.arg()).isValidIndex(tag_state);
  }

  inline bool tagHasTagState(int tag_state) const
  { 
    return tag_cat_->isValidIndex(tag_state);
  }

  inline int  getIndex(const RooCategoryProxy& tag) const
  {
    return dynamic_cast<const RooCategory&>(tag.arg()).getIndex();
  }

  ClassDef(Coefficient,1) // CP coefficient for RooBDecay PDF
};

} // namespace bdecay
} // namespace functions
} // namespace roofit
} // namespace doofit

#endif
