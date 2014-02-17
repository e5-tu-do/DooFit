/*****************************************************************************
 * Project: RooFit                                                           *
 * Package: RooFitModels                                                     *
 *    File: $Id: BiasDelta.h,v 1.16 2007/07/12 20:30:49 wouter Exp $
 * Authors:                                                                  *
 *   WV, Wouter Verkerke, UC Santa Barbara, verkerke@slac.stanford.edu       *
 *   DK, David Kirkby,    UC Irvine,         dkirkby@uci.edu                 *
 *                                                                           *
 * Copyright (c) 2000-2005, Regents of the University of California          *
 *                          and Stanford University. All rights reserved.    *
 *                                                                           *
 * Redistribution and use in source and binary forms,                        *
 * with or without modification, are permitted according to the terms        *
 * listed in LICENSE (http://roofit.sourceforge.net/license.txt)             *
 *****************************************************************************/
#ifndef BIAS_DELTA
#define BIAS_DELTA

#include "RooAbsPdf.h"
#include "RooRealProxy.h"

namespace doofit {
namespace extraroopdfs {

class RooRealVar;

class BiasDelta : public RooAbsPdf {
public:
  BiasDelta() {} ;
  BiasDelta(const char *name, const char *title,
	      RooAbsReal& _x, RooAbsReal& _mean);
  BiasDelta(const BiasDelta& other, const char* name=0) ;
  virtual TObject* clone(const char* newname) const { return new BiasDelta(*this,newname); }
  inline virtual ~BiasDelta() { }

  Int_t getAnalyticalIntegral(RooArgSet& allVars, RooArgSet& analVars, const char* rangeName=0) const ;
  Double_t analyticalIntegral(Int_t code, const char* rangeName=0) const ;

  Int_t getGenerator(const RooArgSet& directVars, RooArgSet &generateVars, Bool_t staticInitOK=kTRUE) const;
  void generateEvent(Int_t code);

protected:

  RooRealProxy x ;
  RooRealProxy mean ;
  
  Double_t evaluate() const ;

private:

  ClassDef(BiasDelta,1) // Delta distribution
};

} // namespace extraroopdfs
} // namespace doofit
#endif
