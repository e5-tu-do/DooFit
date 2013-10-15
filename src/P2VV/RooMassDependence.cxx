/*****************************************************************************
 * Project: RooFit                                                           *
 *                                                                           *
 * Authors:                                                                  *
 *   JvL, Jeroen van Leerdam,    Nikhef, j.van.leerdam@nikhef.nl             *
 *   DMS, Diego Martinez Santos, Nikhef                                      *
 *                                                                           *
 * Copyright (c) 2013, Nikhef. All rights reserved.                          *
 *                                                                           *
 * Redistribution and use in source and binary forms,                        *
 * with or without modification, are permitted according to the terms        *
 * listed in LICENSE (http://roofit.sourceforge.net/license.txt)             *
 *****************************************************************************/

#include <math.h>

#include "Riostream.h"
#include "RooMsgService.h"
#include "RooRealVar.h"
#include "RooArgList.h"
#include "P2VV/RooMassDependence.h"

//_____________________________________________________________________________
RooMassDependence::RooMassDependence(const char* name, const char* title,
    RooRealVar& mass, const RooArgList& parameters1,
    const RooArgList& parameters2, Int_t function1, Int_t function2,
    EvalType type) :
  RooAbsReal(name, title),
  _pars1("parameters1", "parameters function 1", this),
  _pars2("parameters2", "parameters function 2", this),
  _psPars("psPars", "phase space parameters", this),
  _mass("mass", "mass variable", this, mass),
  _type(type),
  _phase(1., 0.),
  _norm1(1.),
  _norm2(1.),
  _func1(function1),
  _func2(function2),
  _psFac(0)
{
  initialize(&parameters1, &parameters2, 0);
}

//_____________________________________________________________________________
RooMassDependence::RooMassDependence(const char* name, const char* title,
    RooRealVar& mass, const RooArgList& parameters1,
    const RooArgList& parameters2, const RooArgList& phaseSpacePars,
    Int_t function1, Int_t function2, Int_t phaseSpaceFactor, EvalType type) :
  RooAbsReal(name, title),
  _pars1("parameters1", "parameters function 1", this),
  _pars2("parameters2", "parameters function 2", this),
  _psPars("psPars", "phase space parameters", this),
  _mass("mass", "mass variable", this, mass),
  _type(type),
  _phase(1., 0.),
  _norm1(1.),
  _norm2(1.),
  _func1(function1),
  _func2(function2),
  _psFac(phaseSpaceFactor)
{
  initialize(&parameters1, &parameters2, &phaseSpacePars);
}

//_____________________________________________________________________________
RooMassDependence::RooMassDependence(const RooMassDependence& other,
    const char* name) :
  RooAbsReal(other, name),
  _pars1("parameters1", this, other._pars1),
  _pars2("parameters2", this, other._pars2),
  _psPars("psPars", this, other._psPars),
  _mass("mass", this, other._mass),
  _type(other._type),
  _phase(other._phase),
  _norm1(other._norm1),
  _norm2(other._norm2),
  _func1(other._func1),
  _func2(other._func2),
  _psFac(other._psFac)
{}

//_____________________________________________________________________________
void RooMassDependence::setType(EvalType type)
{
  // set evaluation type

  _type = type;
  setValueDirty();
  setShapeDirty();
}

//_____________________________________________________________________________
void RooMassDependence::setPhaseOffset(Double_t phase)
{
  // set offset of complex phase

  _phase = std::complex<Double_t>(std::cos(phase), std::sin(phase));
  setValueDirty();
  setShapeDirty();
}

//_____________________________________________________________________________
void RooMassDependence::setNormFactors(Double_t norm1, Double_t norm2)
{
  // set normalization factors

  setNormFactor(0, norm1);
  setNormFactor(1, norm2);
}

//_____________________________________________________________________________
void RooMassDependence::setNormFactor(Int_t index, Double_t norm)
{
  // set one of the two normalization factors

  if (index == 0) _norm1 = norm;
  else _norm2 = norm;
  setValueDirty();
  setShapeDirty();
}

//_____________________________________________________________________________
Int_t RooMassDependence::numFuncPars(Int_t index) const
{
  // return number of function parameters

  switch (index) {
    case 1:    // non-relativistic Breit-Wigner
      return 2;
      break;
    case 2:    // relativistic Breit-Wigner (EvtGen)
      return 4;
      break;
    default:   // constant function
      return 0;
  }
}

//_____________________________________________________________________________
Int_t RooMassDependence::numPSPars(Int_t index) const
{
  // return number of phase space parameters

  switch (index) {
    case 1:    // simple factor
      return 4;
      break;
    default:   // constant factor
      return 0;
  }
}

//_____________________________________________________________________________
std::complex<Double_t> RooMassDependence::funcVal(Int_t /*index*/) const
{
  // return (complex) function value

  return std::complex<Double_t> (1., 0.);
}

//_____________________________________________________________________________
Double_t RooMassDependence::phaseSpaceVal(Int_t /*index*/) const
{
  // return value of phase space factor

  return 1.;
}

//_____________________________________________________________________________
Double_t RooMassDependence::evaluate() const
{
  // return value of |func1|^2, |func2|^2, Re(func1* func2) or Im(func1* func2)

  Double_t funcs = 0., norm = 1.;
  if (_type == Func1Sq || _type == Func2Sq) {
    // |func1|^2 or |func2|^2
    norm = _type == Func1Sq ? _norm1 * _norm1 : _norm2 * _norm2;
    funcs = std::norm(funcVal(_type == Func1Sq ? _func1 : _func2));
  } else if (_type == Real || _type == Imag) {
    // Re(func1* func2) or Im(func1* func2)
    norm = _norm1 * _norm2;
    std::complex<Double_t> prod = _phase * std::conj(funcVal(_func1))
        * funcVal(_func2);
    funcs = _type == Real ? std::real(prod) : std::imag(prod);
  }

  return phaseSpaceVal(_psFac) * funcs / norm;
}

//_____________________________________________________________________________
void RooMassDependence::initialize(const RooArgList* pars1,
    const RooArgList* pars2, const RooArgList* psPars)
{
  // check function indices
  if (_func1 < 0 || _func2 < 0 || _func1 >= _numImplFuncs
      || _func2 > _numImplFuncs) {
    coutF(InputArguments) << "RooMassDependence::initialize("
        << GetName() << "): not a valid combination of function indices: ("
        << _func1 << ", " << _func2 << ")" << std::endl;
    assert(0);
  }

  // check phase space factor index
  if (_psFac < 0 || _psFac >= _numImplPSFacs) {
    coutF(InputArguments) << "RooMassDependence::initialize("
        << GetName() << "): not a valid phase space factor index: " << _psFac
        << std::endl;
    assert(0);
  }

  // check number of function parameters
  if (pars1->getSize() != numFuncPars(_func1)
      || pars2->getSize() != numFuncPars(_func2)) {
    coutF(InputArguments) << "RooMassDependence::initialize("
        << GetName() << "): wrong number of function parameters: "
        << pars1->getSize() << " (" << numFuncPars(_func1)
        << " needed for function " << _func1 << "), " << pars2->getSize()
        << " (" << numFuncPars(_func2) << " needed for function " << _func2
        << ")" << std::endl;
    assert(0);
  }

  // check number of phase space parameters
  if (psPars != 0 && psPars->getSize() != numPSPars(_psFac)) {
    coutF(InputArguments) << "RooMassDependence::initialize("
        << GetName() << "): wrong number of phase space parameters: "
        << psPars->getSize() << " (" << numPSPars(_psFac) << " needed)"
        << std::endl;
    assert(0);
  }

  // add parameters to proxies
  _pars1.add(*pars1);
  _pars2.add(*pars2);
  if (psPars != 0) _psPars.add(*psPars);
}
