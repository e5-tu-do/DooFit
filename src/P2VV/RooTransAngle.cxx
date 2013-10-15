/***************************************************************************** 
 * Project: RooFit                                                           * 
 *                                                                           * 
 * Authors:                                                                  *
 *   JvL, Jeroen van Leerdam, Nikhef, j.van.leerdam@nikhef.nl                *
 *                                                                           *
 * Copyright (c) 2012, Nikhef. All rights reserved.                          *
 *                                                                           *
 * Redistribution and use in source and binary forms,                        *
 * with or without modification, are permitted according to the terms        *
 * listed in LICENSE (http://roofit.sourceforge.net/license.txt)             *
 *****************************************************************************/ 

#include <algorithm>
#include <math.h>

#include "Riostream.h"
#include "RooMsgService.h"
#include "RooAbsReal.h" 
#include "P2VV/RooTransAngle.h" 
#include "TMath.h" 

//_____________________________________________________________________________
RooTransAngle::RooTransAngle(const char* name, const char* title,
    RooAbsReal& cthetaK) :
  RooAbsReal(name, title),
  _azimuth(kFALSE),
  _cthetaK("cthetaK", "Polarization angle 0", this, cthetaK),
  _cthetal("cthetal", "Polarization angle 1", this),
  _phi("phi", "Angle between decay planes", this)
{}

//_____________________________________________________________________________
RooTransAngle::RooTransAngle(const char* name, const char* title,
    RooAbsReal& cthetal, RooAbsReal& phi, Bool_t azimuth) :
  RooAbsReal(name, title),
  _azimuth(azimuth),
  _cthetaK("cthetaK", "Polarization angle 0", this),
  _cthetal("cthetal", "Polarization angle 1", this, cthetal),
  _phi("phi", "Angle between decay planes", this, phi)
{}

//_____________________________________________________________________________
RooTransAngle::RooTransAngle(const RooTransAngle& other, const char* name) :
  RooAbsReal(other, name),
  _azimuth(other._azimuth),
  _cthetaK("cthetaK", this, other._cthetaK),
  _cthetal("cthetal", this, other._cthetal),
  _phi("phi", this, other._phi)
{}

//_____________________________________________________________________________
Double_t RooTransAngle::cthetaK() const
{
  if (_cthetaK.absArg() == 0) return -999.;
  else return _cthetaK;
}

//_____________________________________________________________________________
Double_t RooTransAngle::cthetal() const
{
  if (_cthetal.absArg() == 0) return -999.;
  else return _cthetal;
}

//_____________________________________________________________________________
Double_t RooTransAngle::phi() const
{
  if (_phi.absArg() == 0) return -999.;
  else return _phi;
}

//_____________________________________________________________________________
Double_t RooTransAngle::evaluate() const
{
  // return cos(theta_K) if we are cos(psi)
  if (_cthetaK.absArg() != 0) return _cthetaK;

  // compute cos(theta)
  Double_t cthetal = _cthetal;
  if (cthetal < -1. || cthetal > 1.) {
    coutE(Eval) << "RooTransAngle::evaluate(" << GetName()
                << ") cos(theta_l) value outside range: " << cthetal << std::endl;
    cthetal = std::max(-1., std::min(cthetal, +1.));
  }

  Double_t ctheta = sqrt(1. - cthetal * cthetal) * sin(_phi);
  if (!_azimuth) return ctheta;

  // compute phi
  if (1. - ctheta * ctheta < 1.e-14) {
    coutW(Eval) << "RooTransAngle::evaluate(" << GetName()
                << ") transversity angle phi is undefined: sin(theta_tr) = "
                << sqrt(1. - ctheta * ctheta) << std::endl;
    return -999.;
  }

  return TMath::ATan2(-sqrt(1. - cthetal * cthetal) * cos(_phi), -cthetal);
}
