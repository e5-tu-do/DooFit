/*****************************************************************************
 * Project: RooFit                                                           *
 * Package: RooFitModels                                                     *
 *    File: $Id$
 * Authors:                                                                  *
 *   GR, Gerhard Raven,   Nikhef & VU, Gerhard.Raven@nikhef.nl
 *                                                                           *
 * Copyright (c) 2010, Nikhef & VU. All rights reserved.
 *           
 *                                                                           *
 * Redistribution and use in source and binary forms,                        *
 * with or without modification, are permitted according to the terms        *
 * listed in LICENSE (http://roofit.sourceforge.net/license.txt)             *
 *****************************************************************************/
#ifndef ROO_ThresholdPdf
#define ROO_ThresholdPdf

#include "RooAbsPdf.h"
#include "RooRealProxy.h"
#include "RooListProxy.h"
#include "RooBinning.h"

class RooRealVar;
class RooArgList ;

class RooThresholdPdf : public RooAbsPdf {
public:

  RooThresholdPdf(const char *name, const char *title, RooAbsRealLValue& x);

  RooThresholdPdf(const RooThresholdPdf& other, const char* name = 0);
  virtual TObject* clone(const char* newname) const { return new RooThresholdPdf(*this, newname); }
  virtual ~RooThresholdPdf() ;

  Int_t getAnalyticalIntegral(RooArgSet& allVars, RooArgSet& analVars, const char* rangeName=0) const ;
  Double_t analyticalIntegral(Int_t code, const char* rangeName=0) const ;

  Bool_t addThreshold(Double_t upperLimit, RooAbsReal& eps);
  const RooBinning& getBinning() const { return _bins; }

protected:

  Double_t lastBinValue() const ;

  RooRealProxy _x;
  RooListProxy _coefList ;
  RooBinning  _bins;
  TIterator* _coefIter ;  //! do not persist

  Double_t evaluate() const;

  ClassDef(RooThresholdPdf,1) // Threshold Pdf
};

#endif
