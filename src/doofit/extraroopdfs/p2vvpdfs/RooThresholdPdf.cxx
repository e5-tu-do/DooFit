/*****************************************************************************
 * Project: RooFit                                                           *
 * Package: RooFitModels                                                     *
 * @(#)root/roofit:$Id$
 * Authors:                                                                  *
 *   GR, Gerhard Raven,   Nikhef & VU, Gerhard.Raven@nikhef.nl
 *                                                                           *
 * Copyright (c) 2010, Nikhef & VU. All rights reserved.
 *           
 * Redistribution and use in source and binary forms,                        *
 * with or without modification, are permitted according to the terms        *
 * listed in LICENSE (http://roofit.sourceforge.net/license.txt)             *
 *****************************************************************************/

//////////////////////////////////////////////////////////////////////////////
//
// This PDF is a variation on the  Parametric Step Function PDF which uses 
// a binned distribution whose parameters are the heights of each bin.  
// In this case, the PDF starts with the entire interval of some underlying
// RooAbsReal, and one then adds a threshold, with a corresponding efficiency
// parameter, which represents the fraction of events between the previous
// threshold, and the just added threshold. For now, we build up left 
// to right...
// 
//
				

#include "RooFit.h"
#include "Riostream.h"
#include "RooMsgService.h"

#include "P2VV/RooThresholdPdf.h"
#include "RooAbsRealLValue.h"
#include "RooRealVar.h"


//_____________________________________________________________________________
RooThresholdPdf::RooThresholdPdf(const char* name, const char* title, RooAbsRealLValue& x)
  : RooAbsPdf(name, title)
  , _x("x", "Dependent", this, x)
  , _coefList("coefList","List of coefficients",this)
  , _bins(1,x.getMin(),x.getMax())
{
  // Constructor
  _coefIter = _coefList.createIterator() ;

}



//_____________________________________________________________________________
RooThresholdPdf::RooThresholdPdf(const RooThresholdPdf& other, const char* name) :
  RooAbsPdf(other, name), 
  _x("x", this, other._x), 
  _coefList("coefList",this,other._coefList),
  _bins(other._bins,name)
{
  // Copy constructor
  _coefIter = _coefList.createIterator();
}



//_____________________________________________________________________________
RooThresholdPdf::~RooThresholdPdf()
{
  // Destructor
  delete _coefIter ;
}

Bool_t RooThresholdPdf::addThreshold(Double_t upperLimit, RooAbsReal& eps) {
    // TODO: make sure upperLimit is below any other prior value... 
    //       and below the max value of the range 
    _bins.addBoundary(upperLimit);
    _coefList.add(eps);
    return kTRUE;
}

//_____________________________________________________________________________
Int_t RooThresholdPdf::getAnalyticalIntegral(RooArgSet& allVars, RooArgSet& analVars, const char* /*rangeName*/) const 
{
  if (matchArgs(allVars, analVars, _x)) return 1;
  return 0;
}

//_____________________________________________________________________________
Double_t RooThresholdPdf::analyticalIntegral(Int_t code, const char* rangeName) const 
{
  if (code != 1) {
    coutF(InputArguments) << "RooThresholdPdf::analyticalIntegral("
        << GetName() << "): integration code should be 1 (got " << code << ")"
        << endl;
    assert(0);
  }

  Double_t xmin = _x.min(rangeName) ;
  Double_t xmax = _x.max(rangeName) ;

  Double_t sum=0 ;
  for (Int_t i=0 ; i<_bins.numBins() ; ++i) {
    Double_t binVal = (i<_coefList.getSize()) ? (static_cast<RooAbsReal*>(_coefList.at(i))->getVal()) : lastBinValue() ;
    // binVal is _area_ (aka efficiency) of this bin!!!
    if ( xmin<=_bins.binLow(i) && _bins.binHigh(i)<=xmax) { // Bin fully in the integration domain
      sum += binVal;
    } else if (_bins.binLow(i)<=xmin && xmax<=_bins.binHigh(i)) { // Domain is fully contained in this bin
      sum += binVal*(xmax-xmin)/_bins.binWidth(i);
      // Exit here, this is the last bin to be processed by construction
      break;
    } else if (_bins.binLow(i)<=xmin && _bins.binHigh(i)<=xmax && xmin<=_bins.binHigh(i)) { // Lower domain boundary is in bin
      sum +=  binVal*(_bins.binHigh(i)-xmin)/_bins.binWidth(i) ;
    } else if (xmin<=_bins.binLow(i) && xmax<=_bins.binHigh(i) && _bins.binLow(i)<=xmax) { // Upper domain boundary is in bin
      sum +=  binVal*(xmax-_bins.binLow(i))/_bins.binWidth(i);
      // Exit here, this is the last bin to be processed by construction
      break;
    }
  }
  return sum;  
}

//_____________________________________________________________________________
Double_t RooThresholdPdf::lastBinValue() const
{
  Double_t l(1.);
  RooAbsReal *x(0);
  _coefIter->Reset();
  while ( (x = (RooAbsReal*)_coefIter->Next()) )  l -= x->getVal();
  return l;
}



//_____________________________________________________________________________
Double_t RooThresholdPdf::evaluate() const 
{
  if (_x < _bins.lowBound() || _x > _bins.highBound() ) return 0;
  Int_t nr = _bins.binNumber(_x);
  assert (nr<_bins.numBins());
  Double_t bw = _bins.binWidth(nr);
  _coefIter->Reset();
  RooAbsReal *x(0);
  do { x = (RooAbsReal*)_coefIter->Next(); } while (nr-->0)  ;
  return (x!=0 ? x->getVal() : lastBinValue())/bw;
}

// TODO: add custom generator
