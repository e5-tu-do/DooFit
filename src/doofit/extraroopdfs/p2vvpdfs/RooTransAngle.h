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

#ifndef ROO_TRANS_ANGLE
#define ROO_TRANS_ANGLE

#include "RooRealProxy.h"

class RooAbsReal;
 
class RooTransAngle : public RooAbsReal
{

public:
  RooTransAngle() {}; 

  // constructor for cos(psi)
  RooTransAngle(const char *name, const char *title, RooAbsReal& cthetaK);

  // constructor for cos(theta)/phi
  RooTransAngle(const char *name, const char *title, RooAbsReal& cthetal,
      RooAbsReal& phi, Bool_t azimuth);

  // copy constructor
  RooTransAngle(const RooTransAngle& other, const char* name = 0);
  virtual TObject* clone(const char* name) const
  { 
    return new RooTransAngle(*this, name);
  }

  inline virtual ~RooTransAngle() { }

  Bool_t transSystem() const {return _cthetaK.absArg() == 0;}
  Bool_t azimuth()     const {return _azimuth;}

  Double_t cthetaK() const;
  Double_t cthetal() const;
  Double_t phi()     const;

protected:
  Bool_t _azimuth;
  RooRealProxy _cthetaK;
  RooRealProxy _cthetal;
  RooRealProxy _phi;
  
  Double_t evaluate() const;

private:
  ClassDef(RooTransAngle, 1)
};
 
#endif
