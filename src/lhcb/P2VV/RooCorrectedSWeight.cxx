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


//////////////////////////////////////////////////////////////////////////////
//
// BEGIN_HTML
// Corrects a given signal sWeight with a correction factor for background
// dilution. Optionally, a correction factor is given for each state of the
// split category in a simultaneous fit. Either the signal sWeight or the
// background sWeight is specified.
// END_HTML
//

#include "Riostream.h"
#include "RooMsgService.h"
#include "RooAbsCategory.h"
#include "P2VV/RooCorrectedSWeight.h"

//_____________________________________________________________________________
RooCorrectedSWeight::RooCorrectedSWeight(const char *name, const char* title,
  RooAbsReal& sWeight, Double_t corrFactor, Bool_t bkgWeight) :
  RooAbsReal(name, title),
  _sWeight("sWeight", "sWeight", this, sWeight),
  _splitCat("splitCat", "split category", this),
  _corrFactors(std::vector<Double_t>(1, corrFactor)),
  _bkgWeight(bkgWeight)
{
  // constructor without split category
}

//_____________________________________________________________________________
RooCorrectedSWeight::RooCorrectedSWeight(const char *name, const char* title,
  RooAbsReal& sWeight, RooAbsCategory& splitCat,
  std::vector<Double_t> corrFactors, Bool_t bkgWeight) :
  RooAbsReal(name, title),
  _sWeight("sWeight", "sWeight", this, sWeight),
  _splitCat("splitCat", "split category", this, splitCat),
  _corrFactors(corrFactors),
  _bkgWeight(bkgWeight)
{
  // constructor with split category

  // check number of correction factors
  assert((Int_t)corrFactors.size() == splitCat.numTypes());
}

//_____________________________________________________________________________
RooCorrectedSWeight::RooCorrectedSWeight(
    const RooCorrectedSWeight& other, const char* name) :
  RooAbsReal(other, name),
  _sWeight("sWeight", this, other._sWeight),
  _splitCat("splitCat", this, other._splitCat),
  _corrFactors(other._corrFactors),
  _bkgWeight(other._bkgWeight),
  _positionMap(other._positionMap)
{
  // copy constructor
}

//_____________________________________________________________________________
RooCorrectedSWeight::~RooCorrectedSWeight()
{
  // destructor
}

Int_t RooCorrectedSWeight::position() const
{
  if (_splitCat.absArg() != 0 && _positionMap.empty()) {
    // create map from split category indices to positions in factors vector
    TIterator*  catIter  = _splitCat.arg().typeIterator();
    RooCatType* catType  = 0;
    Int_t       position = 0;
    while ((catType = (RooCatType*)catIter->Next()) != 0) {
      Int_t catIndex = catType->getVal();
      _positionMap[catIndex] = position;
      ++position;
    }

    delete catIter;
  }

  // return position
  return _splitCat.absArg() != 0
      ? _positionMap.find(_splitCat.arg().getIndex())->second : 0;
}

//_____________________________________________________________________________
Double_t RooCorrectedSWeight::evaluate() const
{
  // calculate and return the corrected signal sWeight
  return sigSWeight() * correctionFactor();
}
