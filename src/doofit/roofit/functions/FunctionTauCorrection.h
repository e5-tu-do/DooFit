/*****************************************************************************
 * Project: RooFit                                                           *
 *                                                                           *
  * This code was autogenerated by RooClassFactory                            * 
 *****************************************************************************/

#ifndef DOOFIT_ROOFIT_FUNCTIONS_FUNCTIONTAUCORRECTION
#define DOOFIT_ROOFIT_FUNCTIONS_FUNCTIONTAUCORRECTION

#include "RooAbsReal.h"
#include "RooRealProxy.h"
#include "RooCategoryProxy.h"
#include "RooAbsReal.h"
#include "RooAbsCategory.h"

//#define FUNCTIONS_COUNT_CALLS

namespace doofit {
namespace roofit {
namespace functions {

class FunctionTauCorrection : public RooAbsReal {
public:
  FunctionTauCorrection() {} ;
  FunctionTauCorrection(const char *name, const char *title,
	      RooAbsReal& _par_tau,
	      RooAbsReal& _par_beta);
  FunctionTauCorrection(const FunctionTauCorrection& other, const char* name=0) ;
  virtual TObject* clone(const char* newname) const { return new FunctionTauCorrection(*this,newname); }
  inline virtual ~FunctionTauCorrection() {
    #ifdef FUNCTIONS_COUNT_CALLS
    std::cout << "FunctionTauCorrection::~FunctionTauCorrection(): # evaluate calls: " << num_calls_evaluate_ << std::endl;
    std::cout << "FunctionTauCorrection::~FunctionTauCorrection(): # integral calls: " << num_calls_integral_ << std::endl;
    #endif
  }

protected:

  RooRealProxy par_tau ;
  RooRealProxy par_beta ;

  static long long num_calls_evaluate_;
  static long long num_calls_integral_;
  
  inline Double_t evaluate() const {
    #ifdef FUNCTIONS_COUNT_CALLS
    ++num_calls_evaluate_;
    #endif
    // ENTER EXPRESSION IN TERMS OF VARIABLE ARGUMENTS HERE
    
    //std::cout << par_tau/(1+par_beta*par_tau) << std::endl;
    
    return par_tau/(1.0+par_beta*par_tau);
  }
  
  virtual Int_t	getAnalyticalIntegral(RooArgSet& allVars, RooArgSet& analVars,
                                      const char* rangeName = 0) const;
  
  virtual Int_t	getAnalyticalIntegralWN(RooArgSet& allVars, RooArgSet& analVars, const RooArgSet* normSet, const char* rangeName = 0) const;


private:

  ClassDef(FunctionTauCorrection,1) // Your description goes here...
};
} // namespace functions
} // namespace roofit
} // namespace doofit

#endif