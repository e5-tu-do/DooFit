/*****************************************************************************
* Package: RooRarFit
*    File: $Id: RooCruijff.cc,v 1.5 2011/06/16 13:18:48 fwilson Exp $   *
* Authors:                                                                  *
*    Karsten Koeneke, Massachusetts Institute of Technology, Cambridge, USA *
*    Vouter Hulbergen                                                       *
*                                                                           *
* Copyright (C) 2005-2012, Massachsetts Institute of Technology, Cambridge, USA  *
*****************************************************************************/

// This is an implementation for the Cruijff function for RooFit


#include "Riostream.h"
 
#include "B2DXFitters/RooCruijff.h"
#include "RooRealVar.h"
#include "RooAbsReal.h"
#include "RooRealConstant.h"

//ClassImp(RooCruijff);

RooCruijff::RooCruijff(const char *name, const char *title,
                        RooAbsReal& _x, RooAbsReal& _m0, 
                       RooAbsReal& _sigmaL, RooAbsReal& _sigmaR,
                       RooAbsReal& _alphaL, RooAbsReal& _alphaR)
   :
  RooAbsPdf(name, title),
  x("x", "x", this, _x),
  m0("m0", "m0", this, _m0),
  sigmaL("sigmaL", "sigmaL", this, _sigmaL),
  sigmaR("sigmaR", "sigmaR", this, _sigmaR),
  alphaL("alphaL", "alphaL", this, _alphaL),
  alphaR("alphaR", "alphaR", this, _alphaR)
 {
}

 RooCruijff::RooCruijff(const RooCruijff& other, const char* name) :
   RooAbsPdf(other, name), 
   x("x", this, other.x), 
   m0("m0", this, other.m0),
   sigmaL("sigmaL", this, other.sigmaL), 
   sigmaR("sigmaR", this, other.sigmaR), 
   alphaL("alphaL", this, other.alphaL), 
   alphaR("alphaR", this, other.alphaR)
 {
 }
 
 Double_t RooCruijff::evaluate() const 
 {
   // build the functional form
   const double dx = (x - m0);
   const double s = (dx < 0.) ? sigmaL : sigmaR;
   const double a = (dx < 0.) ? alphaL : alphaR;
   const double f = 2.*s*s + a*dx*dx ;
   return exp(-dx*dx/f) ;
 }
