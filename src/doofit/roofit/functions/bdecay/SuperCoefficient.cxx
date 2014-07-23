#include "Riostream.h" 

#include "SuperCoefficient.h" 
#include "RooAbsReal.h" 
#include "RooAbsCategory.h" 
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
                       RooAbsReal& _production_asym_,
                       bool _calibrate_os_,
                       bool _calibrate_ss_) :
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
  calibrate_os_(_calibrate_os_),
  calibrate_ss_(_calibrate_ss_)//,
  // eta_cal_os_(eta_os_),
  // eta_cal_os_b_(eta_os_),
  // eta_cal_os_bbar_(eta_os_),
  // eta_cal_ss_(eta_ss_),
  // eta_cal_ss_b_(eta_ss_),
  // eta_cal_ss_bbar_(eta_ss_),
  // eta_combined_b_(0.5),
  // eta_combined_bbar_(0.5),
  // tag_combined_(1)
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
  production_asym_("production_asym_",this,other.production_asym_),
  calibrate_os_(other.calibrate_os_),
  calibrate_ss_(other.calibrate_ss_)//,
  // eta_cal_os_(other.eta_cal_os_),
  // eta_cal_os_b_(other.eta_cal_os_b_),
  // eta_cal_os_bbar_(other.eta_cal_os_bbar_),
  // eta_cal_ss_(other.eta_cal_ss_),
  // eta_cal_ss_b_(other.eta_cal_ss_b_),
  // eta_cal_ss_bbar_(other.eta_cal_ss_bbar_),
  // eta_combined_b_(other.eta_combined_b_),
  // eta_combined_bbar_(other.eta_combined_bbar_),
  // tag_combined_(other.tag_combined_)
{ 
} 

void SuperCoefficient::calibrate_os() const
{
  if (calibrate_os_){
    // calculate avg mistag
    double eta_cal_os_avg = p0_os_ + p1_os_ * ( eta_os_ - avg_eta_os_ );
    
    // if mistag is larger or equal 0.5 return 0.5
    if (eta_os_ >= 0.5){
      eta_cal_os_      = 0.5;
      eta_cal_os_b_    = 0.5;
      eta_cal_os_bbar_ = 0.5;
    }
    // if calibrated average mistag is larger or equal 0.5 return 0.5
    else if (eta_cal_os_avg >= 0.5){
      eta_cal_os_      = 0.5;
      eta_cal_os_b_    = 0.5;
      eta_cal_os_bbar_ = 0.5;
    }
    else{
      // calibrate mistag
      eta_cal_os_      = p0_os_ + p1_os_ * ( eta_os_ - avg_eta_os_ );
      eta_cal_os_b_    = p0_os_ + 0.5 * delta_p0_os_ + ( p1_os_ + 0.5 * delta_p1_os_ ) * ( eta_os_ - avg_eta_os_ );
      eta_cal_os_bbar_ = p0_os_ - 0.5 * delta_p0_os_ + ( p1_os_ - 0.5 * delta_p1_os_ ) * ( eta_os_ - avg_eta_os_ );
    }
  }
  else{
    eta_cal_os_      = eta_os_;
    eta_cal_os_b_    = eta_os_;
    eta_cal_os_bbar_ = eta_os_;
  }
}

void SuperCoefficient::calibrate_ss() const
{
  if (calibrate_ss_){
    // calculate avg mistag
    double eta_cal_ss_avg = p0_ss_ + p1_ss_ * ( eta_ss_ - avg_eta_ss_ );
    
    // if mistag is larger or equal 0.5 return 0.5
    if (eta_ss_ >= 0.5){
      eta_cal_ss_      = 0.5;
      eta_cal_ss_b_    = 0.5;
      eta_cal_ss_bbar_ = 0.5;
    }
    // if calibrated average mistag is larger or equal 0.5 return 0.5
    else if (eta_cal_ss_avg >= 0.5){
      eta_cal_ss_      = 0.5;
      eta_cal_ss_b_    = 0.5;
      eta_cal_ss_bbar_ = 0.5;
    }
    else{
      // calibrate mistag
      eta_cal_ss_      = p0_ss_ + p1_ss_ * ( eta_ss_ - avg_eta_ss_ );
      eta_cal_ss_b_    = p0_ss_ + 0.5 * delta_p0_ss_ + ( p1_ss_ + 0.5 * delta_p1_ss_ ) * ( eta_ss_ - avg_eta_ss_ );
      eta_cal_ss_bbar_ = p0_ss_ - 0.5 * delta_p0_ss_ + ( p1_ss_ - 0.5 * delta_p1_ss_ ) * ( eta_ss_ - avg_eta_ss_ );
    }
  }
  else{
    eta_cal_ss_      = eta_ss_;
    eta_cal_ss_b_    = eta_ss_;
    eta_cal_ss_bbar_ = eta_ss_;
  }
}

// mistag combination: the Grabalosa way 
// https://cds.cern.ch/record/1456804/files/CERN-THESIS-2012-075.pdf
// note that the probability e.g. to have a b-tagged meson corresponds to a Bbar-Meson, so tag=-1

void SuperCoefficient::combine_mistags() const
{
  // combine b mistag
  if ((eta_cal_os_b_ >= 0.5) && (eta_cal_ss_b_ >= 0.5)){
     eta_combined_b_ = 0.5;
  }
  else if (eta_cal_os_b_ >= 0.5){
     eta_combined_b_ = eta_cal_ss_b_;
  }
  else if (eta_cal_ss_b_ >= 0.5){
     eta_combined_b_ = eta_cal_os_b_;
  }
  else{
     // probability to have a b-tagged meson
     double p_b = ( ( ( 1. + tag_os_ ) / 2. ) - tag_os_ * ( 1. - eta_cal_os_b_ ) ) * ( ( ( 1. + tag_ss_ ) / 2. ) - tag_ss_ * ( 1. - eta_cal_ss_b_ ) );
     // probability to have a bbar-tagged meson
     double p_bbar = ( ( ( 1. - tag_os_ ) / 2. ) + tag_os_ * ( 1. - eta_cal_os_b_ ) ) * ( ( ( 1. - tag_ss_ ) / 2. ) + tag_ss_ * ( 1. - eta_cal_ss_b_ ) );

     // as the combined eta will be the one associated with the larger probability for a b / bbar quark:
     eta_combined_b_ = 1. - ( std::max(p_b, p_bbar) / (p_b + p_bbar) );
  }

  // combine b mistag
  if ((eta_cal_os_bbar_ >= 0.5) && (eta_cal_ss_bbar_ >= 0.5)){
     eta_combined_bbar_ = 0.5;
  }
  else if (eta_cal_os_bbar_ >= 0.5){
     eta_combined_bbar_ = eta_cal_ss_bbar_;
  }
  else if (eta_cal_ss_bbar_ >= 0.5){
     eta_combined_bbar_ = eta_cal_os_bbar_;
  }
  else{
     // probability to have a b-tagged meson
     double p_b = ( ( ( 1. + tag_os_ ) / 2. ) - tag_os_ * ( 1. - eta_cal_os_bbar_ ) ) * ( ( ( 1. + tag_ss_ ) / 2. ) - tag_ss_ * ( 1. - eta_cal_ss_bbar_ ) );
     // probability to have a bbar-tagged meson
     double p_bbar = ( ( ( 1. - tag_os_ ) / 2. ) + tag_os_ * ( 1. - eta_cal_os_bbar_ ) ) * ( ( ( 1. - tag_ss_ ) / 2. ) + tag_ss_ * ( 1. - eta_cal_ss_bbar_ ) );

     // as the combined eta will be the one associated with the larger probability for a b / bbar quark:
     eta_combined_bbar_ = 1. - ( std::max(p_b, p_bbar) / (p_b + p_bbar) );
  }
}

void SuperCoefficient::combine_tags() const
{
  if ((eta_cal_os_ >= 0.5) && (eta_cal_ss_ >= 0.5)){
    // if untagged set to 1
    tag_combined_ = 1;
  }
  else if (eta_cal_os_ >= 0.5) {
    // if os untagged, set to ss tag
    tag_combined_ = tag_ss_;
  }
  else if (eta_cal_ss_ >= 0.5){
    // if ss untagged, set to os tag
    tag_combined_ = tag_os_;
  }
  else{
    // probability to have a b-tagged meson
    double p_b = ( ( ( 1 + tag_os_ ) / 2 ) - tag_os_ * ( 1 - eta_cal_os_ ) ) * ( ( ( 1 + tag_ss_ ) / 2 ) - tag_ss_ * ( 1 - eta_cal_ss_ ) );
    // probability to have a bbar-tagged meson
    double p_bbar = ( ( ( 1 - tag_os_ ) / 2 ) + tag_os_ * ( 1 - eta_cal_os_ ) ) * ( ( ( 1 - tag_ss_ ) / 2 ) + tag_ss_ * ( 1 - eta_cal_ss_ ) );
    
    if (p_b > p_bbar){
      tag_combined_ = -1;
    }
    else{
      tag_combined_ = +1;
    }
  }
}

Double_t SuperCoefficient::evaluate() const 
{ 
  calibrate_os();
  calibrate_ss();
  combine_mistags();
  combine_tags();

  if (coeff_type_ == kSin){
    return -1.0 * cp_coeff_ * ( tag_combined_ - production_asym_ * ( 1.0 - tag_combined_ * eta_combined_b_ + tag_combined_ * eta_combined_bbar_ ) - tag_combined_ * ( eta_combined_b_ + eta_combined_bbar_ ) );
  }
  else if (coeff_type_ == kCos){
    return +1.0 * cp_coeff_ * ( tag_combined_ - production_asym_ * ( 1.0 - tag_combined_ * eta_combined_b_ + tag_combined_ * eta_combined_bbar_ ) - tag_combined_ * ( eta_combined_b_ + eta_combined_bbar_ ) );
  }
  else if (coeff_type_ == kSinh){
    // TODO: Implement Sinh coefficient
    return cp_coeff_;
  }
  else if (coeff_type_ == kCosh){
    return cp_coeff_ * ( 1.0 - tag_combined_ * production_asym_ * ( 1.0 - eta_combined_b_ - eta_combined_bbar_ ) - tag_combined_ * ( eta_combined_b_ - eta_combined_bbar_ ) );
    // return 1.0 - tag_combined_ * production_asym_ * ( 1.0 - eta_combined_b_ - eta_combined_bbar_ ) - tag_combined_ * ( eta_combined_b_ - eta_combined_bbar_ );
  }
  else{
    std::cout << "ERROR\t" << "SuperCoefficient::evaluate(): No valid coefficient type!" << std::endl;
    abort();
  }
} 

Int_t SuperCoefficient::getAnalyticalIntegral(RooArgSet& allVars, RooArgSet& analVars, const char* /*rangeName*/) const  
{ 
  // debug
  std::printf("CHECK: In %s line %u (%s): #Vars = %d : allVars = ", __func__, __LINE__, __FILE__, allVars.getSize());
  allVars.Print();

  // if (matchArgs(allVars,analVars,x)) return 1 ; 
  return 0 ; 
} 

Double_t SuperCoefficient::analyticalIntegral(Int_t code, const char* rangeName) const  
{ 
  // RETURN ANALYTICAL INTEGRAL DEFINED BY RETURN CODE ASSIGNED BY getAnalyticalIntegral
  // THE MEMBER FUNCTION x.min(rangeName) AND x.max(rangeName) WILL RETURN THE INTEGRATION
  // BOUNDARIES FOR EACH OBSERVABLE x

  // assert(code==1) ; 
  // return (x.max(rangeName)-x.min(rangeName)) ; 
  return 0 ; 
} 

} // namespace bdecay
} // namespace functions
} // namespace roofit
} // namespace doofit
