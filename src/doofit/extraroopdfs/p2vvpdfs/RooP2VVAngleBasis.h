/*****************************************************************************
 * Project: RooFit                                                           *
 * Package: RooFitModels                                                     *
 *    File: $Id$
 * Authors:                                                                  *
 *   GR,  Gerhard Raven,      Nikhef & VU, Gerhard.Raven@nikhef.nl           *
 *   JvL, Jeroen van Leerdam, Nikhef,      j.van.leerdam@nikhef.nl           *
 *                                                                           *
 * Copyright (c) 2013, Nikhef. All rights reserved.                          *
 *                                                                           *
 * Redistribution and use in source and binary forms,                        *
 * with or without modification, are permitted according to the terms        *
 * listed in LICENSE (http://roofit.sourceforge.net/license.txt)             *
 *****************************************************************************/

#ifndef ROO_P2VV_ANGLE_BASIS
#define ROO_P2VV_ANGLE_BASIS

#include <vector>
#include "Riosfwd.h"
#include "RooAddition.h"
#include "RooArgList.h"
#include "RooRealProxy.h"
#include "RooListProxy.h"

class RooAbsReal;
class RooRealVar;

class RooP2VVAngleBasis : public RooAddition
{
public:
  RooP2VVAngleBasis() {};
  RooP2VVAngleBasis(const char *name, const char *title,
      RooRealVar& cpsi, RooRealVar& ctheta, RooRealVar& phi,
      Int_t i, Int_t j, Int_t l, Int_t m, Double_t c = 1.);
  RooP2VVAngleBasis(const char *name, const char *title,
      RooRealVar& cpsi, RooRealVar& ctheta, RooRealVar& phi, RooAbsReal& coef,
      Int_t i, Int_t j, Int_t l, Int_t m, Double_t c = 1.);
  RooP2VVAngleBasis(const RooP2VVAngleBasis& other, const char* name = 0);

  virtual TObject* clone(const char* newname) const
  {
    return new RooP2VVAngleBasis(*this, newname);
  }

  virtual ~RooP2VVAngleBasis();

  Int_t i() const {return _i;}
  Int_t j() const {return _j;}
  Int_t l() const {return _l;}
  Int_t m() const {return _m;}
  Double_t c() const {return _c;}
  Bool_t isProduct() const {return _isProd;}

  const RooAbsReal* coef() const
  {
    return dynamic_cast<RooAbsReal*>(_coef.absArg());
  }

  Bool_t createProduct(Int_t i, Int_t j, Int_t l, Int_t m, Double_t c,
      RooAbsReal& coef, Bool_t doReset = kTRUE);
  Bool_t createProduct(const RooP2VVAngleBasis& func, RooAbsReal& coef,
      Bool_t doReset = kTRUE)
  { 
    return createProduct(func.i(), func.j(), func.l(), func.m(), func.c(),
        coef, doReset);
  }

  Bool_t createProdSum(std::vector<Int_t> i, std::vector<Int_t> j,
      std::vector<Int_t> l, std::vector<Int_t> m, std::vector<Double_t> c,
      const RooArgList& coefs, Bool_t doReset = kTRUE);
  Bool_t createProdSum(const RooArgList& funcs, const RooArgList& coefs,
      Bool_t doReset = kTRUE);

  void removeProduct();

  void printArgs(std::ostream& os) const;

protected:
  RooRealProxy _cpsi;
  RooRealProxy _ctheta;
  RooRealProxy _phi;
  RooRealProxy _coef;
  RooListProxy _prodCoefs;
  RooArgList _createdObjects;

  std::vector<Double_t> _cProd;
  std::vector<Int_t> _iProd, _jProd, _lProd, _mProd;
  Double_t _c;
  Int_t _i, _j, _l, _m;
  Bool_t _isProd;

  void reset();
  void init();
  Bool_t createBasisFunc(Int_t i, Int_t j, Int_t l, Int_t m, Double_t c,
      RooAbsReal* coef);

private:
  ClassDef(RooP2VVAngleBasis, 1)  // associated Legendre x spherical harmonic
};

#endif
