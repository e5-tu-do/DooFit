/*****************************************************************************
 * Project: RooFit                                                           *
 * Package: RooFitModels                                                     *
 *    File: $Id$
 * Authors:                                                                  *
 *   JvL, Jeroen van Leerdam, Nikhef, j.van.leerdam@nikhef.nl                *
 *                                                                           *
 * Copyright (c) 2012, Nikhef. All rights reserved.                          *
 *                                                                           *
 * Redistribution and use in source and binary forms,                        *
 * with or without modification, are permitted according to the terms        *
 * listed in LICENSE (http://roofit.sourceforge.net/license.txt)             *
 *****************************************************************************/

#ifndef ROO_BOX_PDF
#define ROO_BOX_PDF

#include "RooAbsPdf.h"
#include "RooRealProxy.h"

class RooRealVar;
class RooAbsReal;
class RooArgSet;

class RooBoxPdf : public RooAbsPdf
{

public:
  RooBoxPdf() {};

  RooBoxPdf(const char *name, const char *title, RooRealVar& var,
      RooAbsReal& mean, RooAbsReal& width);

  RooBoxPdf(const RooBoxPdf& other, const char* name = 0);

  virtual TObject* clone(const char* newname) const 
  { 
    return new RooBoxPdf(*this, newname);
  }

  inline virtual ~RooBoxPdf() {};

  Double_t evaluate() const;

  Int_t getAnalyticalIntegral(RooArgSet& allVars, RooArgSet& analVars,
     const char* rangeName) const;
  Double_t analyticalIntegral(Int_t code, const char* rangeName) const;

protected:
  RooRealProxy _var;
  RooRealProxy _mean;
  RooRealProxy _width;

private:
  ClassDef(RooBoxPdf, 1) // PDF with a non-zero value around the mean and a zero value everywhere else
};

#endif

