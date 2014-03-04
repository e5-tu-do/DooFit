/*****************************************************************************
 * Project: RooFit                                                           *
 *                                                                           *
 * This code was autogenerated by RooClassFactory                            * 
 *****************************************************************************/

#ifndef ROOAMOROSOPDF
#define ROOAMOROSOPDF

#include "RooAbsPdf.h"
#include "RooRealProxy.h"
#include "RooCategoryProxy.h"
#include "RooAbsReal.h"
#include "RooAbsCategory.h"
 
class RooAmorosoPdf : public RooAbsPdf {
public:
   RooAmorosoPdf() {} ; 
   RooAmorosoPdf(const char *name, const char *title,
              RooAbsReal& _x,
              RooAbsReal& _a,
              RooAbsReal& _theta,
              RooAbsReal& _alpha,
              RooAbsReal& _beta);
   RooAmorosoPdf(const RooAmorosoPdf& other, const char* name=0) ;
   virtual TObject* clone(const char* newname) const { return new RooAmorosoPdf(*this,newname); }
   inline virtual ~RooAmorosoPdf() { }

protected:

   RooRealProxy x ;
   RooRealProxy a ;
   RooRealProxy theta ;
   RooRealProxy alpha ;
   RooRealProxy beta ;
  
   Double_t evaluate() const ;

private:

   ClassDef(RooAmorosoPdf,1) // Your description goes here...
};
 
#endif
