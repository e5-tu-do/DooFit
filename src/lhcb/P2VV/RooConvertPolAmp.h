/*****************************************************************************
 * Project: RooFit                                                           *
 *                                                                           *
 * Authors:                                                                  *
 *   JvL, Jeroen van Leerdam, Nikhef, j.van.leerdam@nikhef.nl                *
 *                                                                           *
 * Copyright (c) 2013, Nikhef. All rights reserved.                          *
 *                                                                           *
 * Redistribution and use in source and binary forms,                        *
 * with or without modification, are permitted according to the terms        *
 * listed in LICENSE (http://roofit.sourceforge.net/license.txt)             *
 *****************************************************************************/

#ifndef ROO_CONVERT_POL_AMP
#define ROO_CONVERT_POL_AMP

#include "Riosfwd.h"
#include "RooListProxy.h"

class RooAbsReal;
class RooArgList;
 
class RooConvertPolAmp : public RooAbsReal
{

public:
  enum EvalType {
    FracToMag,          // sqrt( f / (1 - f) )                {f}
    FracToMagSq,        // f / (1 - f)                        {f}
    MagToFrac,          // A^2 / (1 + A^2)                    {A}
    MagSqToFrac,        // A / (1 + A)                        {A}
    PolToRe,            // A * cos(ph)                        {A,ph}
    PolToIm,            // A * sin(ph)                        {A,ph}
    PolSqToRe,          // sqrt(A) * cos(ph)                  {A,ph}
    PolSqToIm,          // sqrt(A) * sin(ph)                  {A,ph}
    MagSqSinToReIm,     // sqrt(A) * a                        {A,a}
    CarthToMag,         // sqrt( Re^2 + Im^2 )                {Re,Im}
    CarthToMagSq,       // Re^2 + Im^2                        {Re,Im}
    CarthFracToMagSq,   // (fRe^2 + fIm^2) / (1-fRe^2-fIm^2)  {fRe,fIm}
    CarthToPhase,       // arctan( Im / Re )                  {Re,Im}
    ProdCarthPhaseToRe, // Re * cos(ph) - Im * sin(ph)        {Re,Im,ph}
    ProdCarthPhaseToIm, // Re * sin(ph) + Im * cos(ph)        {Re,Im,ph}
    PolSqToReC,         // C * sqrt(A) * cos(ph)              {A,ph,C}
    PolSqToImC,         // C * sqrt(A) * sin(ph)              {A,ph,C}
    ProdCarthToRe,      // Re1 * Re2 - Im1 * Im2              {Re1,Im1,Re2,Im2}
    ProdCarthToIm,      // Re1 * Im2 + Im1 * Re2              {Re1,Im1,Re2,Im2}
    ProdCarthCToRe,     // Re1 * Re2 + Im1 * Im2              {Re1,Im1,Re2,Im2}
    ProdCarthCToIm,     // Re1 * Im2 - Im1 * Re2              {Re1,Im1,Re2,Im2}
    PolSqToReRelC,      // C * sqrt(A) * cos(ph1+ph2)         {A,ph1,ph2,C}
    PolSqToImRelC,      // C * sqrt(A) * sin(ph1+ph2)         {A,ph1,ph2,C}
    FracPolToReC,       // C * sqrt(f/(1-f)) * cos(ph)        {f,ph,C}
    FracPolToImC,       // C * sqrt(f/(1-f)) * sin(ph)        {f,ph,C}
    FracPolToReRelC,    // C * sqrt(f/(1-f)) * cos(ph1+ph2)   {f,ph1,ph2,C}
    FracPolToImRelC,    // C * sqrt(f/(1-f)) * sin(ph1+ph2)   {f,ph1,ph2,C}
    FracCarthToReRelC,  // C * fRe / sqrt(1-fRe^2-fIm^2)      {fRe,fIm,C}
    FracCarthToImRelC,  // C * fIm / sqrt(1-fRe^2-fIm^2)      {fRe,fIm,C}
    PolSqToReRelMagC,   // C * sqrt(A1*A2) * cos(ph1+ph2)     {A1,ph1,A2,ph2,C}
    PolSqToImRelMagC,   // C * sqrt(A1*A2) * sin(ph1+ph2)     {A1,ph1,A2,ph2,C}
    MixSqToReRelMagC,   // C*sqrt(A)*(cos(ph)*Re-sin(ph)*Im)  {A,ph,Re,Im,C}
    MixSqToImRelMagC,   // C*sqrt(A)*(cos(ph)*Im+sin(ph)*Re)  {A,ph,Re,Im,C}
  };

  // default constructor
  RooConvertPolAmp(); 

  // constructor
  RooConvertPolAmp(const char *name, const char *title, EvalType type,
      const RooArgList& arguments);

  // copy constructor
  RooConvertPolAmp(const RooConvertPolAmp& other, const char* name = 0);
  virtual TObject* clone(const char* name) const
  { 
    return new RooConvertPolAmp(*this, name);
  }

  ~RooConvertPolAmp();

  RooArgList arguments() const {return RooArgList(_args, "arguments");}
  EvalType type() const {return _type;}

  void printArgs(std::ostream& os) const ;

protected:
  const EvalType _type;
  RooListProxy _args;
  mutable Double_t* _vals; //!

  Double_t evaluate() const;

private:
  ClassDef(RooConvertPolAmp, 1)
};
 
#endif
