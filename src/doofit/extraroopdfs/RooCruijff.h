/*****************************************************************************
* Package: RooRarFit
*    File: $Id: RooCruijff.rdl,v 1.3 2011/06/16 13:18:48 fwilson Exp $   *
* Authors:                                                                  *
*    Karsten Koeneke, Massachusetts Institute of Technology, Cambridge, USA *
*    Vouter Hulbergen                                                       *
*                                                                           *
* Copyright (C) 2005-2012, Massachsetts Institute of Technology, Cambridge, USA  *
*****************************************************************************/

#ifndef ROO_CRUIJFF
#define ROO_CRUIJFF

#include "RooAbsReal.h" 
#include "RooRealVar.h"
#include "RooAbsPdf.h"
#include "RooRealProxy.h"

//class RooRealVar;
//class RooAbsReal;

class RooCruijff : public RooAbsPdf {
public:
 
  RooCruijff(){};

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
    ClassDef(RooCruijff,1);
 };
 
 #endif
