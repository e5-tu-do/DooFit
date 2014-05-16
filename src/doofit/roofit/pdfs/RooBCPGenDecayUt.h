/*****************************************************************************
 * Project: RooFit                                                           *
 * Package: RooFitModels                                                     *
 *    File: $Id: RooBCPGenDecayUt.h,v 1.13 2007/05/11 09:13:07 verkerke Exp $
 * Authors:                                                                  *
 *   JS, Jim Smith    , University of Colorado, jgsmith@pizero.colorado.edu  *
 *                                                                           *
 * Copyright (c) 2000-2005, Regents of the University of California,         *
 *                          University of Colorado                           *
 *                          and Stanford University. All rights reserved.    *
 *                                                                           *
 * Redistribution and use in source and binary forms,                        *
 * with or without modification, are permitted according to the terms        *
 * listed in LICENSE (http://roofit.sourceforge.net/license.txt)             *
 *****************************************************************************/
#ifndef ROO_BCPGEN_DECAY_UT
#define ROO_BCPGEN_DECAY_UT

#include "RooAbsAnaConvPdf.h"
#include "RooRealProxy.h"
#include "RooCategoryProxy.h"

namespace doofit {
namespace roofit {
namespace pdfs {

class RooBCPGenDecayUt : public RooAbsAnaConvPdf {
public:

  enum DecayType { SingleSided, DoubleSided, Flipped };

  // Constructors, assignment etc
  inline RooBCPGenDecayUt() { }
  RooBCPGenDecayUt(const char *name, const char *title, 
		 RooRealVar& t, RooAbsReal& tau, RooAbsReal& dm,
		 RooAbsReal& a, RooAbsReal& b, RooAbsReal& mu,
		 const RooResolutionModel& model, DecayType type=SingleSided) ;

  RooBCPGenDecayUt(const RooBCPGenDecayUt& other, const char* name=0);
  virtual TObject* clone(const char* newname) const { return new RooBCPGenDecayUt(*this,newname) ; }
  virtual ~RooBCPGenDecayUt();

  virtual Double_t coefficient(Int_t basisIndex) const ;

  virtual Int_t getCoefAnalyticalIntegral(Int_t coef, RooArgSet& allVars, RooArgSet& analVars, const char* rangeName=0) const ;
  virtual Double_t coefAnalyticalIntegral(Int_t coef, Int_t code, const char* rangeName=0) const ;

  Int_t getGenerator(const RooArgSet& directVars, RooArgSet &generateVars, Bool_t staticInitOK=kTRUE) const;
  void initGenerator(Int_t code) ;
  void generateEvent(Int_t code) ;
  
protected:

  RooRealProxy _avgC ;
  RooRealProxy _avgS ;
  RooRealProxy _mu ;
  RooRealProxy _t ;
  RooRealProxy _tau ;
  RooRealProxy _dm ;
  
  DecayType _type ;
  Int_t _basisExp ;
  Int_t _basisSin ;
  Int_t _basisCos ;

  ClassDef(RooBCPGenDecayUt,1)  // B decay time distribution of untagged events with CP violation
};

} // namespace pdfs
} // namespace roofit
} // namespace doofit
#endif
