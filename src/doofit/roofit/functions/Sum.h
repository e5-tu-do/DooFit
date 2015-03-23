#ifndef DOOFIT_ROOFIT_FUNCTIONS_SUM
#define DOOFIT_ROOFIT_FUNCTIONS_SUM

// from STL
#include <string>

#include "RooAbsReal.h"
#include "RooRealProxy.h"
#include "RooCategoryProxy.h"
#include "RooAbsReal.h"
#include "RooAbsCategory.h"

namespace doofit {
namespace roofit {
namespace functions {

class Sum : public RooAbsReal {
public:
  Sum() {};
  Sum(std::string name,
                  RooAbsReal& _par_addend1,
                  RooAbsReal& _par_addend2);
  
  Sum(const Sum& other, const char* name=0) ;
  virtual TObject* clone(const char* newname) const { return new Sum(*this,newname); }
  inline virtual ~Sum() {
    // std::cout << "Sum::~Sum(): num calls " << num_calls_ << std::endl;
  }
  
protected:

  RooRealProxy par_addend1 ;
  RooRealProxy par_addend2 ;
  
  // mutable long long num_calls_;
  
  virtual Int_t	getAnalyticalIntegral(RooArgSet& allVars, RooArgSet& analVars,
                                      const char* rangeName = 0) const;
  
  inline Double_t evaluate() const {
    // ENTER EXPRESSION IN TERMS OF VARIABLE ARGUMENTS HERE
    //++num_calls_;
    //std::cout << par_addend1+par_addend2 << std::endl;
    return par_addend1+par_addend2;
  }
  
private:

  ClassDef(Sum,1)
};
} // namespace functions
} // namespace roofit
} // namespace doofit

#endif
