/*****************************************************************************
 * Project: RooFit                                                           *
 * Package: RooFitModels                                                     *
 * @(#)root/roofit:$Id: DooBkgTagAsymDecay.cxx 36230 2010-10-09 20:21:02Z wouter $
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

//////////////////////////////////////////////////////////////////////////////
//
// BEGIN_HTML
// Class DooBkgTagAsymDecay is a RooAbsAnaConvPdf implementation that describes
// the decay of B mesons with the effects of B0/B0bar mixing.
// Asymmetries at Detection and Production as well as Tagging-Efficiency-Asymmetries
// and Mistagasymmetries are taken into account.
// This function can be analytically convolved with any RooResolutionModel implementation
// END_HTML
//

//from Root
#include "Riostream.h"
#include "TMath.h"
#include "TH1.h"

//from RooFit
#include "RooFit.h"
#include "RooRealVar.h"
#include "RooRealIntegral.h"
#include "RooRandom.h"

//From Project
#include "DooBkgTagAsymDecay.h"

namespace doofit {
namespace roofit {
namespace pdfs {

// This is not required, see http://root.cern.ch/root/roottalk/roottalk08/0008.html
//ClassImp(DooBkgTagAsymDecay); 

//_____________________________________________________________________________
DooBkgTagAsymDecay::DooBkgTagAsymDecay(const char *name, const char *title, 
                                       RooRealVar& t,
                                       RooAbsCategory& tag,
                                       RooAbsReal& tau,
                                       TH1* hist,
                                       const RooResolutionModel& model, 
                                       DecayType type) :
RooAbsAnaConvPdf(name,title,model,t),
_t("_t","time",this,t),
_tag("tag","tag",this,tag),
_tau("tau","Mixing life time",this,tau),
_hist(hist),
_type(type)
{
  // Constructor
  switch(type) {
    case SingleSided:
      _basisExp = declareBasis("exp(-@0/@1)",tau) ;
      break ;
    case Flipped:
      _basisExp = declareBasis("exp(@0/@1)",tau) ;
      break ;
    case DoubleSided:
      _basisExp = declareBasis("exp(-abs(@0)/@1)",tau) ;
      break ;
  }
}



//_____________________________________________________________________________
DooBkgTagAsymDecay::DooBkgTagAsymDecay(const DooBkgTagAsymDecay& other, const char* name) : 
RooAbsAnaConvPdf(other,name), 
_t("t",this,other._t),
_tag("mixState",this,other._tag),
_tau("tau",this,other._tau),
_hist(other._hist),
_type(other._type),
_basisExp(other._basisExp)
{
  // Copy constructor
}



//_____________________________________________________________________________
DooBkgTagAsymDecay::~DooBkgTagAsymDecay()
{
  // Destructor
}



//_____________________________________________________________________________
Double_t DooBkgTagAsymDecay::coefficient(Int_t /*basisIndex*/) const 
{
  return 1 ;
}



//_____________________________________________________________________________
Int_t DooBkgTagAsymDecay::getCoefAnalyticalIntegral(Int_t /*code*/, RooArgSet& allVars, RooArgSet& analVars, const char* rangeName) const 
{
  if (rangeName) return 0 ;
  if (matchArgs(allVars,analVars,_tag)) return 1 ;
  return 0 ;
}



//_____________________________________________________________________________
Double_t DooBkgTagAsymDecay::coefAnalyticalIntegral(Int_t code, const char* /*rangeName*/) const 
{  
  switch(code) {
    // No integration
  case 0: return 1 ;

    // Integration over 'tag'
  case 1: return 2 ;
    break ;

  default:
    assert(0) ;
  }

  return 0 ;
}



//_____________________________________________________________________________
Int_t DooBkgTagAsymDecay::getGenerator(const RooArgSet& directVars, RooArgSet &generateVars, Bool_t staticInitOK) const
{
  if (staticInitOK) {
    if (matchArgs(directVars,generateVars,_t,_tag)) return 2 ;
  }
  if (matchArgs(directVars,generateVars,_t)) return 1 ;  
  return 0 ;
}



//_____________________________________________________________________________
void DooBkgTagAsymDecay::generateEvent(Int_t code)
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
    
    if (tval<_t.max() && tval>_t.min()) {
      _t = tval ;
      // Generate tag-state
      if(code==2) {
        Double_t rand = RooRandom::uniform() ;
        _tag = (Int_t) ((rand <= ((_hist->GetBinContent(_hist->FindBin(tval))+1)/2)) ?  -1 : 1) ;
        break ;
      }
      break ;
    }
  }
}
  
} // namespace pdfs
} // namespace roofit
} // namespace doofit

