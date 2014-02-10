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
// PDF with explicitly specified normalization function. The function and the
// normalization function of the PDF are specified separately.
// END_HTML
//

#include <iostream>
using std::endl;

#include "Riostream.h"
#include "RooMsgService.h"
#include "TMath.h"
#include "P2VV/RooExplicitNormPdf.h"
#include "RooArgSet.h"
#include "RooCustomizer.h"
#include "RooRealVar.h"
#include "RooAbsPdf.h"

//_____________________________________________________________________________
RooExplicitNormPdf::RooExplicitNormPdf(const char *name, const char* title,
      const RooArgSet& obsSet, const RooAbsReal& function,
      const RooAbsReal& normFunc, Double_t normFactor,
      Bool_t integrateNormFunc) :
  RooAbsReal(name, title),
  _obsSet("obsSet", "observables set", this),
  _parSet("parSet", "parameters set", this),
  _functionOrig(&function),
  _normFuncOrig(&normFunc),
  _function(0),
  _normFunc(0),
  _functionInteg(0),
  _normFuncInteg(0),
  _normFactor(normFactor),
  _projData(0),
  _integNormFunc(integrateNormFunc)
{
  initVariables(&obsSet, 0);
}

//_____________________________________________________________________________
RooExplicitNormPdf::RooExplicitNormPdf(const char *name, const char* title,
      const RooArgSet& obsSet, const RooAbsReal& function,
      const RooAbsReal& normFunc, Double_t normFactor,
      const RooAbsData& projectionData, Bool_t integrateNormFunc) :
  RooAbsReal(name, title),
  _obsSet("obsSet", "observables set", this),
  _parSet("parSet", "parameters set", this),
  _functionOrig(&function),
  _normFuncOrig(&normFunc),
  _function(0),
  _normFunc(0),
  _functionInteg(0),
  _normFuncInteg(0),
  _normFactor(normFactor),
  _projData(&projectionData),
  _integNormFunc(integrateNormFunc)
{
  initVariables(&obsSet, 0);
}

//_____________________________________________________________________________
RooExplicitNormPdf::RooExplicitNormPdf(const char *name, const char* title,
      const RooArgSet& obsSet, const RooArgSet& intObsSet,
      const RooAbsReal& function, const RooAbsReal& normFunc,
      Double_t normFactor, Bool_t integrateNormFunc) :
  RooAbsReal(name, title),
  _obsSet("obsSet", "observables set", this),
  _parSet("parSet", "parameters set", this),
  _functionOrig(&function),
  _normFuncOrig(&normFunc),
  _function(0),
  _normFunc(0),
  _functionInteg(0),
  _normFuncInteg(0),
  _normFactor(normFactor),
  _projData(0),
  _integNormFunc(integrateNormFunc)
{
  initVariables(&obsSet, &intObsSet);
}

//_____________________________________________________________________________
RooExplicitNormPdf::RooExplicitNormPdf(const char *name, const char* title,
      const RooArgSet& obsSet, const RooArgSet& intObsSet,
      const RooAbsReal& function, const RooAbsReal& normFunc,
      Double_t normFactor, const RooAbsData& projectionData,
      Bool_t integrateNormFunc) :
  RooAbsReal(name, title),
  _obsSet("obsSet", "observables set", this),
  _parSet("parSet", "parameters set", this),
  _functionOrig(&function),
  _normFuncOrig(&normFunc),
  _function(0),
  _normFunc(0),
  _functionInteg(0),
  _normFuncInteg(0),
  _normFactor(normFactor),
  _projData(&projectionData),
  _integNormFunc(integrateNormFunc)
{
  initVariables(&obsSet, &intObsSet);
}

//_____________________________________________________________________________
RooExplicitNormPdf::RooExplicitNormPdf(const RooExplicitNormPdf& other,
      const char* name) :
  RooAbsReal(other, name),
  _obsSet("obsSet", this, other._obsSet),
  _intObsSet(other._intObsSet),
  _parSet("parSet", this, other._parSet),
  _functionOrig(other._functionOrig),
  _normFuncOrig(other._normFuncOrig),
  _function(0),
  _normFunc(0),
  _functionInteg(0),
  _normFuncInteg(0),
  _normFactor(other._normFactor),
  _projData(other._projData),
  _integNormFunc(other._integNormFunc)
{}

//_____________________________________________________________________________
RooExplicitNormPdf::~RooExplicitNormPdf()
{
  if (_function != 0 && _function != _functionOrig) delete _function;
  if (_normFunc != 0 && _normFunc != _normFuncOrig) delete _normFunc;
}

//_____________________________________________________________________________
Double_t RooExplicitNormPdf::evaluate() const
{
  if (_function == 0) initFunctions();

  Double_t intVal = 0.;
  if (_projData == 0) {
    // get PDF integral value
    intVal = _function->getVal() / _normFunc->getVal();
  } else {
    // get data-weighted average for PDF integral with projection data set
    for (Int_t dataIt = 0; dataIt < _projData->numEntries(); ++dataIt) {
      _projData->get(dataIt);
      intVal += _projData->weight() * _function->getVal() / _normFunc->getVal();
    }
    intVal /= _projData->sumEntries();
    cout << "." << flush;
  }

  return _normFactor * intVal;
}

//_____________________________________________________________________________
void RooExplicitNormPdf::initVariables(const RooArgSet* obsSet,
    const RooArgSet* intObsSet)
{
  // get variables in functions
  RooArgSet* functionVars = _functionOrig->getVariables();
  RooArgSet* normFuncVars = _normFuncOrig->getVariables();

  // set observables
  RooAbsArg* var = 0;
  TIterator* varIter = obsSet->createIterator();
  while ((var = (RooAbsArg*)varIter->Next()) != 0) {
    if (functionVars->find(var->GetName()) == 0) {
      coutW(InputArguments) << "RooExplicitNormPdf::initVariables("
          << GetName()
          << "): specified function does not depend on observable \""
          << var->GetName() << "\"" << endl;
    }

    _obsSet.add(*var);
  }
  delete varIter;

  if (intObsSet != 0) {
    // set integration observables
    varIter = intObsSet->createIterator();
    while ((var = (RooAbsArg*)varIter->Next()) != 0) {
      if (_obsSet.find(var->GetName()) != 0) {
        coutW(InputArguments) << "RooExplicitNormPdf::initVariables("
            << GetName() << "): integration observable \"" << var->GetName()
            << "\" is in the set of observables; not integrating over \""
            << var->GetName() << "\"" << endl;
        continue;
      }

      _intObsSet.add(*var);
    }
    delete varIter;
  }

  // set parameters
  RooArgSet parSet(*functionVars);
  parSet.add(*normFuncVars, kTRUE);
  parSet.remove(_obsSet, kFALSE, kTRUE);
  parSet.remove(_intObsSet, kFALSE, kTRUE);
  if (_projData) parSet.remove(*_projData->get(), kFALSE, kTRUE);
  _parSet.add(parSet);
}

//_____________________________________________________________________________
void RooExplicitNormPdf::initFunctions() const
{
  _functionOrig->getVal();
  _normFuncOrig->getVal();
  if (_function != 0 && _function != _functionOrig)
    delete _function;
  if (_normFunc != 0 && _normFunc != _normFuncOrig)
    delete _normFunc;
  if (_functionInteg != 0 && _functionInteg != _functionOrig)
    delete _functionInteg;
  if (_normFuncInteg != 0 && _normFuncInteg != _normFuncOrig)
    delete _normFuncInteg;

  // create customizers for the function and normalization function
  RooCustomizer funcCust(*_functionOrig, "function");
  RooCustomizer normCust(*_normFuncOrig, "normFunc");

  RooArgSet* funcObsSet = _functionOrig->getVariables();
  RooArgSet* normObsSet = _normFuncOrig->getVariables();
  RooAbsArg* obs = 0;

  // replace observables in functions by observables in our set
  TIterator* obsIter = _obsSet.createIterator();
  while ((obs = (RooAbsArg*)obsIter->Next()) != 0) {
    RooAbsArg* obsFunc = funcObsSet->find(obs->GetName());
    if (obsFunc != 0) funcCust.replaceArg(*obsFunc, *obs);
    obsFunc = normObsSet->find(obs->GetName());
    if (obsFunc != 0) normCust.replaceArg(*obsFunc, *obs);
  }
  delete obsIter;

  if (_projData) {
    // replace observables in functions by observables in projection data set
    obsIter = _projData->get()->createIterator();
    while ((obs = (RooAbsArg*)obsIter->Next()) != 0) {
      if (_obsSet.find(obs->GetName()) != 0) continue;

      RooAbsArg* obsFunc = funcObsSet->find(obs->GetName());
      if (obsFunc != 0) funcCust.replaceArg(*obsFunc, *obs);
      obsFunc = normObsSet->find(obs->GetName());
      if (obsFunc != 0) normCust.replaceArg(*obsFunc, *obs);
    }
    delete obsIter;

    coutI(Eval) << "RooExplicitNormPdf::initFunctions(" << GetName()
        << "): evaluating integral as a data-weighted average with data set \""
        << _projData->GetName() << "\": " << setprecision(1) << fixed
        << _projData->numEntries() << " (" << _projData->sumEntries()
        << ") entries" << endl;
    coutI(Eval) << "conditional observables: ";
    _projData->get()->Print();
  }

  delete funcObsSet;
  delete normObsSet;

  // build functions with replaced observables
  _functionInteg = (RooAbsReal*)funcCust.build();
  _normFuncInteg = (RooAbsReal*)normCust.build();

  if (_intObsSet.getSize() > 0) {
    // integrate over observables in integration set
    RooArgSet normIntSet(_intObsSet);
    if (_integNormFunc) normIntSet.add(_obsSet);
    _function = _functionInteg->createIntegral(_intObsSet);
    _normFunc = _normFuncInteg->createIntegral(normIntSet);

  } else {
    _function = _functionInteg;
    if (_integNormFunc) _normFunc = _normFuncInteg->createIntegral(_obsSet);
    else _normFunc = _normFuncInteg;
  }
}
