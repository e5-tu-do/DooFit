/*****************************************************************************
 * Project: RooFit                                                           *
 * Package: RooFitModels                                                     *
 * @(#)root/roofit:$Id$
 * Authors:                                                                  *
 *   JGS, Jim Smith    , University of Colorado, jgsmith@pizero.colorado.edu *
 * History:
 *   15-Aug-2002 JGS Created initial version
 *   11-Sep-2002 JGS Mods to introduce mu (Mirna van Hoek, JGS, Nick Danielson)
 *                                                                           *
 * Copyright (c) 2000-2005, Regents of the University of California,         *
 *                          University of Colorado                           *
 *                          and Stanford University. All rights reserved.    *
 *                                                                           *
 * Redistribution and use in source and binary forms,                        *
 * with or without modification, are permitted according to the terms        *
 * listed in LICENSE (http://roofit.sourceforge.net/license.txt)             *
 *****************************************************************************/

//////////////////////////////////////////////////////////////////////////////
//
// BEGIN_HTML
// Implement standard CP physics model with S and C (no mention of lambda)
// Suitably stolen and modified from RooBCPEffDecay
// END_HTML
//

#include "RooFit.h"

#include "Riostream.h"
#include "RooRealVar.h"
#include "RooRandom.h"
#include "RooBCPGenDecayUt.h"
#include "RooRealIntegral.h"

using namespace std;

namespace doofit {
namespace roofit {
namespace pdfs {

//_____________________________________________________________________________
RooBCPGenDecayUt::RooBCPGenDecayUt(const char *name, const char *title, 
			       RooRealVar& t, RooAbsReal& tau, RooAbsReal& dm,
			       RooAbsReal& a, RooAbsReal& b, RooAbsReal& mu,
			       const RooResolutionModel& model, DecayType type) :
  RooAbsAnaConvPdf(name,title,model,t), 
  _avgC("C","Coefficient of cos term",this,a),
  _avgS("S","Coefficient of cos term",this,b),
  _mu("mu","Tag efficiency difference and Production asymmetry",this,mu),  
  _t("t","time",this,t),
  _tau("tau","decay time",this,tau),
  _dm("dm","mixing frequency",this,dm),
  _type(type)
{
  // Constructor
  switch(type) {
  case SingleSided:
    _basisExp = declareBasis("exp(-@0/@1)",RooArgList(tau,dm)) ;
    _basisSin = declareBasis("exp(-@0/@1)*sin(@0*@2)",RooArgList(tau,dm)) ;
    _basisCos = declareBasis("exp(-@0/@1)*cos(@0*@2)",RooArgList(tau,dm)) ;
    break ;
  case Flipped:
    _basisExp = declareBasis("exp(@0)/@1)",RooArgList(tau,dm)) ;
    _basisSin = declareBasis("exp(@0/@1)*sin(@0*@2)",RooArgList(tau,dm)) ;
    _basisCos = declareBasis("exp(@0/@1)*cos(@0*@2)",RooArgList(tau,dm)) ;
    break ;
  case DoubleSided:
    _basisExp = declareBasis("exp(-abs(@0)/@1)",RooArgList(tau,dm)) ;
    _basisSin = declareBasis("exp(-abs(@0)/@1)*sin(@0*@2)",RooArgList(tau,dm)) ;
    _basisCos = declareBasis("exp(-abs(@0)/@1)*cos(@0*@2)",RooArgList(tau,dm)) ;
    break ;
  }
}



//_____________________________________________________________________________
RooBCPGenDecayUt::RooBCPGenDecayUt(const RooBCPGenDecayUt& other, const char* name) : 
  RooAbsAnaConvPdf(other,name), 
  _avgC("C",this,other._avgC),
  _avgS("S",this,other._avgS),
  _mu("mu",this,other._mu),
  _t("t",this,other._t),
  _tau("tau",this,other._tau),
  _dm("dm",this,other._dm),
  _type(other._type),
  _basisExp(other._basisExp),
  _basisSin(other._basisSin),
  _basisCos(other._basisCos)
{
  // Copy constructor
}



//_____________________________________________________________________________
RooBCPGenDecayUt::~RooBCPGenDecayUt()
{
  // Destructor
}



//_____________________________________________________________________________
Double_t RooBCPGenDecayUt::coefficient(Int_t basisIndex) const 
{
  if (basisIndex==_basisExp) {
    return  1.;
  }

  if (basisIndex==_basisSin) {
    //sin term: _mu*S
    return _mu*_avgS ;
    // =   _mu* S
  }
  
  if (basisIndex==_basisCos) {
    //cos term: _mu*C
    return -1.*_mu*_avgC ;
    // =   _mu* C
  } 
  
  return 0 ;
}



//_____________________________________________________________________________
Int_t RooBCPGenDecayUt::getCoefAnalyticalIntegral(Int_t /*code*/, RooArgSet& allVars, RooArgSet& analVars, const char* rangeName) const 
{
  return 0 ;
}



//_____________________________________________________________________________
Double_t RooBCPGenDecayUt::coefAnalyticalIntegral(Int_t basisIndex, Int_t code, const char* /*rangeName*/) const 
{
  return coefficient(basisIndex) ;
}



//_____________________________________________________________________________
Int_t RooBCPGenDecayUt::getGenerator(const RooArgSet& directVars, RooArgSet &generateVars, Bool_t staticInitOK) const
{
  if (matchArgs(directVars,generateVars,_t)) return 1 ;  
  return 0 ;
}



//_____________________________________________________________________________
void RooBCPGenDecayUt::initGenerator(Int_t code)
{
  // not needed without tag information
}



//_____________________________________________________________________________
void RooBCPGenDecayUt::generateEvent(Int_t code)
{  
  // Generate delta-t dependent
  while(1) {
    Double_t rand = RooRandom::uniform() ;
    Double_t tval(0) ;

    switch(_type) {
    case SingleSided:
      tval = -_tau*log(rand);
      break ;
    case Flipped:
      tval= +_tau*log(rand);
      break ;
    case DoubleSided:
      tval = (rand<=0.5) ? -_tau*log(2*rand) : +_tau*log(2*(rand-0.5)) ;
      break ;
    }

    // Accept event if T is in generated range
    Double_t maxDil = 1.0 ;
// 2 in next line is conservative and inefficient - allows for delMistag=1!
    Double_t maxAcceptProb = 2 + fabs(maxDil*_avgS) + fabs(maxDil*_avgC);        
    Double_t acceptProb    = 1 + _mu*_avgS*sin(_dm*tval) - _mu*_avgC*cos(_dm*tval);

    Bool_t accept = maxAcceptProb*RooRandom::uniform() < acceptProb ? kTRUE : kFALSE ;
    
    if (tval<_t.max() && tval>_t.min() && accept) {
      _t = tval ;
      break ;
    }
  }
  
}

} // namespace pdfs
} // namespace roofit
} // namespace doofit
