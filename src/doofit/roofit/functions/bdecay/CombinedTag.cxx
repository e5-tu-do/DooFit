#include "Riostream.h" 

#include "CombinedTag.h" 
#include "RooAbsReal.h" 
#include "RooAbsCategory.h" 
#include <math.h> 
#include "TMath.h" 

ClassImp(doofit::roofit::functions::bdecay::CombinedTag) 

namespace doofit {
namespace roofit {
namespace functions {
namespace bdecay {

CombinedTag::CombinedTag(const std::string& name, 
                       RooAbsReal& _eta_one_,
                       RooAbsReal& _eta_two_,
                       RooAbsCategory& _tag_one_,
                       RooAbsCategory& _tag_two_) :
  RooAbsPdf(name.c_str(),name.c_str()), 
  eta_one_("eta_one_","eta_one_",this,_eta_one_),
  eta_two_("eta_two_","eta_two_",this,_eta_two_),
  tag_one_("tag_one_","tag_one_",this,_tag_one_),
  tag_two_("tag_two_","tag_two_",this,_tag_two_)
{ 
} 


CombinedTag::CombinedTag(const CombinedTag& other, const char* name) :  
  RooAbsPdf(other,name), 
  eta_one_("eta_one_",this,other.eta_one_),
  eta_two_("eta_two_",this,other.eta_two_),
  tag_one_("tag_one_",this,other.tag_one_),
  tag_two_("tag_two_",this,other.tag_two_)
{ 
} 


Double_t CombinedTag::evaluate() const 
{ 
  // tag combination: the Grabalosa way 
  // https://cds.cern.ch/record/1456804/files/CERN-THESIS-2012-075.pdf
  // note that the probability e.g. to have a b-tagged meson corresponds to a Bbar-Meson, so tag=-1

  if ((eta_one_ >= 0.5) && (eta_two_ >= 0.5)){
     return 1;
  }
  if (eta_one_ >= 0.5) {
     return tag_two_;
  }
  if (eta_two_ >= 0.5){
     return tag_one_;
  }
  else{
    // probability to have a b-tagged meson
    double p_b = ( ( ( 1 + tag_one_ ) / 2 ) - tag_one_ * ( 1 - eta_one_ ) ) * ( ( ( 1 + tag_two_ ) / 2 ) - tag_two_ * ( 1 - eta_two_ ) );
    // probability to have a bbar-tagged meson
    double p_bbar = ( ( ( 1 - tag_one_ ) / 2 ) + tag_one_ * ( 1 - eta_one_ ) ) * ( ( ( 1 - tag_two_ ) / 2 ) + tag_two_ * ( 1 - eta_two_ ) );
    
    if (p_b > p_bbar){
      return -1;
    }
    else{
      return +1;
    }
  }
} 



Int_t CombinedTag::getAnalyticalIntegral(RooArgSet& allVars, RooArgSet& analVars, const char* /*rangeName*/) const  
{ 
  return 0 ; 
} 



Double_t CombinedTag::analyticalIntegral(Int_t code, const char* rangeName) const  
{ 
  return 0 ; 
} 

} // namespace bdecay
} // namespace functions
} // namespace roofit
} // namespace doofit
