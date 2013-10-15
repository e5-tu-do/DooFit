/*****************************************************************************
 * Project: RooFit                                                           *
 * Package: RooFitModels                                                     *
 *    File: $Id: RooBDecay.h,v 1.7 2007/05/11 09:13:07 verkerke Exp $
 * Authors:                                                                  *
 *   PL, Parker C Lund,   UC Irvine                                          *
 *   DK, David Kirkby,    UC Irvine,         dkirkby@uci.edu                 *
 *   WV, Wouter Verkerke, UC Santa Barbara, verkerke@slac.stanford.edu       *
 *                                                                           *
 * Copyright (c) 2000-2005, Regents of the University of California          *
 *                          and Stanford University. All rights reserved.    *
 *                                                                           *
 * Redistribution and use in source and binary forms,                        *
 * with or without modification, are permitted according to the terms        *
 * listed in LICENSE (http://roofit.sourceforge.net/license.txt)             *
 *****************************************************************************/

#ifndef ROO_TRIVIALTAGDECAY
#define ROO_TRIVIALTAGDECAY

#include "RooAbsAnaConvPdf.h"
#include "RooRealProxy.h"
#include "RooCategoryProxy.h"

class RooTrivialTagDecay : public RooAbsAnaConvPdf
{

public:

  enum DecayType { SingleSided, DoubleSided, Flipped };

  //Constructors, assignment etc
  inline RooTrivialTagDecay() { }
  RooTrivialTagDecay(const char *name, const char *title, RooRealVar& t, RooAbsCategory& tag,
			RooAbsReal& tau, RooAbsReal& dgamma, RooAbsReal& dm, RooAbsReal& tageff,
			RooAbsReal& fcosh, RooAbsReal& fsinh, 
            RooAbsReal& fcos, RooAbsReal& fsin, 
			const RooResolutionModel& model,
			DecayType type);
  RooTrivialTagDecay(const RooTrivialTagDecay& other, const char* name=0);
  virtual TObject* clone(const char* newname) const ;
  virtual ~RooTrivialTagDecay();

  virtual Double_t coefficient(Int_t basisIndex) const;
  RooArgSet* coefVars(Int_t coefIdx) const ;

  Int_t getCoefAnalyticalIntegral(Int_t coef, RooArgSet& allVars, RooArgSet& analVars, const char* rangeName=0) const ;
  Double_t coefAnalyticalIntegral(Int_t coef, Int_t code, const char* rangeName=0) const ;

  Int_t getGenerator(const RooArgSet& directVars, RooArgSet &generateVars, Bool_t staticInitOK=kTRUE) const;
  void generateEvent(Int_t code);
  Bool_t isDirectGenSafe(const RooAbsArg& arg) const ;

private:
  const RooRealProxy* proxy(Int_t basisIndex) const {
      if(basisIndex == _basisCosh) { return &_fcosh; }
      if(basisIndex == _basisSinh) { return &_fsinh; }
      if(basisIndex == _basisCos)  { return &_fcos;  }
      if(basisIndex == _basisSin)  { return &_fsin;  }
      return 0;
  }

  RooRealProxy _t;
  RooCategoryProxy _tag;
  RooRealProxy _tau;
  RooRealProxy _dgamma;
  RooRealProxy _dm;	
  RooRealProxy _tageff;	
  RooRealProxy _fcosh;
  RooRealProxy _fsinh;
  RooRealProxy _fcos;
  RooRealProxy _fsin;
  Int_t _basisCosh;
  Int_t _basisSinh;
  Int_t _basisCos;
  Int_t _basisSin;
  Int_t _basisB;
  DecayType _type;

  ClassDef(RooTrivialTagDecay, 1) // P.d.f of general description of B decay time distribution
};

#endif

