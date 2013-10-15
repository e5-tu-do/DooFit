#ifndef ROOCRUIJFF
#define ROOCRUIJFF

#include "RooAbsPdf.h"
#include "RooRealProxy.h"

class RooRealVar;
class RooAbsReal;

class RooCruijff : public RooAbsPdf {
public:
  RooCruijff(const char *name, const char *title, 
	     RooAbsReal& _x,
	     RooAbsReal& _m0, 
	     RooAbsReal& _sigmaL, RooAbsReal& _sigmaR,
	     RooAbsReal& _alphaL, RooAbsReal& _alphaR);
  
  RooCruijff(const RooCruijff& other, const char* name = 0);

  virtual TObject* clone(const char* newname) const { 
    return new RooCruijff(*this,newname); }

  inline virtual ~RooCruijff() { }

protected:
  RooRealProxy x;
  RooRealProxy m0;
  RooRealProxy sigmaL;
  RooRealProxy sigmaR;
  RooRealProxy alphaL;
  RooRealProxy alphaR;

  Double_t evaluate() const;

private:
  ClassDef(RooCruijff,0)
};

#endif
