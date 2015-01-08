#include "Riostream.h" 

#include "Coefficient.h" 
#include "RooAbsReal.h" 
#include "RooAbsCategory.h"
#include "RooCategory.h" 
#include <math.h> 
#include "TMath.h" 

ClassImp(doofit::roofit::functions::bdecay::Coefficient) 

namespace doofit {
namespace roofit {
namespace functions {
namespace bdecay {

Coefficient::Coefficient(const std::string& name, 
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
                       int         _tag_sign_) :
  RooAbsReal(name.c_str(),name.c_str()), 
  cp_coeff_("cp_coeff_","cp_coeff_",this,_cp_coeff_),
  coeff_type_(_coeff_type_),
  tag_("tag_","tag_",this,_tag_),
  eta_("eta_","eta_",this,_eta_),
  avg_eta_("avg_eta_","avg_eta_",this,_avg_eta_),
  p0_("p0_","p0_",this,_p0_),
  p1_("p1_","p1_",this,_p1_),
  delta_p0_("delta_p0_","delta_p0_",this,_delta_p0_),
  delta_p1_("delta_p1_","delta_p1_",this,_delta_p1_),
  production_asym_("production_asym_","production_asym_",this,_production_asym_),
  tag_sign_(_tag_sign_)
{ 
} 


Coefficient::Coefficient(const Coefficient& other, const char* name) :  
  RooAbsReal(other,name), 
  cp_coeff_("cp_coeff_",this,other.cp_coeff_),
  coeff_type_(other.coeff_type_),
  tag_("tag_",this,other.tag_),
  eta_("eta_",this,other.eta_),
  avg_eta_("avg_eta_",this,other.avg_eta_),
  p0_("p0_",this,other.p0_),
  p1_("p1_",this,other.p1_),
  delta_p0_("delta_p0_",this,other.delta_p0_),
  delta_p1_("delta_p1_",this,other.delta_p1_),
  production_asym_("production_asym_",this,other.production_asym_),
  tag_sign_(other.tag_sign_)
{ 
} 


Double_t Coefficient::evaluate() const 
{ 
  return evaluate(cp_coeff_, coeff_type_, tag_, eta_, avg_eta_, p0_, p1_, delta_p0_, delta_p1_, production_asym_, tag_sign_);
}


Int_t Coefficient::getAnalyticalIntegral(RooArgSet& allVars, RooArgSet& analVars, const char* /*rangeName*/) const  
{ 
  // debug
  // std::printf("CHECK: In %s line %u (%s): #Vars = %d : allVars = ", __func__, __LINE__, __FILE__, allVars.getSize());
  // allVars.Print();

  if (matchArgs(allVars, analVars, tag_)) return 1 ;
  return 0 ;
} 

Int_t Coefficient::getAnalyticalIntegralWN(RooArgSet& allVars, RooArgSet& analVars, const RooArgSet* normSet, const char* /*rangeName*/) const  
{ 
  // debug
  std::printf("CHECK: In %s line %u (%s): #Vars = %d : allVars = ", __func__, __LINE__, __FILE__, allVars.getSize());
  allVars.Print();
  if (normSet) normSet->Print();

  if (matchArgs(allVars, analVars, tag_)) return 1 ;
  return 0 ;
} 

Double_t Coefficient::analyticalIntegral(Int_t code, const char* rangeName) const  
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
    if (isTagInRange(tag_, 0, rangeName)){
      integral += evaluate(cp_coeff_, coeff_type_, 0, eta_, avg_eta_, p0_, p1_, delta_p0_, delta_p1_, production_asym_, tag_sign_);
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
      if (hasTagState(tag_, 0)) integral += evaluate(cp_coeff_, coeff_type_, 0, eta_, avg_eta_, p0_, p1_, delta_p0_, delta_p1_, production_asym_, tag_sign_);
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


std::pair<double, double> Coefficient::calibrate(double eta, double avg_eta, double p0, double p1, double delta_p0, double delta_p1) const
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
  if (eta_cal_b < 0.0)    eta_cal_b = 0;
  if (eta_cal_bbar < 0.0) eta_cal_bbar = 0;

  // the next few lines set every eta value (avg and high/low from asymmetries) to zero
  // if only one of them is below zero. This seems to introduce a fit bias on our CP
  // observables. (CC)
  // if (eta_cal < 0.0 || eta_cal_b < 0.0 || eta_cal_bbar < 0.0){
  //   eta_cal_b    = 0.0;
  //   eta_cal_bbar = 0.0;
  // }
  return std::make_pair(eta_cal_b, eta_cal_bbar);
}


Double_t Coefficient::evaluate(double cp_coeff,
                               CoeffType coeff_type,
                               int    tag,
                               double eta,
                               double avg_eta,
                               double p0,
                               double p1,
                               double delta_p0,
                               double delta_p1,
                               double production_asym,
                               int    tag_sign) const 
{
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


bool Coefficient::isTagInRange(const RooCategoryProxy& tag, int tag_state, const char* rangeName) const 
{
  if (tag.arg().lookupType(tag_state) == nullptr){
    return false;
  }
  else{
    return dynamic_cast<const RooCategory&>(tag.arg()).isStateInRange(rangeName, tag.arg().lookupType(tag_state)->GetName());
  }
}


bool Coefficient::hasTagState(const RooCategoryProxy& tag, int tag_state) const
{ 
  return dynamic_cast<const RooCategory&>(tag.arg()).isValidIndex(tag_state);
}


int  Coefficient::getIndex(const RooCategoryProxy& tag) const
{
  return dynamic_cast<const RooCategory&>(tag.arg()).getIndex();
}


} // namespace bdecay
} // namespace functions
} // namespace roofit
} // namespace doofit
