/*****************************************************************************
 * Project: RooFit                                                           *
 * Package: RooFitModels                                                     *
 *    File: $Id$
 * Authors:                                                                  *
 *   JvL, Jeroen van Leerdam, Nikhef, j.van.leerdam@nikhef.nl                *
 *   RA,  Roel Aaij,          Nikhef,                                        *
 *                                                                           *
 * Copyright (c) 2011, Nikhef. All rights reserved.                          *
 *                                                                           *
 * Redistribution and use in source and binary forms,                        *
 * with or without modification, are permitted according to the terms        *
 * listed in LICENSE (http://roofit.sourceforge.net/license.txt)             *
 *****************************************************************************/


//////////////////////////////////////////////////////////////////////////////
//
// BEGIN_HTML
// <p>RooBinnedPdf is a binned PDF with an arbitrary number of binnings.
// Optionally, the bins are associated with values of continuous variables
// (which results in a step function PDF).</p>
//
// <p>A coefficient is specified by the user for each bin. The PDF function's
// value is determined with the value of a particular bin's coefficient. In
// case of multiple binnings, the function's value equals the product of the
// values of the binnings. The coefficients are either interpreted as bin
// heights or as bin integrals.</p>
//
// <p>The user may either specify coefficients for all the bins of a binning or
// leave it to the function to calculate the value for bin 0. In the former
// case, the coefficients may have any value. In the latter case, the value of
// the coefficient of bin 0 is equal to one minus the sum of the other bin
// coefficients. Negative bin values are then considered to be zero.  If the
// sum of the specified bin coefficients is larger than one, the coefficient of
// bin 0 equals zero and the other coefficients are scaled such that their sum
// yields one.</p>
// END_HTML
//

#include "RooAbsCategory.h"
#include "RooAbsRealLValue.h"
#include "P2VV/RooBinnedPdf.h"
#include "RooBinningCategory.h"
#include "RooArgSet.h"
#include "RooMsgService.h"
#include "TMath.h"
#include "TObjArray.h"
#include "TObjString.h"

#include <memory>

using namespace std;

//_____________________________________________________________________________
RooBinnedPdf::RooBinnedPdf(const char* name, const char* title,
    RooAbsCategory& baseCat, const RooArgList& coefList) :
  RooAbsPdf(name, title),
  _numCats(1),
  _baseCatsList(TString(name) + "_baseCatsList", 0, this),
  _baseVarsList(TString(name) + "_baseVarsList", 0, this),
  _coefLists(1, 0),  
  _function(TString(name) + "_func", TString(name) + "_func", this),
  _continuousBase(kFALSE),
  _forceUnitIntegral(kFALSE),
  _binIntegralCoefs(kTRUE),
  _ignoreFirstBin(kFALSE)
{
  // constructor with one binning, which depends on the value of a category
  //
  // The "current bin" is given by the value of the RooAbsCategory "baseCat".
  // The number of bins is given by the number of category types (N).
  //
  // Bin coefficients are specified in the list "coefList" and inherit from
  // RooAbsReal. One may either specify N or N - 1 coefficients (see also the
  // class description). The order of the coefficients is given by the indices
  // of "baseCat".
  //
  // If "ignoreFirstBin" is true, evaluation of the value of the first bin
  // always yields zero.

  // add category to list of base categories
  _baseCatsList.add(baseCat);

  // initialize coefficients
  TObjArray coefLists(1, 0);
  coefLists.Add(coefList.clone(TString(name) + "_" + coefList.GetName()));
  initCoefs(coefLists);
}

//_____________________________________________________________________________
RooBinnedPdf::RooBinnedPdf(const char* name, const char* title,
    const RooArgList& baseCats, const RooArgList& coefList) :
  RooAbsPdf(name, title),
  _numCats(0),
  _baseCatsList(TString(name) + "_baseCatsList", 0, this),
  _baseVarsList(TString(name) + "_baseVarsList", 0, this),
  _coefLists(1, 0),
  _function(TString(name) + "_func", TString(name) + "_func", this),
  _continuousBase(kFALSE),
  _forceUnitIntegral(kFALSE),
  _binIntegralCoefs(kTRUE),
  _ignoreFirstBin(kFALSE)
{
  // constructor with an arbitrary number of binnings, which depend on the
  // values of an equal number of categories
  //
  // The "current bin" is given by the values of the RooAbsCategories contained
  // by "baseCats". Exactly one category is specified for each binning. The
  // number of bins in a binning is given by the number of category types
  // (N_i).
  //
  // Bin coefficients are specified in the RooArgList "coefList".  The
  // coefficients inherit from RooAbsReal. One may either specify N_i * N_j *
  // ... * N_n or N_i * N_j * ... * N_k - 1 coefficients (see also the class
  // description). The order of the coefficients is given by the indices of the
  // "baseCats".

  // loop over input categories
  RooFIter catIter = baseCats.fwdIterator();
  RooAbsArg* cat = 0;
  while ((cat = (RooAbsArg*)catIter.next()) != 0) {
    // check if this is a category
    if (dynamic_cast<RooAbsCategory*>(cat) == 0) {
      coutF(InputArguments) << "RooBinnedPdf::RooBinnedPdf("
          << GetName() << ") base category '" << cat->GetName()
          << "' is not a RooAbsCategory" << endl;
      reset();
      break;
    }

    // add category to list of base categories
    ++_numCats;
    _baseCatsList.add(*cat);
  }

  // initialize coefficients
  if (_numCats > 0) {
    TObjArray coefLists(1, 0);
    coefLists.Add(coefList.clone(TString(name) + "_" + coefList.GetName()));
    initCoefs(coefLists, kFALSE);
  }
}

//_____________________________________________________________________________
RooBinnedPdf::RooBinnedPdf(const char* name, const char* title,
    const RooArgList& baseCats, const TObjArray& coefLists,
    Bool_t ignoreFirstBin) :
  RooAbsPdf(name, title),
  _numCats(0),
  _baseCatsList(TString(name) + "_baseCatsList", 0, this),
  _baseVarsList(TString(name) + "_baseVarsList", 0, this),
  _coefLists(1, 0),
  _function(TString(name) + "_func", TString(name) + "_func", this),
  _continuousBase(kFALSE),
  _forceUnitIntegral(kFALSE),
  _binIntegralCoefs(kTRUE),
  _ignoreFirstBin(ignoreFirstBin)
{
  // constructor with an arbitrary number of binnings, which depend on the
  // values of an equal number of categories
  //
  // The "current bins" are given by the values of the RooAbsCategories
  // contained by "baseCats". Exactly one category is specified for each
  // binning. The number of bins in a binning is given by the number of
  // category types (N_i).
  //
  // Bin coefficients are specified in the RooArgLists contained by coefLists.
  // The coefficients inherit from RooAbsReal. One may either specify N_i or
  // N_i - 1 coefficients (see also the class description). The order of the
  // coefficients is given by the indices of the "baseCats".
  //
  // If "ignoreFirstBin" is true, evaluation of the function value always
  // yields zero in the case that all the "current bins" are the first bin of
  // the binnings.

  // loop over input categories
  RooFIter catIter = baseCats.fwdIterator();
  RooAbsArg* cat = 0;
  while ((cat = (RooAbsArg*)catIter.next()) != 0) {
    // check if this is a category
    if (dynamic_cast<RooAbsCategory*>(cat) == 0) {
      coutF(InputArguments) << "RooBinnedPdf::RooBinnedPdf("
          << GetName() << ") base category '" << cat->GetName()
          << "' is not a RooAbsCategory" << endl;
      reset();
      break;
    }

    // add category to list of base categories
    ++_numCats;
    _baseCatsList.add(*cat);
  }

  // initialize coefficients
  if (_numCats > 0) initCoefs(coefLists);
}

//_____________________________________________________________________________
RooBinnedPdf::RooBinnedPdf(const char* name, const char* title,
    RooAbsRealLValue& baseVar, const char* binningName,
    const RooArgList& coefList, Bool_t binIntegralCoefs) :
  RooAbsPdf(name, title),
  _numCats(0),
  _baseCatsList(TString(name) + "_baseCatsList", 0, this),
  _baseVarsList(TString(name) + "_baseVarsList", 0, this),
  _coefLists(1, 0),
  _function(TString(name) + "_func", TString(name) + "_func", this),
  _continuousBase(kTRUE),
  _forceUnitIntegral(kFALSE),
  _binIntegralCoefs(binIntegralCoefs),
  _ignoreFirstBin(kFALSE)
{
  // constructor with one binning, which depends on the value of a continuous
  // variable with a binning
  //
  // The "current bin" is given by the value of the RooAbsRealLValue "baseVar"
  // and its binning with N bins, given by "binningName".
  //
  // Bin coefficients are specified in the list "coefList" and inherit from
  // RooAbsReal. One may either specify N or N - 1 coefficients (see also the
  // class description). The order of the coefficients is given by the order of
  // the "baseVar" bins. The coefficients may either be interpreted as bin
  // heights or as bin integrals. This is controlled by "binIntegralCoefs".
  //
  // If "ignoreFirstBin" is true, evaluation of the value of the first bin
  // always yields zero.

  // put the base variable in a list
  RooArgList baseVars(baseVar);

  // put the binning name in a list
  TObjArray binningNames(1, 0);
  TObjString strName(binningName);
  binningNames.Add(&strName);

  // create base category and initialize coefficients
  if (createBaseCats(baseVars, binningNames) > 0) {
    TObjArray coefLists(1, 0);
    coefLists.Add(coefList.clone(TString(name) + "_" + coefList.GetName()));
    initCoefs(coefLists);
  }
}

//_____________________________________________________________________________
RooBinnedPdf::RooBinnedPdf(const char* name, const char* title,
    const RooArgList& baseVars, const TObjArray& binningNames,
    const RooArgList& coefList, Bool_t binIntegralCoefs) :
  RooAbsPdf(name, title),
  _numCats(0),
  _baseCatsList(TString(name) + "_baseCatsList", 0, this),
  _baseVarsList(TString(name) + "_baseVarsList", 0, this),
  _coefLists(1, 0),
  _function(TString(name) + "_func", TString(name) + "_func", this),
  _continuousBase(kTRUE),
  _forceUnitIntegral(kFALSE),
  _binIntegralCoefs(binIntegralCoefs),
  _ignoreFirstBin(kFALSE)
{
  // constructor with an arbitrary number of binnings, which depend on the
  // values of continuous variables with binnings
  //
  // The "current bin" is given by the values of the RooAbsRealLValues
  // contained by "baseVars" and their binnings with N_i bins, given by
  // "binningNames". Exactly one variable is specified for each binning. The
  // "binningNames" are TObjStrings.
  //
  // Bin coefficients are specified in the list "coefList". The coefficients
  // inherit from RooAbsReal. One may either specify N_i * N_j * ... * N_n or
  // N_i * N_j * ... * N_n - 1 coefficients (see also the class description).
  // The order of the coefficients is given by the order of the "baseVars"
  // bins. The coefficients may either be interpreted as bin heights or as bin
  // integrals.  This is controlled by "binIntegralCoefs".

  // create base categories and initialize coefficients
  if (createBaseCats(baseVars, binningNames) > 0) {
    TObjArray coefLists(1, 0);
    coefLists.Add(coefList.clone(TString(name) + "_" + coefList.GetName()));
    initCoefs(coefLists, kFALSE);
  }

}

//_____________________________________________________________________________
RooBinnedPdf::RooBinnedPdf(const char* name, const char* title,
    const RooArgList& baseVars, const TObjArray& binningNames,
    const TObjArray& coefLists, Bool_t binIntegralCoefs,
    Bool_t ignoreFirstBin) :
  RooAbsPdf(name, title),
  _numCats(0),
  _baseCatsList(TString(name) + "_baseCatsList", 0, this),
  _baseVarsList(TString(name) + "_baseVarsList", 0, this),
  _coefLists(1, 0),
  _function(TString(name) + "_func", TString(name) + "_func", this),
  _continuousBase(kTRUE),
  _forceUnitIntegral(kFALSE),
  _binIntegralCoefs(binIntegralCoefs),
  _ignoreFirstBin(ignoreFirstBin)
{
  // constructor with an arbitrary number of binnings, which depend on the
  // values of continuous variables with binnings
  //
  // The "current bins" are given by the values of the RooAbsRealLValues
  // contained by "baseVars" and their binnings with N_i bins, given by
  // "binningNames". Exactly one variable is specified for each binning. The
  // "binningNames" are TObjStrings.
  //
  // Bin coefficients are specified in the lists contained by "coefLists". The
  // coefficients inherit from RooAbsReal. One may either specify N_i or
  // N_i - 1 coefficients (see also the class description). The order of the
  // coefficients is given by the order of the "baseVars" bins. The
  // coefficients may either be interpreted as bin heights or as bin integrals.
  // This is controlled by "binIntegralCoefs".
  //
  // If "ignoreFirstBin" is true, evaluation of the function value always
  // yields zero in the case that all the "current bins" are the first bin of
  // the binnings.

  // create base categories and initialize coefficients
  if (createBaseCats(baseVars, binningNames) > 0) initCoefs(coefLists);
}

//_____________________________________________________________________________
RooBinnedPdf::RooBinnedPdf
(const char* name, const char* title, RooAbsArg& baseVar,
    const char* binning, RooAbsReal& function) :
  RooAbsPdf(name, title),
  _numCats(0),
  _baseCatsList(TString(name) + "_baseCatsList", 0, this),
  _baseVarsList(TString(name) + "_baseVarsList", 0, this),
  _coefLists(),
  _function(TString(name) + "_func", TString(name) + "_func", this, function),
  _continuousBase(kTRUE),
  _forceUnitIntegral(kFALSE),
  _binIntegralCoefs(kFALSE),
  _ignoreFirstBin(kFALSE)
{
  // constructor with a list of variables, the corresponding binnings to be
  // used and a function.
  //
  // The "current bins" are given by the values of the RooAbsRealLValues
  // contained by "baseVars" and their binnings with N_i bins, given by
  // "binningNames". Exactly one variable is specified for each binning. The
  // "binningNames" are TObjStrings.
  //
  // The supplied function will be evaluated a bin centers to give the value.

  std::auto_ptr<RooArgSet> vars(function.getVariables());
  assert(vars->contains(baseVar));
  _baseVarsList.add(baseVar);
  _binningNames.push_back(binning);
}

//_____________________________________________________________________________
RooBinnedPdf::RooBinnedPdf
(const char* name, const char* title, const RooArgList& baseVars,
    const TObjArray& binningNames, RooAbsReal& function)
   : RooAbsPdf(name, title),
     _numCats(0),
     _baseCatsList(TString(name) + "_baseCatsList", 0, this),
     _baseVarsList(TString(name) + "_baseVarsList", 0, this),
     _coefLists(),
     _function(TString(name) + "_func", TString(name) + "_func", this, function),
     _continuousBase(kTRUE),
     _forceUnitIntegral(kFALSE),
     _binIntegralCoefs(kFALSE),
     _ignoreFirstBin(kFALSE)
{
  // constructor with a list of variables, the corresponding binnings to be
  // used and a function.
  //
  // The "current bins" are given by the values of the RooAbsRealLValues
  // contained by "baseVars" and their binnings with N_i bins, given by
  // "binningNames". Exactly one variable is specified for each binning. The
  // "binningNames" are TObjStrings.
  //
  // The supplied function will be evaluated a bin centers to give the value.

  // create base categories and initialize coefficients
  assert(baseVars.getSize() == binningNames.GetEntries());

  std::auto_ptr<const RooArgSet> comps(function.getVariables());
  RooFIter baseVarsIter = baseVars.fwdIterator();
  RooAbsArg* arg = 0;
  while ((arg = static_cast<RooAbsArg*>(baseVarsIter.next()))) {
    assert(comps->contains(*arg));
  }

  _baseVarsList.add(baseVars);
  std::auto_ptr<TIterator> binningNamesIter(binningNames.MakeIterator());
  TObjString* binningName = 0;
  while ((binningName = static_cast<TObjString*>(binningNamesIter->Next()))) {
    _binningNames.push_back(binningName->GetString());
  }
}

//_____________________________________________________________________________
RooBinnedPdf::RooBinnedPdf(const RooBinnedPdf& other, const char* name)
   : RooAbsPdf(other, name),
     _numCats(other._numCats),
     _baseCatsList(TString(name) + "_baseCatsList", this, other._baseCatsList),
     _baseVarsList(TString(name) + "_baseVarsList", this, other._baseVarsList),
     _coefLists(_numCats, 0),
     _function(TString(name) + "_function", this, other._function),
     _indexPositions(other._indexPositions),
     _binningNames(other._binningNames),
     _calcCoefZeros(other._calcCoefZeros),
     _continuousBase(other._continuousBase),
     _forceUnitIntegral(other._forceUnitIntegral),
     _binIntegralCoefs(other._binIntegralCoefs),
     _ignoreFirstBin(other._ignoreFirstBin)
{
  // copy constructor

  // make coefficient lists array owner of its lists
  _coefLists.SetOwner(kTRUE);

  // copy coefficient lists
  for (Int_t cListIter = 0; cListIter < other._coefLists.GetEntries();
      ++cListIter) {
    // get other's coefficients list
    RooListProxy* cListOther
        = (RooListProxy*)other._coefLists.UncheckedAt(cListIter);

    // create new coefficients list
    TString cListName(GetName());
    cListName += "_coefList";
    cListName += cListIter;
    RooListProxy* cListProxy = new RooListProxy(cListName, this, *cListOther);
    _coefLists.Add(cListProxy);
  }
}

//_____________________________________________________________________________
RooBinnedPdf::~RooBinnedPdf()
{
  // destructor
}

//_____________________________________________________________________________
Int_t RooBinnedPdf::getAnalyticalIntegral(RooArgSet& allVars,
    RooArgSet& analVars, const char* rangeName) const
{
  if (_function.absArg() != 0
      || (_coefLists.GetEntries() == _numCats && _numCats != 1)
      || _numCats > 26)
    return 0;

  // determine integration code for independent bin coefficients (1 or x..x2)
  Int_t varsCode = 0;
  for (Int_t catIter = 0; catIter < _numCats; ++catIter) {
    if (_continuousBase) {
      // continuous variable dependence
      RooAbsRealLValue* var = (RooAbsRealLValue*)_baseVarsList.at(catIter);
      if (!allVars.contains(*var)
          || TMath::Abs(var->getMin(rangeName) - var->getMin()) > 1.e-10
          || TMath::Abs(var->getMax(rangeName) - var->getMax()) > 1.e-10)
        continue;
      analVars.add(*var);
    } else {
      // category dependence
      RooAbsCategory* cat = (RooAbsCategory*)_baseCatsList.at(catIter);
      if (!allVars.contains(*cat) || (rangeName != 0
          && cat->hasRange(rangeName)))
        continue;
      analVars.add(*cat);
    }

    varsCode += 1 << catIter;
  }

  // check if we have to integrate over all variables, calculate the
  // coefficient of bin 0 and coefficients are bin integrals
  if ((_forceUnitIntegral || (_calcCoefZeros[0] && (!_continuousBase
      || _binIntegralCoefs))) && varsCode == (1 << _numCats) - 1)
    return 1;

  // return integration code
  return 2 + (varsCode << 5);
}

//_____________________________________________________________________________
Double_t RooBinnedPdf::analyticalIntegral(Int_t code,
    const char* /*rangeName*/) const
{
  // integral over all variables equals 1 if the coefficient of bin 0 is
  // calculated and coefficients are bin integrals (or if integral=1 is forced)
  if (code == 1) return 1.;

  // determine the code that specifies integration variables
  Int_t varsCode = code >> 5;
  if (varsCode == 0) return getVal();

  if (code % 32 == 2) {
    // create vector of bin positions for categories
    std::vector<Int_t> binPos(_numCats, 0);
    if (_calcCoefZeros[0]) {
      for (Int_t catIter = 1; catIter < _numCats; ++catIter) {
        if (((RooAbsCategory*)_baseCatsList.at(catIter))->numTypes() > 1) {
          binPos[catIter] = 1;
          break;
        }
      }
    }

    // loop over coefficients and calculate integral sum
    Double_t integral = 0.;
    Double_t coefSum  = 0.;
    RooFIter coefIter = ((RooArgList*)_coefLists.UncheckedAt(0))->fwdIterator();
    RooAbsReal* coef(0);
    while ((coef = (RooAbsReal*)coefIter.next())) {
      // check if we have to add this bin's coefficient to the integral
      Bool_t addCoef = kTRUE;
      Double_t binVolumeFac = 1.;
      for (Int_t catIter = 0; catIter < _numCats; ++catIter) {
        if (addCoef && (varsCode & 1 << catIter) == 0) {
          // check if the category index corresponds to this bin
          std::map<Int_t, Int_t> indexMap = _indexPositions[catIter];
          Int_t cPos = indexMap[((RooAbsCategory*)_baseCatsList.at(catIter))
              ->getIndex()];

          if (cPos != binPos[catIter])
            // don't use coefficient's value
            addCoef = kFALSE;
        }

        if (addCoef && _continuousBase) {
          // update bin volume factor
          if (!_binIntegralCoefs && (varsCode & 1 << catIter) != 0)
            // multiply bin volume by bin width of this bin
            binVolumeFac *= ((RooAbsRealLValue*)_baseVarsList.at(catIter))
                ->getBinning(_binningNames[catIter]).binWidth(binPos[catIter]);
          else if (_binIntegralCoefs && (varsCode & 1 << catIter) == 0)
            // divide bin volume by bin width of this bin
            binVolumeFac /= ((RooAbsRealLValue*)_baseVarsList.at(catIter))
                ->getBinning(_binningNames[catIter]).binWidth(binPos[catIter]);
        }

        // update bin position for next iteration
        if (catIter == 0 || binPos[catIter - 1]
            == ((RooAbsCategory*)_baseCatsList.at(catIter - 1))->numTypes()) {
          binPos[catIter] = binPos[catIter] + 1;
          if (catIter != 0) binPos[catIter - 1] = 0;
        }
      }

      // get coefficient's value
      Double_t cVal = coef->getVal();

      // add value to sum
      if (_calcCoefZeros[0] && cVal > 0.) coefSum += cVal;

      // make negative values equal to zero, add positive values to integral
      if (addCoef && cVal > 0.) integral += cVal * binVolumeFac;
    }

    if (_calcCoefZeros[0]) {
      // return integral if bin 0 coefficient equals zero
      if (coefSum >= 1.) return integral /= coefSum;

      // check if we have to add the bin 0 coefficient to the integral
      Double_t binVolumeFac = 1.;
      for (Int_t catIter = 0; catIter < _numCats; ++catIter) {
        if ((varsCode & 1 << catIter) == 0) {
          // check if the category index corresponds to this bin
          std::map<Int_t, Int_t> indexMap = _indexPositions[catIter];
          Int_t cPos = indexMap[((RooAbsCategory*)_baseCatsList.at(catIter))
              ->getIndex()];
          if (cPos != 0)
            // don't use coefficient's value
            return integral;
        }

        if (_continuousBase) {
          // update bin volume factor
          if (!_binIntegralCoefs && (varsCode & 1 << catIter) != 0)
            // multiply bin volume by bin width of bin 0
            binVolumeFac *= ((RooAbsRealLValue*)_baseVarsList.at(catIter))
                ->getBinning(_binningNames[catIter]).binWidth(0);
          else if (_binIntegralCoefs && (varsCode & 1 << catIter) == 0)
            // divide bin volume by bin width of bin 0
            binVolumeFac /= ((RooAbsRealLValue*)_baseVarsList.at(catIter))
                ->getBinning(_binningNames[catIter]).binWidth(0);
        }
      }

      // add bin 0 value to integral and return integral
      return integral + (1. - coefSum) * binVolumeFac;
    }

    // return integral value
    return integral;

  }

  return 0.;
}

//_____________________________________________________________________________
Int_t RooBinnedPdf::getMaxVal(const RooArgSet& vars) const 
{
   // Only implemented 1 dimensional pdf for now.
   if (vars.getSize() != 1) {
      return 0;
   }
   RooAbsCollection* common = _baseVarsList.selectCommon(vars);
   if (common->getSize()==_baseVarsList.getSize()) {
      delete common;
      return 1;
   }
   // delete common;
   // common = _baseCatsList.selectCommon(vars);
   // if (common->getSize()==_baseCatsList.getSize()) {
   //    delete common;
   //    return 1;
   // }
   return 0;
}

//_____________________________________________________________________________
Double_t RooBinnedPdf::maxVal(Int_t code) const 
{
   // We only do 1D.
   if (code != 1) {
     coutF(InputArguments) << "RooBinnedPdf::maxVal(" << GetName()
         << "): integration code should be 1 (got" << code << ")" << endl;
     assert(0);
   }
   
   Double_t max = -1;
   

   if (_function.absArg() != 0) {
      Bool_t origState = inhibitDirty();
      setDirtyInhibit(kTRUE);
      
      RooAbsRealLValue* lvar = dynamic_cast<RooAbsRealLValue*>(_baseVarsList.at(0));
      double original = lvar->getVal();
      const RooAbsBinning* binning = lvar->getBinningPtr(_binningNames[0].Data());
      for (Int_t i = 0; i < binning->numBins(); ++i) {
         lvar->setVal(binning->binCenter(i));
         // Get function value
         Double_t value = _function.arg().getVal();
         if (value > max) max = value;
      }
      lvar->setVal(original);
      setDirtyInhibit(origState);	
   } else {
      RooArgList* coefList = (RooArgList*)_coefLists.UncheckedAt(0);
      for (Int_t i = 0; i < coefList->getSize(); ++i) {
         double val = static_cast<const RooAbsReal*>(coefList->at(i))->getVal();
         if (val > max) {
            max = val;
         }
      }
   }
   return max * 1.05;
}

//_____________________________________________________________________________
list<Double_t>* RooBinnedPdf::plotSamplingHint(RooAbsRealLValue& obs,
                                               Double_t xlo, Double_t xhi) const
{
   // Return sampling hint for making curves of (projections) of this function
   // as the recursive division strategy of RooCurve cannot deal efficiently
   // with the vertical lines that occur in a non-interpolated histogram

   if (!_function.absArg()) {
      return 0;
   }

   // Check that observable is in dataset, if not no hint is generated
   Int_t i = 0;
   RooAbsLValue* lvar = 0;
   for (; i < _baseVarsList.getSize(); ++i) {
      RooAbsArg* var = _baseVarsList.at(i);
      lvar = dynamic_cast<RooAbsLValue*>(var);
      if (lvar && var->GetName() == obs.GetName()) {
         break;
      }
   }
   if (!lvar) {
      return 0;
   }

   // Retrieve position of all bin boundaries
   const RooAbsBinning* binning = lvar->getBinningPtr(_binningNames[i].Data());
   Double_t* boundaries = binning->array();

   list<Double_t>* hint = new list<Double_t>;

   // Widen range slighty
   xlo = xlo - 0.01 * (xhi - xlo);
   xhi = xhi + 0.01 * (xhi - xlo);

   Double_t delta = (xhi-xlo)*1e-8;
 
   // Construct array with pairs of points positioned epsilon to the left and
   // right of the bin boundaries
   for (Int_t i = 0; i < binning->numBoundaries(); ++i) {
      if (boundaries[i] >= xlo && boundaries[i] <= xhi) {
         hint->push_back(boundaries[i] - delta);
         hint->push_back(boundaries[i] + delta);
      }
   }
   return hint;
}

//______________________________________________________________________________
std::list<Double_t>* RooBinnedPdf::binBoundaries(RooAbsRealLValue& obs,
    Double_t xlo, Double_t xhi) const 
{
   // Return sampling hint for making curves of (projections) of this function
   // as the recursive division strategy of RooCurve cannot deal efficiently
   // with the vertical lines that occur in a non-interpolated histogram
   
   // No boundaries can be supplied when there is no binning
   if (_binningNames.empty()) {
      return 0;
   }
   
   // Check that we have observable, if not no binning is returned
   RooAbsArg* arg = 0;
   int i = 0;
   for (; i < _baseVarsList.getSize(); ++i) {
      arg = _baseVarsList.at(i);
      if (arg && arg->GetName() == obs.GetName()) break;
   }
   RooAbsLValue* lvarg = dynamic_cast<RooAbsLValue*>(arg);
   if (!lvarg) {
      return 0;
   }
   
   // Retrieve position of all bin boundaries
   const RooAbsBinning* binning = lvarg->getBinningPtr(_binningNames[i]);
   Double_t* boundaries = binning->array();
   
   std::list<Double_t>* bounds = new list<Double_t>;
   
   // Construct array with pairs of points positioned epsilon to the left and
   // right of the bin boundaries
   for (Int_t i=0; i<binning->numBoundaries(); i++) {
      if (boundaries[i]>=xlo && boundaries[i]<=xhi) {
         bounds->push_back(boundaries[i]);
      }
   }
   return bounds;
}

//_____________________________________________________________________________
Double_t RooBinnedPdf::evaluate() const
{
   if (_function.absArg() != 0) {
      return evaluateFunction();
   } else if (_coefLists.GetEntries() == _numCats) {
      return evaluateMultipleCoefs();
   } else {
      return evaluateCoef();
   }
}

//_____________________________________________________________________________
Double_t RooBinnedPdf::evaluateCoef() const
{
  // evaluates and returns the current value of the PDF's function
  // in case there is a coefficient for each bin

  // temporary result of evaluation
  Double_t value = 1.;

  // temporary bin position
  Int_t binPos     = 0;
  Int_t coefPosFac = 1;

  assert(_numCats==_baseCatsList.getSize());
  // loop over base categories
  RooFIter catIter = _baseCatsList.fwdIterator();
  RooFIter baseIter = _baseVarsList.fwdIterator();
  Int_t iCatIter = -1;
  RooAbsCategory* cat(0);
  while((cat = (RooAbsCategory*)catIter.next())) {
    ++iCatIter;

    // get position of coefficient
    const std::map<Int_t, Int_t>& indexMap = _indexPositions[iCatIter];
    Int_t index = cat->getIndex();
    std::map<Int_t, Int_t>::const_iterator it = indexMap.find(index);
    assert(it != indexMap.end());
    Int_t cPos = it->second;

    // update position of bin
    binPos += coefPosFac * cPos;
    coefPosFac *= cat->numTypes();

    // divide by bin widths if coefficients are bin integrals
    if (_continuousBase && _binIntegralCoefs) {
      RooAbsRealLValue* baseVar = (RooAbsRealLValue*)baseIter.next();
      value /= baseVar->getBinning(_binningNames[iCatIter]).binWidth(cPos);
    }
  }

  RooArgList* coefList = (RooArgList*)_coefLists.UncheckedAt(0);
  if (!_calcCoefZeros[0] || binPos > 0) {
    // multiply by coefficient value for bin
    value *= ((RooAbsReal*)coefList->at(binPos - (Int_t)_calcCoefZeros[0]))
        ->getVal();

    // make negative values equal to zero
    if (value <= 0.) return 0.;
  }

  // return if we don't have to calculate the coefficient of bin 0
  if (_ignoreFirstBin && binPos == 0) return 0.;
  if (!_calcCoefZeros[0]) return value;

  // coefficient of bin 0 is not explicitely specified
  // * coefficients have values between zero and one
  // * sum of coefficients is equal to one
  // * coefficient of bin 0 is given by one minus the sum of the other coefs.

  // loop over coefficients and calculate sum
  Double_t coefSum = 0.;
  RooFIter coefIter = coefList->fwdIterator();
  RooAbsReal* arg(0);
  while ((arg = (RooAbsReal*)coefIter.next())) {
    // get coefficient's value
    Double_t cVal = arg->getVal();
    // make negative values equal to zero, add positive values to sum
    if (cVal > 0) coefSum += cVal;
  }

  if (binPos == 0) {
    // multiply result with the calculated value of bin 0 coefficient
    if (coefSum >= 1.) return 0.;
    value *= 1. - coefSum;
  } else if (coefSum > 1.) {
    // scale the coefficients to make their sum equal to one
    value /= coefSum;
  }

  return value;
}

//_____________________________________________________________________________
Double_t RooBinnedPdf::evaluateMultipleCoefs() const
{
  // evaluates and returns the current value of the PDF's function
  // in case the coefficients factorize and there is a list of coefficients for
  // each base category

  // temporary result of evaluation
  Double_t value = 1.;

  // temporary vector of coefficient positions
  std::vector<Int_t> cPos(_numCats, 0);

  // loop over base categories
  Bool_t ignoreBin = _ignoreFirstBin;
  Bool_t calcCoefZeros = kFALSE;

  Int_t catIter = -1;
  RooFIter baseCatsIter = _baseCatsList.fwdIterator();
  RooFIter baseVarsIter = _baseVarsList.fwdIterator();
  RooAbsCategory* baseCats(0);
  while ((baseCats = (RooAbsCategory*)baseCatsIter.next())) {
    ++catIter;
    // get coefficient position
    const std::map<Int_t, Int_t>& indexMap = _indexPositions[catIter];
    Int_t index = baseCats->getIndex();
    std::map<Int_t, Int_t>::const_iterator it = indexMap.find(index);
    assert(it != indexMap.end());
    cPos[catIter] = it->second;
    
    if (cPos[catIter] != 0)      ignoreBin     = kFALSE;
    if (_calcCoefZeros[catIter]) calcCoefZeros = kTRUE;

    // multiply temporary result with value of coefficient
    if (!_calcCoefZeros[catIter] || cPos[catIter] != 0) {
      // get coefficient's value
      Double_t cVal = ((RooAbsReal*)((RooArgList*)_coefLists
          .UncheckedAt(catIter))
          ->at(cPos[catIter] - (Int_t)_calcCoefZeros[catIter]))->getVal();
      // make negative values equal to zero
      if (cVal <= 0.) return 0.;

      // multiply by coefficient't value
      value *= cVal;

      // divide by bin width if coefficients are bin integrals
      if (_continuousBase && _binIntegralCoefs) {
        RooAbsRealLValue *baseVars = (RooAbsRealLValue*)baseVarsIter.next();
        value /= baseVars->getBinning(_binningNames[catIter]).binWidth(cPos[catIter]);
      }
    }
  }

  // return value if we don't have to calculate any bin 0 coefficients
  if (ignoreBin) return 0.;
  if (!calcCoefZeros) return value;

  // loop over base categories again
  for (Int_t catIter = 0; catIter < _numCats; ++catIter) {
    if (!_calcCoefZeros[catIter]) continue;

    // coefficient of bin 0 is not explicitely specified for this category
    // * coefficients have values between zero and one
    // * sum of coefficients is equal to one
    // * coefficient of bin 0 is given by one minus the sum of the other coefs.

    // loop over specified coefficients (#category types - 1)
    Double_t coefSum = 0.;
    RooArgList* coefList = (RooArgList*)_coefLists.UncheckedAt(catIter);
    RooFIter coefIter = coefList->fwdIterator();
    RooAbsReal* arg(0);
    while ((arg = (RooAbsReal*)coefIter.next())) {
      // get coefficient's value
      Double_t cVal = arg->getVal();
      // make negative values equal to zero, add positive values to sum
      if (cVal > 0) coefSum += cVal;
    }

    if (cPos[catIter] == 0) {
      // multiply result with the calculated value of bin 0 coefficient
      if (coefSum >= 1.) {
        return 0.;
      }
      value *= 1. - coefSum;

      // divide by bin width if coefficients are bin integrals
      if (_continuousBase && _binIntegralCoefs) {
        value /= ((RooAbsRealLValue*)_baseVarsList.at(catIter))
            ->getBinning(_binningNames[catIter]).binWidth(cPos[catIter]);
      }
    } else if (coefSum > 1.) {
      // scale these coefficients to make their sum equal to one
      value /= coefSum;
    }
  }


  return value;
}

//_____________________________________________________________________________
Double_t RooBinnedPdf::evaluateFunction() const
{
  std::vector<Double_t> originals(_baseVarsList.getSize(), 0);
  for (Int_t i = 0; i < _baseVarsList.getSize(); ++i) {
    originals[i] = static_cast<const RooAbsReal*>(_baseVarsList.at(i))->getVal();
  }

  // Cache requirement
  Bool_t origState = inhibitDirty();
  setDirtyInhibit(kTRUE);

  // Set vars to bin center
  for (Int_t i = 0; i < _baseVarsList.getSize(); ++i) {
    const char* name = _binningNames[i].Data();
    RooAbsRealLValue* var = dynamic_cast<RooAbsRealLValue*>(_baseVarsList.at(i));
    const RooAbsBinning& binning = var->getBinning(name);
    Int_t bin = binning.binNumber(originals[i]);
    var->setVal(binning.binCenter(bin));
  }
  // Get function value
  Double_t value = _function.arg().getVal();

  // Restore original vars
  for (Int_t i = 0; i < _baseVarsList.getSize(); ++i) {
    RooAbsRealLValue* var = dynamic_cast<RooAbsRealLValue*>(_baseVarsList.at(i));
    var->setVal(originals[i]);
  }
  setDirtyInhibit(origState);
  return value;
}

//_____________________________________________________________________________
Int_t RooBinnedPdf::createBaseCats(const RooArgList& baseVars,
    const TObjArray& binningNames)
{
  // creates a base category for each continuous input base variable

  _numCats = baseVars.getSize();

  // check the number of specified binnings
  if (binningNames.GetEntries() != _numCats) {
    coutF(InputArguments) << "RooBinnedPdf::createBaseCats("
        << GetName() << ") number of binnings (" << binningNames.GetEntries()
        << ") does not match the number of base variables (" << _numCats << ")"
        << endl;
    reset();
    return -1;
  }

  // loop over base variables
  for (Int_t varIter = 0; varIter < _numCats; ++varIter) {
    // get input variable
    TString catName(baseVars.at(varIter)->GetName());
    catName += TString("_") + GetName() + "_cat";
    RooAbsRealLValue* var
        = dynamic_cast<RooAbsRealLValue*>(baseVars.at(varIter));

    // check type of input variable
    if (var == 0) {
      coutF(InputArguments) << "RooBinnedPdf::createBaseCats("
          << GetName() << ") base variable '" << var->GetName()
          << "' is not a RooAbsRealLValue" << endl;
      reset();
      return -2;
    }

    // check if list of base variables already contains this variable
    if (_baseVarsList.find(var->GetName()) != 0) {
      coutF(InputArguments) << "RooBinnedPdf::createBaseCats("
          << GetName() << ") variable '" << var->GetName()
          << "' is not unique in set of base variables" << endl;
      reset();
      return -3;
    }

    // add variable to list of base variables
    _baseVarsList.add(*var);

    // create binning of input variable if necessary
    TString bins = ((const TObjString*)binningNames.At(varIter))->GetString();
    if (!var->hasBinning(bins))
      var->getBinning(bins, kTRUE, kTRUE);

    // add binning name to list
    _binningNames.push_back(bins);

    // create category
    RooBinningCategory* cat = new RooBinningCategory(catName, catName, *var, bins);
    _baseCatsList.addOwned(*cat);
  }
  assert(_baseVarsList.getSize()==_numCats);

  return _baseCatsList.getSize();
}

//_____________________________________________________________________________
Int_t RooBinnedPdf::initCoefs(const TObjArray& coefLists, Bool_t factorize)
{
  // initializes bin coefficients

  // set size of coefficient lists array
  if (factorize) _coefLists.Expand(_numCats);

  // make array owner of the coefficient lists
  _coefLists.SetOwner(kTRUE);

  // loop over coefficient lists
  Int_t listPos = -1;
  TIterator* cListIter = coefLists.MakeIterator();
  RooArgList* cList = 0;
  while ((cList = dynamic_cast<RooArgList*>(cListIter->Next())) != 0) {
    // update list position
    ++listPos;

    // create new coefficients list
    TString cListName(GetName());
    cListName += "_coefList";
    if (factorize) cListName += listPos;
    RooListProxy* cListProxy = new RooListProxy(cListName, 0, this);
    _coefLists.Add(cListProxy);

    // add coefficients to list proxy
    RooFIter coefIter = cList->fwdIterator();
    RooAbsReal* coef = 0;
    while ((coef = dynamic_cast<RooAbsReal*>(coefIter.next())) != 0) {
      // check if coefficient does not depend on our variables
      if ((_continuousBase && coef->dependsOn(_baseVarsList))
          || (!_continuousBase && coef->dependsOn(_baseCatsList))) {
        coutF(InputArguments) << "RooBinnedPdf::initCoefs("
            << GetName() << ") coefficient dependence on base variables is not supported (yet)"
            << endl;
        reset();
        return -3;
      }

      cListProxy->add(*coef);
    }

    // loop over base categories
    RooFIter catIter = _baseCatsList.fwdIterator();
    RooAbsCategory* cat = 0;
    Int_t catPos = -1;
    Int_t numBins = 1;
    while ((cat = (RooAbsCategory*)catIter.next()) != 0) {
      // update category position
      ++catPos;

      // only look at the category for the current list of coefficients
      if (factorize && catPos != listPos) continue;

      if (factorize) {
        // set/check number of coefficients
        if (cListProxy->getSize() == cat->numTypes()) {
          _calcCoefZeros.push_back(kFALSE);
        } else if (cListProxy->getSize() == cat->numTypes() - 1) {
          _calcCoefZeros.push_back(kTRUE);
        } else {
          coutF(InputArguments) << "RooBinnedPdf::initCoefs("
              << GetName() << ") number of coefficients ("
              << cListProxy->getSize()
              << ") does not match number of base variable types ("
              << cat->numTypes() << ")" << endl;
          reset();
          return -2;
        }
      } else {
        // get number of category types
        numBins *= cat->numTypes();
      }

      // get category indices
      vector<Int_t> catIndices;
      RooCatType* catType = 0;
      TIterator* catTypeIter = cat->typeIterator();
      while ((catType = (RooCatType*)catTypeIter->Next()) != 0)
        catIndices.push_back(catType->getVal());
      delete catTypeIter;
      sort(catIndices.begin(), catIndices.end()); // sort indices!!!!!!!!!!!!

      // set position of category indices
      map<Int_t, Int_t> indexPosMap;
      for (Int_t indexPos = 0; indexPos < (Int_t)catIndices.size(); ++indexPos)
        indexPosMap[catIndices.at(indexPos)] = indexPos;
      _indexPositions.push_back(indexPosMap);
    }

    if (!factorize) {
      // check/set number of coefficients
      if (cListProxy->getSize() == numBins) {
        _calcCoefZeros.push_back(kFALSE);
      } else if (cListProxy->getSize() == numBins - 1) {
        _calcCoefZeros.push_back(kTRUE);
      } else {
        coutF(InputArguments) << "RooBinnedPdf::initCoefs("
            << GetName() << ") number of coefficients ("
            << cListProxy->getSize() << ") does not match number of bins ("
            << numBins << ")" << endl;
        reset();
        return -2;
      }
    }
  }

  delete cListIter;

  // check number of coefficient lists
  if (factorize && _coefLists.GetEntries() != _numCats) {
    coutF(InputArguments) << "RooBinnedPdf::initCoefs("
        << GetName() << ") number of coefficient lists ("
        << _coefLists.GetEntries()
        << ") does not match number of base variables (" << _numCats << ")"
        << endl;
    reset();
    return -1;
  } else if (!factorize && _coefLists.GetEntries() != 1) {
    coutF(InputArguments) << "RooBinnedPdf::initCoefs("
        << GetName() << ") multiple coefficient lists specified for non-factorizing coefficients"
        << endl;
    reset();
    return -1;
  }

  return _numCats;
}

//_____________________________________________________________________________
RooArgList* RooBinnedPdf::baseVariables()
{
  // Returns the list of base variables (continuous variables or categories).
  // The caller of this function is responsible for deleting the returned
  // arglist.

  if (_continuousBase) return new RooArgList(_baseVarsList, "base_variables");
  else                 return new RooArgList(_baseCatsList, "base_variables");
}

//_____________________________________________________________________________
void RooBinnedPdf::reset()
{
  _numCats = 0;
  _baseCatsList.removeAll();
  _baseVarsList.removeAll();
  _coefLists.Clear();
  _indexPositions.clear();
  _binningNames.clear();
  _calcCoefZeros.clear();
}
