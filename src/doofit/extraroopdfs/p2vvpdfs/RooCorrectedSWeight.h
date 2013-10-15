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

#ifndef ROO_CORRECTED_SWEIGHT
#define ROO_CORRECTED_SWEIGHT

#include <vector>
#include "RooAbsReal.h"
#include "RooRealProxy.h"
#include "RooCategoryProxy.h"

class RooAbsCategory;

class RooCorrectedSWeight : public RooAbsReal
{

public:
  inline RooCorrectedSWeight() {}

  RooCorrectedSWeight(const char *name, const char *title,
      RooAbsReal& sWeight, Double_t corrFactor,
      Bool_t bkgWeight = kFALSE);

  RooCorrectedSWeight(const char *name, const char *title,
      RooAbsReal& sWeight, RooAbsCategory& splitCat,
      std::vector<Double_t> corrFactors, Bool_t bkgWeight = kFALSE);

  RooCorrectedSWeight(const RooCorrectedSWeight& other, const char* name = 0);

  virtual TObject* clone(const char* newname) const 
  { 
    return new RooCorrectedSWeight(*this, newname);
  }

  ~RooCorrectedSWeight();

  Int_t position() const;
  Double_t correctionFactor() const {return _corrFactors.at(position());}
  Double_t sigSWeight() const {return _bkgWeight ? 1. - _sWeight : _sWeight;}
  Bool_t bkgWeight() const {return _bkgWeight;}

private:
  const RooRealProxy _sWeight;
  const RooCategoryProxy _splitCat;
  std::vector<Double_t> _corrFactors;
  Bool_t _bkgWeight;

  mutable std::map<Int_t, Int_t> _positionMap; //!

  virtual Double_t evaluate() const;

  ClassDef(RooCorrectedSWeight, 1) // corrects sWeights (-ln(L)) for background dilution
};

#endif

