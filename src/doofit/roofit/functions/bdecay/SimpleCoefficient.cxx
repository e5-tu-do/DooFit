#include "Riostream.h" 

#include "SimpleCoefficient.h" 
#include "RooAbsReal.h" 
#include "RooAbsCategory.h" 
#include "RooCategory.h"
#include "RooCatType.h"
#include <math.h> 
#include "TMath.h" 

ClassImp(doofit::roofit::functions::bdecay::SimpleCoefficient) 

namespace doofit {
namespace roofit {
namespace functions {
namespace bdecay {

SimpleCoefficient::SimpleCoefficient(const std::string& name, 
                       RooAbsReal& _cp_coeff_,
                       CoeffType   _coeff_type_,
                       RooAbsCategory& _tag_,
                       RooAbsReal& _eta_,
                       RooAbsReal& _avg_eta_,
                       RooAbsReal& _p0_,
                       RooAbsReal& _p1_
                       ) :
  RooAbsReal(name.c_str(),name.c_str()),
  cp_coeff_("cp_coeff_","cp_coeff_",this,_cp_coeff_),
  coeff_type_(_coeff_type_),
  tag_("tag_","tag_",this,_tag_),
  eta_("eta_","eta_",this,_eta_),
  avg_eta_("avg_eta_","avg_eta_",this,_avg_eta_),
  p0_("p0_","p0_",this,_p0_),
  p1_("p1_","p1_",this,_p1_)
{ 
} 


SimpleCoefficient::SimpleCoefficient(const SimpleCoefficient& other, const char* name) :  
  RooAbsReal(other,name), 
  cp_coeff_("cp_coeff_",this,other.cp_coeff_),
  coeff_type_(other.coeff_type_),
  tag_("tag_",this,other.tag_),
  eta_("eta_",this,other.eta_),
  avg_eta_("avg_eta_",this,other.avg_eta_),
  p0_("p0_",this,other.p0_),
  p1_("p1_",this,other.p1_)
{ 
} 


Double_t SimpleCoefficient::evaluate() const 
{ 
  if (coeff_type_ == kSin){
    return -1.0 * cp_coeff_ * tag_ * ( 1.0 - 2.0 * ( p0_ + p1_ * ( eta_ - avg_eta_ ) ) );
  }
  else if (coeff_type_ == kCos){
    return +1.0 * cp_coeff_ * tag_ * ( 1.0 - 2.0 * ( p0_ + p1_ * ( eta_ - avg_eta_ ) ) );
  }
  else if (coeff_type_ == kSinh){
    // TODO: Implement Sinh coefficient if necessary
    return cp_coeff_;
  }
  else if (coeff_type_ == kCosh){
    // TODO: Implement Cosh coefficient if necessary
    return cp_coeff_;
  }
  else{
    std::cout << "ERROR\t" << "SuperCoefficient::evaluate(): No valid coefficient type!" << std::endl;
    abort();
  }
} 


Int_t SimpleCoefficient::getAnalyticalIntegral(RooArgSet& allVars, RooArgSet& analVars, const char* rangeName) const  
{ 
  // debug
  std::printf("CHECK: In %s line %u (%s): #Vars = %d : Range: %s : allVars = ", __func__, __LINE__, __FILE__, allVars.getSize(), rangeName);
  allVars.Print();

  if (matchArgs(allVars, analVars, tag_) && rangeName) return 2 ;
  if (matchArgs(allVars, analVars, tag_)) return 1 ;
  return 0 ;
}


Double_t SimpleCoefficient::analyticalIntegral(Int_t code, const char* rangeName) const  
{
  if (code == 1){
    if (coeff_type_ == kSin){
      return 0.;
    }
    else if (coeff_type_ == kCos){
      return 0.;
    }
    else if (coeff_type_ == kSinh){
      std::printf("WARNING: In %s line %u (%s) : sinh coefficient not yet implemented! \n", __func__, __LINE__, __FILE__);
      return 2.0 * cp_coeff_;
    }
    else if (coeff_type_ == kCosh){
      std::printf("WARNING: In %s line %u (%s) : cosh coefficient not yet implemented! \n", __func__, __LINE__, __FILE__);
      return 2.0 * cp_coeff_;
    }
    else{
      std::printf("ERROR: In %s line %u (%s) : No valid coefficent! \n", __func__, __LINE__, __FILE__);
      return 0;
      abort();
    }
  }
  else if (code == 2){
    // debug
    std::printf("CHECK: In %s line %u (%s): Range: %s : Code: %d \n", __func__, __LINE__, __FILE__, rangeName, code);
    if (coeff_type_ == kSin){
      double integral = 0;
      if (dynamic_cast<const RooCategory&>(tag_.arg()).isStateInRange(rangeName, tag_.arg().lookupType(+1)->GetName())){
        integral += -1.0 * cp_coeff_ * ( 1.0 - 2.0 * ( p0_ + p1_ * ( eta_ - avg_eta_ ) ) );
      }
      if (dynamic_cast<const RooCategory&>(tag_.arg()).isStateInRange(rangeName, tag_.arg().lookupType(-1)->GetName())){
       integral += +1.0 * cp_coeff_ * ( 1.0 - 2.0 * ( p0_ + p1_ * ( eta_ - avg_eta_ ) ) ); 
      }
      return integral;
    }
    else if (coeff_type_ == kCos){
      double integral = 0;
      if (dynamic_cast<const RooCategory&>(tag_.arg()).isStateInRange(rangeName, tag_.arg().lookupType(+1)->GetName())){
        integral += +1.0 * cp_coeff_ * ( 1.0 - 2.0 * ( p0_ + p1_ * ( eta_ - avg_eta_ ) ) );
      }
      if (dynamic_cast<const RooCategory&>(tag_.arg()).isStateInRange(rangeName, tag_.arg().lookupType(-1)->GetName())){
       integral += -1.0 * cp_coeff_ * ( 1.0 - 2.0 * ( p0_ + p1_ * ( eta_ - avg_eta_ ) ) ); 
      }
      return integral;
    }
    else if (coeff_type_ == kSinh){
      std::printf("WARNING: In %s line %u (%s) : sinh coefficient not yet implemented! \n", __func__, __LINE__, __FILE__);
      return 2.0 * cp_coeff_;
    }
    else if (coeff_type_ == kCosh){
      std::printf("WARNING: In %s line %u (%s) : cosh coefficient not yet implemented! \n", __func__, __LINE__, __FILE__);
      return 2.0 * cp_coeff_;
    }
    else{
      std::printf("ERROR: In %s line %u (%s) : No valid coefficent! \n", __func__, __LINE__, __FILE__);
      return 0;
      abort();
    }
  }
} 


} // namespace bdecay
} // namespace functions
} // namespace roofit
} // namespace doofit
