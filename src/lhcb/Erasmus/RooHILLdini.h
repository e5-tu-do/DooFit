
/*-----------------------------------------------------------------------------------
Author: Paolo Gandini
Email:  paolo.gandini@cern.ch

Hill distribution convoluted with a Gaussian resolution function

-- v3 --
SHIFT included
EFFICIENCY CORRECTION included

-- v8 --
EFFICIENCY is a double GAUSSIAN: two extra parameters: ratio_sigma, fraction_sigma
-----------------------------------------------------------------------------------*/


#ifndef ROO_ROOHILLDINI
#define ROO_ROOHILLDINI

#include "RooAbsPdf.h"
#include "RooRealProxy.h"
#include "Rtypes.h"


class RooRealVar;

class  RooHILLdini : public RooAbsPdf {


public:
  RooHILLdini(const char *name, const char *title,
		  RooAbsReal& _m, RooAbsReal& _a, RooAbsReal& _b, RooAbsReal& _csi, RooAbsReal& _shift, RooAbsReal& _sigma, RooAbsReal& _ratio_sigma, RooAbsReal& _fraction_sigma);
  RooHILLdini(const  RooHILLdini& other, const char* name = 0);
  virtual TObject* clone(const char* newname) const { return new  RooHILLdini(*this,newname); }
  inline virtual ~ RooHILLdini() {}


protected:
  RooRealProxy m;
  RooRealProxy a;
  RooRealProxy b;
  RooRealProxy csi;
  RooRealProxy shift;
  RooRealProxy sigma;
  RooRealProxy ratio_sigma;
  RooRealProxy fraction_sigma;
  Double_t evaluate() const;


private:
  ClassDef( RooHILLdini,2) //  RooHILLdini function PDF
};

#endif
