/*****************************************************************************
 * Project: RooFit                                                           *
 * Package: RooFitModels                                                     *
 *    File: $Id$
 * Authors:                                                                  *
 *   Gerhard Raven
 *                                                                           *
 *                                                                           *
 * Redistribution and use in source and binary forms,                        *
 * with or without modification, are permitted according to the terms        *
 * listed in LICENSE (http://roofit.sourceforge.net/license.txt)             *
 *****************************************************************************/
#ifndef ROO_BINNEDFUN
#define ROO_BINNEDFUN

#include "RooAbsReal.h"
#include "RooRealProxy.h"
#include "RooListProxy.h"
#include "RooAbsGaussModelEfficiency.h"

class RooRealVar;
class RooArgList ;
class TH1;

class RooBinnedFun : public  RooAbsGaussModelEfficiency {
public:
  RooBinnedFun() ;
  RooBinnedFun(const char *name, const char *title, RooRealVar& x,
               const char *binningName, const RooArgList& coefList) ;
  RooBinnedFun(const char* name, const char* title, RooRealVar& x, const TH1* hist,
                    bool constCoeffs = true);

  ~RooBinnedFun() ;

  RooBinnedFun(const RooBinnedFun& other, const char* name = 0);
  RooBinnedFun* clone(const char* newname) const { return new RooBinnedFun(*this, newname); }

  Int_t getAnalyticalIntegral(RooArgSet& allVars, RooArgSet& analVars, const char* rangeName) const;
  Double_t analyticalIntegral(Int_t code, const char* rangeName) const;

  Int_t getMaxVal(const RooArgSet& vars) const;
  Double_t maxVal(Int_t code) const;

  std::list<Double_t>* binBoundaries(RooAbsRealLValue& obs, Double_t xlo, Double_t xhi) const;

  // for use as RooAbsGaussModelEfficiency...
  std::complex<double> productAnalyticalIntegral(Double_t umin, Double_t umax
                                                ,Double_t scale, Double_t offset
                                                ,const std::complex<double>& z) const;

  const RooArgList& coefficients() const { return _coefList; }

  // std::list<Double_t>* plotSamplingHint(RooAbsRealLValue& obs, Double_t xlo, Double_t xhi) const;

private:

  RooRealProxy _x;
  RooListProxy _coefList ;
  std::vector<double> _u;

  Double_t evaluate() const;
  ClassDef(RooBinnedFun,1) // piecewise constant function
};

#endif
