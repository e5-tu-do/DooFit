/***************************************************************************** 
 * Project: RooFit                                                           * 
 *                                                                           * 
 * This code was autogenerated by RooClassFactory                            * 
 *****************************************************************************/ 

// Your description goes here... 

#include "Riostream.h" 

#include "PolyTest.h" 
#include "RooAbsReal.h" 
#include "RooAbsCategory.h" 
#include <math.h> 
#include "TMath.h" 

ClassImp(doofit::roofit::functions::PolyTest) 

namespace doofit {
namespace roofit {
namespace functions {

 PolyTest::PolyTest(const std::string name, 
                        RooAbsReal& _x_,
                        RooAbsReal& _c0_,
                        RooAbsReal& _c1_,
                        RooAbsReal& _c2_,
                        RooAbsReal& _c3_,
                        RooAbsReal& _c4_,
                        RooAbsReal& _c5_,
                        RooAbsReal& _c6_,
                        RooAbsReal& _c7_) :
   RooAbsPdf(name.c_str(),name.c_str()), 
   x_("x_","x_",this,_x_),
   c0_("c0_","c0_",this,_c0_),
   c1_("c1_","c1_",this,_c1_),
   c2_("c2_","c2_",this,_c2_),
   c3_("c3_","c3_",this,_c3_),
   c4_("c4_","c4_",this,_c4_),
   c5_("c5_","c5_",this,_c5_),
   c6_("c6_","c6_",this,_c6_),
   c7_("c7_","c7_",this,_c7_)
 { 
 } 


 PolyTest::PolyTest(const PolyTest& other, const char* name) :  
   RooAbsPdf(other,name), 
   x_("x_",this,other.x_),
   c0_("c0_",this,other.c0_),
   c1_("c1_",this,other.c1_),
   c2_("c2_",this,other.c2_),
   c3_("c3_",this,other.c3_),
   c4_("c4_",this,other.c4_),
   c5_("c5_",this,other.c5_),
   c6_("c6_",this,other.c6_),
   c7_("c7_",this,other.c7_)
 { 
 } 



 Double_t PolyTest::evaluate() const 
 { 
   // ENTER EXPRESSION IN TERMS OF VARIABLE ARGUMENTS HERE 
   // return c0_ + c1_ * TMath::Power(x_, 1) + c2_ *  TMath::Power(x_, 2) + c3_ * TMath::Power(x_, 3) + c4_ * TMath::Power(x_, 4) + c5_ * TMath::Power(x_, 5) + c6_ * TMath::Power(x_, 6); 
   double val = 0;
   // first range: 0.00-0.15
   if ((x_>0.00) && (x_<=0.15)){
      val = TMath::Power(x_, 4); 
   }
   else if ((x_>0.15) && (x_<=0.20)){
      val = c0_ * TMath::Power(x_, 3) + TMath::Power(x_, 4);
   }
   else if ((x_>0.20) && (x_<=0.29)){
      val = c1_ + TMath::Power(x_, 1);
   }
   else if ((x_>0.29) && (x_<=0.45)){
      val = c2_ * TMath::Power(x_, 4) + c3_ * TMath::Power(x_, 5) - TMath::Power(x_, 6); 
   }
   else if ((x_>0.45) && (x_<=0.50)){
      val = c4_ * TMath::Power(x_, 3) + c5_ * TMath::Power(x_, 4) + c6_ * TMath::Power(x_, 5) + c7_ * TMath::Power(x_, 6); 
   }
   return val;
 } 



 Int_t PolyTest::getAnalyticalIntegral(RooArgSet& allVars, RooArgSet& analVars, const char* /*rangeName*/) const  
 { 
   // LIST HERE OVER WHICH VARIABLES ANALYTICAL INTEGRATION IS SUPPORTED, 
   // ASSIGN A NUMERIC CODE FOR EACH SUPPORTED (SET OF) PARAMETERS 
   // THE EXAMPLE BELOW ASSIGNS CODE 1 TO INTEGRATION OVER VARIABLE X
   // YOU CAN ALSO IMPLEMENT MORE THAN ONE ANALYTICAL INTEGRAL BY REPEATING THE matchArgs 
   // EXPRESSION MULTIPLE TIMES

   // if (matchArgs(allVars,analVars,x)) return 1 ; 
   return 0 ; 
 } 



 Double_t PolyTest::analyticalIntegral(Int_t code, const char* rangeName) const  
 { 
   // RETURN ANALYTICAL INTEGRAL DEFINED BY RETURN CODE ASSIGNED BY getAnalyticalIntegral
   // THE MEMBER FUNCTION x.min(rangeName) AND x.max(rangeName) WILL RETURN THE INTEGRATION
   // BOUNDARIES FOR EACH OBSERVABLE x

   // assert(code==1) ; 
   // return (x.max(rangeName)-x.min(rangeName)) ; 
   return 0 ; 
 } 

}
}
}

