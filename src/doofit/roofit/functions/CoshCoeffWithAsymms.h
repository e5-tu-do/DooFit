/*****************************************************************************
 * Project: RooFit                                                           *
 *                                                                           *
  * This code was autogenerated by RooClassFactory                            * 
 *****************************************************************************/

#ifndef DOOFIT_ROOFIT_FUNCTIONS_COSHCOEFFWITHASYMMS
#define DOOFIT_ROOFIT_FUNCTIONS_COSHCOEFFWITHASYMMS

#include "RooAbsReal.h"
#include "RooRealProxy.h"
#include "RooCategoryProxy.h"
#include "RooAbsReal.h"
#include "RooAbsCategory.h"

namespace doofit {
namespace roofit {
namespace functions {

class CoshCoeffWithAsymms : public RooAbsReal {
public:
  CoshCoeffWithAsymms();
  
  CoshCoeffWithAsymms(const char *name, const char *title,
                    RooAbsReal& _par_tag_omega,
                    RooAbsReal& _par_tag_meaneta,
                    RooAbsReal& _par_tag_delta_p0,
                    RooAbsReal& _par_tag_delta_p1,
                    RooAbsReal& _par_tag_eta,
                    RooAbsReal& _par_prod_asym,
                    RooAbsCategory& _cat_tag);
  
  CoshCoeffWithAsymms(const CoshCoeffWithAsymms& other, const char* name=0) ;
  virtual TObject* clone(const char* newname) const { return new CoshCoeffWithAsymms(*this,newname); }
  inline virtual ~CoshCoeffWithAsymms() {
    #ifdef FUNCTIONS_COUNT_CALLS
    std::cout << "CoshCoeffWithAsymms::~CoshCoeffWithAsymms(): # evaluate calls: " << num_calls_evaluate_ << std::endl;
    std::cout << "CoshCoeffWithAsymms::~CoshCoeffWithAsymms(): # integral calls: " << num_calls_integral_ << std::endl;
    #endif
  }

protected:

  RooRealProxy par_tag_omega ;
  RooRealProxy par_tag_meaneta ;
  RooRealProxy par_tag_delta_p0 ;
  RooRealProxy par_tag_delta_p1 ;
  RooRealProxy par_tag_eta ;
  RooRealProxy par_prod_asym ;
  RooCategoryProxy cat_tag ;
  
  static long long num_calls_evaluate_;
  static long long num_calls_integral_;
  
  inline Double_t evaluate() const {
    #ifdef FUNCTIONS_COUNT_CALLS
    ++num_calls_evaluate_;
    #endif
    // ENTER EXPRESSION IN TERMS OF VARIABLE ARGUMENTS HERE
    //std::cout << 1.0 - cat_tag*(par_tag_delta_p0+par_tag_delta_p1*(par_tag_eta-par_tag_meaneta)) - cat_tag*par_prod_asym*(1.0 - 2.0*par_tag_omega) << std::endl;
    
    return 1.0 - cat_tag*(par_tag_delta_p0+par_tag_delta_p1*(par_tag_eta-par_tag_meaneta)) - cat_tag*par_prod_asym*(1.0 - 2.0*par_tag_omega);
  }

  virtual Int_t	getAnalyticalIntegral(RooArgSet& allVars, RooArgSet& analVars,
                                      const char* rangeName = 0) const;
  
  virtual Int_t	getAnalyticalIntegralWN(RooArgSet& allVars, RooArgSet& analVars, const RooArgSet* normSet, const char* rangeName = 0) const;
  
//  virtual Double_t analyticalIntegral(Int_t code, const char* rangeName = 0) const {
//    
//    if (1 == code) {
//      //std::cout << "CoshCoeffWithAsymms::analyticalIntegral(" << code << ", ...): Called." << std::endl;
//      return 0.0;
//    }
//    // must not get here
//    //assert(1 == 0);
//  }
  
private:

  ClassDef(CoshCoeffWithAsymms,1) // Your description goes here...
};
} // namespace functions
} // namespace roofit
} // namespace doofit

#endif
