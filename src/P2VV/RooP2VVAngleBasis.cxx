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

//////////////////////////////////////////////////////////////////////////////
//
// BEGIN_HTML
// END_HTML
//

#include "Riostream.h"
#include "TString.h"
#include "RooMsgService.h"
#include "RooAbsReal.h"
#include "RooRealVar.h"
#include "RooConstVar.h"
#include "RooLegendre.h"
#include "RooSpHarmonic.h"
#include "RooProduct.h"
#include "P2VV/RooP2VVAngleBasis.h"

//_____________________________________________________________________________
RooP2VVAngleBasis::RooP2VVAngleBasis(const char *name, const char *title,
    RooRealVar& cpsi, RooRealVar& ctheta, RooRealVar& phi,
    Int_t i, Int_t j, Int_t l, Int_t m, Double_t c) :
  RooAddition(name, title, RooArgList()),
  _cpsi("cpsi", "cos(psi)", this, cpsi),
  _ctheta("ctheta", "cos(theta)", this, ctheta),
  _phi("phi", "phi", this, phi),
  _coef("coef", "coef", this),
  _prodCoefs("prodCoefs", "prodCoefs", this),
  _c(c), _i(i), _j(j), _l(l), _m(m),
  _isProd(kFALSE)
{
  init();
}

//_____________________________________________________________________________
RooP2VVAngleBasis::RooP2VVAngleBasis(const char *name, const char *title,
    RooRealVar& cpsi, RooRealVar& ctheta, RooRealVar& phi, RooAbsReal& coef,
    Int_t i, Int_t j, Int_t l, Int_t m, Double_t c) :
  RooAddition(name, title, RooArgList()),
  _cpsi("cpsi", "cos(psi)", this, cpsi),
  _ctheta("ctheta", "cos(theta)", this, ctheta),
  _phi("phi", "phi", this, phi),
  _coef("coef", "coef", this, coef),
  _prodCoefs("prodCoefs", "prodCoefs", this),
  _c(c), _i(i), _j(j), _l(l), _m(m),
  _isProd(kFALSE)
{
  init();
}

//_____________________________________________________________________________
RooP2VVAngleBasis::RooP2VVAngleBasis(const RooP2VVAngleBasis& other,
    const char* name) :
  RooAddition(other, name),
  _cpsi("cpsi", this, other._cpsi),
  _ctheta("ctheta", this, other._ctheta),
  _phi("phi", this, other._phi),
  _coef("coef", this, other._coef),
  _prodCoefs("coef", this, other._prodCoefs),
  _cProd(other._cProd), _iProd(other._iProd), _jProd(other._jProd),
  _lProd(other._lProd), _mProd(other._mProd),
  _c(other._c), _i(other._i), _j(other._j), _l(other._l), _m(other._m),
  _isProd(other._isProd)
{}

//_____________________________________________________________________________
RooP2VVAngleBasis::~RooP2VVAngleBasis()
{
  _createdObjects.removeAll();
}

//_____________________________________________________________________________
Bool_t RooP2VVAngleBasis::createProduct(Int_t i, Int_t j, Int_t l, Int_t m,
    Double_t c, RooAbsReal& coef, Bool_t doReset)
{
  if (doReset || !_isProd) reset();
  _isProd = kTRUE;
  if (!createBasisFunc(i, j, l, m, c, &coef)) {
    reset();
    init();
    return kFALSE;
  }
  return kTRUE;
}

//_____________________________________________________________________________
Bool_t RooP2VVAngleBasis::createProdSum(std::vector<Int_t> i,
    std::vector<Int_t> j, std::vector<Int_t> l, std::vector<Int_t> m,
    std::vector<Double_t> c, const RooArgList& coefs, Bool_t doReset)
{
  // check number of functions
  Int_t numBases = coefs.getSize();
  if (i.size() != (UInt_t)numBases || j.size() != (UInt_t)numBases
      || l.size() != (UInt_t)numBases || m.size() != (UInt_t)numBases
      || c.size() != (UInt_t)numBases) {
    coutE(InputArguments) << "RooP2VVAngleBasis::createProdSum(" << GetName()
        << "): lengths of input vectors do not match" << std::endl;
    reset();
    init();
    return kFALSE;
  }

  // create products of functions
  if (doReset) reset();
  for (Int_t it = 0; it < numBases; ++it) {
    // check type of coefficient variable
    RooAbsReal* coef = dynamic_cast<RooAbsReal*>(coefs.at(it));
    if (coef == 0) {
      coutE(InputArguments) << "RooP2VVAngleBasis::createProdSum(" << GetName()
          << "): coefficient " << coefs.at(it)->GetName()
          << " is not of type RooAbsReal" << endl;
      reset();
      init();
      return kFALSE;
    }

    // create product
    if (!createProduct(i.at(it), j.at(it), l.at(it), m.at(it), c.at(it),
        *coef, kFALSE))
      return kFALSE;
  }

  return kTRUE;
}

//_____________________________________________________________________________
Bool_t RooP2VVAngleBasis::createProdSum(const RooArgList& funcs,
    const RooArgList& coefs, Bool_t doReset)
{
  // check number of functions
  Int_t numBases = funcs.getSize();
  if (coefs.getSize() != numBases) {
    coutE(InputArguments) << "RooP2VVAngleBasis::createProdSum(" << GetName()
        << "): lengths of input lists do not match" << std::endl;
    reset();
    init();
    return kFALSE;
  }

  // create products of functions
  if (doReset) reset();
  for (Int_t it = 0; it < numBases; ++it) {
    // check type of function
    RooP2VVAngleBasis* func = dynamic_cast<RooP2VVAngleBasis*>(funcs.at(it));
    if (func == 0) {
      coutE(InputArguments) << "RooP2VVAngleBasis::createProdSum(" << GetName()
          << "): function " << funcs.at(it)->GetName()
          << " is not of type RooP2VVAngleBasis" << endl;
      reset();
      init();
      return kFALSE;
    }

    // check type of coefficient variable
    RooAbsReal* coef = dynamic_cast<RooAbsReal*>(coefs.at(it));
    if (coef == 0) {
      coutE(InputArguments) << "RooP2VVAngleBasis::createProdSum(" << GetName()
          << "): coefficient " << coefs.at(it)->GetName()
          << " is not of type RooAbsReal" << endl;
      reset();
      init();
      return kFALSE;
    }

    // create product
    if (!createProduct(func->i(), func->j(), func->l(), func->m(), func->c(),
        *coef, kFALSE))
      return kFALSE;
  }

  return kTRUE;
}

//_____________________________________________________________________________
void RooP2VVAngleBasis::removeProduct()
{
  if (!_isProd) {
    coutW(ObjectHandling) << "RooP2VVAngleBasis::removeProduct(" << GetName()
        << "): function is not a product" << endl;
    return;
  }

  reset();
  init();
}

//_____________________________________________________________________________
void RooP2VVAngleBasis::printArgs(ostream& os) const
{
  // print function
  os << "[";
  if (!_isProd && _coef.absArg() != 0)
    os << " " << _coef.absArg()->GetName() << " *";
  if (_c != 1.)
    os << " " << setprecision(3) << _c;
  os << " P_" << _i;
  if (_j != 0)
    os << "^" << _j;
  os << " Y_" << _l << _m;

  if (_isProd) {
    // print product function
    os << " *";
    if (_set.getSize() > 1) os << " (";
    for (Int_t it = 0; it < _set.getSize(); ++it) {
      os << " " << _prodCoefs.at(it)->GetName() << " *";
      if (_cProd.at(it) != 1.)
        os << " " << setprecision(3) << _cProd.at(it);
      os << " P_" << _iProd.at(it);
      if (_jProd.at(it) != 0)
        os << "^" << _jProd.at(it);
      os << " Y_" << _lProd.at(it) << _mProd.at(it);
      if (it < _set.getSize() - 1)
        os << " +";
    }
    if (_set.getSize() > 1) os << " )";
  }

  os << " ]";
}
//_____________________________________________________________________________
void RooP2VVAngleBasis::reset()
{
  _set.removeAll();
  _prodCoefs.removeAll();
  _createdObjects.removeAll();
  _cacheMgr.reset();
  _cProd.clear();
  _iProd.clear();
  _jProd.clear();
  _lProd.clear();
  _mProd.clear();
  _isProd = kFALSE;
  setValueDirty();
}

//_____________________________________________________________________________
void RooP2VVAngleBasis::init()
{
  createBasisFunc(-1, 0, -1, 0, 1., 0);
}

//_____________________________________________________________________________
Bool_t RooP2VVAngleBasis::createBasisFunc(Int_t i, Int_t j, Int_t l, Int_t m,
    Double_t c, RooAbsReal* coef)
{
  // make sure value is recalculated
  setValueDirty();

  // create list for product
  RooArgList prodList("prodList");

  // create overall coefficient
  if (i >= 0) {
    prodList.add(*coef);
    _prodCoefs.add(*coef);
  } else if (_coef.absArg() != 0) {
    prodList.add(*_coef.absArg());
  }

  // create coefficient
  if (_c != 1. || c != 1.) {
    TString cName = TString(GetName()) + "_C";
    RooConstVar* cVar = new RooConstVar(cName, cName, _c * c);
    prodList.add(*cVar);
    _createdObjects.addOwned(*cVar);
  }
  if (i >= 0) _cProd.push_back(c);

  // get angles
  RooAbsReal* cpsi   = static_cast<RooAbsReal*>(_cpsi.absArg());
  RooAbsReal* ctheta = static_cast<RooAbsReal*>(_ctheta.absArg());
  RooAbsReal* phi    = static_cast<RooAbsReal*>(_phi.absArg());

  // create an associated Legendre
  TString PName = TString(GetName()) + "_P" + TString::Itoa(_i, 10)
      + (_j < 0 ? "m" : "") + TString::Itoa(_j < 0 ? -_j : _j, 10);
  RooLegendre* PVar(0);
  if (i < 0) {
    PVar = new RooLegendre(PName, PName, *cpsi, _i, _j);
  } else {
    PName += TString("x") + TString::Itoa(i, 10) + (j < 0 ? "m" : "")
        + TString::Itoa(j < 0 ? -j : j, 10);
    PVar = new RooLegendre(PName, PName, *cpsi, _i, _j, i, j);
    _iProd.push_back(i);
    _jProd.push_back(j);
  }
  prodList.add(*PVar);
  _createdObjects.addOwned(*PVar);

  // create a spherical harmonic
  TString YName = TString(GetName()) + "_Y" + TString::Itoa(_l, 10)
      + (_m < 0 ? "m" : "") + TString::Itoa(_m < 0 ? -_m : _m, 10);
  RooSpHarmonic* YVar(0);
  if (i < 0) {
    YVar = new RooSpHarmonic(YName, YName, *ctheta, *phi, _l, _m);
  } else {
    YName += TString("x") + TString::Itoa(l, 10) + (m < 0 ? "m" : "")
        + TString::Itoa(m < 0 ? -m : m, 10);
    YVar = new RooSpHarmonic(YName, YName, *ctheta, *phi, _l, _m, l, m);
    _lProd.push_back(l);
    _mProd.push_back(m);
  }
  prodList.add(*YVar);
  _createdObjects.addOwned(*YVar);

  // build product
  TString prodName = TString(GetName()) + "_"
      + TString::Itoa(_set.getSize(), 10);
  RooProduct* prod = new RooProduct(prodName, prodName, prodList);
  _createdObjects.addOwned(*prod);
  return _set.add(*prod);
}
