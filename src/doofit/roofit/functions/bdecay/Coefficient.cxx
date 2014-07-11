#include "Riostream.h" 

#include "Coefficient.h" 
#include "RooAbsReal.h" 
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
                       RooAbsReal& _tag_,
                       RooAbsReal& _mistag_b_,
                       RooAbsReal& _mistag_bbar_,
                       RooAbsReal& _production_asym_
                       ) :
  RooAbsReal(name.c_str(),name.c_str()), 
  cp_coeff_("cp_coeff_","cp_coeff_",this,_cp_coeff_),
  coeff_type_(_coeff_type_),
  tag_("tag_","tag_",this,_tag_),
  mistag_b_("mistag_b_","mistag_b_",this,_mistag_b_),
  mistag_bbar_("mistag_bbar_","mistag_bbar_",this,_mistag_bbar_),
  production_asym_("production_asym_","production_asym_",this,_production_asym_)
{ 
} 


Coefficient::Coefficient(const Coefficient& other, const char* name) :  
  RooAbsReal(other,name), 
  cp_coeff_("cp_coeff_",this,other.cp_coeff_),
  coeff_type_(other.coeff_type_),
  tag_("tag_",this,other.tag_),
  mistag_b_("mistag_b_",this,other.mistag_b_),
  mistag_bbar_("mistag_bbar_",this,other.mistag_bbar_),
  production_asym_("production_asym_",this,other.production_asym_)
{ 
} 

inline Double_t Coefficient::evaluate() const 
{ 
  if (coeff_type_ == kSin){
    return cp_coeff_ * ( -1.0 * tag_ + production_asym_ * ( 1.0 - tag_ * mistag_b_ + tag_ * mistag_bbar_ ) + tag_ * ( mistag_b_ + mistag_bbar_ ) );
  }
  else if (coeff_type_ == kCos){
    return cp_coeff_ * ( +1.0 * tag_ - production_asym_ * ( 1.0 - tag_ * mistag_b_ + tag_ * mistag_bbar_ ) - tag_ * ( mistag_b_ + mistag_bbar_ ) );
  }
  else if (coeff_type_ == kSinh){
    // TODO: Implement Sinh coefficient
    return cp_coeff_;
  }
  else if (coeff_type_ == kCosh){
    return cp_coeff_ * ( 1.0 + tag_ * production_asym_ * ( 1.0 - mistag_b_ - mistag_bbar_ ) + tag_ * ( mistag_b_ - mistag_bbar_ ) );
  }
  else{
    std::cout << "ERROR\t" << "Coefficient::evaluate(): No valid coefficient type!" << std::endl;
    abort();
  }
} 

} // namespace bdecay
} // namespace functions
} // namespace roofit
} // namespace doofit
