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
#include "RooArgSet.h"

class RooAbsCategory;
class RooAbsRealLValue;
class RooArgSet;
class TObjArray;
class RooAbsReal;

namespace doofit {
namespace extraroopdfs {

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

  Bool_t continuousBase() const {return _continuousBase;}

  Bool_t forceUnitIntegral() {return _forceUnitIntegral;}
  void   setForceUnitIntegral(Bool_t force = kTRUE)
  { _forceUnitIntegral = force; }

  Bool_t binIntegralCoefs() const {return _binIntegralCoefs;}
  void   setBinIntegralCoefs(Bool_t integralCoefs = kTRUE)
  { _binIntegralCoefs = integralCoefs; }

  Bool_t ignoreFirstBin() const {return _ignoreFirstBin;}
  void   setIgnoreFirstBin(Bool_t ignFirstBin = kTRUE)
  { _ignoreFirstBin = ignFirstBin; }

  virtual Int_t getAnalyticalIntegral(RooArgSet& allVars, RooArgSet& analVars,
      const char* rangeName = 0) const;
  virtual Double_t analyticalIntegral(Int_t code,
      const char* rangeName = 0) const;

  virtual Int_t getMaxVal(const RooArgSet& vars) const;
  virtual Double_t maxVal(Int_t code) const;

  Double_t getValV(const RooArgSet* nset) const;

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

  class CacheElem;
  CacheElem& cacheElem(RooArgSet* nset) const;

  Double_t evaluateCoef() const;
  Double_t evaluateMultipleCoefs() const;
  Double_t evaluateFunction() const;
  UInt_t hash(const RooArgSet& s) const;

  RooListProxy _baseCatsList;
  RooListProxy _baseVarsList;
  TObjArray    _coefLists;
  RooRealProxy _function;

  std::vector< std::map<Int_t, Int_t> > _indexPositions;
  std::vector<TString>                  _binningNames;
  std::vector<Bool_t>                   _calcCoefZeros;

  Int_t _numCats;

  Bool_t _continuousBase;
  Bool_t _forceUnitIntegral;
  Bool_t _binIntegralCoefs;
  Bool_t _ignoreFirstBin; 

  /// integral cache
  mutable RooObjCacheManager m_cacheMgr; //! transient member
  mutable RooArgSet* m_nset; //! transient member
  mutable UInt_t m_nsethash; //! transient member
  mutable std::map<UInt_t, RooArgSet> m_nsets; //! transient member

  class CacheElem;
  friend class CacheElem;
  class CacheElem : public RooAbsCacheElement
  {
      public:
	  /// constructor
	  CacheElem(const RooBinnedPdf& parent);
	  /// destructor
	  virtual ~CacheElem();
	  /// return list of contained RooFit variables
	  virtual RooArgList containedArgs(Action);
	  /// return function at bin centres
	  RooAbsReal& function() const;
	  /// return mapping from parent's base var to our copy
	  RooAbsRealLValue* operator[](RooAbsRealLValue* var) const;
	  /// return maximum value of function over all bins
	  Double_t max() const;

      private:
	  const RooBinnedPdf& _parent;
	  RooAbsReal* _function;
	  std::vector<RooAbsRealLValue*> _baseVarsList;
	  std::vector<const RooAbsBinning*> _binningList;
	  std::map<RooAbsRealLValue*, RooAbsRealLValue*> _baseVarsMapping;
  };

  ClassDef(RooBinnedPdf, 2) // binned PDF
};

}
}

#endif

