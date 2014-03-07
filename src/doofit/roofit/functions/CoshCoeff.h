/*****************************************************************************
 * Project: RooFit                                                           *
 *                                                                           *
  * This code was autogenerated by RooClassFactory                            * 
 *****************************************************************************/

#ifndef DOOFIT_ROOFIT_FUNCTIONS_COSHCOEFF
#define DOOFIT_ROOFIT_FUNCTIONS_COSHCOEFF

#include "RooAbsReal.h"
#include "RooRealProxy.h"
#include "RooCategoryProxy.h"
#include "RooAbsReal.h"
#include "RooAbsCategory.h"

namespace doofit {
namespace roofit {
namespace functions {

class CoshCoeff : public RooAbsReal {
public:
  CoshCoeff() {};
  
  CoshCoeff(const char *name, const char *title,
                    RooAbsReal& _par_tag_omega_Bd,
                    RooAbsReal& _par_tag_omega_Bdb,
                    RooAbsReal& _par_prod_asym,
                    RooAbsCategory& _cat_tag);
  
  CoshCoeff(const CoshCoeff& other, const char* name=0) ;
  virtual TObject* clone(const char* newname) const { return new CoshCoeff(*this,newname); }
  inline virtual ~CoshCoeff() {
    #ifdef FUNCTIONS_COUNT_CALLS
    std::cout << "CoshCoeff::~CoshCoeff(): # evaluate calls: " << num_calls_evaluate_ << std::endl;
    std::cout << "CoshCoeff::~CoshCoeff(): # integral calls: " << num_calls_integral_ << std::endl;
    #endif
  }

protected:

  RooRealProxy par_tag_omega_Bd ;
  RooRealProxy par_tag_omega_Bdb ;
  RooRealProxy par_prod_asym ;
  RooCategoryProxy cat_tag ;
  
  static long long num_calls_evaluate_;
  static long long num_calls_integral_;
  
  inline Double_t evaluate() const {
    #ifdef FUNCTIONS_COUNT_CALLS
    ++num_calls_evaluate_;
    #endif
    // ENTER EXPRESSION IN TERMS OF VARIABLE ARGUMENTS HERE
    //std::cout << 1.0 - cat_tag*(par_tag_omega_Bd - par_tag_omega_Bdb) - cat_tag*par_prod_asym*(1.0 - par_tag_omega_Bd - par_tag_omega_Bdb) << std::endl;
    return 1.0 - cat_tag*(par_tag_omega_Bd - par_tag_omega_Bdb) - cat_tag*par_prod_asym*(1.0 - par_tag_omega_Bd - par_tag_omega_Bdb);
  }

  virtual Int_t	getAnalyticalIntegral(RooArgSet& allVars, RooArgSet& analVars,
                                      const char* rangeName = 0) const;
  
  virtual Int_t	getAnalyticalIntegralWN(RooArgSet& allVars, RooArgSet& analVars, const RooArgSet* normSet, const char* rangeName = 0) const;
  
//  virtual Double_t analyticalIntegral(Int_t code, const char* rangeName = 0) const {
//    
//    if (1 == code) {
//      //std::cout << "CoshCoeff::analyticalIntegral(" << code << ", ...): Called." << std::endl;
//      return 0.0;
//    }
//    // must not get here
//    //assert(1 == 0);
//  }
  
private:

  ClassDef(CoshCoeff,1) // Your description goes here...
};
} // namespace functions
} // namespace roofit
} // namespace doofit

#endif