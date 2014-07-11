#include "Riostream.h" 

#include "CalibratedMistag.h" 
#include "RooAbsReal.h" 
#include "RooAbsCategory.h" 
#include <math.h> 
#include "TMath.h" 

ClassImp(doofit::roofit::functions::bdecay::CalibratedMistag)

namespace doofit {
namespace roofit {
namespace functions {
namespace bdecay {

CalibratedMistag::CalibratedMistag(const std::string& name, 
                       RooAbsReal& _eta_,
                       RooAbsReal& _avg_eta_,
                       RooAbsReal& _p0_,
                       RooAbsReal& _p1_,
                       RooAbsReal& _delta_p0_,
                       RooAbsReal& _delta_p1_,
                       RooAbsCategory& _tag_,
                       const MistagType& _mistag_type_) :
  RooAbsReal(name.c_str(),name.c_str()), 
  eta_("eta_","eta_",this,_eta_),
  avg_eta_("avg_eta_","avg_eta_",this,_avg_eta_),
  p0_("p0_","p0_",this,_p0_),
  p1_("p1_","p1_",this,_p1_),
  delta_p0_("delta_p0_","delta_p0_",this,_delta_p0_),
  delta_p1_("delta_p1_","delta_p1_",this,_delta_p1_),
  tag_("tag_","tag_",this,_tag_),
  mistag_type_(_mistag_type_)
{ 
} 


CalibratedMistag::CalibratedMistag(const CalibratedMistag& other, const char* name) :  
  RooAbsReal(other,name), 
  eta_("eta_",this,other.eta_),
  avg_eta_("avg_eta_",this,other.avg_eta_),
  p0_("p0_",this,other.p0_),
  p1_("p1_",this,other.p1_),
  delta_p0_("delta_p0_",this,other.delta_p0_),
  delta_p1_("delta_p1_",this,other.delta_p1_),
  tag_("tag_",this,other.tag_),
  mistag_type_(other.mistag_type_)
{ 
} 



Double_t CalibratedMistag::evaluate() const 
{ 
  // if mistag is larger or equal 0.5 return 0.5
  if (eta_ >= 0.5){
    return 0.5;
  }
  else{
    // calibrate mistag
    double mistag = p0_ + ( mistag_type_ * 0.5 * delta_p0_ ) + ( p1_ + ( mistag_type_ * 0.5 * delta_p1_ ) ) * ( eta_ - avg_eta_ );
    ///// debug
    // double xcheck_mistag = p0_ - ( mistag_type_ * 0.5 * delta_p0_ ) + ( p1_ - ( mistag_type_ * 0.5 * delta_p1_ ) ) * ( eta_ - avg_eta_ );
    // double xcheck2_mistag = p0_ + p1_ * ( eta_ - avg_eta_ );
    // if (((delta_p0_ != 0) || (delta_p1_ != 0)) && ((mistag >= 0.5) || (xcheck_mistag >= 0.5))) std::cout << "WARNING\t" << "Mistag larger 0.5" << "\t eta: " << eta_ << " : " << xcheck2_mistag << " : " << mistag << " " << xcheck_mistag << std::endl;
    ///// debug
    if (mistag >= 0.5){
      // if calibrated mistag is larger or equal 0.5 return 0.5
      return 0.5;
    }
    else{
      // else return calibrated mistag
      return mistag;
    }
  }
} 



Int_t CalibratedMistag::getAnalyticalIntegral(RooArgSet& allVars, RooArgSet& analVars, const char* /*rangeName*/) const  
{ 
  return 0 ; 
} 



Double_t CalibratedMistag::analyticalIntegral(Int_t code, const char* rangeName) const  
{ 
  return 0 ; 
} 

} // namespace bdecay
} // namespace functions
} // namespace roofit
} // namespace doofit

