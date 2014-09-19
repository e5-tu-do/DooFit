#include "Riostream.h" 

#include "Coefficient2.h" 
#include "RooAbsReal.h" 
#include "RooAbsCategory.h"
#include "RooCategory.h" 
#include <math.h> 
#include "TMath.h" 

ClassImp(doofit::roofit::functions::bdecay::Coefficient2) 

namespace doofit {
namespace roofit {
namespace functions {
namespace bdecay {

Coefficient2::Coefficient2(const std::string& name, 
                       RooAbsReal& _cp_coeff_,
                       CoeffType   _coeff_type_,
                       RooAbsCategory& _tag_os_,
                       RooAbsReal& _eta_os_,
                       RooAbsReal& _avg_eta_os_,
                       RooAbsReal& _p0_os_,
                       RooAbsReal& _p1_os_,
                       RooAbsReal& _delta_p0_os_,
                       RooAbsReal& _delta_p1_os_,
                       RooAbsCategory& _tag_ss_,
                       RooAbsReal& _eta_ss_,
                       RooAbsReal& _avg_eta_ss_,
                       RooAbsReal& _p0_ss_,
                       RooAbsReal& _p1_ss_,
                       RooAbsReal& _delta_p0_ss_,
                       RooAbsReal& _delta_p1_ss_,
                       RooAbsReal& _production_asym_,
                       int         _tag_sign_os_,
                       int         _tag_sign_ss_) :
  RooAbsReal(name.c_str(),name.c_str()), 
  cp_coeff_("cp_coeff_","cp_coeff_",this,_cp_coeff_),
  coeff_type_(_coeff_type_),
  tag_os_("tag_os_","tag_os_",this,_tag_os_),
  eta_os_("eta_os_","eta_os_",this,_eta_os_),
  avg_eta_os_("avg_eta_os_","avg_eta_os_",this,_avg_eta_os_),
  p0_os_("p0_os_","p0_os_",this,_p0_os_),
  p1_os_("p1_os_","p1_os_",this,_p1_os_),
  delta_p0_os_("delta_p0_os_","delta_p0_os_",this,_delta_p0_os_),
  delta_p1_os_("delta_p1_os_","delta_p1_os_",this,_delta_p1_os_),
  tag_ss_("tag_ss_","tag_ss_",this,_tag_ss_),
  eta_ss_("eta_ss_","eta_ss_",this,_eta_ss_),
  avg_eta_ss_("avg_eta_ss_","avg_eta_ss_",this,_avg_eta_ss_),
  p0_ss_("p0_ss_","p0_ss_",this,_p0_ss_),
  p1_ss_("p1_ss_","p1_ss_",this,_p1_ss_),
  delta_p0_ss_("delta_p0_ss_","delta_p0_ss_",this,_delta_p0_ss_),
  delta_p1_ss_("delta_p1_ss_","delta_p1_ss_",this,_delta_p1_ss_),
  production_asym_("production_asym_","production_asym_",this,_production_asym_),
  tag_sign_os_(_tag_sign_os_),
  tag_sign_ss_(_tag_sign_ss_)
{ 
} 


Coefficient2::Coefficient2(const Coefficient2& other, const char* name) :  
  RooAbsReal(other,name), 
  cp_coeff_("cp_coeff_",this,other.cp_coeff_),
  coeff_type_(other.coeff_type_),
  tag_os_("tag_os_",this,other.tag_os_),
  eta_os_("eta_os_",this,other.eta_os_),
  avg_eta_os_("avg_eta_os_",this,other.avg_eta_os_),
  p0_os_("p0_os_",this,other.p0_os_),
  p1_os_("p1_os_",this,other.p1_os_),
  delta_p0_os_("delta_p0_os_",this,other.delta_p0_os_),
  delta_p1_os_("delta_p1_os_",this,other.delta_p1_os_),
  tag_ss_("tag_ss_",this,other.tag_ss_),
  eta_ss_("eta_ss_",this,other.eta_ss_),
  avg_eta_ss_("avg_eta_ss_",this,other.avg_eta_ss_),
  p0_ss_("p0_ss_",this,other.p0_ss_),
  p1_ss_("p1_ss_",this,other.p1_ss_),
  delta_p0_ss_("delta_p0_ss_",this,other.delta_p0_ss_),
  delta_p1_ss_("delta_p1_ss_",this,other.delta_p1_ss_),
  production_asym_("production_asym_",this,other.production_asym_),
  tag_sign_os_(other.tag_sign_os_),
  tag_sign_ss_(other.tag_sign_ss_)
{ 
} 


Double_t Coefficient2::evaluate() const 
{ 
  return evaluate(cp_coeff_, coeff_type_, tag_os_, eta_os_, avg_eta_os_, p0_os_, p1_os_, delta_p0_os_, delta_p1_os_, tag_ss_, eta_ss_, avg_eta_ss_, p0_ss_, p1_ss_, delta_p0_ss_, delta_p1_ss_, production_asym_, tag_sign_os_, tag_sign_ss_);
}


Int_t Coefficient2::getAnalyticalIntegral(RooArgSet& allVars, RooArgSet& analVars, const char* /*rangeName*/) const  
{ 
  // debug
  // std::printf("CHECK: In %s line %u (%s): #Vars = %d : allVars = ", __func__, __LINE__, __FILE__, allVars.getSize());
  // allVars.Print();

  if (matchArgs(allVars, analVars, tag_os_, tag_ss_)) return 2 ;
  if (matchArgs(allVars, analVars, tag_os_)) return 1 ;
  if (matchArgs(allVars, analVars, tag_ss_)) return -1 ;
  return 0 ;
} 

Int_t Coefficient2::getAnalyticalIntegralWN(RooArgSet& allVars, RooArgSet& analVars, const RooArgSet* normSet, const char* /*rangeName*/) const  
{ 
  // debug
  std::printf("CHECK: In %s line %u (%s): #Vars = %d : allVars = ", __func__, __LINE__, __FILE__, allVars.getSize());
  allVars.Print();
  if (normSet) normSet->Print();

  if (matchArgs(allVars, analVars, tag_os_, tag_ss_)) return 2 ;
  if (matchArgs(allVars, analVars, tag_os_)) return 1 ;
  if (matchArgs(allVars, analVars, tag_ss_)) return -1 ;
  return 0 ;
} 

Double_t Coefficient2::analyticalIntegral(Int_t code, const char* rangeName) const  
{ 
  // debug
  // std::printf("CHECK: In %s line %u (%s): Range: %s : Code: %d \n", __func__, __LINE__, __FILE__, rangeName, code);
  
  // first return analytical integral for defined range
  if (rangeName){
    double integral = 0;
    if (isTagInRange(tag_ss_, +1, rangeName)){
      if (isTagInRange(tag_os_, +1, rangeName)){
        integral += evaluate(cp_coeff_, coeff_type_, +1, eta_os_, avg_eta_os_, p0_os_, p1_os_, delta_p0_os_, delta_p1_os_, +1, eta_ss_, avg_eta_ss_, p0_ss_, p1_ss_, delta_p0_ss_, delta_p1_ss_, production_asym_, tag_sign_os_, tag_sign_ss_);
        // debug
        // std::cout << "Coeff: " << coeff_type_ << " Range: B0B0 : " << integral << std::endl;
      }
      if (isTagInRange(tag_os_, -1, rangeName)){
        integral += evaluate(cp_coeff_, coeff_type_, -1, eta_os_, avg_eta_os_, p0_os_, p1_os_, delta_p0_os_, delta_p1_os_, +1, eta_ss_, avg_eta_ss_, p0_ss_, p1_ss_, delta_p0_ss_, delta_p1_ss_, production_asym_, tag_sign_os_, tag_sign_ss_);
        // debug
        // std::cout << "Coeff: " << coeff_type_ << " Range: B0barB0 : " << integral << std::endl;
      }
      if (isTagInRange(tag_os_, 0, rangeName)){
        integral += evaluate(cp_coeff_, coeff_type_, 0, eta_os_, avg_eta_os_, p0_os_, p1_os_, delta_p0_os_, delta_p1_os_, +1, eta_ss_, avg_eta_ss_, p0_ss_, p1_ss_, delta_p0_ss_, delta_p1_ss_, production_asym_, tag_sign_os_, tag_sign_ss_);
        // debug
        // std::cout << "Coeff: " << coeff_type_ << " Range: B0barB0 : " << integral << std::endl;
      }
    }
    if (isTagInRange(tag_ss_, -1, rangeName)){
      if (isTagInRange(tag_os_, +1, rangeName)){
        integral += evaluate(cp_coeff_, coeff_type_, +1, eta_os_, avg_eta_os_, p0_os_, p1_os_, delta_p0_os_, delta_p1_os_, -1, eta_ss_, avg_eta_ss_, p0_ss_, p1_ss_, delta_p0_ss_, delta_p1_ss_, production_asym_, tag_sign_os_, tag_sign_ss_);
        // debug
        // std::cout << "Coeff: " << coeff_type_ << " Range: B0B0bar : " << integral << std::endl;
      }
      if (isTagInRange(tag_os_, -1, rangeName)){
        integral += evaluate(cp_coeff_, coeff_type_, -1, eta_os_, avg_eta_os_, p0_os_, p1_os_, delta_p0_os_, delta_p1_os_, -1, eta_ss_, avg_eta_ss_, p0_ss_, p1_ss_, delta_p0_ss_, delta_p1_ss_, production_asym_, tag_sign_os_, tag_sign_ss_);
        // debug
        // std::cout << "Coeff: " << coeff_type_ << " Range: B0barB0bar : " << integral << std::endl;
      }
      if (isTagInRange(tag_os_, 0, rangeName)){
        integral += evaluate(cp_coeff_, coeff_type_, 0, eta_os_, avg_eta_os_, p0_os_, p1_os_, delta_p0_os_, delta_p1_os_, -1, eta_ss_, avg_eta_ss_, p0_ss_, p1_ss_, delta_p0_ss_, delta_p1_ss_, production_asym_, tag_sign_os_, tag_sign_ss_);
        // debug
        // std::cout << "Coeff: " << coeff_type_ << " Range: B0barB0bar : " << integral << std::endl;
      }
    }
    if (isTagInRange(tag_ss_, 0, rangeName)){
      if (isTagInRange(tag_os_, +1, rangeName)){
        integral += evaluate(cp_coeff_, coeff_type_, +1, eta_os_, avg_eta_os_, p0_os_, p1_os_, delta_p0_os_, delta_p1_os_, 0, eta_ss_, avg_eta_ss_, p0_ss_, p1_ss_, delta_p0_ss_, delta_p1_ss_, production_asym_, tag_sign_os_, tag_sign_ss_);
        // debug
        // std::cout << "Coeff: " << coeff_type_ << " Range: B0B0bar : " << integral << std::endl;
      }
      if (isTagInRange(tag_os_, -1, rangeName)){
        integral += evaluate(cp_coeff_, coeff_type_, -1, eta_os_, avg_eta_os_, p0_os_, p1_os_, delta_p0_os_, delta_p1_os_, 0, eta_ss_, avg_eta_ss_, p0_ss_, p1_ss_, delta_p0_ss_, delta_p1_ss_, production_asym_, tag_sign_os_, tag_sign_ss_);
        // debug
        // std::cout << "Coeff: " << coeff_type_ << " Range: B0barB0bar : " << integral << std::endl;
      }
      if (isTagInRange(tag_os_, 0, rangeName)){
        integral += evaluate(cp_coeff_, coeff_type_, 0, eta_os_, avg_eta_os_, p0_os_, p1_os_, delta_p0_os_, delta_p1_os_, 0, eta_ss_, avg_eta_ss_, p0_ss_, p1_ss_, delta_p0_ss_, delta_p1_ss_, production_asym_, tag_sign_os_, tag_sign_ss_);
        // debug
        // std::cout << "Coeff: " << coeff_type_ << " Range: B0barB0bar : " << integral << std::endl;
      }
    }
    return integral;
  }
  else{
    if (code == +1){
      double integral = 0.;
      if (hasTagState(tag_os_, +1)) integral += evaluate(cp_coeff_, coeff_type_, +1, eta_os_, avg_eta_os_, p0_os_, p1_os_, delta_p0_os_, delta_p1_os_, 0, eta_ss_, avg_eta_ss_, p0_ss_, p1_ss_, delta_p0_ss_, delta_p1_ss_, production_asym_, tag_sign_os_, tag_sign_ss_);
      if (hasTagState(tag_os_, -1)) integral += evaluate(cp_coeff_, coeff_type_, -1, eta_os_, avg_eta_os_, p0_os_, p1_os_, delta_p0_os_, delta_p1_os_, 0, eta_ss_, avg_eta_ss_, p0_ss_, p1_ss_, delta_p0_ss_, delta_p1_ss_, production_asym_, tag_sign_os_, tag_sign_ss_);
      if (hasTagState(tag_os_, 0)) integral += evaluate(cp_coeff_, coeff_type_, 0, eta_os_, avg_eta_os_, p0_os_, p1_os_, delta_p0_os_, delta_p1_os_, 0, eta_ss_, avg_eta_ss_, p0_ss_, p1_ss_, delta_p0_ss_, delta_p1_ss_, production_asym_, tag_sign_os_, tag_sign_ss_);
      // debug
      // std::cout << "Coeff: " << coeff_type_ << " : OS Integral : " << integral << std::endl;
      return integral;
    }
    if (code == -1){
      double integral = 0.;
      if (hasTagState(tag_ss_, +1)) integral += evaluate(cp_coeff_, coeff_type_, 0, eta_os_, avg_eta_os_, p0_os_, p1_os_, delta_p0_os_, delta_p1_os_, +1, eta_ss_, avg_eta_ss_, p0_ss_, p1_ss_, delta_p0_ss_, delta_p1_ss_, production_asym_, tag_sign_os_, tag_sign_ss_);
      if (hasTagState(tag_ss_, -1)) integral += evaluate(cp_coeff_, coeff_type_, 0, eta_os_, avg_eta_os_, p0_os_, p1_os_, delta_p0_os_, delta_p1_os_, -1, eta_ss_, avg_eta_ss_, p0_ss_, p1_ss_, delta_p0_ss_, delta_p1_ss_, production_asym_, tag_sign_os_, tag_sign_ss_);
      if (hasTagState(tag_ss_, 0)) integral += evaluate(cp_coeff_, coeff_type_, 0, eta_os_, avg_eta_os_, p0_os_, p1_os_, delta_p0_os_, delta_p1_os_, 0, eta_ss_, avg_eta_ss_, p0_ss_, p1_ss_, delta_p0_ss_, delta_p1_ss_, production_asym_, tag_sign_os_, tag_sign_ss_);
      // debug
      // std::cout << "Coeff: " << coeff_type_ << " : SS Integral : " << integral << std::endl;
      return integral;
    }
    if (code == 2){
      double integral = 0.;
      if (hasTagState(tag_os_, +1) && hasTagState(tag_ss_, +1)) integral += evaluate(cp_coeff_, coeff_type_, +1, eta_os_, avg_eta_os_, p0_os_, p1_os_, delta_p0_os_, delta_p1_os_, +1, eta_ss_, avg_eta_ss_, p0_ss_, p1_ss_, delta_p0_ss_, delta_p1_ss_, production_asym_, tag_sign_os_, tag_sign_ss_);
      if (hasTagState(tag_os_, +1) && hasTagState(tag_ss_, -1)) integral += evaluate(cp_coeff_, coeff_type_, +1, eta_os_, avg_eta_os_, p0_os_, p1_os_, delta_p0_os_, delta_p1_os_, -1, eta_ss_, avg_eta_ss_, p0_ss_, p1_ss_, delta_p0_ss_, delta_p1_ss_, production_asym_, tag_sign_os_, tag_sign_ss_);
      if (hasTagState(tag_os_, +1) && hasTagState(tag_ss_, 0))  integral += evaluate(cp_coeff_, coeff_type_, +1, eta_os_, avg_eta_os_, p0_os_, p1_os_, delta_p0_os_, delta_p1_os_, 0, eta_ss_, avg_eta_ss_, p0_ss_, p1_ss_, delta_p0_ss_, delta_p1_ss_, production_asym_, tag_sign_os_, tag_sign_ss_);
      if (hasTagState(tag_os_, -1) && hasTagState(tag_ss_, +1)) integral += evaluate(cp_coeff_, coeff_type_, -1, eta_os_, avg_eta_os_, p0_os_, p1_os_, delta_p0_os_, delta_p1_os_, +1, eta_ss_, avg_eta_ss_, p0_ss_, p1_ss_, delta_p0_ss_, delta_p1_ss_, production_asym_, tag_sign_os_, tag_sign_ss_);
      if (hasTagState(tag_os_, -1) && hasTagState(tag_ss_, -1)) integral += evaluate(cp_coeff_, coeff_type_, -1, eta_os_, avg_eta_os_, p0_os_, p1_os_, delta_p0_os_, delta_p1_os_, -1, eta_ss_, avg_eta_ss_, p0_ss_, p1_ss_, delta_p0_ss_, delta_p1_ss_, production_asym_, tag_sign_os_, tag_sign_ss_);
      if (hasTagState(tag_os_, -1) && hasTagState(tag_ss_, 0))  integral += evaluate(cp_coeff_, coeff_type_, -1, eta_os_, avg_eta_os_, p0_os_, p1_os_, delta_p0_os_, delta_p1_os_, 0, eta_ss_, avg_eta_ss_, p0_ss_, p1_ss_, delta_p0_ss_, delta_p1_ss_, production_asym_, tag_sign_os_, tag_sign_ss_);
      if (hasTagState(tag_os_, 0) && hasTagState(tag_ss_, +1)) integral += evaluate(cp_coeff_, coeff_type_, 0, eta_os_, avg_eta_os_, p0_os_, p1_os_, delta_p0_os_, delta_p1_os_, +1, eta_ss_, avg_eta_ss_, p0_ss_, p1_ss_, delta_p0_ss_, delta_p1_ss_, production_asym_, tag_sign_os_, tag_sign_ss_);
      if (hasTagState(tag_os_, 0) && hasTagState(tag_ss_, -1)) integral += evaluate(cp_coeff_, coeff_type_, 0, eta_os_, avg_eta_os_, p0_os_, p1_os_, delta_p0_os_, delta_p1_os_, -1, eta_ss_, avg_eta_ss_, p0_ss_, p1_ss_, delta_p0_ss_, delta_p1_ss_, production_asym_, tag_sign_os_, tag_sign_ss_);
      if (hasTagState(tag_os_, 0) && hasTagState(tag_ss_, 0))  integral += evaluate(cp_coeff_, coeff_type_, 0, eta_os_, avg_eta_os_, p0_os_, p1_os_, delta_p0_os_, delta_p1_os_, 0, eta_ss_, avg_eta_ss_, p0_ss_, p1_ss_, delta_p0_ss_, delta_p1_ss_, production_asym_, tag_sign_os_, tag_sign_ss_);
      // debug
      // std::cout << "Coeff: " << coeff_type_ << " : OS + SS Integral : " << integral << std::endl;
      return integral;
    }
  }
  return 0;
} 


std::pair<double, double> Coefficient2::calibrate(double eta, double avg_eta, double p0, double p1, double delta_p0, double delta_p1) const
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


Double_t Coefficient2::evaluate(double cp_coeff,
                                CoeffType coeff_type,
                                int    tag_os,
                                double eta_os,
                                double avg_eta_os,
                                double p0_os,
                                double p1_os,
                                double delta_p0_os,
                                double delta_p1_os,
                                int    tag_ss,
                                double eta_ss,
                                double avg_eta_ss,
                                double p0_ss,
                                double p1_ss,
                                double delta_p0_ss,
                                double delta_p1_ss,
                                double production_asym,
                                int    tag_sign_os,
                                int    tag_sign_ss) const 
{
  // calibrate single tagger
  std::pair<double, double> calibrated_mistag_os = calibrate(eta_os, avg_eta_os, p0_os, p1_os, delta_p0_os, delta_p1_os);
  std::pair<double, double> calibrated_mistag_ss = calibrate(eta_ss, avg_eta_ss, p0_ss, p1_ss, delta_p0_ss, delta_p1_ss);

  double eta_os_b    = calibrated_mistag_os.first;
  double eta_os_bbar = calibrated_mistag_os.second;
  double eta_ss_b    = calibrated_mistag_ss.first;
  double eta_ss_bbar = calibrated_mistag_ss.second;

  // combine taggers and calculate intermediate steps
  double sum = 0.5 * ( 1. + tag_sign_os * tag_os * tag_sign_ss * tag_ss * ( 1. 
                                                + 2. * ( eta_os_b * eta_ss_b + eta_os_bbar * eta_ss_bbar ) 
                                                - ( eta_os_b + eta_os_bbar ) 
                                                - ( eta_ss_b + eta_ss_bbar ) 
                                              )
                          - tag_sign_os * tag_os * ( eta_os_b - eta_os_bbar ) 
                          - tag_sign_ss * tag_ss * ( eta_ss_b - eta_ss_bbar ) );

  double difference = 0.5 * ( tag_sign_os * tag_os * tag_sign_ss * tag_ss * ( 2. * ( eta_os_b * eta_ss_b - eta_os_bbar * eta_ss_bbar ) 
                                                  - ( eta_os_b - eta_os_bbar )
                                                  - ( eta_ss_b - eta_ss_bbar ) 
                                                )
                            + tag_sign_os * tag_os * ( 1. - eta_os_b - eta_os_bbar ) 
                            + tag_sign_ss * tag_ss * ( 1. - eta_ss_b - eta_ss_bbar ) );

  // calculate and return coefficients
  if (coeff_type == kSin){
    return -0.5 * cp_coeff * ( difference - production_asym * sum );
  }
  else if (coeff_type == kCos){
    return +0.5 * cp_coeff * ( difference - production_asym * sum );
  }
  else if (coeff_type == kSinh){
    return +0.5 * cp_coeff * ( sum - production_asym * difference );
  }
  else if (coeff_type == kCosh){
    return +0.5 * cp_coeff * ( sum - production_asym * difference );
  }
  else{
    std::cout << "ERROR\t" << "Coefficient2::evaluate(...): No valid coefficient type!" << std::endl;
    abort();
  }
}


bool Coefficient2::isTagInRange(const RooCategoryProxy& tag, int tag_state, const char* rangeName) const 
{
  return dynamic_cast<const RooCategory&>(tag.arg()).isStateInRange(rangeName, tag.arg().lookupType(tag_state)->GetName());
}


bool Coefficient2::hasTagState(const RooCategoryProxy& tag, int tag_state) const
{ 
  return dynamic_cast<const RooCategory&>(tag.arg()).isValidIndex(tag_state);
}


int  Coefficient2::getIndex(const RooCategoryProxy& tag) const
{
  return dynamic_cast<const RooCategory&>(tag.arg()).getIndex();
}


} // namespace bdecay
} // namespace functions
} // namespace roofit
} // namespace doofit
