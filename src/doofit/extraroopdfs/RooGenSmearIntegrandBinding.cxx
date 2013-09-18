/*****************************************************************************
 * Project: RooFit                                                           *
 * Package: RooFitCore                                                       *
 * @(#)root/roofitcore:$Id: RooGenSmearIntegrandBinding.cxx 36209 2010-10-08 21:37:36Z wouter $
 * Authors:                                                                  *
 *   WV, Wouter Verkerke, UC Santa Barbara, verkerke@slac.stanford.edu       *
 *   DK, David Kirkby,    UC Irvine,         dkirkby@uci.edu                 *
 *                                                                           *
 * Copyright (c) 2000-2005, Regents of the University of California          *
 *                          and Stanford University. All rights reserved.    *
 *                                                                           *
 * Jul  5, 2012	Manuel Schiller	<manuel.schiller@nikhef.nl>                  *
 * 	renamed to RooGenSmearIntegrandBinding to help perform a generalised *
 * 	smearing integral which is not neccessarily a smearing               *
 *                                                                           *
 * Redistribution and use in source and binary forms,                        *
 * with or without modification, are permitted according to the terms        *
 * listed in LICENSE (http://roofit.sourceforge.net/license.txt)             *
 *****************************************************************************/

//////////////////////////////////////////////////////////////////////////////
//
// BEGIN_HTML
// Implementation of RooAbsFunc that represent the the integrand
// of a generic (numeric) smearing A (k) B (k) so that it can be
// passed to a numeric integrator. This is a utility class for
// RooGeneralisedSmearing
// END_HTML
//

#include <RooFit.h>

#include <RooAbsReal.h>
#include <RooArgSet.h>
#include <RooAbsRealLValue.h>
#include <RooMsgService.h>

#include <cassert>

#include "B2DXFitters/RooGenSmearIntegrandBinding.h"

//ClassImp(RooGenSmearIntegrandBinding);

//_____________________________________________________________________________
RooGenSmearIntegrandBinding::RooGenSmearIntegrandBinding(const RooAbsReal& func,
	const RooAbsReal& model, RooAbsReal& k,
	const RooArgSet* nset, Bool_t clipInvalid) :
    RooAbsFunc(1), _func(&func), _model(&model), _var(0), _nset(nset),
    _clipInvalid(clipInvalid)
{
    // Constructor where func and model
    //
    // 'func'  = func(k)
    // 'model' = model(k)
    //
    // and

    // 'k' is the RRV that should be connected to func and model and is to be
    // integrated over
    //
    // this function returns RCBB = f[k]*g[k]
    //
    // the integral of this binding over its 1st arg yields the smearing
    // integral

    // check that all of the arguments are real valued and store them
    _var = dynamic_cast<RooAbsRealLValue*>(&k);
    if (0 == _var) {
	oocoutE(&func,InputArguments) <<
	    "RooGenSmearIntegrandBinding: cannot bind to ";
	k.Print("1");
	_valid = kFALSE;
    }

    _xvecValid = kTRUE;
}

//_____________________________________________________________________________
RooGenSmearIntegrandBinding::~RooGenSmearIntegrandBinding()
{ /* Destructor */ }

//_____________________________________________________________________________
void RooGenSmearIntegrandBinding::loadValues(const Double_t xvector[],
	Bool_t clipInvalid) const
{
    // Load external input values
    if (clipInvalid && !_var->isValidReal(xvector[0])) {
	_xvecValid = kFALSE;
    } else {
	_xvecValid = kTRUE;
	_var->setVal(xvector[0]);
    }
}

//_____________________________________________________________________________
Double_t RooGenSmearIntegrandBinding::operator()(const Double_t xvector[]) const
{
    // Evaluate self at given parameter values
    assert(isValid());
    ++_ncall;

    loadValues(xvector, kTRUE);
    if (!_xvecValid) return 0.;
    const Double_t f_k = _func->getVal(_nset);
    const Double_t g_k = _model->getVal(_nset);

    return f_k * g_k;
}


//_____________________________________________________________________________
Double_t RooGenSmearIntegrandBinding::getMinLimit(UInt_t index) const
{
    // Retrieve lower limit of i-th observable
    assert(isValid());
    assert(index == 0);
    return _var->getMin();
}


//_____________________________________________________________________________
Double_t RooGenSmearIntegrandBinding::getMaxLimit(UInt_t index) const
{
    // Retrieve upper limit of i-th observable
    assert(isValid());
    assert(index == 0);
    return _var->getMax();
}
