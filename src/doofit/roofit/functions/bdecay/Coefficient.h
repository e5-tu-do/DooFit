#ifndef DOOFIT_ROOFIT_FUNCTIONS_BDECAY_COEFFICIENT
#define DOOFIT_ROOFIT_FUNCTIONS_BDECAY_COEFFICIENT

#include <iostream>
#include "RooAbsReal.h"
#include "RooRealProxy.h"
#include "RooAbsCategory.h"
#include "RooCategoryProxy.h"

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
  inline virtual ~Coefficient() { 
    // std::cout << "Coefficient::~Coefficient(): Number of protected evaluations: " << num_protected_ << std::endl;
  }

  Int_t getAnalyticalIntegral(RooArgSet& allVars, RooArgSet& analVars, const char* rangeName=0) const ;
  Int_t getAnalyticalIntegralWN(RooArgSet& allVars, RooArgSet& analVars, const RooArgSet* normSet, const char* rangeName=0) const ;
  Double_t analyticalIntegral(Int_t code, const char* rangeName=0) const ;

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

  int tag_sign_ ;

  mutable unsigned int num_protected_;

  Double_t evaluate() const ;

private:

  std::pair<double, double> calibrate(double eta, double avg_eta, double p0, double p1, double delta_p0, double delta_p1) const ;

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
                    int    tag_sign) const ;

  bool isTagInRange(const RooCategoryProxy& tag, int tag_state, const char* rangeName) const ;
  bool hasTagState(const RooCategoryProxy& tag, int tag_state) const;
  int  getIndex(const RooCategoryProxy& tag) const;

  double Protect(double x) const;

  ClassDef(Coefficient,1) // CP coefficient for RooBDecay PDF
};

} // namespace bdecay
} // namespace functions
} // namespace roofit
} // namespace doofit

#endif
