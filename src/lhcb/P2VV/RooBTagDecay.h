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

#ifndef ROO_B_TAG_DECAY
#define ROO_B_TAG_DECAY

#include "RooAbsAnaConvPdf.h"
#include "RooArgList.h"
#include "RooRealProxy.h"
#include "RooCategoryProxy.h"
#include "RooListProxy.h"
#include "RooRealVar.h"

class RooCategory;
class RooRealVar;
class TObjArray;

class RooBTagDecay : public RooAbsAnaConvPdf
{

public:
  enum DecayType {SingleSided, DoubleSided, Flipped};

  inline RooBTagDecay() {}

  // constructor without flavour tags
  RooBTagDecay(const char *name, const char *title,
    RooRealVar& time, RooAbsReal& tau, RooAbsReal& dGamma, RooAbsReal& dm,
    RooAbsReal& coshCoef, RooAbsReal& sinhCoef, RooAbsReal& cosCoef,
    RooAbsReal& sinCoef, const RooResolutionModel& model, DecayType type,
    Bool_t checkTags = kTRUE);

  // constructor with both initial and final state flavour tags
  RooBTagDecay(const char *name, const char* title,
    RooRealVar& time, RooCategory& iTag, RooCategory& fTag,
    RooAbsReal& tau, RooAbsReal& dGamma, RooAbsReal& dm, RooAbsReal& dilution,
    RooAbsReal& ADilWTag, RooAbsReal& ANorm, RooAbsReal& avgCEven,
    RooAbsReal& avgCOdd, RooAbsReal& cosCoef, const RooResolutionModel& model,
    DecayType type, Bool_t checkTags = kTRUE);

  // constructor with only an initial state flavour tag
  RooBTagDecay(const char *name, const char* title,
    RooRealVar& time, RooCategory& iTag, RooAbsReal& tau,
    RooAbsReal& dGamma, RooAbsReal& dm, RooAbsReal& dilution,
    RooAbsReal& ADilWTag, RooAbsReal& avgCEven, RooAbsReal& avgCOdd,
    RooAbsReal& coshCoef, RooAbsReal& sinhCoef, RooAbsReal& cosCoef,
    RooAbsReal& sinCoef, const RooResolutionModel& model, DecayType type,
    Bool_t checkTags = kTRUE);

  // constructor with initial and final state tags and tagging categories
  RooBTagDecay(const char *name, const char* title,
    RooRealVar& time, RooCategory& iTag, RooCategory& fTag,
    RooCategory& tagCat, RooAbsReal& tau, RooAbsReal& dGamma,
    RooAbsReal& dm, const RooArgList& dilutions, const RooArgList& ADilWTags,
    RooAbsReal& ANorm, const RooArgList& avgCEvens, const RooArgList& avgCOdds,
    const RooArgList& tagCatCoefs, RooAbsReal& cosCoef,
    const RooResolutionModel& model, DecayType type, Bool_t checkTags = kTRUE);

  // constructor with an initial state tag and tagging categories
  RooBTagDecay(const char *name, const char* title,
    RooRealVar& time, RooCategory& iTag, RooCategory& tagCat, RooAbsReal& tau,
    RooAbsReal& dGamma, RooAbsReal& dm, const RooArgList& dilutions,
    const RooArgList& ADilWTags, const RooArgList& avgCEvens,
    const RooArgList& avgCOdds, const RooArgList& tagCatCoefs,
    RooAbsReal& coshCoef, RooAbsReal& sinhCoef, RooAbsReal& cosCoef,
    RooAbsReal& sinCoef, const RooResolutionModel& model, DecayType type,
    Bool_t checkTags = kTRUE);

  // constructor with two initial state tags and a final state tag
  RooBTagDecay(const char *name, const char* title,
    RooRealVar& time, RooCategory& iTag0, RooCategory& iTag1,
    RooCategory& fTag, RooCategory& tagCat0, RooCategory& tagCat1,
    RooAbsReal& tau, RooAbsReal& dGamma, RooAbsReal& dm,
    const RooArgList& dilutions0, const RooArgList& dilutions1,
    const RooArgList& ADilWTags0, const RooArgList& ADilWTags1,
    RooAbsReal& ANorm, const TObjArray& avgCEvens, const TObjArray& avgCOdds,
    const TObjArray& tagCatCoefs, RooAbsReal& cosCoef,
    const RooResolutionModel& model, DecayType type, Bool_t checkTags = kTRUE);

  // constructor with two initial state tags
  RooBTagDecay(const char *name, const char* title,
    RooRealVar& time, RooCategory& iTag0, RooCategory& iTag1,
    RooCategory& tagCat0, RooCategory& tagCat1, RooAbsReal& tau,
    RooAbsReal& dGamma, RooAbsReal& dm, const RooArgList& dilutions0,
    const RooArgList& dilutions1, const RooArgList& ADilWTags0,
    const RooArgList& ADilWTags1, const TObjArray& avgCEvens,
    const TObjArray& avgCOdds, const TObjArray& tagCatCoefs,
    RooAbsReal& coshCoef, RooAbsReal& sinhCoef, RooAbsReal& cosCoef,
    RooAbsReal& sinCoef, const RooResolutionModel& model, DecayType type,
    Bool_t checkTags = kTRUE);

  RooBTagDecay(const RooBTagDecay& other, const char* name = 0);

  virtual TObject* clone(const char* newname) const 
  { 
    return new RooBTagDecay(*this, newname);
  }

  virtual ~RooBTagDecay();

  virtual Double_t coefficient(Int_t basisIndex) const;
  Double_t tagCatCoef() const;
  Double_t tagCatCoef(Int_t& cat0, Int_t&cat1) const;
  RooArgSet* coefVars(Int_t coefIdx) const;
  RooResolutionModel& resolutionModel() const {return (RooResolutionModel&)_model.arg();}  // TODO: move this method to RooAbsAnaConvPdf

  const char *pruneRangeName(const char* rangeName, const RooArgSet& vars) const;
  Int_t getCoefAnalyticalIntegral(Int_t coef, RooArgSet& allVars,
      RooArgSet& analVars, const char* rangeName = 0) const;
  Double_t coefAnalyticalIntegral(Int_t coef, Int_t code,
      const char* rangeName = 0) const;

  void setMaxVal(const Double_t val);
  Int_t getMaxVal(const RooArgSet& vars) const;
  Double_t maxVal(Int_t code) const;
  Int_t getGenerator(const RooArgSet& directVars, RooArgSet &generateVars,
      Bool_t staticInitOK = kTRUE) const;
  void generateEvent(Int_t code);

  Int_t getTagCatPosition(Int_t tagCatIndex, Bool_t tag1 = kFALSE) const;
  Int_t getTagCatIndex(Int_t tagCatPosition, Bool_t tag1 = kFALSE) const;

  RooArgList* dilutions(Bool_t tag1 = kFALSE) const;
  RooArgList* ADilWTags(Bool_t tag1 = kFALSE) const;
  RooArgList* avgCEvens(Int_t tagCat0 = 0) const;
  RooArgList* avgCOdds(Int_t tagCat0 = 0) const;
  RooArgList* tagCatCoefs(Int_t tagCat0 = 0) const;

  Bool_t changeModel(const RooResolutionModel& newModel)
  {
    // TODO: remove this function and make RooAbsAnaConvPdf::changeModel(const RooResolutionModel& newModel) public
    return RooAbsAnaConvPdf::changeModel(newModel);
  }

protected:
  void initTaggingCats(const TObjArray& tagCatCoefs,
      const RooArgList& dilutions0, const RooArgList& dilutions1,
      const RooArgList& ADilWTags0, const RooArgList& ADilWTags1,
      const TObjArray& avgCEvens, const TObjArray& avgCOdds);
  void initTag(Bool_t iTag, Bool_t tag1);
  void initTagCatMaps(Bool_t tag1) const;
  void declareBases();

  Double_t tagCatCoefUnsafe(Int_t cat0, Int_t cat1) const;

  Bool_t checkVarDep(const RooAbsArg& var, Bool_t warn = kFALSE,
      Bool_t onlyTagPars = kFALSE) const;
  Bool_t checkTag(Bool_t iTag, Bool_t tag1) const;

  RooRealProxy     _time;
  RooCategoryProxy _iTag0;
  RooCategoryProxy _iTag1;
  RooCategoryProxy _fTag;
  RooCategoryProxy _tagCat0;
  RooCategoryProxy _tagCat1;

  RooRealProxy _tau;
  RooRealProxy _dGamma;
  RooRealProxy _dm;

  RooListProxy _dilutions0;
  RooListProxy _dilutions1;
  RooListProxy _ADilWTags0;
  RooListProxy _ADilWTags1;
  RooRealProxy _ANorm;
  RooRealProxy _avgCEvenSum;
  RooRealProxy _avgCOddSum;
  TObjArray    _avgCEvens;
  TObjArray    _avgCOdds;
  TObjArray    _tagCatCoefs;

  RooRealProxy _coshCoef;
  RooRealProxy _sinhCoef;
  RooRealProxy _cosCoef;
  RooRealProxy _sinCoef;


  RooListProxy _createdVars;

  mutable std::map<Int_t, Int_t> _tagCat0Positions; //!
  mutable std::map<Int_t, Int_t> _tagCat1Positions; //!
  mutable std::map<Int_t, Int_t> _tagCat0Indices;   //!
  mutable std::map<Int_t, Int_t> _tagCat1Indices;   //!

  Int_t _coshBasis;
  Int_t _sinhBasis;
  Int_t _cosBasis;
  Int_t _sinBasis;

  const RooAbsReal& coefArg(Int_t coef) const {
    if (coef == _coshBasis)  return _coshCoef.arg();
    if (coef == _sinhBasis)  return _sinhCoef.arg();
    if (coef == _cosBasis )  return  _cosCoef.arg();
    if (coef == _sinBasis )  return  _sinCoef.arg();
    throw std::string("Unknown coefficient");
    static RooRealVar dummy; // never reached...
    return dummy;
  }

  DecayType    _decayType;
  Int_t        _tagCat0Type;
  Int_t        _tagCat1Type;
  Int_t        _tags;
  Int_t        _iTag0Val;
  Int_t        _iTag1Val;
  Int_t        _fTagVal;
  Bool_t       _checkVars; 
  Double_t     _maxVal;

private:
  ClassDef(RooBTagDecay, 1) // PDF of B decay time distribution with flavour tagging
};

#endif

