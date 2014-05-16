/*****************************************************************************
 * Project: RooFit                                                           *
 * Package: RooFitModels                                                     *
 * @(#)root/roofit:$Id: RooBMixAsymDecay.cxx 36230 2010-10-09 20:21:02Z wouter $
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
// Class RooBMixAsymDecay is a RooAbsAnaConvPdf implementation that describes
// the decay of B mesons with the effects of B0/B0bar mixing.
// Asymmetries at Detection and Production as well as Tagging-Efficiency-Asymmetries
// and Mistagasymmetries are taken into account.
// This function can be analytically convolved with any RooResolutionModel implementation
// END_HTML
//

//from Root
#include "Riostream.h"
#include "TMath.h"

//from RooFit
#include "RooFit.h"
#include "RooRealVar.h"
#include "RooRealIntegral.h"
#include "RooRandom.h"

//From Project
#include "RooBMixAsymDecay.h"

namespace doofit {
namespace roofit {
namespace pdfs {

// This is not required, see http://root.cern.ch/root/roottalk/roottalk08/0008.html
//ClassImp(RooBMixAsymDecay); 

//_____________________________________________________________________________
RooBMixAsymDecay::RooBMixAsymDecay(const char *name, const char *title, 
                           RooRealVar& t, RooAbsCategory& mixState,
                           RooAbsCategory& fstateFlav,
                           RooAbsReal& tau, RooAbsReal& dm,			   
                           RooAbsReal& mistag, RooAbsReal& delMistag,
                           RooAbsReal& tagEfficiency, RooAbsReal& delTagEfficiency,
                           RooAbsReal& productAsymmetry, RooAbsReal& detectAsymmetry,
                           const RooResolutionModel& model, 
                           DecayType type) :
RooAbsAnaConvPdf(name,title,model,t), 
_type(type),
_mistag("mistag","Mistag rate",this,mistag),
_delMistag("delMistag","Delta mistag rate",this,delMistag),
_tagEfficiency("tagEfficiency","Tagging-Efficiency",this,tagEfficiency),
_delTagEfficiency("delTagEfficiency","Delta Tagging-Efficiency",this,delTagEfficiency),
_productAsymmetry("productAsymmetry","Production Asymmetry",this,productAsymmetry),
_detectAsymmetry("detectAsymmetry","Detection Asymmetry",this,detectAsymmetry),
_mixState("mixState","Mixing state",this,mixState),
_fstateFlav("fstateFlav","Flavour of final state",this,fstateFlav),
_tau("tau","Mixing life time",this,tau),
_dm("dm","Mixing frequency",this,dm),
_t("_t","time",this,t)
//_t("_t","time",this,t), _genMixFrac(0)
{
  // Constructor
  switch(type) {
    case SingleSided:
      _basisExp = declareBasis("exp(-@0/@1)",RooArgList(tau,dm)) ;
      _basisCos = declareBasis("exp(-@0/@1)*cos(@0*@2)",RooArgList(tau,dm)) ;
      break ;
    case Flipped:
      _basisExp = declareBasis("exp(@0/@1)",RooArgList(tau,dm)) ;
      _basisCos = declareBasis("exp(@0/@1)*cos(@0*@2)",RooArgList(tau,dm)) ;
      break ;
    case DoubleSided:
      _basisExp = declareBasis("exp(-abs(@0)/@1)",RooArgList(tau,dm)) ;
      _basisCos = declareBasis("exp(-abs(@0)/@1)*cos(@0*@2)",RooArgList(tau,dm)) ;
      break ;
  }
}



//_____________________________________________________________________________
RooBMixAsymDecay::RooBMixAsymDecay(const RooBMixAsymDecay& other, const char* name) : 
RooAbsAnaConvPdf(other,name), 
_type(other._type),
_mistag("mistag",this,other._mistag),
_delMistag("delMistag",this,other._delMistag),
_tagEfficiency("tagEfficiency",this,other._tagEfficiency),
_delTagEfficiency("delTagEfficiency",this,other._delTagEfficiency),
_productAsymmetry("productAsymmetry",this,other._productAsymmetry),
_detectAsymmetry("detectAsymmetry",this,other._detectAsymmetry),
_mixState("mixState",this,other._mixState),
_fstateFlav("fstateFlav",this,other._fstateFlav),
_tau("tau",this,other._tau),
_dm("dm",this,other._dm),
_t("t",this,other._t),
_basisExp(other._basisExp),
_basisCos(other._basisCos)
//_genMixFrac(other._genMixFrac),
//_genFlavFrac(other._genFlavFrac),
//_genFlavFracMix(other._genFlavFracMix),
//_genFlavFracUnmix(other._genFlavFracUnmix)
{
  // Copy constructor
}



//_____________________________________________________________________________
RooBMixAsymDecay::~RooBMixAsymDecay()
{
  // Destructor
}



//_____________________________________________________________________________
Double_t RooBMixAsymDecay::coefficient(Int_t basisIndex) const 
{
  //Case differentiation: tagged --> abs(_mixState == 1) or abs(_mixState == 0)
  /*if (_mixState != 0){
    if (basisIndex==_basisExp) {
      return (1 + _fstateFlav * _detectAsymmetry) * 
      (_tagEfficiency * (1 - _mixState * _fstateFlav * _delMistag) + 
       _mixState * _fstateFlav * _tagEfficiency * _productAsymmetry * (1 - 2 * _mistag) + 
       _mixState * _fstateFlav * _delTagEfficiency* 0.5 * (1 + _mixState * _fstateFlav * _productAsymmetry - _delMistag * _productAsymmetry - 2 * _mistag)); 
    }
    
    if (basisIndex==_basisCos) {
      return _mixState*(1 + _fstateFlav * _detectAsymmetry) * 
      (_tagEfficiency * (1 - 2 * _mistag) + 
       _tagEfficiency * _productAsymmetry * (_fstateFlav * _mixState - _delMistag) + 
       _delTagEfficiency * 0.5 * (_productAsymmetry + _mixState * _fstateFlav - 2 * _mixState * _productAsymmetry - _delMistag));   
    }
  }
  //Case differentiation: untagged --> _mixState == 0
  else {
    if (basisIndex==_basisExp) {
      return (1 + _fstateFlav * _detectAsymmetry) * 
      (2 * _tagEfficiency + _delTagEfficiency * _productAsymmetry);
    }
    
    if (basisIndex==_basisCos) {
      return (1 + _fstateFlav * _detectAsymmetry) * 
      (_delTagEfficiency * _fstateFlav + 2 * _tagEfficiency * _productAsymmetry * _fstateFlav);
    }
  }*/
  
  //Untagged Events
  if (_mixState == 0){
    if (_fstateFlav == 1){
      if (basisIndex == _basisExp){
        return (1 + _detectAsymmetry) * (   (_tagEfficiency + 0.5 * _delTagEfficiency)  * (1 + _productAsymmetry) 
                                          + (_tagEfficiency - 0.5 * _delTagEfficiency)  * (1 - _productAsymmetry));
      }
      else if (basisIndex == _basisCos){
        return (1 + _detectAsymmetry) * (   (_tagEfficiency + 0.5 * _delTagEfficiency)  * (1 + _productAsymmetry) 
                                          - (_tagEfficiency - 0.5 * _delTagEfficiency)  * (1 - _productAsymmetry));
      }
    }
    else if (_fstateFlav == -1){
      if (basisIndex == _basisExp){
        return (1 - _detectAsymmetry) * (   (_tagEfficiency + 0.5 * _delTagEfficiency) * (1 + _productAsymmetry) 
                                          + (_tagEfficiency - 0.5 * _delTagEfficiency) * (1 - _productAsymmetry));
      }
      else if (basisIndex == _basisCos){
        return (1 - _detectAsymmetry) * ( - (_tagEfficiency + 0.5 * _delTagEfficiency) * (1 + _productAsymmetry) 
                                          + (_tagEfficiency - 0.5 * _delTagEfficiency) * (1 - _productAsymmetry));
      }
    }
  }
  
  //Tagged unmixed Events
  else if (_mixState == 1){
    if (_fstateFlav == 1){
      if (basisIndex == _basisExp){
        return (1 + _detectAsymmetry) * (   (_tagEfficiency + 0.5 * _delTagEfficiency) * (1 - (_mistag + 0.5 * _delMistag) ) * (1 + _productAsymmetry) 
                                          + (_tagEfficiency - 0.5 * _delTagEfficiency) * (    (_mistag - 0.5 * _delMistag) ) * (1 - _productAsymmetry));
      }
      else if (basisIndex == _basisCos){
        return (1 + _detectAsymmetry) * (   (_tagEfficiency + 0.5 * _delTagEfficiency) * (1 - (_mistag + 0.5 * _delMistag) ) * (1 + _productAsymmetry) 
                                          - (_tagEfficiency - 0.5 * _delTagEfficiency) * (    (_mistag - 0.5 * _delMistag) ) * (1 - _productAsymmetry));
      }
    }
    else if (_fstateFlav == -1){
      if (basisIndex == _basisExp){
        return (1 - _detectAsymmetry) * (   (_tagEfficiency + 0.5 * _delTagEfficiency) * (    (_mistag + 0.5 * _delMistag) ) * (1 + _productAsymmetry) 
                                          + (_tagEfficiency - 0.5 * _delTagEfficiency) * (1 - (_mistag - 0.5 * _delMistag) ) * (1 - _productAsymmetry));
      }
      else if (basisIndex == _basisCos){
        return (1 - _detectAsymmetry) * ( - (_tagEfficiency + 0.5 * _delTagEfficiency) * (    (_mistag + 0.5 * _delMistag) ) * (1 + _productAsymmetry) 
                                          + (_tagEfficiency - 0.5 * _delTagEfficiency) * (1 - (_mistag - 0.5 * _delMistag) ) * (1 - _productAsymmetry));
      }
    }
  }
  
  //Tagged mixed Events
  else if (_mixState == -1) {
    if (_fstateFlav == 1){
      if (basisIndex == _basisExp){
        return (1 + _detectAsymmetry) * (   (_tagEfficiency + 0.5 * _delTagEfficiency) * (    (_mistag + 0.5 * _delMistag) ) * (1 + _productAsymmetry) 
                                          + (_tagEfficiency - 0.5 * _delTagEfficiency) * (1 - (_mistag - 0.5 * _delMistag) ) * (1 - _productAsymmetry));
      }
      else if (basisIndex == _basisCos){
        return (1 + _detectAsymmetry) * (   (_tagEfficiency + 0.5 * _delTagEfficiency) * (    (_mistag + 0.5 * _delMistag) ) * (1 + _productAsymmetry) 
                                          - (_tagEfficiency - 0.5 * _delTagEfficiency) * (1 - (_mistag - 0.5 * _delMistag) ) * (1 - _productAsymmetry));
      }
    }
    else if (_fstateFlav == -1){
      if (basisIndex == _basisExp){
        return (1 - _detectAsymmetry) * (   (_tagEfficiency + 0.5 * _delTagEfficiency) * (1 - (_mistag + 0.5 * _delMistag) ) * (1 + _productAsymmetry) 
                                          + (_tagEfficiency - 0.5 * _delTagEfficiency) * (    (_mistag - 0.5 * _delMistag) ) * (1 - _productAsymmetry));
      }
      else if (basisIndex == _basisCos){
        return (1 - _detectAsymmetry) * ( - (_tagEfficiency + 0.5 * _delTagEfficiency) * (1 - (_mistag + 0.5 * _delMistag) ) * (1 + _productAsymmetry) 
                                          + (_tagEfficiency - 0.5 * _delTagEfficiency) * (    (_mistag - 0.5 * _delMistag) ) * (1 - _productAsymmetry));
      }
    }
  }
  return 0 ;
}



//_____________________________________________________________________________
Int_t RooBMixAsymDecay::getCoefAnalyticalIntegral(Int_t /*code*/, RooArgSet& allVars, RooArgSet& analVars, const char* rangeName) const 
{
  //   cout << "RooBMixAsymDecay::getCoefAI " ; allVars.Print("1") ;
  if (rangeName) {
    return 0 ;
  }
  
  if (matchArgs(allVars,analVars,_mixState,_fstateFlav)) return 3 ;
  if (matchArgs(allVars,analVars,_mixState)) return 2 ;
  if (matchArgs(allVars,analVars,_fstateFlav)) return 1 ;
  return 0 ;
}



//_____________________________________________________________________________
Double_t RooBMixAsymDecay::coefAnalyticalIntegral(Int_t basisIndex, Int_t code, const char* /*rangeName*/) const 
{  
  //Case differentiation: abs(_mixState == 1) or abs(_mixState == 0)
  //Case 1: tagged (_mixState == 1)
  if (_mixState != 0){

    switch(code) {
        // No integration
      case 0: return coefficient(basisIndex) ;
        
        // Integration over 'mixState' and 'fstateFlav' (1 or -1) (1 or -1)
      case 3:
        if (basisIndex==_basisExp) {
          return 4 * _tagEfficiency + 2 * _delTagEfficiency * _productAsymmetry ;
        }    
        if (basisIndex==_basisCos) {
          return 4 * _detectAsymmetry * _tagEfficiency * _productAsymmetry + 2 * _detectAsymmetry * _delTagEfficiency;
        }
      //return coefficient(basisIndex) ;
        break ;
        
        // Integration over 'mixState' (1 or -1)
      case 2:
        if (basisIndex==_basisExp) {
          return (1 + _fstateFlav * _detectAsymmetry) * (2 * _tagEfficiency + _delTagEfficiency * _productAsymmetry);
        }    
        if (basisIndex==_basisCos) {
          return (1 + _fstateFlav * _detectAsymmetry) * (_fstateFlav * (2 * _tagEfficiency * _productAsymmetry + _delTagEfficiency));
        }
        break ;
        
        // Integration over 'fstateFlav' (1 or -1)
      case 1:
        if (basisIndex==_basisExp) {
          return 2 * _tagEfficiency * (1 - _fstateFlav * _detectAsymmetry * (_delMistag - _productAsymmetry + 2 * _productAsymmetry * _mistag)) + 
          _delTagEfficiency * _mixState * (_mixState * _productAsymmetry + _detectAsymmetry * (1 - _delMistag * _productAsymmetry - 2 * _mistag));
        }    
        if (basisIndex==_basisCos) {
          return 2 * _mixState * _tagEfficiency * (1 - 2 * _mistag - _productAsymmetry * _delMistag + _mixState * _detectAsymmetry * _productAsymmetry)
          + _mixState * _delTagEfficiency * (_productAsymmetry * (1 - 2*_mistag) - _delMistag + _mixState * _detectAsymmetry);
        }
        break ;
        
      default:
        assert(0) ;
    }
  }
  //Case 2: untagged (_mixState == 1)
  else {
    switch(code) {
        // No integration
      case 0: return coefficient(basisIndex) ;
        
        // Integration over 'mixState' and 'fstateFlav' (0) (1 or -1)
      case 3:
        if (basisIndex==_basisExp) {
          return 4 * _tagEfficiency + 2 * _delTagEfficiency * _productAsymmetry ;
        }    
        if (basisIndex==_basisCos) {
          return 4 * _detectAsymmetry * _tagEfficiency * _productAsymmetry + 2 * _detectAsymmetry * _delTagEfficiency;
        }
        break ;
        
        // Integration over 'mixState' (0)
      case 2: return coefficient(basisIndex);
        break ;
        
        // Integration over 'fstateFlav' (1 or -1)
      case 1:
        if (basisIndex==_basisExp) {
          return 4 * _tagEfficiency + 2 * _delTagEfficiency * _productAsymmetry ;
        }    
        if (basisIndex==_basisCos) {
          return 4 * _detectAsymmetry * _tagEfficiency * _productAsymmetry + 2 * _detectAsymmetry * _delTagEfficiency;
        }
        break ;
        
      default:
        assert(0) ;
    }
  }
    
  return 0 ;
}



/*/_____________________________________________________________________________
Int_t RooBMixAsymDecay::getGenerator(const RooArgSet& directVars, RooArgSet &generateVars, Bool_t staticInitOK) const
{
  if (staticInitOK) {
    if (matchArgs(directVars,generateVars,_t,_mixState,_tagFlav)) return 4 ;  
    if (matchArgs(directVars,generateVars,_t,_mixState)) return 3 ;  
    if (matchArgs(directVars,generateVars,_t,_tagFlav)) return 2 ;  
  }
  
  if (matchArgs(directVars,generateVars,_t)) return 1 ;  
  return 0 ;
}*/



/*/_____________________________________________________________________________
void RooBMixAsymDecay::initGenerator(Int_t code)
{
  switch (code) {
    case 2:
    {
      // Calculate the fraction of B0bar events to generate
      Double_t sumInt = RooRealIntegral("sumInt","sum integral",*this,RooArgSet(_t.arg(),_tagFlav.arg())).getVal() ;
      _tagFlav = 1 ; // B0 
      Double_t flavInt = RooRealIntegral("flavInt","flav integral",*this,RooArgSet(_t.arg())).getVal() ;
      _genFlavFrac = flavInt/sumInt ;
      break ;
    }  
    case 3:
    {
      // Calculate the fraction of mixed events to generate
      Double_t sumInt = RooRealIntegral("sumInt","sum integral",*this,RooArgSet(_t.arg(),_mixState.arg())).getVal() ;
      _mixState = -1 ; // mixed
      Double_t mixInt = RooRealIntegral("mixInt","mix integral",*this,RooArgSet(_t.arg())).getVal() ;
      _genMixFrac = mixInt/sumInt ;
      break ;
    }  
    case 4:
    {
      // Calculate the fraction of mixed events to generate
      Double_t sumInt = RooRealIntegral("sumInt","sum integral",*this,RooArgSet(_t.arg(),_mixState.arg(),_tagFlav.arg())).getVal() ;
      _mixState = -1 ; // mixed
      Double_t mixInt = RooRealIntegral("mixInt","mix integral",*this,RooArgSet(_t.arg(),_tagFlav.arg())).getVal() ;
      _genMixFrac = mixInt/sumInt ;
      
      // Calculate the fractio of B0bar tags for mixed and unmixed
      RooRealIntegral dtInt("mixInt","mix integral",*this,RooArgSet(_t.arg())) ;
      _mixState = -1 ; // Mixed
      _tagFlav  =  1 ; // B0
      _genFlavFracMix   = dtInt.getVal() / mixInt ;
      _mixState =  1 ; // Unmixed
      _tagFlav  =  1 ; // B0
      _genFlavFracUnmix = dtInt.getVal() / (sumInt - mixInt) ;
      break ;
    }
  }
}*/




/*/_____________________________________________________________________________
void RooBMixAsymDecay::generateEvent(Int_t code)
{
  // Generate mix-state dependent
  switch(code) {
    case 2:
    {
      Double_t rand = RooRandom::uniform() ;
      _tagFlav = (Int_t) ((rand<=_genFlavFrac) ?  1 : -1) ;
      break ;
    }
    case 3:
    {
      Double_t rand = RooRandom::uniform() ;
      _mixState = (Int_t) ((rand<=_genMixFrac) ? -1 : 1) ;
      break ;
    }
    case 4:
    {
      Double_t rand = RooRandom::uniform() ;
      _mixState = (Int_t) ((rand<=_genMixFrac) ? -1 : 1) ;
      
      rand = RooRandom::uniform() ;
      Double_t genFlavFrac = (_mixState==-1) ? _genFlavFracMix : _genFlavFracUnmix ;
      _tagFlav = (Int_t) ((rand<=genFlavFrac) ?  1 : -1) ;
      break ;
    }
  }
  
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
    Double_t dil = 1-2.*_mistag ;
    Double_t maxAcceptProb = 1 + TMath::Abs(_delMistag) + TMath::Abs(dil) ;
    Double_t acceptProb = (1-_tagFlav*_delMistag) + _mixState*dil*cos(_dm*tval);
    Bool_t mixAccept = maxAcceptProb*RooRandom::uniform() < acceptProb ? kTRUE : kFALSE ;
    
    if (tval<_t.max() && tval>_t.min() && mixAccept) {
      _t = tval ;
      break ;
    }
  }
}*/
  
} // namespace pdfs
} // namespace roofit
} // namespace doofit

