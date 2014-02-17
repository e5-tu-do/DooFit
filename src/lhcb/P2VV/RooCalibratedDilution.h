/*****************************************************************************
 * Project: RooFit                                                           *
 *                                                                           *
 * Authors:                                                                  *
 *   RA,  Roel Aaij,          Nikhef                                         *
 *   JvL, Jeroen van Leerdam, Nikhef,      j.van.leerdam@nikhef.nl           *
 *                                                                           *
 * Copyright (c) 2012, Nikhef. All rights reserved.                          *
 *                                                                           *
 * Redistribution and use in source and binary forms,                        *
 * with or without modification, are permitted according to the terms        *
 * listed in LICENSE (http://roofit.sourceforge.net/license.txt)             *
 *****************************************************************************/

#ifndef ROO_CALIBRATED_DILUTION
#define ROO_CALIBRATED_DILUTION

#include "RooRealProxy.h"

class RooAbsReal;
 
class RooCalibratedDilution : public RooAbsReal
{

public:
  RooCalibratedDilution() {}; 

  // dilution without calibration parameters
  RooCalibratedDilution(const char *name, const char *title,
      RooAbsReal& WTag);

  // dilution scaled wrong-tag asymmetry without calibration parameters
  RooCalibratedDilution(const char *name, const char *title,
      RooAbsReal& WTag, RooAbsReal& AWTag);

  // dilution with calibration parameters
  RooCalibratedDilution(const char *name, const char *title,
      RooAbsReal& estWTag, RooAbsReal& avgEstWTag,
      RooAbsReal& P0, RooAbsReal& P1);

  // dilution scaled wrong-tag asymmetry with calibration parameters
  RooCalibratedDilution(const char *name, const char *title,
      RooAbsReal& estWTag, RooAbsReal& avgEstWTag,
      RooAbsReal& P0, RooAbsReal& P1, RooAbsReal& AP0, RooAbsReal& AP1);

  // copy constructor
  RooCalibratedDilution(const RooCalibratedDilution& other,
      const char* name = 0);
  virtual TObject* clone(const char* name) const
  { 
    return new RooCalibratedDilution(*this, name);
  }

  inline virtual ~RooCalibratedDilution() { }

  Double_t WTag() const;
  Double_t estWTag() const;
  Double_t avgEstWTag() const;
  Double_t P0() const;
  Double_t P1() const;
  Double_t AP0() const;
  Double_t AP1() const;

protected:
  RooRealProxy _estWTag;
  RooRealProxy _avgEstWTag;
  RooRealProxy _P0;
  RooRealProxy _P1;
  RooRealProxy _AP0;
  RooRealProxy _AP1;
  
  Double_t evaluate() const;

private:
  ClassDef(RooCalibratedDilution, 1)
};
 
#endif
