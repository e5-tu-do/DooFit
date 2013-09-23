/*****************************************************************************
 * Project: RooFit                                                           *
 * Package: RooFitCore                                                       *
 * @(#)root/roofitcore:$Id: RooEffConvGenContext.cxx 28259 2009-04-16 16:21:16Z wouter $
 * Authors:                                                                  *
 *   WV, Wouter Verkerke, UC Santa Barbara, verkerke@slac.stanford.edu       *
 *   DK, David Kirkby,    UC Irvine,         dkirkby@uci.edu                 *
 *                                                                           *
 * Copyright (c) 2000-2005, Regents of the University of California          *
 *                          and Stanford University. All rights reserved.    *
 *                                                                           *
 * Redistribution and use in source and binary forms,                        *
 * with or without modification, are permitted according to the terms        *
 * listed in LICENSE (http://roofit.sourceforge.net/license.txt)             *
 *****************************************************************************/

//////////////////////////////////////////////////////////////////////////////
// 
// BEGIN_HTML
// RooEffConvGenContext is an efficient implementation of the generator context
// specific for RooAbsAnaConvPdf objects. The physics model is generated
// with a truth resolution model and the requested resolution model is generated
// separately as a PDF. The convolution variable of the physics model is 
// subsequently explicitly smeared with the resolution model distribution.
// END_HTML
//

#include "RooFit.h"

#include "RooMsgService.h"
#include "SplineRooEffConvGenContext.h"
#include "RooAbsAnaConvPdf.h"
#include "RooNumConvPdf.h"
#include "RooFFTConvPdf.h"
#include "RooProdPdf.h"
#include "RooDataSet.h"
#include "RooArgSet.h"
#include "RooTruthModel.h"
#include "Riostream.h"
#include "RooRandom.h"

#include "SplineRooEffResModel.h"
#include <iostream>
using std::endl;

namespace doofit {
namespace extraroopdfs {
namespace splinepdfs {

//_____________________________________________________________________________
RooEffConvGenContext::RooEffConvGenContext(const RooAbsAnaConvPdf &model, const RooArgSet &vars, 
                                           const RooDataSet *prototype, const RooArgSet* auxProto, Bool_t verbose)
   : RooConvGenContext(model, vars, prototype, auxProto, verbose)
{
   initEfficiency();
}

//_____________________________________________________________________________
RooEffConvGenContext::RooEffConvGenContext(const RooNumConvPdf &model, const RooArgSet &vars, 
                                           const RooDataSet *prototype, const RooArgSet* auxProto, Bool_t verbose)
   : RooConvGenContext(model, vars, prototype, auxProto, verbose)
{
   initEfficiency();
}

//_____________________________________________________________________________
RooEffConvGenContext::RooEffConvGenContext(const RooFFTConvPdf &model, const RooArgSet &vars, 
                                           const RooDataSet *prototype, const RooArgSet* auxProto, Bool_t verbose)
   : RooConvGenContext(model, vars, prototype, auxProto, verbose)
{
   initEfficiency();
}

//_____________________________________________________________________________
void RooEffConvGenContext::initGenerator(const RooArgSet& theEvent) 
{
   RooConvGenContext::initGenerator(theEvent);
   // Attach the output value of the convolution variable to the efficiencies,
   // so the final hit-miss is with respect to the correct (smeared) value;
   const_cast<RooAbsReal*>(efficiency())->recursiveRedirectServers(RooArgSet(*_cvOut), kFALSE);
}
//_____________________________________________________________________________
void RooEffConvGenContext::generateEvent(RooArgSet &theEvent, Int_t remaining)
{
   // Generate a single event 

   while(1) {      
      // Generate pdf and model data
      _modelGen->generateEvent(*_modelVars, remaining) ;
      _pdfGen->generateEvent(*_pdfVars, remaining) ;    
    
      // Construct smeared convolution variable
      Double_t convValSmeared = _cvPdf->getVal() + _cvModel->getVal() ;

      if (!_cvOut->isValidReal(convValSmeared)) continue;
      
      // Hit-miss on the efficiency
      // This has to be set first to get the proper value of the efficiency
      _cvOut->setVal(convValSmeared) ;

      double val = efficiency()->getVal();
      if (val > _maxEff && !efficiency()->getMaxVal(*_modelVars)) {
         coutE(Generation) << ClassName() << "::" << GetName() 
                           << ":generateEvent: value of efficiency is larger "
                           << "than assumed maximum of 1." << endl;
         continue;
      }
      if (val > RooRandom::uniform() * _maxEff) {
         // Smeared value in acceptance range, transfer values to output set
         theEvent = *_modelVars ;
         theEvent = *_pdfVars ;
         return;
      }
   }
}

//_____________________________________________________________________________
void RooEffConvGenContext::initEfficiency()
{
   // Check if efficiency supports maximum finding
   const RooAbsEffResModel* model = dynamic_cast<const RooAbsEffResModel*>(_modelCloneSet->first());
   assert(model);
   const RooAbsReal* efficiency = model->efficiency();
   Int_t maxCode = efficiency->getMaxVal(*_modelVars);
   _maxEff = ( maxCode ?  efficiency->maxVal(maxCode) : 1. ) ;
}

//_____________________________________________________________________________
const RooAbsReal* RooEffConvGenContext::efficiency()
{
   const RooAbsEffResModel* model = dynamic_cast<const RooAbsEffResModel*>(_modelCloneSet->first());
   assert(model);
   const RooAbsReal* efficiency = model->efficiency();
   assert(efficiency);
   return efficiency;
}

}
}
}