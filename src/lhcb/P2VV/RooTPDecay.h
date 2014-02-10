/*****************************************************************************
 * Project: RooFit                                                           *
 * Package: RooFitModels                                                     *
 * Authors:                                                                  *
 *                                                                           *
 * Copyright (c) 2000-2005, Regents of the University of California          *
 *                          and Stanford University. All rights reserved.    *
 *                                                                           *
 * Redistribution and use in source and binary forms,                        *
 * with or without modification, are permitted according to the terms        *
 * listed in LICENSE (http://roofit.sourceforge.net/license.txt)             *
 *****************************************************************************/
#ifndef ROO_TPDECAY
#define ROO_TPDECAY

#include "RooAbsAnaConvPdf.h"
#include "RooDecay.h"
#include "RooRealProxy.h"
#include "RooListProxy.h"
#include "Roo1DTable.h"
#include "RooConstVar.h"
#include "RooRealVar.h"

class RooGaussian;

class RooTPDecay : public RooDecay {
public:

   // Constructors, assignment etc
   inline RooTPDecay() { }
   RooTPDecay(const char *name, const char *title, RooRealVar& t, 
              RooAbsReal& tau, RooArgList& tps, const RooResolutionModel& model, 
              const Roo1DTable& nPV, 
              RooAbsReal& z, RooAbsPdf& zPDF);
   RooTPDecay(const RooTPDecay& other, const char* name=0);
   virtual TObject* clone(const char* newname) const { return new RooTPDecay(*this, newname); }
   virtual ~RooTPDecay();

   virtual Int_t getGenerator(const RooArgSet& directVars, RooArgSet &generateVars, Bool_t staticInitOK=kTRUE) const;
   virtual void initGenerator(Int_t /*code*/);
   virtual void generateEvent(Int_t code);
  
   double getDistance() const { return _mean.getVal(); }
   void setDistance(const double d) { _mean.setVal(d); }

   double getSmearing() const { return _sigma.getVal(); }
   void setSmearing(const double s) { _sigma.setVal(s); }

protected:

   RooListProxy _tps;
   Roo1DTable   _nPV;
   RooRealProxy _z;
   RooRealProxy _zPDF;
   mutable int _zCode;

   // default distance = 0.1 mm (0.3ps * c_light)
   RooRealVar   _d;
   RooRealVar   _mean;
   RooConstVar  _gmean;
   RooRealVar   _sigma;
   RooGaussian* _gauss;
   mutable int  _gCode;
   double       _min;
   double       _max;

   ClassDef(RooTPDecay,1) // General decay function p.d.f 
};

#endif
