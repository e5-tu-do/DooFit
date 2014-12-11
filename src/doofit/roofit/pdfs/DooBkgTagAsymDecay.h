/*****************************************************************************
 * Project: RooFit(DooFit)                                                   *
 * Package: RooFitModels(doofit/extraroopdfs)                                *
 *    File: $Id: DooBkgTagAsymDecay.h
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
#ifndef DOOFIT_ROOFIT_PDFS_DOOBKGTAGASYMDECAY
#define DOOFIT_ROOFIT_PDFS_DOOBKGTAGASYMDECAY

//from RooFit
#include "RooAbsAnaConvPdf.h"
#include "RooRealProxy.h"
#include "RooCategoryProxy.h"
#include "TH1.h"

namespace doofit {
namespace roofit {
namespace pdfs {

class DooBkgTagAsymDecay : public RooAbsAnaConvPdf {
public:
  
  enum DecayType { SingleSided, DoubleSided, Flipped };
  
  // Constructors, assignment etc
  inline DooBkgTagAsymDecay() { }
  DooBkgTagAsymDecay(const char *name, const char *title, 
                     RooRealVar& t,
                     RooAbsCategory& tag,
                     RooAbsReal& tau,
                     TH1* hist,
                     const RooResolutionModel& model,
                     DecayType type=SingleSided) ;
  
  DooBkgTagAsymDecay(const DooBkgTagAsymDecay& other, const char* name=0);
  virtual TObject* clone(const char* newname) const { return new DooBkgTagAsymDecay(*this,newname) ; }
  virtual ~DooBkgTagAsymDecay();
  
  virtual Double_t coefficient(Int_t basisIndex) const ;
  
  virtual Int_t getCoefAnalyticalIntegral(Int_t code, RooArgSet& allVars, RooArgSet& analVars, const char* rangeName=0) const ;
  virtual Double_t coefAnalyticalIntegral(Int_t code, const char* rangeName=0) const ;
  
  Int_t getGenerator(const RooArgSet& directVars, RooArgSet &generateVars, Bool_t staticInitOK=kTRUE) const ;
  void generateEvent(Int_t code) ;
  
protected:
  
  RooRealProxy     _t ;
  RooCategoryProxy _tag ;
  RooRealProxy     _tau ;
  TH1*       _hist ;
  DecayType        _type ;
  Int_t _basisExp ;
  
  ClassDef(DooBkgTagAsymDecay,1) // Exponential with time-dependent tagging asymmetry
};

} // namespace pdfs
} // namespace roofit
} // namespace doofit

#endif