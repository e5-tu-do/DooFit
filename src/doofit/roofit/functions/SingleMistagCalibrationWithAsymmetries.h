/*****************************************************************************
 * Project: RooFit                                                           *
 *                                                                           *
  * This code was autogenerated by RooClassFactory                            * 
 *****************************************************************************/

#ifndef DOOFIT_ROOFIT_FUNCTIONS_SINGLEMISTAGCALIBRATIONWITHASYMMETRIES
#define DOOFIT_ROOFIT_FUNCTIONS_SINGLEMISTAGCALIBRATIONWITHASYMMETRIES

#include "RooAbsReal.h"
#include "RooRealProxy.h"
#include "RooCategoryProxy.h"
#include "RooAbsReal.h"
#include "RooAbsCategory.h"

namespace doofit {
namespace roofit {
namespace functions {

class SingleMistagCalibrationWithAsymmetries : public RooAbsReal {
public:
  enum TagType {
    kBdType  = +1,
    kBdbType = -1
  };

  SingleMistagCalibrationWithAsymmetries();
    
  SingleMistagCalibrationWithAsymmetries(const char *name, const char *title,
                                         RooAbsReal& _par_tag_eta,
                                         RooAbsReal& _par_tag_p0,
                                         RooAbsReal& _par_tag_p1,
                                         RooAbsReal& _par_tag_delta_p0,
                                         RooAbsReal& _par_tag_delta_p1,
                                         RooAbsReal& _par_tag_meaneta,
                                         TagType _type_tag);
  
  SingleMistagCalibrationWithAsymmetries(const SingleMistagCalibrationWithAsymmetries& other, const char* name=0) ;
  virtual TObject* clone(const char* newname) const { return new SingleMistagCalibrationWithAsymmetries(*this,newname); }
  inline virtual ~SingleMistagCalibrationWithAsymmetries() {
    #ifdef FUNCTIONS_COUNT_CALLS
    std::cout << "SingleMistagCalibrationWithAsymmetries::~SingleMistagCalibrationWithAsymmetries(): # evaluate calls: " << num_calls_evaluate_ << std::endl;
    std::cout << "SingleMistagCalibrationWithAsymmetries::~SingleMistagCalibrationWithAsymmetries(): # integral calls: " << num_calls_integral_ << std::endl;
    #endif
  }

protected:

  RooRealProxy par_tag_p0 ;
  RooRealProxy par_tag_p1 ;
  RooRealProxy par_tag_meaneta ;
  RooRealProxy par_tag_eta ;
  RooRealProxy par_tag_delta_p0 ;
  RooRealProxy par_tag_delta_p1 ;
  
  const TagType type_tag ;
  
  static long long num_calls_evaluate_;
  static long long num_calls_integral_;
  
  inline Double_t evaluate() const {
    #ifdef FUNCTIONS_COUNT_CALLS
    ++num_calls_evaluate_;
    #endif
    // ENTER EXPRESSION IN TERMS OF VARIABLE ARGUMENTS HERE
    
    //std::cout <<  << std::endl;
    
    return par_tag_p0 + 0.5*type_tag*par_tag_delta_p0 + (par_tag_p1 + 0.5*type_tag*par_tag_delta_p1) * (par_tag_eta - par_tag_meaneta);
  }

  virtual Int_t	getAnalyticalIntegral(RooArgSet& allVars, RooArgSet& analVars,
                                      const char* rangeName = 0) const;
  
  virtual Int_t	getAnalyticalIntegralWN(RooArgSet& allVars, RooArgSet& analVars, const RooArgSet* normSet, const char* rangeName = 0) const;
  
  virtual Double_t analyticalIntegral(Int_t code, const char* rangeName = 0) const {
    
    if (1 == code) {
      //std::cout << "SingleMistagCalibrationWithAsymmetries::analyticalIntegral(" << code << ", ...): Called." << std::endl;
      return 0.0;
    }
    // must not get here
    //assert(1 == 0);
  }
  
private:

  ClassDef(SingleMistagCalibrationWithAsymmetries,1) // Your description goes here...
};
} // namespace functions
} // namespace roofit
} // namespace doofit

#endif