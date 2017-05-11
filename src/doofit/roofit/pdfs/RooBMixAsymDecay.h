/*****************************************************************************
 * Project: RooFit(DooFit)                                                           *
 * Package: RooFitModels(doofit/extraroopdfs)                                                    *
 *    File: $Id: RooBMixAsymDecay.h,v 1.14 2007/05/11 09:13:07 verkerke Exp $
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
#ifndef EXTRAROOPDFS_ROO_BMIX_ASYM_DECAY
#define EXTRAROOPDFS_ROO_BMIX_ASYM_DECAY

//from RooFit
#include "RooAbsAnaConvPdf.h"
#include "RooRealProxy.h"
#include "RooCategoryProxy.h"

namespace doofit {
namespace roofit {
namespace pdfs {

class RooBMixAsymDecay : public RooAbsAnaConvPdf {
public:
  
  enum DecayType { SingleSided, DoubleSided, Flipped };
  
  // Constructors, assignment etc
  inline RooBMixAsymDecay() { }
  RooBMixAsymDecay(const char *name, const char *title, 
               RooRealVar& t, RooAbsCategory& mixState, RooAbsCategory& fstateFlav,
               RooAbsReal& tau, RooAbsReal& dm,
               RooAbsReal& mistag, RooAbsReal& delMistag, 
               RooAbsReal& tagEfficiency, RooAbsReal& delTagEfficiency,
               RooAbsReal& productAsymmetry, RooAbsReal& detectAsymmetry,
               const RooResolutionModel& model, 
               DecayType type=DoubleSided) ;
  
  RooBMixAsymDecay(const RooBMixAsymDecay& other, const char* name=0);
  virtual TObject* clone(const char* newname) const { return new RooBMixAsymDecay(*this,newname) ; }
  virtual ~RooBMixAsymDecay();
  
  virtual Double_t coefficient(Int_t basisIndex) const ;
  
  virtual Int_t getCoefAnalyticalIntegral(Int_t coef, RooArgSet& allVars, RooArgSet& analVars, const char* rangeName=0) const ;
  virtual Double_t coefAnalyticalIntegral(Int_t coef, Int_t code, const char* rangeName=0) const ;
  
  // For Generator
  Int_t getGenerator(const RooArgSet& directVars, RooArgSet &generateVars, Bool_t staticInitOK=kTRUE) const;
  void initGenerator(Int_t code) ;
  void generateEvent(Int_t code);
  
protected:
  
  DecayType        _type ;
  RooRealProxy     _mistag ;
  RooRealProxy     _delMistag ;
  RooRealProxy     _tagEfficiency ;
  RooRealProxy     _delTagEfficiency ;
  RooRealProxy     _productAsymmetry ;
  RooRealProxy     _detectAsymmetry ;
  RooCategoryProxy _mixState ;
  RooCategoryProxy _fstateFlav ;
  RooRealProxy     _tau ;
  RooRealProxy     _dm ;
  RooRealProxy     _t ;
  Int_t _basisExp ;
  Int_t _basisCos ;
  
  // For Generator
  Double_t _genMixFrac ;   //! do not persist
  //Double_t _genFlavFrac ;  //!
  Double_t _genFlavFracMix ;   //!
  Double_t _genFlavFracUnmix ; //!

  
  ClassDef(RooBMixAsymDecay,1) // B Mixing decay PDF
};

} // namespace pdfs
} // namespace roofit
} // namespace doofit

#endif