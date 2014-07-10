#include "Riostream.h" 

#include "Coefficient.h" 
#include "RooAbsReal.h" 
#include "RooAbsCategory.h" 
#include <math.h> 
#include "TMath.h" 

ClassImp(doofit::roofit::functions::bdecay::Coefficient) 

namespace doofit {
namespace roofit {
namespace functions {
namespace bdecay {

Coefficient::Coefficient(const std::string& name, 
                       RooAbsReal& _cp_coeff_,
                       CoeffType&  _coeff_type_,
                       RooAbsCategory& _tag_,
                       RooAbsReal& _mistag_b_,
                       RooAbsReal& _mistag_bbar_,
                       RooAbsReal& _production_asym_
                       ) :
  RooAbsPdf(name.c_str(),name.c_str()), 
  cp_coeff_("cp_coeff_","cp_coeff_",this,_cp_coeff_),
  coeff_type_(_coeff_type_),
  tag_("tag_","tag_",this,_tag_),
  mistag_b_("mistag_b_","mistag_b_",this,_mistag_b_),
  mistag_bbar_("mistag_bbar_","mistag_bbar_",this,_mistag_bbar_),
  production_asym_("production_asym_","production_asym_",this,_production_asym_)
{ 
} 


Coefficient::Coefficient(const Coefficient& other, const char* name) :  
  RooAbsPdf(other,name), 
  cp_coeff_("cp_coeff_",this,other.cp_coeff_),
  coeff_type_(other.coeff_type_),
  tag_("tag_",this,other.tag_),
  mistag_b_("mistag_b_",this,other.mistag_b_),
  mistag_bbar_("mistag_bbar_",this,other.mistag_bbar_),
  production_asym_("production_asym_",this,other.production_asym_)
{ 
} 

Double_t Coefficient::evaluate() const 
{ 
  switch (coeff_type_){
    case 3: // Sin coefficient, aka f3 in RooBDecay notation
      std::cout << "CoeffType: Sin" << std::endl;
      
      return -1.0 * ( tag_ - production_asym_ * ( 1.0 - tag_ * mistag_b_ + tag_ * mistag_bbar_ ) - tag_ * ( mistag_b_ + mistag_bbar_ ) ) * cp_coeff_;
      
      break;
    case 2: // Cos coefficient aka f2
      std::cout << "CoeffType: Cos" << std::endl;
      
      return +1.0 * ( tag_ - production_asym_ * ( 1.0 - tag_ * mistag_b_ + tag_ * mistag_bbar_ ) - tag_ * ( mistag_b_ + mistag_bbar_ ) ) * cp_coeff_;

      break;
    case 1: // Sinh coefficient aka f1
      std::cout << "CoeffType: Sinh" << std::endl;

      // TODO: Implement correct Sinh coefficient
      return cp_coeff_;
      
      break;
    case 0: // Cosh coefficient aka f0
      std::cout << "CoeffType: Cosh" << std::endl;

      return ( 1.0 + tag_ * production_asym_ * ( 1.0 - mistag_b_ - mistag_bbar_ ) + tag_ * ( mistag_b_ - mistag_bbar_ ) ) * cp_coeff_;

      break;
    default:
      std::cout << "ERROR in doofit::roofit::functions::bdecay::Coefficient! No valid coefficient type!" << std::endl;
      abort();
  }
} 



Int_t Coefficient::getAnalyticalIntegral(RooArgSet& allVars, RooArgSet& analVars, const char* /*rangeName*/) const  
{
  return 0 ; 
} 



Double_t Coefficient::analyticalIntegral(Int_t code, const char* rangeName) const  
{ 
  return 0 ; 
} 

} // namespace bdecay
} // namespace functions
} // namespace roofit
} // namespace doofit
