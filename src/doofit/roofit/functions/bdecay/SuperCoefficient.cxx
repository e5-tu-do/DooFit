#include "Riostream.h" 

#include "SuperCoefficient.h" 
#include "RooAbsReal.h" 
#include "RooAbsCategory.h"
#include "RooCategory.h" 
#include <math.h> 
#include "TMath.h" 

ClassImp(doofit::roofit::functions::bdecay::SuperCoefficient) 

namespace doofit {
namespace roofit {
namespace functions {
namespace bdecay {

SuperCoefficient::SuperCoefficient(const std::string& name, 
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
                       RooAbsReal& _production_asym_) :
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
  production_asym_("production_asym_","production_asym_",this,_production_asym_)
{ 
} 


SuperCoefficient::SuperCoefficient(const SuperCoefficient& other, const char* name) :  
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
  production_asym_("production_asym_",this,other.production_asym_)
{ 
} 


Double_t SuperCoefficient::evaluate() const 
{ 
  return evaluate(cp_coeff_, coeff_type_, tag_os_, eta_os_, avg_eta_os_, p0_os_, p1_os_, delta_p0_os_, delta_p1_os_, tag_ss_, eta_ss_, avg_eta_ss_, p0_ss_, p1_ss_, delta_p0_ss_, delta_p1_ss_, production_asym_);
}


Int_t SuperCoefficient::getAnalyticalIntegral(RooArgSet& allVars, RooArgSet& analVars, const char* /*rangeName*/) const  
{ 
  // debug
  // std::printf("CHECK: In %s line %u (%s): #Vars = %d : allVars = ", __func__, __LINE__, __FILE__, allVars.getSize());
  // allVars.Print();

  if (matchArgs(allVars, analVars, tag_os_, tag_ss_)) return 2 ;
  if (matchArgs(allVars, analVars, tag_os_)) return 1 ;
  if (matchArgs(allVars, analVars, tag_ss_)) return -1 ;
  return 0 ;
} 

Int_t SuperCoefficient::getAnalyticalIntegralWN(RooArgSet& allVars, RooArgSet& analVars, const RooArgSet* normSet, const char* /*rangeName*/) const  
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

Double_t SuperCoefficient::analyticalIntegral(Int_t code, const char* rangeName) const  
{ 
  // debug
  // std::printf("CHECK: In %s line %u (%s): Range: %s : Code: %d \n", __func__, __LINE__, __FILE__, rangeName, code);
  
  // first return analytical integral for defined range
  if (rangeName){
    double integral = 0;
    if (isTagInRange(tag_ss_, +1, rangeName)){
      if (isTagInRange(tag_os_, +1, rangeName)){
        integral += evaluate(cp_coeff_, coeff_type_, +1, eta_os_, avg_eta_os_, p0_os_, p1_os_, delta_p0_os_, delta_p1_os_, +1, eta_ss_, avg_eta_ss_, p0_ss_, p1_ss_, delta_p0_ss_, delta_p1_ss_, production_asym_);
        // debug
        // std::cout << "Coeff: " << coeff_type_ << " Range: B0B0 : " << integral << std::endl;
      }
      if (isTagInRange(tag_os_, -1, rangeName)){
        integral += evaluate(cp_coeff_, coeff_type_, -1, eta_os_, avg_eta_os_, p0_os_, p1_os_, delta_p0_os_, delta_p1_os_, +1, eta_ss_, avg_eta_ss_, p0_ss_, p1_ss_, delta_p0_ss_, delta_p1_ss_, production_asym_);
        // debug
        // std::cout << "Coeff: " << coeff_type_ << " Range: B0barB0 : " << integral << std::endl;
      }
    }
    if (isTagInRange(tag_ss_, -1, rangeName)){
      if (isTagInRange(tag_os_, +1, rangeName)){
        integral += evaluate(cp_coeff_, coeff_type_, +1, eta_os_, avg_eta_os_, p0_os_, p1_os_, delta_p0_os_, delta_p1_os_, -1, eta_ss_, avg_eta_ss_, p0_ss_, p1_ss_, delta_p0_ss_, delta_p1_ss_, production_asym_);
        // debug
        // std::cout << "Coeff: " << coeff_type_ << " Range: B0B0bar : " << integral << std::endl;
      }
      if (isTagInRange(tag_os_, -1, rangeName)){
        integral += evaluate(cp_coeff_, coeff_type_, -1, eta_os_, avg_eta_os_, p0_os_, p1_os_, delta_p0_os_, delta_p1_os_, -1, eta_ss_, avg_eta_ss_, p0_ss_, p1_ss_, delta_p0_ss_, delta_p1_ss_, production_asym_);
        // debug
        // std::cout << "Coeff: " << coeff_type_ << " Range: B0barB0bar : " << integral << std::endl;
      }
    }
    return integral;
  }
  else{
    if (code == +1){
      double integral = 0.;
      if (hasTagState(tag_os_, +1)) integral += evaluate(cp_coeff_, coeff_type_, +1, eta_os_, avg_eta_os_, p0_os_, p1_os_, delta_p0_os_, delta_p1_os_, 0, eta_ss_, avg_eta_ss_, p0_ss_, p1_ss_, delta_p0_ss_, delta_p1_ss_, production_asym_);
      if (hasTagState(tag_os_, -1)) integral += evaluate(cp_coeff_, coeff_type_, -1, eta_os_, avg_eta_os_, p0_os_, p1_os_, delta_p0_os_, delta_p1_os_, 0, eta_ss_, avg_eta_ss_, p0_ss_, p1_ss_, delta_p0_ss_, delta_p1_ss_, production_asym_);
      // debug
      // std::cout << "Coeff: " << coeff_type_ << " : OS Integral : " << integral << std::endl;
      return integral;
    }
    if (code == -1){
      double integral = 0.;
      if (hasTagState(tag_ss_, +1)) integral += evaluate(cp_coeff_, coeff_type_, 0, eta_os_, avg_eta_os_, p0_os_, p1_os_, delta_p0_os_, delta_p1_os_, +1, eta_ss_, avg_eta_ss_, p0_ss_, p1_ss_, delta_p0_ss_, delta_p1_ss_, production_asym_);
      if (hasTagState(tag_ss_, -1)) integral += evaluate(cp_coeff_, coeff_type_, 0, eta_os_, avg_eta_os_, p0_os_, p1_os_, delta_p0_os_, delta_p1_os_, -1, eta_ss_, avg_eta_ss_, p0_ss_, p1_ss_, delta_p0_ss_, delta_p1_ss_, production_asym_);
      // debug
      // std::cout << "Coeff: " << coeff_type_ << " : SS Integral : " << integral << std::endl;
      return integral;
    }
    if (code == 2){
      double integral = 0.;
      if (hasTagState(tag_os_, +1) && hasTagState(tag_ss_, +1) /*&& (getIndex(tag_os_) == +1) && (getIndex(tag_ss_) == +1)*/) integral += evaluate(cp_coeff_, coeff_type_, +1, eta_os_, avg_eta_os_, p0_os_, p1_os_, delta_p0_os_, delta_p1_os_, +1, eta_ss_, avg_eta_ss_, p0_ss_, p1_ss_, delta_p0_ss_, delta_p1_ss_, production_asym_);
      if (hasTagState(tag_os_, +1) && hasTagState(tag_ss_, -1) /*&& (getIndex(tag_os_) == +1) && (getIndex(tag_ss_) == -1)*/) integral += evaluate(cp_coeff_, coeff_type_, +1, eta_os_, avg_eta_os_, p0_os_, p1_os_, delta_p0_os_, delta_p1_os_, -1, eta_ss_, avg_eta_ss_, p0_ss_, p1_ss_, delta_p0_ss_, delta_p1_ss_, production_asym_);
      if (hasTagState(tag_os_, -1) && hasTagState(tag_ss_, +1) /*&& (getIndex(tag_os_) == -1) && (getIndex(tag_ss_) == +1)*/) integral += evaluate(cp_coeff_, coeff_type_, -1, eta_os_, avg_eta_os_, p0_os_, p1_os_, delta_p0_os_, delta_p1_os_, +1, eta_ss_, avg_eta_ss_, p0_ss_, p1_ss_, delta_p0_ss_, delta_p1_ss_, production_asym_);
      if (hasTagState(tag_os_, -1) && hasTagState(tag_ss_, -1) /*&& (getIndex(tag_os_) == -1) && (getIndex(tag_ss_) == -1)*/) integral += evaluate(cp_coeff_, coeff_type_, -1, eta_os_, avg_eta_os_, p0_os_, p1_os_, delta_p0_os_, delta_p1_os_, -1, eta_ss_, avg_eta_ss_, p0_ss_, p1_ss_, delta_p0_ss_, delta_p1_ss_, production_asym_);
      // debug
      // std::cout << "Coeff: " << coeff_type_ << " : OS + SS Integral : " << integral << std::endl;
      return integral;
    }
  }
} 


std::pair<double, double> SuperCoefficient::calibrate(double eta, double avg_eta, double p0, double p1, double delta_p0, double delta_p1) const
{
  double eta_cal_ss = 0;
  double eta_cal_ss_b = 0;
  double eta_cal_ss_bbar = 0;

  // calculate calibrated average mistag
  eta_cal_ss = p0 + p1 * ( eta - avg_eta );
  
  // if mistag is larger or equal 0.5 return 0.5
  if (eta >= 0.5){
    eta_cal_ss      = 0.5;
    eta_cal_ss_b    = 0.5;
    eta_cal_ss_bbar = 0.5;
  }
  // if calibrated average mistag is larger or equal 0.5 return 0.5
  else if (eta_cal_ss >= 0.5){
    eta_cal_ss_b    = 0.5;
    eta_cal_ss_bbar = 0.5;
  }
  else{
    // calibrate mistag
    eta_cal_ss_b    = p0 + 0.5 * delta_p0 + ( p1 + 0.5 * delta_p1 ) * ( eta - avg_eta );
    eta_cal_ss_bbar = p0 - 0.5 * delta_p0 + ( p1 - 0.5 * delta_p1 ) * ( eta - avg_eta );
  }

  return std::make_pair(eta_cal_ss_b, eta_cal_ss_bbar);
}


Double_t SuperCoefficient::evaluate(double cp_coeff,
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
                                    double production_asym) const 
{
  // calibrate single tagger
  std::pair<double, double> calibrated_mistag_os = calibrate(eta_os, avg_eta_os, p0_os, p1_os, delta_p0_os, delta_p1_os);
  std::pair<double, double> calibrated_mistag_ss = calibrate(eta_ss, avg_eta_ss, p0_ss, p1_ss, delta_p0_ss, delta_p1_ss);

  double eta_os_b    = calibrated_mistag_os.first;
  double eta_os_bbar = calibrated_mistag_os.second;
  double eta_ss_b    = calibrated_mistag_ss.first;
  double eta_ss_bbar = calibrated_mistag_ss.second;

  // combine taggers and calculate coefficients
  double sum = 0.5 * ( 1. + tag_os * tag_ss * ( 1. 
                                                + 2. * ( eta_os_b * eta_ss_b + eta_os_bbar * eta_ss_bbar ) 
                                                - ( eta_os_b + eta_os_bbar ) 
                                                - ( eta_ss_b + eta_ss_bbar ) 
                                              )
                          - tag_os * ( eta_os_b - eta_os_bbar ) 
                          - tag_ss * ( eta_ss_b - eta_ss_bbar ) );

  double difference = 0.5 * ( tag_os * tag_ss * ( 2. * ( eta_os_b * eta_ss_b - eta_os_bbar * eta_ss_bbar ) 
                                                  - ( eta_os_b - eta_os_bbar )
                                                  - ( eta_ss_b - eta_ss_bbar ) 
                                                )
                            + tag_os * ( 1. - eta_os_b - eta_os_bbar ) 
                            + tag_ss * ( 1. - eta_ss_b - eta_ss_bbar ) );

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
    std::cout << "ERROR\t" << "SuperCoefficient::evaluate(...): No valid coefficient type!" << std::endl;
    abort();
  }
}


bool SuperCoefficient::isTagInRange(const RooCategoryProxy& tag, int tag_state, const char* rangeName) const 
{
  return dynamic_cast<const RooCategory&>(tag.arg()).isStateInRange(rangeName, tag.arg().lookupType(tag_state)->GetName());
}


bool SuperCoefficient::hasTagState(const RooCategoryProxy& tag, int tag_state) const
{ 
  return dynamic_cast<const RooCategory&>(tag.arg()).isValidIndex(tag_state);
}


int  SuperCoefficient::getIndex(const RooCategoryProxy& tag) const
{
  return dynamic_cast<const RooCategory&>(tag.arg()).getIndex();
}


} // namespace bdecay
} // namespace functions
} // namespace roofit
} // namespace doofit
