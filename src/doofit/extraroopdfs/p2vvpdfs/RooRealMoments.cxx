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

#include <cmath>
#include <iostream>
#include "P2VV/ProgressDisplay.h"
#include "P2VV/RooRealMoments.h"
#include "TMath.h"
#include "RooArgSet.h"

//_____________________________________________________________________________
RooRealMoment::RooRealMoment(const RooAbsReal& basisFunc, Double_t norm,
    const char* name) :
  _basisFunc(basisFunc),
  _name(TString(name).Length() > 0 ? name : _basisFunc.GetName()), _norm(norm),
  _m0(0.), _m1(0.), _n0(0.), _n1(0.), _n2(1, 0.), _reset(kTRUE) {}

RooRealMoment::RooRealMoment(const RooAbsReal& basisFunc,
    const RooRealMomentsVector& moments, Double_t norm, const char* name) :
  _basisFunc(basisFunc),
  _name(TString(name).Length() > 0 ? name : _basisFunc.GetName()), _norm(norm),
  _m0(0.), _m1(0.), _n0(0.), _n1(0.), _n2(1, 0.), _reset(kFALSE)
{
  for (RooRealMomentsVector::const_iterator it = moments.begin();
      it != moments.end(); ++it) {
    _moments.push_back(*it);
    _n2.push_back(0.);
  }
  reset();
}

//_____________________________________________________________________________
Double_t RooRealMoment::coefficient(Bool_t normalize) const
{
  if (_m0 <= 0.) {
    std::cout << "P2VV - ERROR: RooRealMoment::coefficient(" << GetName()
        << "): sum of weights: " << _m0 << std::endl;
     return -999.;
  }

  if (normalize) return _m1 / _m0 * _norm;
  else           return _m1 / _m0;
}

//_____________________________________________________________________________
Double_t RooRealMoment::variance(Bool_t normalize) const
{
  return covariance(0, normalize);
}

//_____________________________________________________________________________
Double_t RooRealMoment::covariance(Int_t index, Bool_t normalize) const
{
  /*
    the following formulas follow either from the jackknife method
    or from error propagation using the following error on weight_j:
        sigma^2( weight_j ) = weight_j^2
    (this is also exactly how it works with s-weight).

    jackknife: sigma2 = (N - 1)/N * sum_j ( mj1 - m )^2), where mj1 is the
    value of m if you would leave measurement j away

    we make one approximation: we ignore the contribution of a
    single weight to the total in a normalization term

    var(mu) = 1/m0^2 * sum  w_j^2 (x_j - mu)^2
    cov(mu1, mu2) = 1/m0^2 * sum w_j^2 (x1_j - mu1) * (x2_j - mu2)
  */

  // check index
  if (index < 0 || index >= (Int_t)_n2.size()) {
    std::cout << "P2VV - ERROR: RooRealMoment::covariance(" << GetName()
        << "): index \"" << index << "\" out of range" << std::endl;
     return -999.;
  }

  // check sums of weights
  if (_m0 <= 0. || (index > 0 && _moments[index - 1]->m0() != _m0)) {
    std::cout << "P2VV - ERROR: RooRealMoment::covariance(" << GetName()
        << "): sums of weights: " << _m0 << " / "
        << (index == 0 ? _m0 : _moments[index - 1]->m0()) << std::endl;
     return -999.;
  }

  // calculate covariance
  Double_t mu    = coefficient(kFALSE);
  Double_t muOth = index == 0 ?  mu : _moments[index - 1]->coefficient(kFALSE);
  Double_t n1Oth = index == 0 ? _n1 : _moments[index - 1]->n1();
  Double_t covMu = (_n2[index] - _n1 * muOth - n1Oth * mu + _n0 * mu * muOth)
                   / (_m0 * _m0);

  // return covariance with or without normalized functions
  if (normalize)
    return covMu * _norm * (index == 0 ? _norm : _moments[index - 1]->norm());
  else
    return covMu;
}

//_____________________________________________________________________________
Double_t RooRealMoment::correlation(Int_t index) const
{
  // return 1 for the correlation with our own function
  if (index == 0) return 1.;

  // check index
  if (index < 0 || index >= (Int_t)_n2.size()) {
    std::cout << "P2VV - ERROR: RooRealMoment::correlation(" << GetName()
        << "): index \"" << index << "\" out of range" << std::endl;
     return -999.;
  }

  // calculate correlation
  Double_t cov = covariance(index, kFALSE);
  Double_t stdDev1 = stdDev(kFALSE);
  Double_t stdDev2 = _moments[index - 1]->stdDev(kFALSE);
  return stdDev1 > 0. && stdDev2 > 0. ? cov / stdDev1 / stdDev2 : -999.;
}

//_____________________________________________________________________________
Double_t RooRealMoment::stdDev(Bool_t normalize) const
{
  Double_t var = variance(normalize);
  if (TMath::Abs(var) < 1.e-10) return 0.;
  return var < 0. ? -999. : std::sqrt(var);
}

//_____________________________________________________________________________
Double_t RooRealMoment::significance() const
{
  Double_t mu  = coefficient(kFALSE);
  Double_t var = variance(kFALSE);
  if (TMath::Abs(var) < 1.e-10) return TMath::Infinity();
  return var < 0. ? -999. : std::sqrt(mu * mu / var);
}

//_____________________________________________________________________________
Double_t RooRealMoment::n2(Int_t index) const
{
  if (index < 0 || index >= (Int_t)_n2.size()) return -999;
  return _n2[index];
}

//_____________________________________________________________________________
void RooRealMoment::inc(Double_t weight)
{
  if (_reset) _reset = kFALSE;

  Double_t funcVal = evaluate();
  _m0 += weight;
  _m1 += weight * funcVal;

  // these we need to compute the error using the jackknife method
  Double_t wSqFuncVal = weight * weight * funcVal;
  _n0    += weight * weight;
  _n1    += wSqFuncVal;
  _n2[0] += wSqFuncVal * funcVal;
  if (!_moments.empty()) {
    std::vector<Double_t>::iterator n2It(_n2.begin());
    for (RooRealMomentsVector::iterator momIt = _moments.begin();
        momIt != _moments.end(); ++momIt) {
      ++n2It;
      *n2It += wSqFuncVal * (*momIt)->evaluate();
    }
  }
}

//_____________________________________________________________________________
void RooRealMoment::reset()
{
  if (_reset) return;
  _reset = kTRUE;

  _m0 = _m1 = _n0 = _n1 = 0.;
  for (std::vector<Double_t>::iterator it = _n2.begin(); it != _n2.end();
      ++it) {
    *it = 0.;
  }
  for (RooRealMomentsVector::iterator it = _moments.begin();
      it != _moments.end(); ++it) {
    (*it)->reset();
  }
}

//_____________________________________________________________________________
void RooRealMoment::appendMoments(const RooRealMomentsVector& moments)
{
  for (RooRealMomentsVector::const_iterator it = moments.begin();
      it != moments.end(); ++it) {
    _moments.push_back(*it);
    _n2.push_back(0.);
  }
  _reset = kFALSE;
  reset();
}

//_____________________________________________________________________________
void RooRealMoment::clearMoments()
{
  _moments.clear();
  _n2.clear();
  _n2.push_back(0.);
  _reset = kFALSE;
  reset();
}

//_____________________________________________________________________________
ostream& RooRealMoment::print(ostream& os, Bool_t normalize) const
{
  return os << "moment(" << _name << ") = " << coefficient(normalize) << " +- "
      << stdDev(normalize) << " (significance: " << significance() << ")"
      << std::endl;
}

//_____________________________________________________________________________
RooRealEffMoment::RooRealEffMoment(RooAbsReal& basisFunc, const RooAbsPdf& pdf,
    const RooArgSet& intSet, const RooArgSet& normSet, Double_t norm,
    const char* name) :
    RooRealMoment(basisFunc, norm, TString(name).Length() > 0
        ? TString(name) : TString(basisFunc.GetName()) + "_" + pdf.GetName()),
    _pdf(pdf), _pdfInt(0), _intSet(intSet), _normSet(normSet)
{
  _pdfInt = _pdf.createIntegral(_intSet, _normSet);
}

//_____________________________________________________________________________
RooRealEffMoment::RooRealEffMoment(RooAbsReal& basisFunc,
    const RooRealMomentsVector& moments, const RooAbsPdf& pdf,
    const RooArgSet& intSet, const RooArgSet& normSet, Double_t norm,
    const char* name) :
    RooRealMoment(basisFunc, moments, norm, TString(name).Length() > 0
        ? TString(name) : TString(basisFunc.GetName()) + "_" + pdf.GetName()),
    _pdf(pdf), _pdfInt(0), _intSet(intSet), _normSet(normSet)
{
  _pdfInt = _pdf.createIntegral(_intSet, _normSet);
}

//_____________________________________________________________________________
RooRealEffMoment::~RooRealEffMoment()
{
  if (_pdfInt) delete _pdfInt;
}

//_____________________________________________________________________________
Int_t computeRooRealMoments(RooAbsData& data, RooRealMomentsVector& moments,
    Bool_t resetFirst, Bool_t verbose)
{
  typedef RooRealMomentsVector::iterator RealMomIter;

  if (moments.empty()) {
    std::cout << "P2VV - ERROR: computeRealMoments: moments vector is empty"
        << std::endl;
    return -1;
  }

  if (resetFirst) {
    for (RealMomIter mom = moments.begin(); mom != moments.end(); ++mom)
      (*mom)->reset();
  }

  RooArgSet* obs = moments.front()->getObservables(data);
  
  if (verbose)
    std::cout << "P2VV - INFO: computeRealMoments: computing "
        << moments.size() << " moment(s) for data set '" << data.GetName()
        << "' with " << data.numEntries() << " events" << std::endl;

  Int_t dataIter = 0;
  ProgressDisplay *prog = verbose ? new ProgressDisplay(data.numEntries()) : 0;
  while (dataIter < data.numEntries()) {
    obs->assignValueOnly(*data.get(dataIter++));
    for (RealMomIter mom = moments.begin(); mom != moments.end(); ++mom)
      (*mom)->inc(data.isWeighted() ? data.weight() : 1.);

    if (prog) ++*prog;
  }
  if (prog) { 
    std::cout << std::endl;
    delete prog;
  }
  return dataIter;
}
