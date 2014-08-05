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
  inline Double_t analyticalIntegral(Int_t code, const char* rangeName=0) const
  { 
    // debug
    // std::printf("CHECK: In %s line %u (%s): Range: %s : Code: %d \n", __func__, __LINE__, __FILE__, rangeName, code);
    
    // first return analytical integral for defined range
    if (rangeName){
      double integral = 0;
      if (isTagInRange(tag_, +1, rangeName)){
        integral += evaluate(cp_coeff_, coeff_type_, +1, eta_, avg_eta_, p0_, p1_, delta_p0_, delta_p1_, production_asym_, tag_sign_);
        // debug
        // std::cout << "Coeff: " << coeff_type_ << " Range: B0B0 : " << integral << std::endl;
      }
      if (isTagInRange(tag_, -1, rangeName)){
        integral += evaluate(cp_coeff_, coeff_type_, -1, eta_, avg_eta_, p0_, p1_, delta_p0_, delta_p1_, production_asym_, tag_sign_);
        // debug
        // std::cout << "Coeff: " << coeff_type_ << " Range: B0barB0 : " << integral << std::endl;
      }
      return integral;
    }
    else{
      if (code == 1){
        double integral = 0.;
        if (hasTagState(tag_, +1)) integral += evaluate(cp_coeff_, coeff_type_, +1, eta_, avg_eta_, p0_, p1_, delta_p0_, delta_p1_, production_asym_, tag_sign_);
        if (hasTagState(tag_, -1)) integral += evaluate(cp_coeff_, coeff_type_, -1, eta_, avg_eta_, p0_, p1_, delta_p0_, delta_p1_, production_asym_, tag_sign_);
        // debug
        // std::cout << "Coeff: " << coeff_type_ << " : OS Integral : " << integral << std::endl;
        return integral;
      }
      return 0;
    }

    // OLD: Explicit integrals
    // if (coeff_type_ == kSin){
    //   return +2.0 * production_asym_ * cp_coeff_;
    // }
    // else if (coeff_type_ == kCos){
    //   return -2.0 * production_asym_ * cp_coeff_;
    // }
    // else if (coeff_type_ == kSinh){
    //   return 2.0 * cp_coeff_;
    // }
    // else if (coeff_type_ == kCosh){
    //   return 2.0 * cp_coeff_;
    // }
    // else{
    //   std::printf("ERROR: In %s line %u (%s) : No valid coefficent! \n", __func__, __LINE__, __FILE__);
    //   return 0;
    //   abort();
    // }
  } 

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

  inline Double_t evaluate() const { 
   return evaluate(cp_coeff_, coeff_type_, tag_, eta_, avg_eta_, p0_, p1_, delta_p0_, delta_p1_, production_asym_, tag_sign_);
  }

private:

  inline std::pair<double, double> calibrate(double eta, double avg_eta, double p0, double p1, double delta_p0, double delta_p1) const 
  {
    double eta_cal = 0;
    double eta_cal_b = 0;
    double eta_cal_bbar = 0;

    // calculate calibrated average eta
    eta_cal = p0 + p1 * ( eta - avg_eta );
    
    // if eta is larger or equal 0.5 return 0.5
    if (eta >= 0.5){
      eta_cal      = 0.5;
      eta_cal_b    = 0.5;
      eta_cal_bbar = 0.5;
    }
    else{
      // calibrate eta
      eta_cal_b    = p0 + 0.5 * delta_p0 + ( p1 + 0.5 * delta_p1 ) * ( eta - avg_eta );
      eta_cal_bbar = p0 - 0.5 * delta_p0 + ( p1 - 0.5 * delta_p1 ) * ( eta - avg_eta );
    }
    // if calibrated average eta is larger or equal 0.5 return 0.5
    if (eta_cal >= 0.5){
      eta_cal_b    = 0.5;
      eta_cal_bbar = 0.5;
    }
    // if calibrated eta is smaller than 0 return 0
    if (eta_cal < 0.0 || eta_cal_b < 0.0 || eta_cal_bbar < 0.0){
      eta_cal_b    = 0.0;
      eta_cal_bbar = 0.0;
    }
    return std::make_pair(eta_cal_b, eta_cal_bbar);
  }

  inline Double_t evaluate(double cp_coeff,
                    CoeffType coeff_type,
                    int    tag,
                    double eta,
                    double avg_eta,
                    double p0,
                    double p1,
                    double delta_p0,
                    double delta_p1,
                    double production_asym,
                    int    tag_sign) const {
    // calibrate single tagger
    std::pair<double, double> calibrated_mistag = calibrate(eta, avg_eta, p0, p1, delta_p0, delta_p1);

    double eta_b    = calibrated_mistag.first;
    double eta_bbar = calibrated_mistag.second;

    // calculate coefficients
    if (coeff_type == kSin){
      return -1.0 * cp_coeff * ( tag_sign * tag - production_asym * ( 1.0 - tag_sign * tag * eta_b + tag_sign * tag * eta_bbar ) - tag_sign * tag * ( eta_b + eta_bbar ) );
    }
    else if (coeff_type == kCos){
      return +1.0 * cp_coeff * ( tag_sign * tag - production_asym * ( 1.0 - tag_sign * tag * eta_b + tag_sign * tag * eta_bbar ) - tag_sign * tag * ( eta_b + eta_bbar ) );
    }
    else if (coeff_type == kSinh){
      return cp_coeff * ( 1.0 - tag_sign * tag * production_asym * ( 1.0 - eta_b - eta_bbar ) - tag_sign * tag * ( eta_b - eta_bbar ) );
    }
    else if (coeff_type == kCosh){
      return cp_coeff * ( 1.0 - tag_sign * tag * production_asym * ( 1.0 - eta_b - eta_bbar ) - tag_sign * tag * ( eta_b - eta_bbar ) );
    }
    else{
      std::cout << "ERROR\t" << "Coefficient::evaluate(): No valid coefficient type!" << std::endl;
      abort();
    }
  }

  inline bool isTagInRange(const RooCategoryProxy& tag, int tag_state, const char* rangeName) const
  {
   return dynamic_cast<const RooCategory&>(tag.arg()).isStateInRange(rangeName, tag.arg().lookupType(tag_state)->GetName());
  }
  inline bool hasTagState(const RooCategoryProxy& tag, int tag_state) const
  { 
    return dynamic_cast<const RooCategory&>(tag.arg()).isValidIndex(tag_state);
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
