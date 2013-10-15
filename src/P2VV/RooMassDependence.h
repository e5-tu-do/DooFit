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

#ifndef ROO_MASS_DEPENDENCE
#define ROO_MASS_DEPENDENCE

#include <complex>

#include "RooRealProxy.h"
#include "RooListProxy.h"

class RooRealVar;
class RooArgList;

class RooMassDependence : public RooAbsReal
{

public:
  enum EvalType {Func1Sq, Func2Sq, Real, Imag};

  inline RooMassDependence() {}

  // constructors
  RooMassDependence(const char *name, const char *title, RooRealVar& mass,
      const RooArgList& parameters1, const RooArgList& parameters2,
      Int_t function1, Int_t function2, EvalType type);
  RooMassDependence(const char *name, const char *title, RooRealVar& mass,
      const RooArgList& parameters1, const RooArgList& parameters2,
      const RooArgList& phaseSpacePars, Int_t function1, Int_t function2,
      Int_t phaseSpaceFactor, EvalType type);

  // copy constructor
  RooMassDependence(const RooMassDependence& other, const char* name = 0);
  virtual TObject* clone(const char* name) const
  {
    return new RooMassDependence(*this, name);
  }

  inline ~RooMassDependence() {}

  Double_t mass() const {return (Double_t)_mass;}
  EvalType type() const {return _type;}
  Double_t phaseOffset() const {return std::arg(_phase);}
  Double_t normFactor(Int_t index) const {return index == 0 ? _norm1 : _norm2;}

  void setType(EvalType type);
  void setPhaseOffset(Double_t phase);
  void setNormFactors(Double_t norm1, Double_t norm2);
  void setNormFactor(Int_t index, Double_t norm);

  Int_t numFuncPars(Int_t index) const;
  Int_t numPSPars(Int_t index) const;
  std::complex<Double_t> funcVal(Int_t index) const;
  Double_t phaseSpaceVal(Int_t index) const;

protected:
  RooListProxy           _pars1;
  RooListProxy           _pars2;
  RooListProxy           _psPars;
  RooRealProxy           _mass;
  EvalType               _type;
  std::complex<Double_t> _phase;
  Double_t               _norm1;
  Double_t               _norm2;
  Int_t                  _func1;
  Int_t                  _func2;
  Int_t                  _psFac;

  const static Int_t _numImplFuncs = 3;
  const static Int_t _numImplPSFacs = 2;

  Double_t evaluate() const;

  void initialize(const RooArgList* pars1, const RooArgList* pars2,
      const RooArgList* psPars = 0);

private:
  ClassDef(RooMassDependence, 1)
};

#endif
