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
    return -1.0 * cp_coeff_ * ( tag_ - production_asym_ * ( 1.0 - tag_ * mistag_b_ + tag_ * mistag_bbar_ ) - tag_ * ( mistag_b_ + mistag_bbar_ ) );
  }
  else if (coeff_type_ == kCos){
    return +1.0 * cp_coeff_ * ( tag_ - production_asym_ * ( 1.0 - tag_ * mistag_b_ + tag_ * mistag_bbar_ ) - tag_ * ( mistag_b_ + mistag_bbar_ ) );
  }
  else if (coeff_type_ == kSinh){
    // TODO: Implement Sinh coefficient
    return cp_coeff_;
  }
  else if (coeff_type_ == kCosh){
    return cp_coeff_ * ( 1.0 - tag_ * production_asym_ * ( 1.0 - mistag_b_ - mistag_bbar_ ) - tag_ * ( mistag_b_ - mistag_bbar_ ) );
    // return 1.0 - tag_ * production_asym_ * ( 1.0 - mistag_b_ - mistag_bbar_ ) - tag_ * ( mistag_b_ - mistag_bbar_ );
  }
  else{
    std::cout << "ERROR\t" << "Coefficient::evaluate(): No valid coefficient type!" << std::endl;
    abort();
  }
}

Int_t Coefficient::getAnalyticalIntegralWN(RooArgSet& allVars, RooArgSet& analVars, const RooArgSet* normSet, const char* rangeName) const{
  std::printf("CHECK: In %s line %u (%s): #Vars = %d : allVars = ", __func__, __LINE__, __FILE__, allVars.getSize());
  allVars.Print();

  return 0;
}

Int_t Coefficient::getAnalyticalIntegral(RooArgSet& allVars, RooArgSet& analVars, const char* rangeName) const{
  // WARNING: works only if untagged events hold a tag state of ±1
  
  // return 1: integration over one tag state 
  // return 2: integration over two tag states
  
  // Now we have to handle the different possibilities:
  // 1.) a uncalibrated + uncombined tag is used (single RooRealVar)
  // 2.) a calibrated + uncombined tag ist used (single RooAbsReal)
  // 3.) a calibrated + combined tag is used (two RooAbsReals)
  // since we cannot access the observables, we have to trust that only
  // two possible integrals are requested, namely the integral over
  // a single tag state or the integral over two tag states.
  // For all other cases this implementation fails.


  // debug
  // std::printf("CHECK: In %s line %u (%s): #Vars = %d : allVars = ", __func__, __LINE__, __FILE__, allVars.getSize());
  // allVars.Print();

  if (allVars.getSize() == 0){
    std::printf("ERROR: In %s line %u (%s): CASE 0 : allVars = ", __func__, __LINE__, __FILE__);
    allVars.Print();
    return 0;
  }
  else if (allVars.getSize() == 1){
    // case 1. and 2.: only one tag
    return 1;
  }
  else if (allVars.getSize() == 2){
    // case 3.: integration over two tag states
    return 2;
  }
  else{
    std::printf("ERROR: In %s line %u (%s): CASE ELSE : allVars = ", __func__, __LINE__, __FILE__);
    allVars.Print();
    return 0;
  }
  // return 0;
}

Double_t Coefficient::analyticalIntegral(Int_t code, const char* rangeName) const{
  if (!(code == 1 || code == 2)){
    std::printf("ERROR: In %s line %u (%s) : code is not 1 or 2 \n", __func__, __LINE__, __FILE__);
    return 0;
    abort();
  }
  if (coeff_type_ == kSin){
    return +2.0 * production_asym_ * cp_coeff_ * code;
  }
  else if (coeff_type_ == kCos){
    return -2.0 * production_asym_ * cp_coeff_ * code;
  }
  else if (coeff_type_ == kSinh){
    std::printf("WARNING: In %s line %u (%s) : sinh coefficient not yet implemented! \n", __func__, __LINE__, __FILE__);
    return 1;
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


} // namespace bdecay
} // namespace functions
} // namespace roofit
} // namespace doofit
