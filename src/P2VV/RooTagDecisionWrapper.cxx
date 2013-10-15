/*****************************************************************************
 * Project: RooFit                                                           *
 * Package: RooFitModels                                                     *
 *    File: $Id$
 * Authors:                                                                  *
 *   JvL, Jeroen van Leerdam, Nikhef,      j.van.leerdam@nikhef.nl           *
 *   GR,  Gerhard Raven,      Nikhef & VU, Gerhard.Raven@nikhef.nl           *
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
// Wrapper for a "tag decision" flavour tagging category with indices 0, +1 and
// -1. The wrapped index is equal to the tag decision index for +1 and -1. For
// index 0, the wrapped index is randomly set to +1 or -1 with equal
// probabilities.
// END_HTML
//

#include "Riostream.h"
#include "RooMsgService.h"
#include "RooRandom.h"
#include "P2VV/RooTagDecisionWrapper.h"

#include <iostream>
using std::endl;

//_____________________________________________________________________________
RooTagDecisionWrapper::RooTagDecisionWrapper(const char *name,
    const char* title, RooAbsCategory& tagDecision) :
  RooAbsCategory(name, title),
  _tagDecision("tagDecision", "Tag Decision", this, tagDecision)
{
  // constructor

  // check tag decision types
  if (tagDecision.numTypes() != 3 || !tagDecision.isValidIndex(+1)
      || !tagDecision.isValidIndex(-1) || !tagDecision.isValidIndex(0)) {
    coutF(InputArguments) << "RooTagDecisionWrapper::RooTagDecisionWrapper("
        << GetName()
        << ") expected types 0, +1 and -1 for a tag decision category"
        << endl;
    assert(0);
  }

  // set B and Bbar types
  defineType(tagDecision.lookupType(+1)->GetName(), +1);
  defineType(tagDecision.lookupType(-1)->GetName(), -1);
}

//_____________________________________________________________________________
RooTagDecisionWrapper::RooTagDecisionWrapper(
    const RooTagDecisionWrapper& other, const char* name) :
  RooAbsCategory(other, name),
  _tagDecision("tagDecision", this, other._tagDecision)
{
  // copy constructor
}

//_____________________________________________________________________________
RooTagDecisionWrapper::~RooTagDecisionWrapper()
{
  // destructor
}

//_____________________________________________________________________________
RooCatType RooTagDecisionWrapper::evaluate() const
{
  // calculate and return the wrapped category value

  // get index
  Int_t index = _tagDecision.arg().getIndex();

  if (index == 0) {
    // set index randomly to either +1 or -1
    if (RooRandom::uniform() > 0.5) index = +1;
    else index = -1;
  }

  // return the category type corresponding to the index
  return *lookupType(index);
}
