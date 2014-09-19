/*****************************************************************************
 * Project: RooFit                                                           *
 *                                                                           *
  * This code was autogenerated by RooClassFactory                            * 
 *****************************************************************************/

#ifndef DOOFIT_ROOFIT_FUNCTIONS_PRODUCT
#define DOOFIT_ROOFIT_FUNCTIONS_PRODUCT

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

class Product : public RooAbsReal {
public:
  Product() {};
  Product(std::string name,
                  RooAbsReal& _par_factor1,
                  RooAbsReal& _par_factor2);
  
  Product(const Product& other, const char* name=0) ;
  virtual TObject* clone(const char* newname) const { return new Product(*this,newname); }
  inline virtual ~Product() {
//    std::cout << "Product::~Product(): num calls " << num_calls_ << std::endl;
  }
  
protected:

  RooRealProxy par_factor1 ;
  RooRealProxy par_factor2 ;
  
  mutable long long num_calls_;
  
  virtual Int_t	getAnalyticalIntegral(RooArgSet& allVars, RooArgSet& analVars,
                                      const char* rangeName = 0) const;
  
  inline Double_t evaluate() const {
    // ENTER EXPRESSION IN TERMS OF VARIABLE ARGUMENTS HERE
    //++num_calls_;
    //std::cout << par_factor1*par_factor2 << std::endl;
    return par_factor1*par_factor2;
  }
  
private:

  ClassDef(Product,1) // Your description goes here...
};
} // namespace functions
} // namespace roofit
} // namespace doofit

#endif