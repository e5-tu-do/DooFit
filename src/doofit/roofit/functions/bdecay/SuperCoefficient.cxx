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
  std::printf("CHECK: In %s line %u (%s): #Vars = %d : allVars = ", __func__, __LINE__, __FILE__, allVars.getSize());
  allVars.Print();

  if (matchArgs(allVars, analVars, tag_os_, tag_ss_)) return 2 ;
  if (matchArgs(allVars, analVars, tag_os_)) return 1 ;
  if (matchArgs(allVars, analVars, tag_ss_)) return 1 ;
  return 0 ;
} 


Double_t SuperCoefficient::analyticalIntegral(Int_t code, const char* rangeName) const  
{ 
  std::printf("CHECK: In %s line %u (%s): Range: %s : Code: %d \n", __func__, __LINE__, __FILE__, rangeName, code);
  // first return analytical integral for defined range
  if (rangeName){
    double integral = 0;
    if (isTagInRange(tag_ss_, +1, rangeName)){
      if (isTagInRange(tag_os_, +1, rangeName)){
        integral += evaluate(cp_coeff_, coeff_type_, +1, eta_os_, avg_eta_os_, p0_os_, p1_os_, delta_p0_os_, delta_p1_os_, +1, eta_ss_, avg_eta_ss_, p0_ss_, p1_ss_, delta_p0_ss_, delta_p1_ss_, production_asym_);
        std::cout << "Coeff: " << coeff_type_ << " Range: B0B0 : " << integral << std::endl;
      }
      if (isTagInRange(tag_os_, -1, rangeName)){
        integral += evaluate(cp_coeff_, coeff_type_, -1, eta_os_, avg_eta_os_, p0_os_, p1_os_, delta_p0_os_, delta_p1_os_, +1, eta_ss_, avg_eta_ss_, p0_ss_, p1_ss_, delta_p0_ss_, delta_p1_ss_, production_asym_);
        std::cout << "Coeff: " << coeff_type_ << " Range: B0barB0 : " << integral << std::endl;
      }
    }
    if (isTagInRange(tag_ss_, -1, rangeName)){
      if (isTagInRange(tag_os_, +1, rangeName)){
        integral += evaluate(cp_coeff_, coeff_type_, +1, eta_os_, avg_eta_os_, p0_os_, p1_os_, delta_p0_os_, delta_p1_os_, -1, eta_ss_, avg_eta_ss_, p0_ss_, p1_ss_, delta_p0_ss_, delta_p1_ss_, production_asym_);
        std::cout << "Coeff: " << coeff_type_ << " Range: B0B0bar : " << integral << std::endl;
      }
      if (isTagInRange(tag_os_, -1, rangeName)){
        integral += evaluate(cp_coeff_, coeff_type_, -1, eta_os_, avg_eta_os_, p0_os_, p1_os_, delta_p0_os_, delta_p1_os_, -1, eta_ss_, avg_eta_ss_, p0_ss_, p1_ss_, delta_p0_ss_, delta_p1_ss_, production_asym_);
        std::cout << "Coeff: " << coeff_type_ << " Range: B0barB0bar : " << integral << std::endl;
      }
    }
    return integral;
  }
  else{

    if (coeff_type_ == kSin){
      return +2.0 * production_asym_ * cp_coeff_ * code;
    }
    else if (coeff_type_ == kCos){
      return -2.0 * production_asym_ * cp_coeff_ * code;
    }
    else if (coeff_type_ == kSinh){
      std::printf("WARNING: In %s line %u (%s) : sinh coefficient not yet implemented! \n", __func__, __LINE__, __FILE__);
      return 2.0;
    }
    else if (coeff_type_ == kCosh){
      return 2.0 * cp_coeff_ * code;
    }
    else{
      std::printf("ERROR: In %s line %u (%s) : No valid coefficent! \n", __func__, __LINE__, __FILE__);
      return 0;
      abort();
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


std::pair<double, double> SuperCoefficient::combine_mistags(std::pair<double, double> calibrated_mistag_os, std::pair<double, double> calibrated_mistag_ss, int tag_os, int tag_ss) const
{
  // mistag combination: the Grabalosa way 
  // https://cds.cern.ch/record/1456804/files/CERN-THESIS-2012-075.pdf
  // note that the probability e.g. to have a b-tagged meson corresponds to a Bbar-Meson, so tag=-1
  // debug
  // std::cout << "initial values: OS " << calibrated_mistag_os.first << " / " << calibrated_mistag_os.second << " SS: " << calibrated_mistag_ss.first << " / " << calibrated_mistag_ss.second << " Tag OS " << tag_os << " Tag SS " << tag_ss << std::endl;
  // combine b mistag
  double eta_cal_combined_b = 0;
  if ((calibrated_mistag_os.first >= 0.5) && (calibrated_mistag_ss.first >= 0.5)){
     eta_cal_combined_b = 0.5;
  }
  else if (calibrated_mistag_os.first >= 0.5){
     eta_cal_combined_b = calibrated_mistag_ss.first;
  }
  else if (calibrated_mistag_ss.first >= 0.5){
     eta_cal_combined_b = calibrated_mistag_os.first;
  }
  else{
     // probability to have a b-tagged meson
     double p_b = ( ( ( 1. + tag_os ) / 2. ) - tag_os * ( 1. - calibrated_mistag_os.first ) ) * ( ( ( 1. + tag_ss ) / 2. ) - tag_ss * ( 1. - calibrated_mistag_ss.first ) );
     // probability to have a bbar-tagged meson
     double p_bbar = ( ( ( 1. - tag_os ) / 2. ) + tag_os * ( 1. - calibrated_mistag_os.first ) ) * ( ( ( 1. - tag_ss ) / 2. ) + tag_ss * ( 1. - calibrated_mistag_ss.first ) );

     // as the combined eta will be the one associated with the larger probability for a b / bbar quark:
     // catch possibility that both taggers are equaly sure they are right
     if (abs(p_b + p_bbar) < std::numeric_limits<double>::epsilon()){
      eta_cal_combined_b = 0.5;
     }
     else{
      eta_cal_combined_b = 1. - ( std::max(p_b, p_bbar) / (p_b + p_bbar) );
     }
     // debug
     // std::cout << "p_b " << p_b << " p_bbar " << p_bbar << " combined mistag b " << eta_cal_combined_b << std::endl;
  }

  // combine bbar mistag
  double eta_cal_combined_bbar = 0;
  if ((calibrated_mistag_os.second >= 0.5) && (calibrated_mistag_ss.second >= 0.5)){
     eta_cal_combined_bbar = 0.5;
  }
  else if (calibrated_mistag_os.second >= 0.5){
     eta_cal_combined_bbar = calibrated_mistag_ss.second;
  }
  else if (calibrated_mistag_ss.second >= 0.5){
     eta_cal_combined_bbar = calibrated_mistag_os.second;
  }
  else{
     // probability to have a b-tagged meson
     double p_b = ( ( ( 1. + tag_os ) / 2. ) - tag_os * ( 1. - calibrated_mistag_os.second ) ) * ( ( ( 1. + tag_ss ) / 2. ) - tag_ss * ( 1. - calibrated_mistag_ss.second ) );
     // probability to have a bbar-tagged meson
     double p_bbar = ( ( ( 1. - tag_os ) / 2. ) + tag_os * ( 1. - calibrated_mistag_os.second ) ) * ( ( ( 1. - tag_ss ) / 2. ) + tag_ss * ( 1. - calibrated_mistag_ss.second ) );

     // as the combined eta will be the one associated with the larger probability for a b / bbar quark:
     // catch possibility that both taggers are equaly sure they are right
     if (abs(p_b + p_bbar) < std::numeric_limits<double>::epsilon()){
      eta_cal_combined_bbar = 0.5;
     }
     else{
      eta_cal_combined_bbar = 1. - ( std::max(p_b, p_bbar) / (p_b + p_bbar) );
     }
     // debug
     // std::cout << "p_b " << p_b << " p_bbar " << p_bbar << " combined mistag bbar " << eta_cal_combined_bbar << std::endl;
  }

  return std::make_pair(eta_cal_combined_b, eta_cal_combined_bbar);
}


int SuperCoefficient::combine_tags(std::pair<double, double> calibrated_mistag_os, std::pair<double, double> calibrated_mistag_ss, int tag_os, int tag_ss) const
{
  int tag_combined = 1;
  double eta_cal_os = ( calibrated_mistag_os.first + calibrated_mistag_os.second ) / 2.;
  double eta_cal_ss = ( calibrated_mistag_ss.first + calibrated_mistag_ss.second ) / 2.;
  if ((eta_cal_os >= 0.5) && (eta_cal_ss >= 0.5)){
    // if untagged set to 1
    tag_combined = 1;
  }
  else if (eta_cal_os >= 0.5) {
    // if os untagged, set to ss tag
    tag_combined = tag_ss;
  }
  else if (eta_cal_ss >= 0.5){
    // if ss untagged, set to os tag
    tag_combined = tag_os;
  }
  else{
    // probability to have a b-tagged meson
    double p_b = ( ( ( 1. + tag_os ) / 2. ) - tag_os * ( 1. - eta_cal_os ) ) * ( ( ( 1. + tag_ss ) / 2. ) - tag_ss * ( 1. - eta_cal_ss ) );
    // probability to have a bbar-tagged meson
    double p_bbar = ( ( ( 1. - tag_os ) / 2. ) + tag_os * ( 1. - eta_cal_os ) ) * ( ( ( 1. - tag_ss ) / 2. ) + tag_ss * ( 1. - eta_cal_ss ) );
    
    if (p_b > p_bbar){
      tag_combined = -1;
    }
    else{
      tag_combined = +1;
    }
  }
  return tag_combined;
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
  std::pair<double, double> calibrated_mistag_os = calibrate(eta_os, avg_eta_os, p0_os, p1_os, delta_p0_os, delta_p1_os);
  std::pair<double, double> calibrated_mistag_ss = calibrate(eta_ss, avg_eta_ss, p0_ss, p1_ss, delta_p0_ss, delta_p1_ss);
  int combined_tag = combine_tags(calibrated_mistag_os, calibrated_mistag_ss, tag_os, tag_ss);
  double eta_combined_b = combine_mistags(calibrated_mistag_os, calibrated_mistag_ss, tag_os, tag_ss).first;
  double eta_combined_bbar = combine_mistags(calibrated_mistag_os, calibrated_mistag_ss, tag_os, tag_ss).second;

  if (coeff_type == kSin){
    return -1.0 * cp_coeff * ( combined_tag - production_asym * ( 1.0 - combined_tag * eta_combined_b + combined_tag * eta_combined_bbar ) - combined_tag * ( eta_combined_b + eta_combined_bbar ) );
  }
  else if (coeff_type == kCos){
    return +1.0 * cp_coeff * ( combined_tag - production_asym * ( 1.0 - combined_tag * eta_combined_b + combined_tag * eta_combined_bbar ) - combined_tag * ( eta_combined_b + eta_combined_bbar ) );
  }
  else if (coeff_type == kSinh){
    // TODO: Implement Sinh coefficient
    return cp_coeff;
  }
  else if (coeff_type == kCosh){
    return cp_coeff * ( 1.0 - combined_tag * production_asym * ( 1.0 - eta_combined_b - eta_combined_bbar ) - combined_tag * ( eta_combined_b - eta_combined_bbar ) );
  }
  else{
    std::cout << "ERROR\t" << "SuperCoefficient::evaluate(): No valid coefficient type!" << std::endl;
    abort();
  }
}


bool SuperCoefficient::isTagInRange(const RooCategoryProxy& tag, int tag_state, const char* rangeName) const 
{
  return dynamic_cast<const RooCategory&>(tag.arg()).isStateInRange(rangeName, tag.arg().lookupType(tag_state)->GetName());
}


} // namespace bdecay
} // namespace functions
} // namespace roofit
} // namespace doofit
