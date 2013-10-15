/*****************************************************************************
 * Project: RooFit                                                           *
 * Package: RooFitModels                                                     *
 *    File: $Id$
 * Authors:                                                                  *
 *   JvL, Jeroen van Leerdam, Nikhef,      j.van.leerdam@nikhef.nl           *
 *   GR,  Gerhard Raven,      Nikhef & VU, Gerhard.Raven@nikhef.nl           *
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
// Description of a B decay time distribution with flavour tagging, effects
// of CP violation, mixing and life time differences. This function can 
// be analytically convolved with any RooResolutionModel implementation.
// END_HTML
//


#include <memory>
#include <iostream>
using std::endl;

#include "Riostream.h"
#include "RooFit.h"
#include "RooMsgService.h"

#include "TMath.h"
#include "P2VV/RooBTagDecay.h"
#include "RooCategory.h"
#include "RooRandom.h"
#include "RooRealVar.h"

#include "TObjArray.h"


//_____________________________________________________________________________
RooBTagDecay::RooBTagDecay(const char *name, const char* title, 
    RooRealVar& time, RooAbsReal& tau, RooAbsReal& dGamma, RooAbsReal& dm,
    RooAbsReal& coshCoef, RooAbsReal& sinhCoef, RooAbsReal& cosCoef,
    RooAbsReal& sinCoef, const RooResolutionModel& model, DecayType type,
    Bool_t checkVars) :
  RooAbsAnaConvPdf(name, title, model, time),
  _time("time", "B lifetime", this, time),
  _iTag0("iTag0", "initial state tag 0", this),
  _iTag1("iTag1", "initial state tag 1", this),
  _fTag("fTag", "final state tag", this),
  _tagCat0("tagCat0", "tagging category 0", this),
  _tagCat1("tagCat1", "tagging category 1", this),
  _tau("tau", "mean B lifetime", this, tau),
  _dGamma("dGamma", "Delta Gamma", this, dGamma),
  _dm("dm", "Delta mass", this, dm),
  _dilutions0("dilutions0", 0, this),
  _dilutions1("dilutions1", 0, this),
  _ADilWTags0("ADilWTags0", 0, this),
  _ADilWTags1("ADilWTags1", 0, this),
  _ANorm("ANorm", "normalization asymmetry", this),
  _avgCEvenSum("avgCEvenSum", "weighted sum of average even coeffs", this),
  _avgCOddSum("avgCOddSum", "weighted sum of average odd coeffs", this),
  _coshCoef("coshCoef", "cosh coefficient", this, coshCoef),
  _sinhCoef("sinhCoef", "sinh coefficient", this, sinhCoef),
  _cosCoef("cosCoef", "cos coefficient", this, cosCoef),
  _sinCoef("sinCoef", "sin coefficient", this, sinCoef),
  _createdVars("createdVars", 0, this),
  _decayType(type),
  _tagCat0Type(0),
  _tagCat1Type(0),
  _tags(0),
  _iTag0Val(2),
  _iTag1Val(2),
  _fTagVal(2),
  _checkVars(checkVars),
  _maxVal(-1)
{
  // constructor without flavour tags (behaves like RooBDecay)
  if (!checkVarDep(time, kTRUE)) assert(0);

  declareBases();
}

//_____________________________________________________________________________
RooBTagDecay::RooBTagDecay(const char *name, const char* title,
    RooRealVar& time, RooCategory& iTag, RooCategory& fTag,
    RooAbsReal& tau, RooAbsReal& dGamma, RooAbsReal& dm, RooAbsReal& dilution,
    RooAbsReal& ADilWTag, RooAbsReal& ANorm, RooAbsReal& avgCEven,
    RooAbsReal& avgCOdd, RooAbsReal& cosCoef, const RooResolutionModel& model,
    DecayType type, Bool_t checkVars) :
  RooAbsAnaConvPdf(name, title, model, time),
  _time("time", "B lifetime", this, time),
  _iTag0("iTag0", "initial state tag 0", this),
  _iTag1("iTag1", "initial state tag 1", this, iTag),
  _fTag("fTag", "final state tag", this, fTag),
  _tagCat0("tagCat0", "tagging category 0", this),
  _tagCat1("tagCat1", "tagging category 1", this),
  _tau("tau", "mean B lifetime", this, tau),
  _dGamma("dGamma", "Delta Gamma", this, dGamma),
  _dm("dm", "Delta mass", this, dm),
  _dilutions0("dilutions0", 0, this),
  _dilutions1("dilutions1", 0, this),
  _ADilWTags0("ADilWTags0", 0, this),
  _ADilWTags1("ADilWTags1", 0, this),
  _ANorm("ANorm", "normalization asymmetry", this, ANorm),
  _avgCEvenSum("avgCEvenSum", "weighted sum of average even coeffs", this),
  _avgCOddSum("avgCOddSum", "weighted sum of average odd coeffs", this),
  _coshCoef("coshCoef", "cosh coefficient", this, cosCoef),
  _sinhCoef("sinhCoef", "sinh coefficient", this),
  _cosCoef("cosCoef", "cos coefficient", this, cosCoef),
  _sinCoef("sinCoef", "sin coefficient", this),
  _createdVars("createdVars", 0, this),
  _decayType(type),
  _tagCat0Type(0),
  _tagCat1Type(1),
  _tags(3),
  _iTag0Val(2),
  _iTag1Val(2),
  _fTagVal(2),
  _checkVars(checkVars),
  _maxVal(-1)
{
  // constructor with both initial and final state flavour tags
  // (decay into a flavour specific final state)

  initTag(kTRUE, kTRUE);
  initTag(kFALSE, kFALSE);

  TObjArray  tagCatCoefsArray;
  TObjArray  avgCEvensArray;
  TObjArray  avgCOddsArray;
  RooArgList avgCEvens(avgCEven);
  RooArgList avgCOdds(avgCOdd);
  avgCEvensArray.Add(&avgCEvens);
  avgCOddsArray.Add(&avgCOdds);

  RooArgList dilutions0;
  RooArgList dilutions(dilution);
  RooArgList ADilWTags0;
  RooArgList ADilWTags(ADilWTag);

  initTaggingCats(tagCatCoefsArray, dilutions0, dilutions, ADilWTags0,
      ADilWTags, avgCEvensArray, avgCOddsArray);

  if (!checkVarDep(time, kTRUE)) assert(0);
  if (!checkVarDep(iTag, kTRUE)) assert(0);
  if (!checkVarDep(fTag, kTRUE)) assert(0);

  declareBases();
}

RooBTagDecay::RooBTagDecay(const char *name, const char* title,
    RooRealVar& time, RooCategory& iTag, RooAbsReal& tau,
    RooAbsReal& dGamma, RooAbsReal& dm, RooAbsReal& dilution,
    RooAbsReal& ADilWTag, RooAbsReal& avgCEven, RooAbsReal& avgCOdd,
    RooAbsReal& coshCoef, RooAbsReal& sinhCoef, RooAbsReal& cosCoef,
    RooAbsReal& sinCoef, const RooResolutionModel& model, DecayType type,
    Bool_t checkVars) :
  RooAbsAnaConvPdf(name, title, model, time),
  _time("time", "B lifetime", this, time),
  _iTag0("iTag0", "initial state tag 0", this),
  _iTag1("iTag1", "initial state tag 1", this, iTag),
  _fTag("fTag", "final state tag", this),
  _tagCat0("tagCat0", "tagging category 0", this),
  _tagCat1("tagCat1", "tagging category 1", this),
  _tau("tau", "mean B lifetime", this, tau),
  _dGamma("dGamma", "Delta Gamma", this, dGamma),
  _dm("dm", "Delta mass", this, dm),
  _dilutions0("dilutions0", 0, this),
  _dilutions1("dilutions1", 0, this),
  _ADilWTags0("ADilWTags0", 0, this),
  _ADilWTags1("ADilWTags1", 0, this),
  _ANorm("ANorm", "normalization asymmetry", this),
  _avgCEvenSum("avgCEvenSum", "weighted sum of average even coeffs", this),
  _avgCOddSum("avgCOddSum", "weighted sum of average odd coeffs", this),
  _coshCoef("coshCoef", "cosh coefficient", this, coshCoef),
  _sinhCoef("sinhCoef", "sinh coefficient", this, sinhCoef),
  _cosCoef("cosCoef", "cos coefficient", this, cosCoef),
  _sinCoef("sinCoef", "sin coefficient", this, sinCoef),
  _createdVars("createdVars", 0, this),
  _decayType(type),
  _tagCat0Type(0),
  _tagCat1Type(1),
  _tags(1),
  _iTag0Val(2),
  _iTag1Val(2),
  _fTagVal(2),
  _checkVars(checkVars),
  _maxVal(-1)
{
  // constructor with only an initial state flavour tag
  // (decay into CP self-conjugate state)

  initTag(kTRUE, kTRUE);

  TObjArray  tagCatCoefsArray;
  TObjArray  avgCEvensArray;
  TObjArray  avgCOddsArray;
  RooArgList avgCEvens(avgCEven);
  RooArgList avgCOdds(avgCOdd);
  avgCEvensArray.Add(&avgCEvens);
  avgCOddsArray.Add(&avgCOdds);

  RooArgList dilutions0;
  RooArgList dilutions(dilution);
  RooArgList ADilWTags0;
  RooArgList ADilWTags(ADilWTag);

  initTaggingCats(tagCatCoefsArray, dilutions0, dilutions, ADilWTags0,
      ADilWTags, avgCEvensArray, avgCOddsArray);

  if (!checkVarDep(time, kTRUE)) assert(0);
  if (!checkVarDep(iTag, kTRUE)) assert(0);

  declareBases();
}

//_____________________________________________________________________________
RooBTagDecay::RooBTagDecay(const char *name, const char* title,
    RooRealVar& time, RooCategory& iTag, RooCategory& fTag,
    RooCategory& tagCat, RooAbsReal& tau, RooAbsReal& dGamma, RooAbsReal& dm,
    const RooArgList& dilutions, const RooArgList& ADilWTags,
    RooAbsReal& ANorm, const RooArgList& avgCEvens, const RooArgList& avgCOdds,
    const RooArgList& tagCatCoefs, RooAbsReal& cosCoef,
    const RooResolutionModel& model, DecayType type, Bool_t checkVars) :
  RooAbsAnaConvPdf(name, title, model, time),
  _time("time", "B lifetime", this, time),
  _iTag0("iTag0", "initial state tag 0", this),
  _iTag1("iTag1", "initial state tag 1", this, iTag),
  _fTag("fTag", "final state tag", this, fTag),
  _tagCat0("tagCat0", "tagging category 0", this),
  _tagCat1("tagCat1", "tagging category 1", this, tagCat),
  _tau("tau", "mean B lifetime", this, tau),
  _dGamma("dGamma", "Delta Gamma", this, dGamma),
  _dm("dm", "Delta mass", this, dm),
  _dilutions0("dilutions0", 0, this),
  _dilutions1("dilutions1", 0, this),
  _ADilWTags0("ADilWTags0", 0, this),
  _ADilWTags1("ADilWTags1", 0, this),
  _ANorm("ANorm", "normalization asymmetry", this, ANorm),
  _avgCEvenSum("avgCEvenSum", "weighted sum of average even coeffs", this),
  _avgCOddSum("avgCOddSum", "weighted sum of average odd coeffs", this),
  _coshCoef("coshCoef", "cosh coefficient", this, cosCoef),
  _sinhCoef("sinhCoef", "sinh coefficient", this),
  _cosCoef("cosCoef", "cos coefficient", this, cosCoef),
  _sinCoef("sinCoef", "sin coefficient", this),
  _createdVars("createdVars", 0, this),
  _decayType(type),
  _tagCat0Type(0),
  _tagCat1Type(2),
  _tags(3),
  _iTag0Val(2),
  _iTag1Val(2),
  _fTagVal(2),
  _checkVars(checkVars),
  _maxVal(-1)
{
  // constructor with both initial and final state flavour tags (decay into
  // a flavour specific final state) and with tagging categories

  initTag(kTRUE, kTRUE);
  initTag(kFALSE, kFALSE);

  TObjArray  tagCatCoefsArray;
  TObjArray  avgCEvensArray;
  TObjArray  avgCOddsArray;
  RooArgList tagCatCoefsCopy(tagCatCoefs);
  RooArgList avgCEvensCopy(avgCEvens);
  RooArgList avgCOddsCopy(avgCOdds);
  tagCatCoefsArray.Add(&tagCatCoefsCopy);
  avgCEvensArray.Add(&avgCEvensCopy);
  avgCOddsArray.Add(&avgCOddsCopy);

  RooArgList dilutions0;
  RooArgList ADilWTags0;

  initTaggingCats(tagCatCoefsArray, dilutions0, dilutions, ADilWTags0,
      ADilWTags, avgCEvensArray, avgCOddsArray);

  if (!checkVarDep(time, kTRUE))   assert(0);
  if (!checkVarDep(tagCat, kTRUE)) assert(0);
  if (!checkVarDep(iTag, kTRUE))   assert(0);
  if (!checkVarDep(fTag, kTRUE))   assert(0);

  declareBases();
}

//_____________________________________________________________________________
RooBTagDecay::RooBTagDecay(const char *name, const char* title,
    RooRealVar& time, RooCategory& iTag, RooCategory& tagCat, RooAbsReal& tau,
    RooAbsReal& dGamma, RooAbsReal& dm, const RooArgList& dilutions,
    const RooArgList& ADilWTags, const RooArgList& avgCEvens,
    const RooArgList& avgCOdds, const RooArgList& tagCatCoefs,
    RooAbsReal& coshCoef, RooAbsReal& sinhCoef, RooAbsReal& cosCoef,
    RooAbsReal& sinCoef, const RooResolutionModel& model, DecayType type,
    Bool_t checkVars) :
  RooAbsAnaConvPdf(name, title, model, time),
  _time("time", "B lifetime", this, time),
  _iTag0("iTag0", "initial state tag 0", this),
  _iTag1("iTag1", "initial state tag 1", this, iTag),
  _fTag("fTag", "final state tag", this),
  _tagCat0("tagCat0", "tagging category 0", this),
  _tagCat1("tagCat1", "tagging category 1", this, tagCat),
  _tau("tau", "mean B lifetime", this, tau),
  _dGamma("dGamma", "Delta Gamma", this, dGamma),
  _dm("dm", "Delta mass", this, dm),
  _dilutions0("dilutions0", 0, this),
  _dilutions1("dilutions1", 0, this),
  _ADilWTags0("ADilWTags0", 0, this),
  _ADilWTags1("ADilWTags1", 0, this),
  _ANorm("ANorm", "normalization asymmetry", this),
  _avgCEvenSum("avgCEvenSum", "weighted sum of average even coeffs", this),
  _avgCOddSum("avgCOddSum", "weighted sum of average odd coeffs", this),
  _coshCoef("coshCoef", "cosh coefficient", this, coshCoef),
  _sinhCoef("sinhCoef", "sinh coefficient", this, sinhCoef),
  _cosCoef("cosCoef", "cos coefficient", this, cosCoef),
  _sinCoef("sinCoef", "sin coefficient", this, sinCoef),
  _createdVars("createdVars", 0, this),
  _decayType(type),
  _tagCat0Type(0),
  _tagCat1Type(2),
  _tags(1),
  _iTag0Val(2),
  _iTag1Val(2),
  _fTagVal(2),
  _checkVars(checkVars),
  _maxVal(-1)
{
  // constructor with only an initial state flavour tag
  // (decay into CP self-conjugate state) and with tagging categories

  initTag(kTRUE, kTRUE);

  TObjArray  tagCatCoefsArray;
  TObjArray  avgCEvensArray;
  TObjArray  avgCOddsArray;
  RooArgList tagCatCoefsCopy(tagCatCoefs);
  RooArgList avgCEvensCopy(avgCEvens);
  RooArgList avgCOddsCopy(avgCOdds);
  tagCatCoefsArray.Add(&tagCatCoefsCopy);
  avgCEvensArray.Add(&avgCEvensCopy);
  avgCOddsArray.Add(&avgCOddsCopy);

  RooArgList dilutions0;
  RooArgList ADilWTags0;

  initTaggingCats(tagCatCoefsArray, dilutions0, dilutions, ADilWTags0,
      ADilWTags, avgCEvensArray, avgCOddsArray);

  if (!checkVarDep(time, kTRUE)) assert(0);
  if (!checkVarDep(tagCat, kTRUE)) assert(0);
  if (!checkVarDep(iTag, kTRUE)) assert(0);

  declareBases();
}

//_____________________________________________________________________________
RooBTagDecay::RooBTagDecay(const char *name, const char* title,
    RooRealVar& time, RooCategory& iTag0, RooCategory& iTag1,
    RooCategory& fTag, RooCategory& tagCat0, RooCategory& tagCat1,
    RooAbsReal& tau, RooAbsReal& dGamma, RooAbsReal& dm,
    const RooArgList& dilutions0, const RooArgList& dilutions1,
    const RooArgList& ADilWTags0, const RooArgList& ADilWTags1,
    RooAbsReal& ANorm, const TObjArray& avgCEvens, const TObjArray& avgCOdds,
    const TObjArray& tagCatCoefs, RooAbsReal& cosCoef,
    const RooResolutionModel& model, DecayType type, Bool_t checkVars) :
  RooAbsAnaConvPdf(name, title, model, time),
  _time("time", "B lifetime", this, time),
  _iTag0("iTag0", "initial state tag 0", this, iTag0),
  _iTag1("iTag1", "initial state tag 1", this, iTag1),
  _fTag("fTag", "final state tag", this, fTag),
  _tagCat0("tagCat0", "tagging category 0", this, tagCat0),
  _tagCat1("tagCat1", "tagging category 1", this, tagCat1),
  _tau("tau", "mean B lifetime", this, tau),
  _dGamma("dGamma", "Delta Gamma", this, dGamma),
  _dm("dm", "Delta mass", this, dm),
  _dilutions0("dilutions0", 0, this),
  _dilutions1("dilutions1", 0, this),
  _ADilWTags0("ADilWTags0", 0, this),
  _ADilWTags1("ADilWTags1", 0, this),
  _ANorm("ANorm", "normalization asymmetry", this, ANorm),
  _avgCEvenSum("avgCEvenSum", "weighted sum of average even coeffs", this),
  _avgCOddSum("avgCOddSum", "weighted sum of average odd coeffs", this),
  _coshCoef("coshCoef", "cosh coefficient", this, cosCoef),
  _sinhCoef("sinhCoef", "sinh coefficient", this),
  _cosCoef("cosCoef", "cos coefficient", this, cosCoef),
  _sinCoef("sinCoef", "sin coefficient", this),
  _createdVars("createdVars", 0, this),
  _decayType(type),
  _tagCat0Type(2),
  _tagCat1Type(2),
  _tags(3),
  _iTag0Val(2),
  _iTag1Val(2),
  _fTagVal(2),
  _checkVars(checkVars),
  _maxVal(-1)
{
  // constructor with two initial state flavour tags and a final state flavour
  // tag (decay into a flavour specific final state) and with tagging
  // categories

  initTag(kTRUE, kFALSE);
  initTag(kTRUE, kTRUE);
  initTag(kFALSE, kFALSE);

  initTaggingCats(tagCatCoefs, dilutions0, dilutions1, ADilWTags0,
      ADilWTags1, avgCEvens, avgCOdds);

  if (!checkVarDep(time,    kTRUE)) assert(0);
  if (!checkVarDep(tagCat0, kTRUE)) assert(0);
  if (!checkVarDep(tagCat1, kTRUE)) assert(0);
  if (!checkVarDep(iTag0,   kTRUE)) assert(0);
  if (!checkVarDep(iTag1,   kTRUE)) assert(0);
  if (!checkVarDep(fTag,    kTRUE)) assert(0);

  declareBases();
}

//_____________________________________________________________________________
RooBTagDecay::RooBTagDecay(const char *name, const char* title,
    RooRealVar& time, RooCategory& iTag0, RooCategory& iTag1,
    RooCategory& tagCat0, RooCategory& tagCat1, RooAbsReal& tau,
    RooAbsReal& dGamma, RooAbsReal& dm, const RooArgList& dilutions0,
    const RooArgList& dilutions1, const RooArgList& ADilWTags0,
    const RooArgList& ADilWTags1, const TObjArray& avgCEvens,
    const TObjArray& avgCOdds, const TObjArray& tagCatCoefs,
    RooAbsReal& coshCoef, RooAbsReal& sinhCoef, RooAbsReal& cosCoef,
    RooAbsReal& sinCoef, const RooResolutionModel& model, DecayType type,
    Bool_t checkVars) :
  RooAbsAnaConvPdf(name, title, model, time),
  _time("time", "B lifetime", this, time),
  _iTag0("iTag0", "initial state tag 0", this, iTag0),
  _iTag1("iTag1", "initial state tag 1", this, iTag1),
  _fTag("fTag", "final state tag", this),
  _tagCat0("tagCat0", "tagging category 0", this, tagCat0),
  _tagCat1("tagCat1", "tagging category 1", this, tagCat1),
  _tau("tau", "mean B lifetime", this, tau),
  _dGamma("dGamma", "Delta Gamma", this, dGamma),
  _dm("dm", "Delta mass", this, dm),
  _dilutions0("dilutions0", 0, this),
  _dilutions1("dilutions1", 0, this),
  _ADilWTags0("ADilWTags0", 0, this),
  _ADilWTags1("ADilWTags1", 0, this),
  _ANorm("ANorm", "normalization asymmetry", this),
  _avgCEvenSum("avgCEvenSum", "weighted sum of average even coeffs", this),
  _avgCOddSum("avgCOddSum", "weighted sum of average odd coeffs", this),
  _coshCoef("coshCoef", "cosh coefficient", this, coshCoef),
  _sinhCoef("sinhCoef", "sinh coefficient", this, sinhCoef),
  _cosCoef("cosCoef", "cos coefficient", this, cosCoef),
  _sinCoef("sinCoef", "sin coefficient", this, sinCoef),
  _createdVars("createdVars", 0, this),
  _decayType(type),
  _tagCat0Type(2),
  _tagCat1Type(2),
  _tags(1),
  _iTag0Val(2),
  _iTag1Val(2),
  _fTagVal(2),
  _checkVars(checkVars),
  _maxVal(-1)
{
  // constructor with two initial state flavour tags
  // (decay into CP self-conjugate state) and with tagging categories

  initTag(kTRUE, kTRUE);
  initTag(kTRUE, kFALSE);

  initTaggingCats(tagCatCoefs, dilutions0, dilutions1, ADilWTags0,
      ADilWTags1, avgCEvens, avgCOdds);

  if (!checkVarDep(time, kTRUE)) assert(0);
  if (!checkVarDep(tagCat0, kTRUE)) assert(0);
  if (!checkVarDep(tagCat1, kTRUE)) assert(0);
  if (!checkVarDep(iTag0, kTRUE)) assert(0);
  if (!checkVarDep(iTag1, kTRUE)) assert(0);

  declareBases();
}

//_____________________________________________________________________________
RooBTagDecay::RooBTagDecay(const RooBTagDecay& other, const char* name) :
  RooAbsAnaConvPdf(other, name),
  _time("time", this, other._time),
  _iTag0("iTag0", this, other._iTag0),
  _iTag1("iTag1", this, other._iTag1),
  _fTag("fTag", this, other._fTag),
  _tagCat0("tagCat0", this, other._tagCat0),
  _tagCat1("tagCat1", this, other._tagCat1),
  _tau("tau", this, other._tau),
  _dGamma("dGamma", this, other._dGamma),
  _dm("dm", this, other._dm),
  _dilutions0("dilutions0", this, other._dilutions0),
  _dilutions1("dilutions1", this, other._dilutions1),
  _ADilWTags0("ADilWTags0", this, other._ADilWTags0),
  _ADilWTags1("ADilWTags1", this, other._ADilWTags1),
  _ANorm("ANorm", this, other._ANorm),
  _avgCEvenSum("avgCEvenSum", this, other._avgCEvenSum),
  _avgCOddSum("avgCOddSum", this, other._avgCOddSum),
  _coshCoef("coshCoef", this, other._coshCoef),
  _sinhCoef("sinhCoef", this, other._sinhCoef),
  _cosCoef("cosCoef", this, other._cosCoef),
  _sinCoef("sinCoef", this, other._sinCoef),
  _createdVars("createdVars", 0, this),
  _tagCat0Positions(other._tagCat0Positions),
  _tagCat1Positions(other._tagCat1Positions),
  _tagCat0Indices(other._tagCat0Indices),
  _tagCat1Indices(other._tagCat1Indices),
  _coshBasis(other._coshBasis),
  _sinhBasis(other._sinhBasis),
  _cosBasis(other._cosBasis),
  _sinBasis(other._sinBasis),
  _decayType(other._decayType),
  _tagCat0Type(other._tagCat0Type),
  _tagCat1Type(other._tagCat1Type),
  _tags(other._tags),
  _iTag0Val(other._iTag0Val),
  _iTag1Val(other._iTag1Val),
  _fTagVal(other._fTagVal),
  _checkVars(other._checkVars),
  _maxVal(other._maxVal)
{
  // copy constructor

  // make tagging coefficient arrays owners of their lists
  _tagCatCoefs.SetOwner(kTRUE);
  _avgCEvens.SetOwner(kTRUE);
  _avgCOdds.SetOwner(kTRUE);

  // copy tagging category coefficient lists
  for (Int_t cListIter = 0; cListIter < other._tagCatCoefs.GetEntries();
      ++cListIter) {
    // get other's coefficients list
    RooListProxy* cListOth
        = (RooListProxy*)other._tagCatCoefs.UncheckedAt(cListIter);

    // create new coefficients list
    TString cListName("tagCatCoefs");
    cListName += cListIter;
    RooListProxy* cListProx = new RooListProxy(cListName, this, *cListOth);
    _tagCatCoefs.Add(cListProx);
  }

  // copy average even and odd coefficient lists
  for (Int_t cListIter = 0; cListIter < other._avgCEvens.GetEntries();
      ++cListIter) {
    // get other's coefficients lists
    RooListProxy* cEListOth
        = (RooListProxy*)other._avgCEvens.UncheckedAt(cListIter);
    RooListProxy* cOListOth
        = (RooListProxy*)other._avgCOdds.UncheckedAt(cListIter);

    // create new coefficients lists
    TString cEListName("avgCEvens");
    TString cOListName("avgCOdds");
    cEListName += cListIter;
    cOListName += cListIter;
    RooListProxy* cEListProx = new RooListProxy(cEListName, this, *cEListOth);
    RooListProxy* cOListProx = new RooListProxy(cOListName, this, *cOListOth);
    _avgCEvens.Add(cEListProx);
    _avgCOdds.Add(cOListProx);
  }
}

//_____________________________________________________________________________
RooBTagDecay::~RooBTagDecay()
{
  // destructor
}

//_____________________________________________________________________________
Double_t RooBTagDecay::coefficient(Int_t basisIndex) const
{
  // return the value of the specified basis function's coefficient

  // check basis index
  if (basisIndex != _coshBasis && basisIndex != _cosBasis && (_tags > 1
      || (basisIndex != _sinhBasis && basisIndex != _sinBasis))) {
    coutE(InputArguments) << "RooBTagDecay::coefficient(" << GetName()
        << ") cannot find basis with index " << basisIndex << endl;
    return 0.;
  }

  // get coefficient's value
  Double_t coefVal = 0.;
  if (basisIndex == _coshBasis) {
    coefVal = _coshCoef;
  } else if (basisIndex == _sinhBasis) {
    if (_tags > 1) return 0.;
    else coefVal = _sinhCoef;
  } else if (basisIndex == _cosBasis) {
    coefVal = _cosCoef;
  } else if (basisIndex == _sinBasis) {
    if (_tags > 1) return 0.;
    else coefVal = _sinCoef;
  }

  // return the coefficient if there are no explicit tags
  if (_tags < 1 || coefVal == 0.) return coefVal;

  // determine if basis function is even or odd
  Bool_t odd = basisIndex == _cosBasis || basisIndex == _sinBasis;

  // determine in which tagging category we are
  Int_t catPos0 = -1, catPos1 = -1;
  coefVal *= tagCatCoef(catPos0, catPos1);

  // get average even and odd asymmetry coefficients
  Double_t cEvenOdd0 = ((RooAbsReal*)((RooArgList*)
      (odd ? _avgCOdds : _avgCEvens).At(catPos0))->at(catPos1))->getVal();
  Double_t cEvenOdd1 = ((RooAbsReal*)((RooArgList*)
      (odd ? _avgCEvens : _avgCOdds).At(catPos0))->at(catPos1))->getVal();

  // get values of initial state flavour tags
  Int_t iT0Val = _tagCat0Type > 0 && _iTag0Val > 1 ? _iTag0 : _iTag0Val;
  Int_t iT1Val = _tagCat1Type > 0 && _iTag1Val > 1 ? _iTag1 : _iTag1Val;

  // calculate terms in asymmetry coefficient: depend on initial state tags
  Double_t term0 = 0., term1 = 0., term01 = cEvenOdd0;
  Bool_t calc0 = _tagCat0Type > 0 && iT0Val != 0;
  Bool_t calc1 = _tagCat1Type > 0 && iT1Val != 0;
  if (calc0) {
    // calculate tag 0 term
    term0 += iT0Val * ((RooAbsReal*)_dilutions0.at(catPos0))->getVal()
        * (cEvenOdd1 - ((RooAbsReal*)_ADilWTags0.at(catPos0))->getVal()
        * cEvenOdd0);
  }
  if (calc1) {
    // calculate tag 1 term
    term1 += iT1Val * ((RooAbsReal*)_dilutions1.at(catPos1))->getVal()
        * (cEvenOdd1 - ((RooAbsReal*)_ADilWTags1.at(catPos1))->getVal()
        * cEvenOdd0);
  }
  if (calc0 && calc1) {
    // calculate tag0/tag1 interference term
    Double_t ADilWTag2 = ((RooAbsReal*)_ADilWTags0.at(catPos0))->getVal()
        * ((RooAbsReal*)_ADilWTags1.at(catPos1))->getVal();
    term01 += iT0Val * iT1Val
        * ((RooAbsReal*)_dilutions0.at(catPos0))->getVal()
        * ((RooAbsReal*)_dilutions1.at(catPos1))->getVal()
        * ((1. + ADilWTag2) * cEvenOdd0 - ADilWTag2 * cEvenOdd1);
  }

  coefVal *= term01 + term0 + term1;
  if (_tagCat0Type < 1 || _iTag0Val == 0) coefVal *= 2.;
  if (_tagCat1Type < 1 || _iTag1Val == 0) coefVal *= 2.;

  // calculate factor for final state tag
  if (_tags == 1) {
    // no final state tag
    return coefVal;
  } else  if (_fTagVal == 0) {
    // integrate over final state tag
    return (odd ? -2. * _ANorm : 2.) * coefVal;
  } else {
    // calculate factor with tag
    Int_t fTVal = _fTagVal == 2 ? _fTag : _fTagVal;
    return (odd ? fTVal - _ANorm : 1. - fTVal * _ANorm) * coefVal;
  }

  return 0.;
}

//_____________________________________________________________________________
Double_t RooBTagDecay::tagCatCoef() const
{
  Int_t cat0 = -1, cat1 = -1;
  return tagCatCoef(cat0, cat1);
}

//_____________________________________________________________________________
Double_t RooBTagDecay::tagCatCoef(Int_t& cat0, Int_t& cat1) const
{
  // check if we have tagging categories
  if (_tagCat0Type < 1 && _tagCat1Type < 1) {
    coutE(Eval) << "RooBTagDecay::tagCatCoef(" << GetName()
        << ") this PDF does not contain tagging categories" << endl;
    if (cat0 < 0) cat0 = -1;
    if (cat1 < 0) cat1 = -1;
    return -1.;
  } else if (_tagCat0Type < 2 && _tagCat1Type < 2) {
    if (cat0 < 1 && cat1 < 1) {
      cat0 = 0;
      cat1 = 0;
      return 1.;
    } else {
      coutE(InputArguments) << "RooBTagDecay::tagCatCoef(" << GetName()
          << ") category (" << cat0 << ", " << cat1 << ") does not exist"
          << endl;
      return -1.;
    }
  }

  // get category 0
  if (cat0 < 0) {
    cat0 = _tagCat0Type > 1
        ? getTagCatPosition(_tagCat0.arg().getIndex(), kFALSE) : 0;
  } else if (cat0 >= (_tagCat0Type > 1 ? _tagCat0.arg().numTypes() : 1)) {
    coutE(InputArguments) << "RooBTagDecay::tagCatCoef(" << GetName()
        << ") category " << cat0 << " does not exist for tag 0" << endl;
    return -1.;
  }

  // get category 1
  if (cat1 < 0) {
    cat1 = _tagCat1Type > 1
        ? getTagCatPosition(_tagCat1.arg().getIndex(), kTRUE) : 0;
  } else if (cat1 >= (_tagCat1Type > 1 ? _tagCat1.arg().numTypes() : 1)) {
    coutE(InputArguments) << "RooBTagDecay::tagCatCoef(" << GetName()
        << ") category " << cat1 << " does not exist for tag 1" << endl;
    return -1.;
  }

  return tagCatCoefUnsafe(cat0, cat1);
}

//_____________________________________________________________________________
Double_t RooBTagDecay::tagCatCoefUnsafe(Int_t cat0, Int_t cat1) const
{
  // get tagging category coefficient
  Double_t coef = ((RooAbsReal*)((RooArgList*)_tagCatCoefs.At(cat0))->at(cat1))
      ->getVal();

  // check coefficient
  if (coef < 0. || ((_tagCat0Type > 2 || _tagCat1Type > 2) && coef > 1.)) {
    coutE(Eval) << "RooBTagDecay::tagCatCoefUnsafe(" << GetName()
        << ") tagging category coefficient (" << cat0 << ", " << cat1
        << ") is not in range (" << coef << "): returning -1"
        << endl;
    return -1.;
  }

  return coef;
}

//_____________________________________________________________________________
RooArgSet* RooBTagDecay::coefVars(Int_t basisIndex) const 
{
  // Return the set of variables used exclusively by the coefficient of the
  // specified basis function. The caller of this function is responsible for
  // deleting the returned argset.

  RooArgSet* coefVars = 0;

  // get variables from specified coefficient
  if (basisIndex == _coshBasis)
    coefVars = _coshCoef.arg().getVariables();
  if (basisIndex == _sinhBasis && _tags < 2)
    coefVars = _sinhCoef.arg().getVariables();
  if (basisIndex == _cosBasis)
    coefVars = _cosCoef .arg().getVariables();
  if (basisIndex == _sinBasis && _tags < 2)
    coefVars = _sinCoef .arg().getVariables();

  if (coefVars == 0) {
    coefVars = new RooArgSet("parameters");
  } else {
    // add tagging variables
    if (_tags > 0) {
      std::auto_ptr<RooArgSet> tempSet;

      // add tagging categories
      if (_tagCat0Type > 1) coefVars->add(_tagCat0.arg());
      if (_tagCat1Type > 1) coefVars->add(_tagCat1.arg());

      // add initial state tags
      if (_tagCat0Type > 0) coefVars->add(_iTag0.arg());
      if (_tagCat1Type > 0) coefVars->add(_iTag1.arg());

      if (_tags > 1) {
        // add final state tag
        coefVars->add(_fTag.arg());
        tempSet.reset(_ANorm.arg().getVariables());
        coefVars->add(tempSet->getSize() > 0 ? *tempSet : _ANorm.arg());
      }

      // add average tagging category coefficient sums
      tempSet.reset(_avgCEvenSum.arg().getVariables());
      coefVars->add(tempSet->getSize() > 0 ? *tempSet: _avgCEvenSum.arg());
      tempSet.reset(_avgCOddSum.arg().getVariables());
      coefVars->add(tempSet->getSize() > 0 ? *tempSet : _avgCOddSum.arg());

      // add tagging category parameters
      Int_t numTagCats0 = _tagCat0Type > 1 ? _tagCat0.arg().numTypes() : 1;
      Int_t numTagCats1 = _tagCat1Type > 1 ? _tagCat1.arg().numTypes() : 1;
      for (Int_t cat0It = 0; cat0It < numTagCats0; ++cat0It) {
        if (_tagCat0Type > 0) {
          // add dilutions tag 0
          tempSet.reset(_dilutions0.at(cat0It)->getVariables());
          coefVars->add(tempSet->getSize() > 0
              ? *tempSet : *_dilutions0.at(cat0It));

          tempSet.reset(_ADilWTags0.at(cat0It)->getVariables());
          coefVars->add(tempSet->getSize() > 0
              ? *tempSet : *_ADilWTags0.at(cat0It));
        }

        for (Int_t cat1It = 0; cat1It < numTagCats1; ++cat1It) {
          if (_tagCat0Type > 1 || _tagCat1Type > 1) {
            // add tagging category coefficients
            tempSet.reset(((RooArgList*)_tagCatCoefs.At(cat0It))->at(cat1It)
                ->getVariables());
            coefVars->add((tempSet->getSize() > 0) ? *tempSet
                : *((RooArgList*)_tagCatCoefs.At(cat0It))->at(cat1It));
          }

          // add tagging category asymmetry coefficients
          tempSet.reset(((RooArgList*)_avgCEvens.At(cat0It))->at(cat1It)
              ->getVariables());
          coefVars->add(tempSet->getSize() > 0 ? *tempSet
              : *((RooArgList*)_avgCEvens.At(cat0It))->at(cat1It));

          tempSet.reset(((RooArgList*)_avgCOdds.At(cat0It))->at(cat1It)
              ->getVariables());
          coefVars->add(tempSet->getSize() > 0 ? *tempSet
              : *((RooArgList*)_avgCOdds.At(cat0It))->at(cat1It));

          if (cat0It == 0 &&_tagCat1Type > 0) {
            // add dilutions tag 1
            tempSet.reset(_dilutions1.at(cat1It)->getVariables());
            coefVars->add(tempSet->getSize() > 0
                ? *tempSet : *_dilutions1.at(cat1It));

            tempSet.reset(_ADilWTags1.at(cat1It)->getVariables());
            coefVars->add(tempSet->getSize() > 0
                ? *tempSet : *_ADilWTags1.at(cat1It));
          }
        }
      }
    }

    // remove variables that are in basis functions
    TIterator* coefVarsIter = coefVars->createIterator();
    RooAbsArg* coefArg;
    while ((coefArg = (RooAbsArg*)coefVarsIter->Next()) != 0) {
      for (Int_t basisIter = 0; basisIter < _convSet.getSize(); ++basisIter) {
        if (_convSet.at(basisIter)->dependsOn(*coefArg))
          coefVars->remove(*coefArg);
      }
    }
    delete coefVarsIter;
  }

  return coefVars;
}

const char *RooBTagDecay::pruneRangeName(const char* rangeName, const RooArgSet& vars) const
{
    if (rangeName == 0 || strlen(rangeName)==0) return rangeName;
    TIterator* i = vars.createIterator();
    RooAbsArg* x = 0;
    while ((x = (RooAbsArg*)i->Next()) != 0) {
        if (!x->hasRange(rangeName)) continue; // OK to drop
        const RooAbsRealLValue *rar = dynamic_cast<const RooAbsRealLValue*>(x);
        if (rar && rar->getMin(rangeName)==rar->getMin() && rar->getMax(rangeName)==rar->getMax()) continue; // OK to drop!
        //const RooAbsCategory *rac = dynamic_cast<const RooAbsCategory*>(x);
        // if (rac &&  ) continue;
        cxcoutD(Integration) << "RooBTagDecay::pruneRangeName(" << GetName()
            << ") cannot check range content for " << x->GetName()
            << " yet.... " << endl;

        return rangeName; // neither real, nor category... just give up...

    }
    delete i;
    return 0;
}

//_____________________________________________________________________________
Int_t RooBTagDecay::getCoefAnalyticalIntegral(Int_t coef, RooArgSet& allVars,
    RooArgSet& analVars, const char* rName) const 
{
  // copy variables that can be integrated over analytically from allVars to
  // analVars for the specified basis function's coefficient and return
  // integration code

  cxcoutD(Integration) << "RooBTagDecay::getCoefAnalyticalIntegral("
      << GetName() << ") (" << coef << ", " << allVars << ", "
      << (rName ? rName: "<none>") << ")" << endl;

  // integrate numerically if variables are unchecked
  if (!_checkVars || !checkVarDep(_time.arg())
      || (_tagCat0Type > 1 && !checkVarDep(_tagCat0.arg()))
      || (_tagCat1Type > 1 && !checkVarDep(_tagCat1.arg()))
      || (_tagCat0Type > 0 && (!checkVarDep(_iTag0.arg())
      || !checkTag(kTRUE, kFALSE)))
      || (_tagCat1Type > 0 && (!checkVarDep(_iTag1.arg())
      || !checkTag(kTRUE, kTRUE)))
      || (_tags > 1 && (!checkVarDep(_fTag.arg())
      || !checkTag(kFALSE, kFALSE))))
    return 0;

  // check that tagging parameters don't depend on integration variables
  RooArgSet intVars(allVars);
  TIterator* varIter = intVars.createIterator();
  RooAbsArg* var = 0;
  while ((var = (RooAbsArg*)varIter->Next()) != 0) {
    if (!checkVarDep(*var, kFALSE, kTRUE)) intVars.remove(*var, kTRUE, kTRUE);
  }
  delete varIter;

  // check basis index
  if (coef != _coshBasis && coef != _cosBasis && (_tags > 1
      || (coef != _sinhBasis && coef != _sinBasis))) {
    coutE(InputArguments) << "RooBTagDecay::getCoefAnalyticalIntegral("
        << GetName() << ") cannot find basis with index " << coef << endl;
    return 0.;
  }

  // initialize integration code variables
  Int_t  intCode   = 0;
  Bool_t intTagCat = kTRUE;
  Bool_t intITag0  = kFALSE;
  Bool_t intITag1  = kFALSE;
  Bool_t intFTag   = kFALSE;

  // can we drop the rangeName???
  const char *rangeName = pruneRangeName( rName, intVars );
  if ( rangeName == 0 && rName != 0 )  { 
    cxcoutD(Integration) << "RooBTagDecay::getCoefAnalyticalIntegral("
        << GetName() << ") dropping rangeName \"" << rName << "\"" << endl;
    intCode += 16;
  }

  // integrate over tagging categories?
  if ((_tagCat0Type > 1 && !intVars.remove(_tagCat0.arg(), kTRUE, kTRUE))
      || (_tagCat1Type > 1 && !intVars.remove(_tagCat1.arg(), kTRUE, kTRUE)))
    intTagCat = kFALSE;

  // integrate over initial state tags?
  if (_tagCat0Type > 0) intITag0 = intVars.remove(_iTag0.arg(), kTRUE, kTRUE);
  if (_tagCat1Type > 0) intITag1 = intVars.remove(_iTag1.arg(), kTRUE, kTRUE);

  // integrate over final state tag?
  if (_tags > 1) intFTag = intVars.remove(_fTag.arg(), kTRUE, kTRUE);

  // get integration code
  if (intVars.getSize() > 0) {
    bool odd = ( coef == _sinhBasis || coef == _sinBasis );
    if (odd && _tags > 1 ) return 0;
    
    intCode += 32 * coefArg(coef).getAnalyticalIntegral(intVars, analVars,
        rangeName);
  }

  // return the integration code if there are no explicit tags
  if (_tags == 0) return intCode;

  if (intTagCat && (rangeName == 0
      || ((_tagCat0Type > 1 ? !_tagCat0.hasRange(rangeName) : kTRUE)
      && (_tagCat1Type > 1 ? !_tagCat1.hasRange(rangeName) : kTRUE)))) {
    // add the tagging category to integration variables
    intCode += 8;
    if (_tagCat0Type > 1) analVars.add(_tagCat0.arg());
    if (_tagCat1Type > 1) analVars.add(_tagCat1.arg());

    // initialize tagging category maps
    if (_tagCat0Indices.empty()) initTagCatMaps(0);
    if (_tagCat1Indices.empty()) initTagCatMaps(1);
  }

  if (intITag0 && (rangeName == 0 || !_iTag0.hasRange(rangeName))) {
    // add initial state tag 0 to integration variables
    intCode += 4;
    analVars.add(_iTag0.arg());
  }

  if (intITag1 && (rangeName == 0 || !_iTag1.hasRange(rangeName))) {
    // add initial state tag 1 to integration variables
    intCode += 2;
    analVars.add(_iTag1.arg());
  }

  if (intFTag && (rangeName == 0 || !_fTag.hasRange(rangeName))) {
    // add the final state tag to integration variables
    intCode += 1;
    analVars.add(_fTag.arg());
  }

  return intCode;
}

//_____________________________________________________________________________
Double_t RooBTagDecay::coefAnalyticalIntegral(Int_t coef, Int_t code,
    const char* rangeName) const 
{
  // return analytical integral for basis function's coefficient

  // drop rangeName?
  if (code&16) rangeName = 0;

  // get integration code for coefficient
  Int_t  coefCode  = code >> 5;

  // get coefficient's integral
  Double_t coefInt = 0.;
  // determine if basis function is even or odd
  Bool_t odd = coef == _cosBasis || coef == _sinBasis;
  if (odd && _tags>1) return 0;
  coefInt = (coefCode!=0?coefArg(coef).analyticalIntegral(coefCode, rangeName):coefArg(coef).getVal());

  // return the integral if we don't have to evaluate explicit tags
  if (_tags < 1 || coefInt == 0.) return coefInt;

  // get tagging integration codes
  Bool_t intTagCat = (Bool_t)(code & 8);
  Bool_t intITag0  = (Bool_t)(code & 4);
  Bool_t intITag1  = (Bool_t)(code & 2);
  Bool_t intFTag   = (Bool_t)(code & 1);


  // get values of initial state flavour tags
  Int_t iT0Val = _tagCat0Type > 0 && _iTag0Val > 1 ? _iTag0 : _iTag0Val;
  Int_t iT1Val = _tagCat1Type > 0 && _iTag1Val > 1 ? _iTag1 : _iTag1Val;

  // calculate terms in asymmetry coefficient: depend on initial state tags
  Double_t term0 = 0., term1 = 0., term01 = 0.;
  Bool_t calc0 = _tagCat0Type > 0 && iT0Val != 0 && !intITag0;
  Bool_t calc1 = _tagCat1Type > 0 && iT1Val != 0 && !intITag1;
  Int_t catPos0 = -1, catPos1 = -1;
  if (intTagCat) {
    // sum over tagging categories
    term01 += odd ? _avgCOddSum : _avgCEvenSum;
  } else {
    // don't sum over tagging categories
    Double_t catCoef = tagCatCoef(catPos0, catPos1);
    term01 += catCoef * ((RooAbsReal*)((RooArgList*)
        (odd ? _avgCOdds : _avgCEvens).At(catPos0))->at(catPos1))->getVal();
  }

  if (calc0 || calc1) {
    // loop over tagging categories
    Int_t cat0High = catPos0 > -1 ? catPos0 + 1 : (_tagCat0Type < 1 ? 1
        : _tagCat0.arg().numTypes());
    Int_t cat1High = catPos1 > -1 ? catPos1 + 1 : (_tagCat1Type < 1 ? 1
        : _tagCat1.arg().numTypes());
    for (Int_t cat0It = catPos0 > -1 ? catPos0 : 0; cat0It < cat0High;
        ++cat0It) {
      for (Int_t cat1It = catPos1 > -1 ? catPos1 : 0; cat1It < cat1High;
          ++cat1It) {
        // get category coefficients
        Double_t catCoef = tagCatCoefUnsafe(cat0It, cat1It);
        Double_t cEvenOdd0 = ((RooAbsReal*)((RooArgList*)
            (odd ? _avgCOdds : _avgCEvens).At(cat0It))->at(cat1It))
            ->getVal();
        Double_t cEvenOdd1 = ((RooAbsReal*)((RooArgList*)
            (odd ? _avgCEvens : _avgCOdds).At(cat0It))->at(cat1It))
            ->getVal();

        if (calc0) {
          // calculate tag 0 term
          term0 += catCoef * iT0Val
              * ((RooAbsReal*)_dilutions0.at(cat0It))->getVal()
              * (cEvenOdd1 - ((RooAbsReal*)_ADilWTags0.at(cat0It))->getVal()
              * cEvenOdd0);
        }
        if (calc1) {
          // calculate tag 1 term
          term1 += catCoef * iT1Val
              * ((RooAbsReal*)_dilutions1.at(cat1It))->getVal()
              * (cEvenOdd1 - ((RooAbsReal*)_ADilWTags1.at(cat1It))->getVal()
              * cEvenOdd0);
        }
        if (calc0 && calc1) {
          // calculate tag0/tag1 interference term
          Double_t ADilWTag2 = ((RooAbsReal*)_ADilWTags0.at(cat0It))->getVal()
              * ((RooAbsReal*)_ADilWTags1.at(cat1It))->getVal();
          term01 += catCoef * iT0Val * iT1Val
              * ((RooAbsReal*)_dilutions0.at(cat0It))->getVal()
              * ((RooAbsReal*)_dilutions1.at(cat1It))->getVal()
              * ((1. + ADilWTag2) * cEvenOdd0 - ADilWTag2 * cEvenOdd1);
        }
      }
    }
  }

  coefInt *= term01 + term0 + term1;
  if ((intITag0 && _iTag0Val > 1) || _tagCat0Type < 1 || _iTag0Val == 0)
    coefInt *= 2.;
  if ((intITag1 && _iTag1Val > 1) || _tagCat1Type < 1 || _iTag1Val == 0)
    coefInt *= 2.;

  // integrate over final state tag if required
  if (_tags == 1) {
    // no final state tag
    return coefInt;
  } else if ((intFTag && _fTagVal == 2) || _fTagVal == 0) {
    // integrate over the final state tag
    return (odd ? -2. * _ANorm : 2.) * coefInt;
  } else if (_fTagVal == 2) {
    // don't integrate over final state tag
    Int_t fTVal = _fTagVal == 2 ? _fTag : _fTagVal;
    return (odd ? fTVal - _ANorm : 1. - fTVal * _ANorm) * coefInt;
  }

  return 0.;
}

//_____________________________________________________________________________
void RooBTagDecay::setMaxVal(const Double_t val)
{
  _maxVal = val;
}

//_____________________________________________________________________________
Int_t RooBTagDecay::getMaxVal(const RooArgSet& /*vars*/) const
{
  return _maxVal < 0 ? 0 : _maxVal;
}

//_____________________________________________________________________________
Double_t RooBTagDecay::maxVal(Int_t code) const
{
  if (code == 0) {
    coutF(InputArguments) << "RooBTagDecay::maxVal(" << GetName()
        << "): got code 0" << endl;
    assert(0);
  }

  return _maxVal;
}

//_____________________________________________________________________________
Int_t RooBTagDecay::getGenerator(const RooArgSet& directVars,
    RooArgSet &generateVars, Bool_t /*staticInitOK*/) const
{
  // copy variables that can be generated directly by RooBTagDecay from
  // directVars to generateVars and return generation code

  // use accept/reject in certain cases
  if (!_checkVars || !checkVarDep(_time.arg())
      || (_tagCat0Type > 1 && (!checkVarDep(_tagCat0.arg()) || _iTag0Val < 2
          || !checkVarDep(_iTag0.arg()) || !checkTag(kTRUE, kFALSE)))
      || (_tagCat1Type > 1 && (!checkVarDep(_tagCat1.arg()) || _iTag1Val < 2
          || !checkVarDep(_iTag1.arg()) || !checkTag(kTRUE, kTRUE)))
      || (_tags > 1 && (_fTagVal < 2 || !checkVarDep(_fTag.arg())
          || !checkTag(kFALSE, kFALSE))))
    return 0;

  Int_t genCode = 0;

  // find time variable
  RooAbsArg* arg0 = directVars.find(_time.arg().GetName());
  if (arg0 != 0) {
    genCode += 1;
    generateVars.add(*arg0);
  }

  if (_tags < 1) return genCode;

  // check which tagging variables to generate
  arg0 = 0;
  RooAbsArg* arg10 = 0;
  if (_tagCat0Type > 1) {
    // find the tagging category variable for tag 0
    arg0 = directVars.find(_tagCat0.arg().GetName());
    if (arg0 == 0) return genCode;

    // find initial state tag 0 variable
    arg10 = directVars.find(_iTag0.arg().GetName());
    if (arg10 == 0) return genCode;
  }

  RooAbsArg* arg01 = 0;
  RooAbsArg* arg11 = 0;
  if (_tagCat1Type > 1) {
    // find the tagging category variable for tag 1
    arg01 = directVars.find(_tagCat1.arg().GetName());
    if (arg01 == 0) return genCode;

    // find initial state tag 1 variable
    arg11 = directVars.find(_iTag1.arg().GetName());
    if (arg11 == 0) return genCode;
  }

  RooAbsArg* arg2 = 0;
  if (_tags > 1) {
    // find final state tag variable
    arg2 = directVars.find(_fTag.arg().GetName());
    if (arg2 == 0) return genCode;
  }

  // build generation code and add variables to generation variables set
  if (arg0 != 0 || arg01 != 0) {
    // generate tagging category
    genCode += 2;
    if (arg0 != 0)  generateVars.add(*arg0);
    if (arg01 != 0) generateVars.add(*arg01);
  }

  if (arg10 != 0) {
    // generate initial state tag 0
    genCode += 4;
    generateVars.add(*arg10);
  }

  if (arg11 != 0) {
    // generate initial state tag 1
    genCode += 8;
    generateVars.add(*arg11);
  }

  if (arg2 != 0) {
    // generate final state tag
    genCode += 16;
    generateVars.add(*arg2);
  }

  return genCode;
}

//_____________________________________________________________________________
void RooBTagDecay::generateEvent(Int_t code)
{
  // generate values for the variables corresponding to the generation code

  // set minimum Gamma for time envelope: Gamma - |DeltaGamma| / 2
  Double_t gammaMin  = 1. / _tau - TMath::Abs(_dGamma) / 2.;

  // generate event
  while(true) {
    Double_t timeGen = 0.;
    if (code & 1) {
      // generate time variable with the exponential envelope function
      timeGen = -log(RooRandom::uniform()) / gammaMin;
      if (_decayType == Flipped || (_decayType == DoubleSided
          && RooRandom::uniform() > 0.5))
        timeGen *= -1.;
      if (timeGen < _time.min() || timeGen > _time.max()) continue;
    } else {
      // get generated time value
      timeGen = _time;
    }

    // get (unnormalized) PDF and envelope values for generated time
    Double_t tAbs         = TMath::Abs(timeGen);
    Double_t pdfVal       = 0.;
    Double_t envVal       = 0.;
    Double_t evenTerms    = _coshCoef * cosh(_dGamma * timeGen / 2.);
    Double_t oddTerms     = _cosCoef * cos(_dm * timeGen);
    Double_t evenTermsEnv = 0.;
    Double_t oddTermsEnv  = 0.;
    if (_tags < 2) {
        evenTerms    += _sinhCoef * sinh(_dGamma * timeGen / 2.);
        oddTerms     += _sinCoef * sin(_dm * timeGen);
        evenTermsEnv  = TMath::Abs(_coshCoef) + TMath::Abs(_sinhCoef) / 2.;
        oddTermsEnv   = sqrt(_cosCoef * _cosCoef + _sinCoef * _sinCoef);
    } else {
        evenTermsEnv  = TMath::Abs(_coshCoef);
        oddTermsEnv   = TMath::Abs(_cosCoef);
    }

    if (code > 1) {
      if (_tags == 1) {
        // generate tagging category and initial state flavour(s):
        // use PDF integrated over tagging variables
        pdfVal = _avgCEvenSum * evenTerms + _avgCOddSum * oddTerms;
        envVal = TMath::Abs(_avgCEvenSum) * evenTermsEnv
            + TMath::Abs(_avgCOddSum) * oddTermsEnv;
      } else {
        // generate tagging category, initial state flavour(s) and final state
        // flavour: use PDF integrated over tagging variables
        pdfVal = _avgCEvenSum * evenTerms - _ANorm * _avgCOddSum * oddTerms;
        envVal = TMath::Abs(_avgCEvenSum) * evenTermsEnv
            + TMath::Abs(_ANorm * _avgCOddSum) * oddTermsEnv;
      }

    } else if (_tags > 0) {
      // generate a value for the time variable only, use PDF with flavour
      // tag(s) and tagging categories
      Int_t cat0Pos = -1, cat1Pos = -1;
      Double_t catCoef = tagCatCoef(cat0Pos, cat1Pos);

      // check tagging category coefficient
      if (catCoef < 0.) {
        coutF(Generation) << "RooBTagDecay::generateEvent(" << GetName()
            << ") not a valid tagging category coefficient"
            << endl;
        assert(0);
      }

      // get asymmetry coefficients
      Double_t avgCEven = ((RooAbsReal*)((RooArgList*)
          _avgCEvens.At(cat0Pos))->at(cat1Pos))->getVal();
      Double_t avgCOdd = ((RooAbsReal*)((RooArgList*)
          _avgCOdds.At(cat0Pos))->at(cat1Pos))->getVal();

      // calculate even and odd coefficients
      Double_t cEven = avgCEven;
      Double_t cOdd  = avgCOdd;
      if (_tagCat0Type > 0) {
        // tag 0 term
        cEven += _iTag0 * ((RooAbsReal*)_dilutions0.at(cat0Pos))->getVal()
            * (avgCOdd - ((RooAbsReal*)_ADilWTags0.at(cat0Pos))->getVal()
            * avgCEven);
        cOdd += _iTag0 * ((RooAbsReal*)_dilutions0.at(cat0Pos))->getVal()
            * (avgCEven - ((RooAbsReal*)_ADilWTags0.at(cat0Pos))->getVal()
            * avgCOdd);
      }
      if (_tagCat1Type > 0) {
        // tag 1 term
        cEven += _iTag1 * ((RooAbsReal*)_dilutions1.at(cat1Pos))->getVal()
            * (avgCOdd - ((RooAbsReal*)_ADilWTags1.at(cat1Pos))->getVal()
            * avgCEven);
        cOdd += _iTag1 * ((RooAbsReal*)_dilutions1.at(cat1Pos))->getVal()
            * (avgCEven - ((RooAbsReal*)_ADilWTags1.at(cat1Pos))->getVal()
            * avgCOdd);
      }
      if (_tagCat0Type > 0 && _tagCat1Type > 0) {
        // tag 0/tag 1 interference term
        Double_t ADilWTag2 = ((RooAbsReal*)_ADilWTags0.at(cat0Pos))->getVal()
            * ((RooAbsReal*)_ADilWTags1.at(cat1Pos))->getVal();
        cEven += _iTag0 * _iTag1
            * ((RooAbsReal*)_dilutions0.at(cat0Pos))->getVal()
            * ((RooAbsReal*)_dilutions1.at(cat1Pos))->getVal()
            * ((1. + ADilWTag2) * avgCEven - ADilWTag2 * avgCOdd);
        cOdd += _iTag0 * _iTag1
            * ((RooAbsReal*)_dilutions0.at(cat0Pos))->getVal()
            * ((RooAbsReal*)_dilutions1.at(cat1Pos))->getVal()
            * ((1. + ADilWTag2) * avgCOdd - ADilWTag2 * avgCEven);
      }

      if (_tags > 1) {
        cEven *= 1. - _fTag * _ANorm;
        cOdd  *= _fTag - _ANorm;
      }

      // calculate PDF and envelope values
      pdfVal = cEven * evenTerms + cOdd * oddTerms;
      envVal = TMath::Abs(cEven) * evenTermsEnv
          + TMath::Abs(cOdd) * oddTermsEnv;

    } else {
      // generate a value for the time variable only, use PDF without tagging
      pdfVal = oddTerms + evenTerms;
      envVal = evenTermsEnv + oddTermsEnv;
    }

    // check if PDF value is positive
    if (pdfVal < 0.) {
      coutF(Generation) << "RooBTagDecay::generateEvent(" << GetName()
          << ") PDF is negative for generated time value" << endl;
      assert(0);
    }

    if (code & 1) {
      // accept/reject generated time value, using the envelope as maximum
      if (exp(-tAbs / _tau) * pdfVal
          < RooRandom::uniform() * exp(-gammaMin * tAbs) * envVal)
        continue;

      // set time value
      _time = timeGen;
    }

    // exit generation loop if we don't generate tagging variables
    if (code < 2) break;

    Int_t catGen[2] = {0, 0};
    Double_t avgCEven = 0., avgCOdd = 0.;
    if (_tagCat0Type > 1 || _tagCat1Type > 1) {
      // generate value for the tagging category
      Double_t catPdfSum = 0.;
      Double_t rand = RooRandom::uniform();
      Int_t cat0Max = _tagCat0Type < 1 ? 1 : _tagCat0.arg().numTypes();
      Int_t cat1Max = _tagCat1Type < 1 ? 1 : _tagCat1.arg().numTypes();
      Bool_t generatedTagCat = kFALSE;
      while (catGen[0] < cat0Max) {
        while (catGen[1] < cat1Max) {
          // get tagging category coefficient
          Double_t catCoef = tagCatCoefUnsafe(catGen[0], catGen[1]);

          // check coefficient
          if (catCoef < 0.) {
            coutF(Generation) << "RooBTagDecay::generateEvent(" << GetName()
                << ") not a valid tagging category coefficient"
                << endl;
            assert(0);
          }

          // get average even and odd coefficients
          avgCEven = ((RooAbsReal*)((RooArgList*)_avgCEvens.At(catGen[0]))
              ->at(catGen[1]))->getVal();
          avgCOdd = ((RooAbsReal*)((RooArgList*)_avgCOdds.At(catGen[0]))
              ->at(catGen[1]))->getVal();

          // add the value of the (tags-integrated) PDF to the categories sum
          if (_tags < 2)
            catPdfSum += catCoef * (avgCEven * evenTerms + avgCOdd * oddTerms);
          else
            catPdfSum += catCoef * (avgCEven * evenTerms
                - _ANorm * avgCOdd * oddTerms);

          if (catPdfSum < rand * pdfVal) {
            // go to next tagging category
            if (catGen[1] < cat1Max - 1) {
              ++catGen[1];
            } else {
              catGen[0] = 0;
              ++catGen[0];
            }
          } else {
            // generate in this tagging category
            generatedTagCat = kTRUE;
            break;
          }
        }

        if (generatedTagCat) break;
      }

      // check generated value
      if (!generatedTagCat) {
        coutW(Generation) << "RooBTagDecay::generateEvent(" << GetName()
            << ") generation of event aborted due to a numerical problem in generation of the tagging category"
            << endl;
        continue;
      }

      // set tagging category value
      if (_tagCat0Type > 1) _tagCat0 = getTagCatIndex(catGen[0], kFALSE);
      if (_tagCat1Type > 1) _tagCat1 = getTagCatIndex(catGen[1], kTRUE);
    }

    // calculate dilution factors
    Double_t cEDil0 = 0., cODil0 = 0., cEDil1 = 0., cODil1 = 0.,
        cEDil01 = 0., cODil01 = 0.;
    if (_tagCat0Type > 0) {
      cEDil0 = ((RooAbsReal*)_dilutions0.at(catGen[0]))->getVal() * (avgCOdd
          - ((RooAbsReal*)_ADilWTags0.at(catGen[0]))->getVal() * avgCEven);
      cODil0 = ((RooAbsReal*)_dilutions0.at(catGen[0]))->getVal() * (avgCEven
          - ((RooAbsReal*)_ADilWTags0.at(catGen[0]))->getVal() * avgCOdd);
    }
    if (_tagCat1Type > 0) {
      cEDil1 = ((RooAbsReal*)_dilutions1.at(catGen[1]))->getVal() * (avgCOdd
          - ((RooAbsReal*)_ADilWTags1.at(catGen[1]))->getVal() * avgCEven);
      cODil1 = ((RooAbsReal*)_dilutions1.at(catGen[1]))->getVal() * (avgCEven
          - ((RooAbsReal*)_ADilWTags1.at(catGen[1]))->getVal() * avgCOdd);
    }
    if (_tagCat0Type > 0 && _tagCat1Type > 0) {
      Double_t ADilWTag2 = ((RooAbsReal*)_ADilWTags0.at(catGen[0]))->getVal()
          * ((RooAbsReal*)_ADilWTags1.at(catGen[1]))->getVal();
      cEDil01 = ((RooAbsReal*)_dilutions0.at(catGen[0]))->getVal()
          * ((RooAbsReal*)_dilutions1.at(catGen[1]))->getVal()
          * ((1. + ADilWTag2) * avgCEven - ADilWTag2 * avgCOdd);
      cODil01 = ((RooAbsReal*)_dilutions0.at(catGen[0]))->getVal()
          * ((RooAbsReal*)_dilutions1.at(catGen[1]))->getVal()
          * ((1. + ADilWTag2) * avgCOdd - ADilWTag2 * avgCEven);
    }

    // generate value for initial state tags
    Int_t iTagGen[2] = {1, 1};

    // first generate initial state tag 1
    if (_tagCat1Type > 0) {
      if (_iTag1Val == 3 && catGen[1] == 0) {
        // tagging category 0 is interpreted as the "untagged" category
        // if the initial state tag can assume values +1, -1 and 0!!!!!!!!!
        iTagGen[1] = 0;
      } else {
        // use CP asymmetry if initial state tag only has values +1 and -1
        Double_t ACP = 0.;
        if (_tags < 2)
          ACP = (cEDil1 * evenTerms + cODil1 * oddTerms)
              / (avgCEven * evenTerms + avgCOdd * oddTerms);
        else
          ACP = (cEDil1 * evenTerms - _ANorm * cODil1 * oddTerms)
          / (avgCEven * evenTerms - _ANorm * avgCOdd * oddTerms);

        if (2. * RooRandom::uniform() > 1. + ACP) iTagGen[1] = -1;
      }

      // set initial state tag value
      _iTag1 = iTagGen[1];
    }

    // then generate initial state tag 0
    if (_tagCat0Type > 0) {
      if (_iTag0Val == 3 && catGen[0] == 0) {
        // tagging category 0 is interpreted as the "untagged" category
        // if the initial state tag can assume values +1, -1 and 0!!!!!!!!!
        iTagGen[0] = 0;
      } else {
        // use CP asymmetry if initial state tag only has values +1 and -1
        Double_t ACP = 0.;
        if (_tags < 2)
          ACP = ((cEDil0 + iTagGen[1] * cEDil01) * evenTerms
              + (cODil0 + iTagGen[1] * cODil01) * oddTerms)
              / ((avgCEven + iTagGen[1] * cEDil1) * evenTerms
              + (avgCOdd + iTagGen[1] * cODil1) * oddTerms);
        else
          ACP = ((cEDil0 + iTagGen[1] * cEDil01) * evenTerms
              - _ANorm * (cODil0 + iTagGen[1] * cODil01) * oddTerms)
              / ((avgCEven + iTagGen[1] * cEDil1) * evenTerms
              - _ANorm *  (avgCOdd + iTagGen[1] * cODil1) * oddTerms);

        if (2. * RooRandom::uniform() > 1. + ACP) iTagGen[0] = -1;
      }

      // set initial state tag value
      _iTag0 = iTagGen[0];
    }

    if (_tags > 1) {
      // generate value for final state tag
      Int_t fTagGen = 1;
      Double_t cEven = avgCEven + iTagGen[0] * cEDil0 + iTagGen[1] * cEDil1
          + iTagGen[0] * iTagGen[1] * cEDil01;
      Double_t cOdd = avgCOdd + iTagGen[0] * cODil0 + iTagGen[1] * cODil1
          + iTagGen[0] * iTagGen[1] * cODil01;
      Double_t Af = (-_ANorm * cEven * evenTerms + cOdd * oddTerms)
          / (cEven * evenTerms - _ANorm * cOdd * oddTerms);

      if (2. * RooRandom::uniform() > 1. + Af) fTagGen = -1;

      // set final state tag value
      _fTag = fTagGen;
    }

    // exit generation loop
    break;
  }
}

//_____________________________________________________________________________
Int_t RooBTagDecay::getTagCatPosition(Int_t tagCatIndex, Bool_t tag1) const
{
  if (!tag1 && _tagCat0Positions.empty()) initTagCatMaps(0);
  else if (tag1 && _tagCat1Positions.empty()) initTagCatMaps(1);

  return tag1 ? _tagCat1Positions.find(tagCatIndex)->second
      : _tagCat0Positions.find(tagCatIndex)->second;
}

//_____________________________________________________________________________
Int_t RooBTagDecay::getTagCatIndex(Int_t tagCatPosition, Bool_t tag1) const
{
  if (!tag1 && _tagCat0Indices.empty()) initTagCatMaps(0);
  else if (tag1 && _tagCat1Indices.empty()) initTagCatMaps(1);

  return tag1 ? _tagCat1Indices.find(tagCatPosition)->second
      : _tagCat0Indices.find(tagCatPosition)->second;
}

//_____________________________________________________________________________
RooArgList* RooBTagDecay::dilutions(Bool_t tag1) const
{
  if (tag1) return new RooArgList(_dilutions1, "dilutions1");
  return new RooArgList(_dilutions0, "dilutions0");
}

//_____________________________________________________________________________
RooArgList* RooBTagDecay::ADilWTags(Bool_t tag1) const
{
  if (tag1) return new RooArgList(_ADilWTags1, "ADilWTags1");
  return new RooArgList(_ADilWTags0, "ADilWTags0");
}

//_____________________________________________________________________________
RooArgList* RooBTagDecay::avgCEvens(Int_t tagCat0) const
{
  if (tagCat0 < 0 || tagCat0 >= _avgCEvens.GetEntries())
    return new RooArgList("avgCEvens");
  return new RooArgList(*(RooArgList*)_avgCEvens.At(tagCat0), "avgCEvens");
}

//_____________________________________________________________________________
RooArgList* RooBTagDecay::avgCOdds(Int_t tagCat0) const
{
  if (tagCat0 < 0 || tagCat0 >= _avgCOdds.GetEntries())
    return new RooArgList("avgCOdds");
  return new RooArgList(*(RooArgList*)_avgCOdds.At(tagCat0), "avgCOdds");
}

//_____________________________________________________________________________
RooArgList* RooBTagDecay::tagCatCoefs(Int_t tagCat0) const
{
  if (tagCat0 < 0 || tagCat0 >= _tagCatCoefs.GetEntries())
    return new RooArgList("tagCatCoefs");
  return new RooArgList(*(RooArgList*)_tagCatCoefs.At(tagCat0), "tagCatCoefs");
}

//_____________________________________________________________________________
void RooBTagDecay::initTaggingCats(const TObjArray& tagCatCoefs,
    const RooArgList& dilutions0, const RooArgList& dilutions1,
    const RooArgList& ADilWTags0, const RooArgList& ADilWTags1,
    const TObjArray& avgCEvens, const TObjArray& avgCOdds)
{
  // set proxies for tagging category coefficients, dilutions,
  // dilution/wrong-tag asymmetries, average even coefficients and
  // average odd coefficients

  // get number of tagging categories
  Int_t numTagCats[2] = {0, 0};
  if (_tagCat0Type == 1) numTagCats[0] = 1;
  else if (_tagCat0Type > 1) numTagCats[0] = _tagCat0.arg().numTypes();
  if (_tagCat1Type == 1) numTagCats[1] = 1;
  else if (_tagCat1Type > 1) numTagCats[1] = _tagCat1.arg().numTypes();

  // check for cases with 0 or 1 categories
  Int_t numIters[2] = {numTagCats[0], numTagCats[1]};
  if (numTagCats[0] == 0 && numTagCats[1] == 0) {
    coutW(InputArguments) << "RooBTagDecay::initTaggingCats(" << GetName()
        << ") there are no tagging categories" << endl;
    _tagCat0Type = 0;
    _tagCat1Type = 0;
    return;
  } else if (numTagCats[0] == 1 && numTagCats[1] == 0) {
    _tagCat0Type = 1;
    _tagCat1Type = 0;
    numIters[1] = 1;
  } else if (numTagCats[0] == 0 && numTagCats[1] == 1) {
    _tagCat0Type = 0;
    _tagCat1Type = 1;
    numIters[0] = 1;
  } else if (numTagCats[0] == 1 && numTagCats[1] == 1) {
    _tagCat0Type = 1;
    _tagCat1Type = 1;
  } else if (numTagCats[0] == 0 && numTagCats[1] > 1) {
    numIters[0] = 1;
  } else if (numTagCats[0] > 1 && numTagCats[1] == 0) {
    numIters[1] = 1;
  }

  if (numTagCats[0] < 2 && numTagCats[1] < 2) {
    // only one tagging category: no coefficients
    if (tagCatCoefs.GetEntries() != 0)
      coutW(InputArguments) << "RooBTagDecay::initTaggingCats(" << GetName()
          << ") there is only one tagging category: category coefficient(s) will not be used"
          << endl;
  } else {
    // check number of category coefficient arrays
    if (!(numTagCats[0] > 0 && tagCatCoefs.GetEntries() == numTagCats[0])
        && !(numTagCats[0] == 0 && tagCatCoefs.GetEntries() == 1)) {
      coutF(InputArguments) << "RooBTagDecay::initTaggingCats(" << GetName()
          << ") number of tagging category coefficients does not match number of category types"
          << endl;
      assert(0);
    }
  }

  // check number of asymmetry coefficient arrays
  if (!(numTagCats[0] > 0 && avgCEvens.GetEntries() == numTagCats[0])
        && !(numTagCats[0] == 0 && avgCEvens.GetEntries() == 1)) {
    coutF(InputArguments) << "RooBTagDecay::initTaggingCats(" << GetName()
        << ") number of average even asymmetry coefficients does not match number of category types"
        << endl;
    assert(0);
  }
  if (!(numTagCats[0] > 0 && avgCOdds.GetEntries() == numTagCats[0])
        && !(numTagCats[0] == 0 && avgCOdds.GetEntries() == 1)) {
    coutF(InputArguments) << "RooBTagDecay::initTaggingCats(" << GetName()
        << ") number of average odd asymmetry coefficients does not match number of category types"
        << endl;
    assert(0);
  }

  // check number of dilutions
  if (dilutions0.getSize() != numTagCats[0]) {
    coutF(InputArguments) << "RooBTagDecay::initTaggingCats(" << GetName()
        << ") number of dilutions for tag 0 does not match number of category types"
        << endl;
    assert(0);
  }
  if (dilutions1.getSize() != numTagCats[1]) {
    coutF(InputArguments) << "RooBTagDecay::initTaggingCats(" << GetName()
        << ") number of dilutions for tag 1 does not match number of category types"
        << endl;
    assert(0);
  }

  // check number of dilution/wrong-tag asymmetries
  if (ADilWTags0.getSize() != numTagCats[0]) {
    coutF(InputArguments) << "RooBTagDecay::initTaggingCats(" << GetName()
        << ") number of dilution/wrong-tag asymmetries for tag 0 does not match number of category types"
        << endl;
    assert(0);
  }
  if (ADilWTags1.getSize() != numTagCats[1]) {
    coutF(InputArguments) << "RooBTagDecay::initTaggingCats(" << GetName()
        << ") number of dilution/wrong-tag asymmetries for tag 1 does not match number of category types"
        << endl;
    assert(0);
  }

  // initialize tagging category maps
  initTagCatMaps(0);
  initTagCatMaps(1);

  // make tagging coefficient arrays owners of their lists
  _tagCatCoefs.SetOwner(kTRUE);
  _avgCEvens.SetOwner(kTRUE);
  _avgCOdds.SetOwner(kTRUE);

  // loop over tagging categories for tag 0
  TString avgCoefFormString;
  TString tagCatCoefFormString("1.");
  for (Int_t cat0It = 0; cat0It < numIters[0]; ++cat0It) {
    RooArgList* cat1Coefs = 0;
    if (numTagCats[0] > 1 || numTagCats[1] > 1) {
      // get tagging category coefficients array
      cat1Coefs = dynamic_cast<RooArgList*>(tagCatCoefs.At(cat0It));
      if (cat1Coefs == 0) {
        coutF(InputArguments) << "RooBTagDecay::initTaggingCats(" << GetName()
            << ") expected the tagCatCoefs TObjArray to contain RooArgLists with tagging category coefficients"
            << endl;
        assert(0);
      }

      // check number of tagging category coefficients
      Bool_t checkCat1Coefs = kFALSE;
      if (cat0It == 0) {
        if ((numTagCats[1] == 0 && cat1Coefs->getSize() == 1)
            || (numTagCats[1] > 0 && cat1Coefs->getSize() == numTagCats[1])) {
          if (numTagCats[0] > 1) _tagCat0Type = 2;
          if (numTagCats[1] > 1) _tagCat1Type = 2;
          checkCat1Coefs = kTRUE;
        } else if ((numTagCats[1] == 0 && cat1Coefs->getSize() == 0)
            || (numTagCats[1] > 0 && cat1Coefs->getSize() == numTagCats[1]-1)) {
          if (numTagCats[0] > 1) _tagCat0Type = 3;
          if (numTagCats[1] > 1) _tagCat1Type = 3;
          checkCat1Coefs = kTRUE;
        }
      } else if ((numTagCats[1] == 0 && cat1Coefs->getSize() == 1)
          || (numTagCats[1] > 0 && cat1Coefs->getSize() == numTagCats[1])) {
        checkCat1Coefs = kTRUE;
      }

      if (!checkCat1Coefs) {
        coutF(InputArguments) << "RooBTagDecay::initTaggingCats(" << GetName()
            << ") number of tagging category coefficients does not match number of category types"
            << endl;
        assert(0);
      }
    }

    // get tagging category asymmetry coefficients arrays
    RooArgList* cat1CEvens = dynamic_cast<RooArgList*>(avgCEvens.At(cat0It));
    if (cat1CEvens == 0) {
      coutF(InputArguments) << "RooBTagDecay::initTaggingCats(" << GetName()
          << ") expected the avgCEvens TObjArray to contain RooArgLists with average even coeffients"
          << endl;
      assert(0);
    }
    RooArgList* cat1COdds = dynamic_cast<RooArgList*>(avgCOdds.At(cat0It));
    if (cat1COdds == 0) {
      coutF(InputArguments) << "RooBTagDecay::initTaggingCats(" << GetName()
          << ") expected the avgCOdds TObjArray to contain RooArgLists with average odd coeffients"
          << endl;
      assert(0);
    }

    // check number of tagging category asymmetry coefficients
    if (!(numTagCats[1] > 0 && cat1CEvens->getSize() == numTagCats[1])
          && !(numTagCats[1] == 0 && cat1CEvens->getSize() == 1)) {
      coutF(InputArguments) << "RooBTagDecay::initTaggingCats(" << GetName()
          << ") number of average even coefficients does not match number of category types"
          << endl;
      assert(0);
    }
    if (!(numTagCats[1] > 0 && cat1COdds->getSize() == numTagCats[1])
          && !(numTagCats[1] == 0 && cat1COdds->getSize() == 1)) {
      coutF(InputArguments) << "RooBTagDecay::initTaggingCats(" << GetName()
          << ") number of average odd coefficients does not match number of category types"
          << endl;
      assert(0);
    }

    if (numTagCats[0] > 0) {
      // get dilution for tag 0
      RooAbsReal* dil0 = dynamic_cast<RooAbsReal*>(dilutions0.at(cat0It));
      if (dil0 != 0) _dilutions0.add(*dil0);
      else {
        coutF(InputArguments) << "RooBTagDecay::initTaggingCats("
            << GetName() << ") dilution '"
            << dilutions0.at(cat0It)->GetName()
            << "' is not a real-valued variable" << endl;
        assert(0);
      }

      // get dilution/wrong-tag asymmetry for tag 0
      RooAbsReal* ADil0 = dynamic_cast<RooAbsReal*>(ADilWTags0.at(cat0It));
      if (ADil0 != 0) _ADilWTags0.add(*ADil0);
      else {
        coutF(InputArguments) << "RooBTagDecay::initTaggingCats("
            << GetName() << ") dilution/wrong-tag asymmetry '"
            << ADilWTags0.at(cat0It)->GetName()
            << "' is not a real-valued variable" << endl;
        assert(0);
      }
    }

    // create list proxy for tagging category coefficients
    RooListProxy* catCoefList = 0;
    if (numTagCats[0] > 1 || numTagCats[1] > 1) {
      TString coefsName("tagCatCoefs");
      coefsName += cat0It;
      catCoefList = new RooListProxy(coefsName, coefsName, this);
      _tagCatCoefs.Add(catCoefList);
    }

    // create list proxies for tagging category asymmetry coefficients
    TString cEName("avgCEvens");
    TString cOName("avgCOdds");
    cEName += cat0It;
    cOName += cat0It;
    RooListProxy* avgCEvenList = new RooListProxy(cEName, cEName, this);
    RooListProxy* avgCOddList  = new RooListProxy(cOName, cOName, this);
    _avgCEvens.Add(avgCEvenList);
    _avgCOdds.Add(avgCOddList);

    // loop over tagging categories for tag 1
    for (Int_t cat1It = 0; cat1It < numIters[1]; ++cat1It) {
      if ((numTagCats[0] > 1 || numTagCats[1] > 1)
          && !((_tagCat0Type > 2 || _tagCat1Type > 2)
          && cat0It == 0 && cat1It == 0)) {
        // get tagging category coefficient
        RooAbsArg* coef = (_tagCat0Type > 2 || _tagCat1Type > 2)
            && cat0It == 0 ? cat1Coefs->at(cat1It - 1) : cat1Coefs->at(cat1It);
        RooAbsReal* tagCatCoef = dynamic_cast<RooAbsReal*>(coef);

        if (tagCatCoef != 0) {
          catCoefList->add(*tagCatCoef);
        } else {
          coutF(InputArguments) << "RooBTagDecay::initTaggingCats("
              << GetName() << ") tagging category coefficient '"
              << coef->GetName()
              << "' is not a real-valued variable" << endl;
          assert(0);
        }
      }

      // get average even coefficient
      RooAbsReal* avgCEven = dynamic_cast<RooAbsReal*>(cat1CEvens->at(cat1It));
      if (avgCEven != 0) {
        avgCEvenList->add(*avgCEven);
      } else {
        coutF(InputArguments) << "RooBTagDecay::initTaggingCats("
            << GetName() << ") average even coefficient '"
            << cat1CEvens->at(cat1It)->GetName()
            << "' is not a real-valued variable" << endl;
        assert(0);
      }

      // get average odd coefficient
      RooAbsReal* avgCOdd  = dynamic_cast<RooAbsReal*>(cat1COdds->at(cat1It));
      if (avgCOdd != 0) {
        avgCOddList->add(*avgCOdd);
      } else {
        coutF(InputArguments) << "RooBTagDecay::initTaggingCats("
            << GetName() << ") average odd coefficient '"
            << cat1COdds->at(cat1It)->GetName()
            << "' is not a real-valued variable" << endl;
        assert(0);
      }

      if (cat0It == 0 && numTagCats[1] > 0) {
        // get dilution for tag 1
        RooAbsReal* dil1 = dynamic_cast<RooAbsReal*>(dilutions1.at(cat1It));
        if (dil1 != 0) _dilutions1.add(*dil1);
        else {
          coutF(InputArguments) << "RooBTagDecay::initTaggingCats("
              << GetName() << ") dilution '"
              << dilutions1.at(cat1It)->GetName()
              << "' is not a real-valued variable" << endl;
          assert(0);
        }

        // get dilution/wrong-tag asymmetry for tag 1
        RooAbsReal* ADil1 = dynamic_cast<RooAbsReal*>(ADilWTags1.at(cat1It));
        if (ADil1 != 0) _ADilWTags1.add(*ADil1);
        else {
          coutF(InputArguments) << "RooBTagDecay::initTaggingCats("
              << GetName() << ") dilution/wrong-tag asymmetry '"
              << ADilWTags1.at(cat1It)->GetName()
              << "' is not a real-valued variable" << endl;
          assert(0);
        }
      }
    }  // end loop over tag 1 categories
  }  // end loop over tag 0 categories

  // set average even and average odd coefficients
  if (_tagCat0Type < 2 && _tagCat1Type < 2) {
    // set the sums of the average even and average odd coefficients
    _avgCEvenSum.setArg(*((RooAbsReal*)((RooArgList*)_avgCEvens.At(0))->at(0)));
    _avgCOddSum.setArg(*((RooAbsReal*)((RooArgList*)_avgCOdds.At(0))->at(0)));

  } else {
    Bool_t comp = _tagCat0Type > 2 || _tagCat1Type > 2;

    Int_t coefIndex = 0;
    TString formString("");
    RooArgList catCoefList;
    RooArgList avgCEvenList;
    RooArgList avgCOddList;

    if (comp) {
      // create coefficient for complementary category
      formString += "1.";
      for (Int_t cat0It = 0; cat0It < _tagCatCoefs.GetEntries(); ++cat0It) {
        RooArgList* cat1Coefs = (RooArgList*)_tagCatCoefs.At(cat0It);
        catCoefList.add(*cat1Coefs);
        for (Int_t cat1It = 0; cat1It < cat1Coefs->getSize(); ++cat1It) {
          formString += "-@";
          formString += coefIndex;
          ++coefIndex;
        }
      }

      TString name = TString(GetName()) + "_tagCatCoef0";
      RooFormulaVar* tagCatCoef0 = new RooFormulaVar(name.Data(),
          "Tagging category coefficient category 0", formString, catCoefList);
      _createdVars.addOwned(*tagCatCoef0);

      // add complementary coefficient to category coefficients proxy
      Int_t coefListSize = ((RooArgList*)_tagCatCoefs.At(0))->getSize();
      ((RooArgList*)_tagCatCoefs.At(0))->removeAll();
      ((RooArgList*)_tagCatCoefs.At(0))->add(*tagCatCoef0);
      for (Int_t cat1It = 0; cat1It < coefListSize; ++cat1It)
        ((RooArgList*)_tagCatCoefs.At(0))->add(*catCoefList.at(cat1It));

      coefIndex = 0;
      formString = "(";
      catCoefList.removeAll();
    }

    // create lists of coefficients and formula strings for asymmetries
    for (Int_t cat0It = 0; cat0It < _tagCatCoefs.GetEntries(); ++cat0It) {
      RooArgList* cat1Coefs  = (RooArgList*)_tagCatCoefs.At(cat0It);
      RooArgList* cat1CEvens = (RooArgList*)_avgCEvens.At(cat0It);
      RooArgList* cat1COdds  = (RooArgList*)_avgCOdds.At(cat0It);
      catCoefList.add(*cat1Coefs);
      avgCEvenList.add(*cat1CEvens);
      avgCOddList.add(*cat1COdds);
      for (Int_t cat1It = 0; cat1It < cat1Coefs->getSize(); ++cat1It) {
        formString += cat0It == 0 && (cat1It == 0 || (cat1It == 1 && comp))
            ? "@" : "+@";
        formString += coefIndex;
        if (cat0It == 0 && cat1It == 0 && comp) {
          formString += "-(";
        } else {
          formString += "*@";
          formString += _tagCatCoefs.GetEntries() * cat1Coefs->getSize()
              + coefIndex;
        }
        ++coefIndex;
      }
    }

    avgCEvenList.add(catCoefList);
    avgCOddList.add(catCoefList);
    // set the average even and odd asymmetry coefficients
    if (comp) {
      // set asymmetry coefficient sums
      _avgCEvenSum.setArg(*((RooAbsReal*)avgCEvenList.at(0)));
      _avgCOddSum.setArg(*((RooAbsReal*)avgCOddList.at(0)));

      // create asymmetry coefficients for complementary category
      formString += "))/@";
      formString += _tagCatCoefs.GetEntries()
          * ((RooArgList*)_tagCatCoefs.At(0))->getSize();

      TString name = TString(GetName()) + "_avgCEven0";
      RooFormulaVar* avgCEven0 = new RooFormulaVar(name.Data(),
          "Average even coefficient category 0", formString, avgCEvenList);
      name = TString(GetName()) + "_avgCOdd0";
      RooFormulaVar* avgCOdd0 = new RooFormulaVar(name.Data(),
          "Average odd coefficient category 0", formString, avgCOddList);
      _createdVars.addOwned(*avgCEven0);
      _createdVars.addOwned(*avgCOdd0);

      // (re)set asymmetry coefficients for categories
      Int_t coefListSize = ((RooArgList*)_avgCEvens.At(0))->getSize();
      ((RooArgList*)_avgCEvens.At(0))->removeAll();
      ((RooArgList*)_avgCOdds.At(0))->removeAll();
      ((RooArgList*)_avgCEvens.At(0))->add(*avgCEven0);
      ((RooArgList*)_avgCOdds.At(0))->add(*avgCOdd0);
      for (Int_t cat1It = 1; cat1It < coefListSize; ++cat1It) {
        ((RooArgList*)_avgCEvens.At(0))->add(*avgCEvenList.at(cat1It));
        ((RooArgList*)_avgCOdds.At(0))->add(*avgCOddList.at(cat1It));
      }

    } else {
      // set asymmetry coefficient sums
      RooFormulaVar* avgCEvenSum = new RooFormulaVar("avgCEvenSum",
          "Sum of average even coefficients", formString, avgCEvenList);
      RooFormulaVar* avgCOddSum = new RooFormulaVar("avgCOddSum",
          "Sum of average odd coefficients", formString, avgCOddList);
      _createdVars.addOwned(*avgCEvenSum);
      _createdVars.addOwned(*avgCOddSum);
      _avgCEvenSum.setArg(*avgCEvenSum);
      _avgCOddSum.setArg(*avgCOddSum);
    }
  }
}

//_____________________________________________________________________________
void RooBTagDecay::initTag(Bool_t iTag, Bool_t tag1)
{
    // initialize flavour tag

    // get tag parameters
    const char* tagName;
    Int_t  tagVal = 2, numTypes = 0;
    Bool_t BIndex= kFALSE, BbarIndex = kFALSE, noTagIndex = kFALSE;
    if (iTag && !tag1) {
      tagName    = _iTag0.GetName();
      numTypes   = _iTag0.arg().numTypes();
      BIndex     = _iTag0.arg().isValidIndex(+1);
      BbarIndex  = _iTag0.arg().isValidIndex(-1);
      noTagIndex = _iTag0.arg().isValidIndex(0);
    } else if (iTag && tag1) {
      tagName    = _iTag1.GetName();
      numTypes   = _iTag1.arg().numTypes();
      BIndex     = _iTag1.arg().isValidIndex(+1);
      BbarIndex  = _iTag1.arg().isValidIndex(-1);
      noTagIndex = _iTag1.arg().isValidIndex(0);
    } else {
      tagName   = _fTag.GetName();
      numTypes  = _fTag.arg().numTypes();
      BIndex    = _fTag.arg().isValidIndex(+1);
      BbarIndex = _fTag.arg().isValidIndex(-1);
    }

    if (!_checkVars) {
      // print warning message if flavour tags are not to be checked
      coutW(InputArguments) << "RooBTagDecay::initTag(" << GetName()
          << ") unchecked flavour tag "
          << tagName
          << " (use of \"checkVars = false\" is not recommended): integrals will be calculated numerically and event generation will use accept/reject"
          << endl;
    } else if (numTypes == 2 && BIndex && BbarIndex) {
      // both B and Bbar
      tagVal = 2;
    } else if (numTypes == 1 && BIndex) {
      // only B
      tagVal = +1;
    } else if (numTypes == 1 && BbarIndex) {
      // only Bbar
      tagVal = -1;
    } else if (iTag && numTypes == 1 && noTagIndex) {
      // sum of B and Bbar
      tagVal = 0;
    } else if (iTag && numTypes == 3  && BIndex && BbarIndex && noTagIndex) {
      // B, Bbar and sum of B and Bbar
      tagVal = 3;
      coutW(InputArguments) << "RooBTagDecay::initTag(" << GetName()
          << ") initial state tag \"" << tagName
          << "\" can assume three values (-1, 0, +1): please make sure you know what you are doing:"
          << endl
          << "    * the value of the decay rate with tag = 0 is equal to the value with dilution = 0"
          << endl
          << "    * the integral of the decay rate is equal to the sum of B (+1) and Bbar (-1)"
          << endl
          << "    * events with tag = 0 will be generated in tagging category 0"
          << endl;
    } else {
      // not a valid configuration
      coutE(InputArguments) << "RooBTagDecay::initTag(" << GetName()
          << ") flavour tag \"" << tagName
          << "\" can only have values "
          << (iTag ? "+1, -1 and 0" : "+1 and -1")
          << ": use \"checkVars = false\" if you insist on using other/additional values"
          << endl;
      assert(0);
    }

    // set tag value
    if (iTag && !tag1) _iTag0Val = tagVal;
    else if (iTag && tag1) _iTag1Val = tagVal;
    else _fTagVal = tagVal;
}

//_____________________________________________________________________________
void RooBTagDecay::initTagCatMaps(Bool_t tag1) const
{
  // check number of categories
  if (!tag1 && _tagCat0Type < 2) return;
  if ( tag1 && _tagCat1Type < 2) return;

  // get tagging category type iterator
  TIterator* tagCatTypeIter = 0;
  if (!tag1) tagCatTypeIter = _tagCat0.arg().typeIterator();
  else tagCatTypeIter = _tagCat1.arg().typeIterator();

  // loop over tagging categories
  RooCatType* tagCatIndex = 0;
  Int_t       position    = 0;
  while ((tagCatIndex = (RooCatType*)tagCatTypeIter->Next()) != 0) {
    // set tagging category position and index
    Int_t index = tagCatIndex->getVal();
    if (!tag1) {
      _tagCat0Positions[index]  = position;
      _tagCat0Indices[position] = index;
    } else {
      _tagCat1Positions[index]  = position;
      _tagCat1Indices[position] = index;
    }

    // update position
    ++position;
  }

  delete tagCatTypeIter;
}

//_____________________________________________________________________________
void RooBTagDecay::declareBases()
{
  // create basis functions for time variable

  if (_decayType == SingleSided) {
    // create basis functions for positive time values
    _coshBasis = declareBasis("exp(-@0/@1)*cosh(@0*@2/2)",
        RooArgList(_tau.arg(), _dGamma.arg()));
    _cosBasis = declareBasis("exp(-@0/@1)*cos(@0*@2)",
        RooArgList(_tau.arg(), _dm.arg()));

    if (_tags < 2) {
      _sinhBasis = declareBasis("exp(-@0/@1)*sinh(@0*@2/2)",
          RooArgList(_tau.arg(), _dGamma.arg()));
      _sinBasis = declareBasis("exp(-@0/@1)*sin(@0*@2)",
          RooArgList(_tau.arg(), _dm.arg()));
    }

  } else if (_decayType == Flipped) {
    // create basis functions for negative time values
    _coshBasis = declareBasis("exp(@0/@1)*cosh(@0*@2/2)",
        RooArgList(_tau.arg(), _dGamma.arg()));
    _cosBasis = declareBasis("exp(@0/@1)*cos(@0*@2)",
        RooArgList(_tau.arg(), _dm.arg()));

    if (_tags < 2) {
      _sinhBasis = declareBasis("exp(@0/@1)*sinh(@0*@2/2)",
          RooArgList(_tau.arg(), _dGamma.arg()));
      _sinBasis = declareBasis("exp(@0/@1)*sin(@0*@2)",
          RooArgList(_tau.arg(), _dm.arg()));
    }

  } else if (_decayType == DoubleSided) {
    // create basis functions for both positive and negative time values
    _coshBasis = declareBasis("exp(-TMath::Abs(@0)/@1)*cosh(@0*@2/2)",
        RooArgList(_tau.arg(), _dGamma.arg()));
    _cosBasis = declareBasis("exp(-TMath::Abs(@0)/@1)*cos(@0*@2)",
        RooArgList(_tau.arg(), _dm.arg()));

    if (_tags < 2) {
      _sinhBasis = declareBasis("exp(-TMath::Abs(@0)/@1)*sinh(@0*@2/2)",
          RooArgList(_tau.arg(), _dGamma.arg()));
      _sinBasis = declareBasis("exp(-TMath::Abs(@0)/@1)*sin(@0*@2)",
          RooArgList(_tau.arg(), _dm.arg()));
    }
  }
}

//_____________________________________________________________________________
Bool_t RooBTagDecay::checkVarDep(const RooAbsArg& var, Bool_t warn,
    Bool_t onlyTagPars) const
{
  // check if parameters depend on specified variable

  if (_checkVars) {
    Bool_t checks = kTRUE;

    // check parameters in time-dependent functions and their coefficients
    if ((!onlyTagPars && (_tau.arg().dependsOn(var)
        || _dGamma.arg().dependsOn(var) || _dm.arg().dependsOn(var)
        || _coshCoef.arg().dependsOn(var) || _cosCoef.arg().dependsOn(var)
        || (_tags < 2 && (_sinhCoef.arg().dependsOn(var)
        || _sinCoef.arg().dependsOn(var)))))
        || (_tags > 1 && _ANorm.arg().dependsOn(var))
        || (_tags > 0 && (_avgCEvenSum.arg().dependsOn(var)
        || _avgCOddSum.arg().dependsOn(var))))
      checks = kFALSE;

    if (checks && _tags > 0) {
      // check tagging parameters
      Int_t numTagCats0 = _tagCat0Type > 1 ? _tagCat0.arg().numTypes() : 1;
      Int_t numTagCats1 = _tagCat1Type > 1 ? _tagCat1.arg().numTypes() : 1;
      for (Int_t cat0It = 0; cat0It < numTagCats0; ++cat0It) {
        // check dilutions for tag 0
        if (_tagCat0Type > 0 && (_dilutions0.at(cat0It)->dependsOn(var)
            || _ADilWTags0.at(cat0It)->dependsOn(var))) {
          checks = kFALSE;
          break;
        }

        for (Int_t cat1It = 0; cat1It < numTagCats1; ++cat1It) {
          // check tagging category coefficient
          if ((_tagCat0Type > 1 || _tagCat1Type > 1)
              && ((RooArgList*)_tagCatCoefs.At(cat0It))->at(cat1It)
              ->dependsOn(var)) {
            checks = kFALSE;
            break;
          }

          // check tagging category asymmetry coefficients
          if (((RooArgList*)_avgCOdds.At(cat0It))->at(cat1It)->dependsOn(var)
              || ((RooArgList*)_avgCOdds.At(cat0It))->at(cat1It)
              ->dependsOn(var)) {
            checks = kFALSE;
            break;
          }

          // check dilutions for tag 1
          if (cat0It == 0 &&_tagCat1Type > 0
              && (_dilutions1.at(cat1It)->dependsOn(var)
              || _ADilWTags1.at(cat1It)->dependsOn(var))) {
            checks = kFALSE;
            break;
          }
        }

        if (!checks) break;
      }
    }

    if (!checks && warn)
      coutW(InputArguments) << "RooBTagDecay::checkVarDep(" << GetName()
          << ") parameters depend on " << var.GetName()
          << ": use \"checkVars = false\" if you insist on using this dependence"
          << endl;

    return checks;

  } else if (warn) {
    coutW(InputArguments) << "RooBTagDecay::checkVarDep(" << GetName()
        << ") parameters dependence on "
        << var.GetName()
        << " is unchecked (use of \"checkVars = false\" is not recommended): integrals will be calculated numerically and event generation will use accept/reject"
        << endl;
  }

  return kTRUE;
}

//_____________________________________________________________________________
Bool_t RooBTagDecay::checkTag(Bool_t iTag, Bool_t tag1) const
{
  // check flavour tag state
  if (_checkVars) {
    // get tag parameters
    Int_t  tagVal = 2, nTypes = 0;
    Bool_t hasB = kFALSE, hasBbar = kFALSE, hasNoTag = kFALSE;
    if (iTag && !tag1) {
      tagVal   = _iTag0Val;
      nTypes   = _iTag0.arg().numTypes();
      hasB     = _iTag0.arg().isValidIndex(+1);
      hasBbar  = _iTag0.arg().isValidIndex(-1);
      hasNoTag = _iTag0.arg().isValidIndex(0);
    } else if (iTag && tag1) {
      tagVal   = _iTag1Val;
      nTypes   = _iTag1.arg().numTypes();
      hasB     = _iTag1.arg().isValidIndex(+1);
      hasBbar  = _iTag1.arg().isValidIndex(-1);
      hasNoTag = _iTag1.arg().isValidIndex(0);
    } else {
      tagVal  = _fTagVal;
      nTypes  = _fTag.arg().numTypes();
      hasB    = _fTag.arg().isValidIndex(+1);
      hasBbar = _fTag.arg().isValidIndex(-1);
    }

    // check tag
    if ((tagVal == 2 && (nTypes != 2 || !hasB || !hasBbar))
        || (tagVal == +1 && (nTypes != 1 || !hasB))
        || (tagVal == -1 && (nTypes != 1 || !hasBbar))
        || (tagVal ==  3 && (nTypes != 3 || !hasB || !hasBbar || !hasNoTag))
        || (tagVal ==  0 && (nTypes != 1 || !hasNoTag)))
      return kFALSE;
  }

  return kTRUE;
}

