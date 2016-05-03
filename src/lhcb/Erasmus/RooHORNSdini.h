
/*-----------------------------------------------------------------------------------
Author: Paolo Gandini
Email:  paolo.gandini@cern.ch

Double horn distribution convoluted with a Gaussian resolution function

-- v3 --
SHIFT included
EFFICIENCY CORRECTION included

-- v8 --
EFFICIENCY is a double GAUSSIAN: two extra parameters: ratio_sigma, fraction_sigma
-----------------------------------------------------------------------------------*/

#ifndef ROO_ROOHORNSDINI
#define ROO_ROOHORNSDINI

#include "RooAbsPdf.h"
#include "RooRealProxy.h"
#include "Rtypes.h"


class RooRealVar;

class  RooHORNSdini : public RooAbsPdf {


public:
  RooHORNSdini(const char *name, const char *title,
		  RooAbsReal& _m, RooAbsReal& _a, RooAbsReal& _b, RooAbsReal& _csi, RooAbsReal& _shift, RooAbsReal& _sigma, RooAbsReal& _ratio_sigma, RooAbsReal& _fraction_sigma);
  RooHORNSdini(const  RooHORNSdini& other, const char* name = 0);
  virtual TObject* clone(const char* newname) const { return new  RooHORNSdini(*this,newname); }
  inline virtual ~ RooHORNSdini() {}


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
  ClassDef( RooHORNSdini,2) //  RooHORNSdini function PDF
};

#endif
