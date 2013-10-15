/*****************************************************************************
 * Project: RooFit                                                           *
 *                                                                           *
 * Authors:                                                                  *
 *   GR,  Gerhard Raven,      Nikhef & VU, Gerhard.Raven@nikhef.nl           *
 *   WH,  Wouter Hulsbergen,  Nikhef                                         *
 *   JvL, Jeroen van Leerdam, Nikhef,      j.van.leerdam@nikhef.nl           *
 *                                                                           *
 * Copyright (c) 2011, Nikhef. All rights reserved.                          *
 *                                                                           *
 * Redistribution and use in source and binary forms,                        *
 * with or without modification, are permitted according to the terms        *
 * listed in LICENSE (http://roofit.sourceforge.net/license.txt)             *
 *****************************************************************************/

#ifndef ROO_REAL_MOMENTS_H
#define ROO_REAL_MOMENTS_H

#include "RooAbsPdf.h"
#include "RooAbsData.h"
#include "TString.h"
#include <iosfwd>
#include <vector>

//_____________________________________________________________________________
class RooRealMoment;
typedef std::vector<RooRealMoment*> RooRealMomentsVector;

//_____________________________________________________________________________
class RooRealMoment
{
public:
  RooRealMoment(const RooAbsReal& basisFunc, Double_t norm = 1.,
      const char* name = "");
  RooRealMoment(const RooAbsReal& basisFunc,
      const RooRealMomentsVector& moments, Double_t norm = 1.,
      const char* name = "");
  virtual ~RooRealMoment() {};

  const char* name() const {return _name;}
  const char* GetName() const {return name();}
  const RooAbsReal& basisFunc() const {return _basisFunc;}
  RooRealMomentsVector moments() {return RooRealMomentsVector(_moments);}
  Double_t norm() const {return _norm;}

  virtual RooArgSet* getObservables(const RooArgSet* set) const
  {
    return basisFunc().getObservables(set);
  }
  RooArgSet* getObservables(const RooAbsData& data) const
  {
    return getObservables(data.get());
  }

  Double_t coefficient(Bool_t normalize = kTRUE) const;
  Double_t variance(Bool_t normalize = kTRUE) const;
  Double_t covariance(Int_t index, Bool_t normalize = kTRUE) const;
  Double_t correlation(Int_t index) const;
  Double_t stdDev(Bool_t normalize = kTRUE) const;
  Double_t significance() const;

  Double_t m0() const {return _m0;}
  Double_t m1() const {return _m1;}
  Double_t n0() const {return _n0;}
  Double_t n1() const {return _n1;}
  Double_t n2(Int_t index = 0) const;

  virtual Double_t evaluate() const {return _basisFunc.getVal();}
  void inc(Double_t weight = 1.);
  void reset();
  void appendMoments(const RooRealMomentsVector& moments);
  void clearMoments();

  virtual std::ostream& print(std::ostream& os, Bool_t normalize = kTRUE) const;
  void Print(Bool_t normalize = kTRUE) const {print(std::cout, normalize);}

protected:
  const RooAbsReal& _basisFunc; //!
  RooRealMomentsVector _moments;
  TString _name;
  Double_t _norm;
  Double_t _m0, _m1;
  Double_t _n0, _n1;
  std::vector<Double_t> _n2;
  Bool_t _reset;
};

//_____________________________________________________________________________
class RooRealEffMoment : public RooRealMoment
{
public:
  RooRealEffMoment(RooAbsReal& basisFunc, const RooAbsPdf& pdf,
      const RooArgSet& intSet, const RooArgSet& normSet, Double_t norm = 1.,
      const char* name = "");
  RooRealEffMoment(RooAbsReal& basisFunc, const RooRealMomentsVector& moments,
      const RooAbsPdf& pdf, const RooArgSet& intSet, const RooArgSet& normSet,
      Double_t norm = 1., const char* name = "");
  virtual ~RooRealEffMoment();

  const RooAbsPdf& pdf() const {return _pdf;}
  RooAbsReal* pdfInt() const {return _pdfInt;}
  RooArgSet* intSet() const {return new RooArgSet(_intSet);}
  RooArgSet* normSet() const {return new RooArgSet(_normSet);}

  Double_t evaluate() const {return _basisFunc.getVal() / _pdfInt->getVal();}
  virtual RooArgSet* getObservables(const RooArgSet* set) const
  {
    return _pdf.getObservables(set);
  }

private:
  const RooAbsPdf&  _pdf;     //!
  RooAbsReal*       _pdfInt;
  const RooArgSet&  _intSet;  //!
  const RooArgSet&  _normSet; //!
};

//_____________________________________________________________________________
Int_t computeRooRealMoments(RooAbsData& data, RooRealMomentsVector& moments,
    Bool_t resetFirst = kFALSE, Bool_t verbose = kTRUE);

#endif
