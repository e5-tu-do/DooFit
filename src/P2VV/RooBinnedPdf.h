/*****************************************************************************
 * Project: RooFit                                                           *
 * Package: RooFitModels                                                     *
 *    File: $Id$
 * Authors:                                                                  *
 *   JvL, Jeroen van Leerdam, Nikhef, j.van.leerdam@nikhef.nl                *
 *   RA,  Roel Aaij,          Nikhef                                         *
 *                                                                           *
 * Copyright (c) 2011, Nikhef. All rights reserved.                          *
 *                                                                           *
 * Redistribution and use in source and binary forms,                        *
 * with or without modification, are permitted according to the terms        *
 * listed in LICENSE (http://roofit.sourceforge.net/license.txt)             *
 *****************************************************************************/

#ifndef ROO_BINNED_PDF
#define ROO_BINNED_PDF

#include <map>
#include <vector>
#include <list>

#include "RooAbsPdf.h"
#include "RooRealProxy.h"
#include "RooListProxy.h"

class RooAbsCategory;
class RooAbsRealLValue;
class RooArgSet;
class TObjArray;

class RooBinnedPdf : public RooAbsPdf
{

public:
  inline RooBinnedPdf() {}

  RooBinnedPdf(const char *name, const char *title,
      RooAbsCategory& baseCat, const RooArgList& coefList);

  RooBinnedPdf(const char *name, const char *title,
      const RooArgList& baseCats, const RooArgList& coefList);

  RooBinnedPdf(const char *name, const char *title,
      const RooArgList& baseCats, const TObjArray& coefLists,
      Bool_t ignoreFirstBin = kFALSE);

  RooBinnedPdf(const char *name, const char *title,
      RooAbsRealLValue& baseVar, const char* binning,
      const RooArgList& coefList, Bool_t binIntegralCoefs = kFALSE);

  RooBinnedPdf(const char *name, const char *title,
      const RooArgList& baseVars, const TObjArray& binningNames,
      const RooArgList& coefList, Bool_t binIntegralCoefs = kFALSE);

  RooBinnedPdf(const char *name, const char *title,
      const RooArgList& baseVars, const TObjArray& binningNames,
      const TObjArray& coefLists, Bool_t binIntegralCoefs = kFALSE,
      Bool_t ignoreFirstBin = kFALSE);

  RooBinnedPdf(const char* name, const char* title,
      RooAbsArg& baseVar, const char* binning, RooAbsReal& function);

  RooBinnedPdf(const char* name, const char* title,
      const RooArgList& baseVars, const TObjArray& binningNames,
      RooAbsReal& function);

  RooBinnedPdf(const RooBinnedPdf& other, const char* name = 0);

  virtual TObject* clone(const char* newname) const 
  { 
    return new RooBinnedPdf(*this, newname);
  }

  virtual ~RooBinnedPdf();

  virtual std::list<Double_t>* binBoundaries
  (RooAbsRealLValue& obs, Double_t xlo, Double_t xhi) const;
  std::list<Double_t>* plotSamplingHint
  (RooAbsRealLValue& obs, Double_t xlo, Double_t xhi) const;

  RooArgList* baseVariables();

  Bool_t continuousBase() {return _continuousBase;}

  Bool_t forceUnitIntegral() {return _forceUnitIntegral;}
  void   setForceUnitIntegral(Bool_t force = kTRUE)
  {
    _forceUnitIntegral = force;
  }

  Bool_t binIntegralCoefs() {return _binIntegralCoefs;}
  void   setBinIntegralCoefs(Bool_t integralCoefs = kTRUE)
  {
    _binIntegralCoefs = integralCoefs;
  }

  Bool_t ignoreFirstBin() {return _ignoreFirstBin;}
  void   setIgnoreFirstBin(Bool_t ignoreFirstBin = kTRUE)
  {
    _ignoreFirstBin = ignoreFirstBin;
  }

  virtual Int_t getAnalyticalIntegral(RooArgSet& allVars, RooArgSet& analVars,
      const char* rangeName = 0) const;
  virtual Double_t analyticalIntegral(Int_t code,
      const char* rangeName = 0) const;

  virtual Int_t getMaxVal(const RooArgSet& vars) const;
  virtual Double_t maxVal(Int_t code) const;

protected:
  virtual Double_t evaluate() const;

private:
  Int_t createBaseCats(const RooArgList& baseVars,
      const TObjArray& binningNames);
  Int_t initCoefs(const TObjArray& coefLists, Bool_t factorize = kTRUE);

  void reset();
   
  const RooAbsRealLValue& function() {
    return dynamic_cast<const RooAbsRealLValue&>(_function.arg());
  }

  Double_t evaluateCoef() const;
  Double_t evaluateMultipleCoefs() const;
  Double_t evaluateFunction() const;

  Int_t _numCats;

  RooListProxy _baseCatsList;
  RooListProxy _baseVarsList;
  TObjArray    _coefLists;
  RooRealProxy _function;

  std::vector< std::map<Int_t, Int_t> > _indexPositions;
  std::vector<TString>                  _binningNames;
  std::vector<Bool_t>                   _calcCoefZeros;

  Bool_t _continuousBase;
  Bool_t _forceUnitIntegral;
  Bool_t _binIntegralCoefs;
  Bool_t _ignoreFirstBin;

  ClassDef(RooBinnedPdf, 1) // binned PDF
};

#endif

