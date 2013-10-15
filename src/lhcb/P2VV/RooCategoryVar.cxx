/***************************************************************************** 
 * Project: RooFit                                                           * 
 *                                                                           * 
 * Authors:                                                                  *
 *   JvL, Jeroen van Leerdam, Nikhef, j.van.leerdam@nikhef.nl                *
 *                                                                           *
 * Copyright (c) 2013, Nikhef. All rights reserved.                          *
 *                                                                           *
 * Redistribution and use in source and binary forms,                        *
 * with or without modification, are permitted according to the terms        *
 * listed in LICENSE (http://roofit.sourceforge.net/license.txt)             *
 *****************************************************************************/ 

#include <memory>

#include "TIterator.h"
#include "Riostream.h"
#include "RooMsgService.h"
#include "RooAbsReal.h" 
#include "RooAbsCategory.h" 
#include "P2VV/RooCategoryVar.h" 

//_____________________________________________________________________________
RooCategoryVar::RooCategoryVar(const char* name, const char* title,
    RooAbsCategory& category, const RooArgList& variables) :
  RooAbsReal(name, title),
  _cat("cat", "category", this, category),
  _vars("vars", "variables", this)
{
  _vars.add(variables);
}

//_____________________________________________________________________________
RooCategoryVar::RooCategoryVar(const RooCategoryVar& other, const char* name) :
  RooAbsReal(other, name),
  _cat("cat", this, other._cat),
  _vars("vars", this, other._vars)
{}

//_____________________________________________________________________________
Double_t RooCategoryVar::evaluate() const
{
  if (_varIndices.empty()) initVarsMap();
  return static_cast<RooAbsReal*>(_vars.at(_varIndices[_cat]))->getVal();
}

void RooCategoryVar::initVarsMap() const
{
  if (_cat.arg().numTypes() != _vars.getSize()) {
    coutF(InputArguments) << "RooCategoryVar::initVarsMap(" << GetName()
        << "): number of variables does not match number of category types"
        << endl;
    assert(0);
  }

  std::auto_ptr<TIterator> stateIt(_cat.arg().typeIterator());
  RooCatType* state(0);
  Int_t varsIter(-1);
  _varIndices.clear();
  while ((state = static_cast<RooCatType*>(stateIt->Next())) != 0) {
    RooAbsReal* var(dynamic_cast<RooAbsReal*>(_vars.at(++varsIter)));
    if (var == 0) {
      coutF(InputArguments) << "RooCategoryVar::initVarsMap(" << GetName()
          << "): variable at index " << varsIter
          << " in list does not inherit from \"RooAbsReal\"" << endl;
      assert(0);
    }
    _varIndices[state->getVal()] = varsIter;
  }
}
