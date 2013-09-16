/*****************************************************************************
 * Project: RooFit                                                           *
 * Package: RooFitCore                                                       *
 * @(#)root/roofitcore:$Id: RooGeneralisedSmearingBase.cxx 38909 2011-04-18 21:05:15Z wouter $
 * Authors:                                                                  *
 *   WV, Wouter Verkerke, UC Santa Barbara, verkerke@slac.stanford.edu       *
 *   DK, David Kirkby,    UC Irvine,         dkirkby@uci.edu                 *
 *                                                                           *
 * Copyright (c) 2000-2005, Regents of the University of California          *
 *                          and Stanford University. All rights reserved.    *
 *                                                                           *
 * Jul  3, 2012	Manuel Schiller	<manuel.schiller@nikhef.nl>                  *
 * 	renamed to RooGeneralisedSmearingBase to perform a generalised smearing  *
 * 	integral which is not neccessarily a smearing                     *
 *                                                                           *
 * Redistribution and use in source and binary forms,                        *
 * with or without modification, are permitted according to the terms        *
 * listed in LICENSE (http://roofit.sourceforge.net/license.txt)             *
 *****************************************************************************/

//////////////////////////////////////////////////////////////////////////////
//
// BEGIN_HTML
// Numeric 1-dimensional generalised smearing operator PDF. This class can
// smear any PDF with any other PDF by computing an integral over a common
// smearing integrand; specifically, this will compute the integral
// \int_{k} PDF1(..., k, ...) PDF2(..., k, ...) dk
// <p>
// This class should not be used blindly as numeric integration is computing
// intensive and prone to fitting stability problems. If an analytic expression
// for the smearing integral can be calculated, you should use that or
// implement it if not available. RooGeneralisedSmearingBase implements reasonable
// defaults that should smear most functions reasonably well, but results
// strongly depend on the shape of your input PDFS so always check your result.
// <p>
// The default integration engine for the numeric integration in smearing is
// the adaptive Gauss-Kronrod method, which empirically seems the most robust
// for this task. You can override the smearing integration settings via
// the RooNumIntConfig object reference returned by the convIntConfig() member
// function
// <p>
// By default the numeric smearing is integrated from -infinity to
// +infinity through a <pre>x -> 1/x</pre> coordinate transformation of the
// tails. For smearing with a very small bandwidth it may be
// advantageous (for both CPU consumption and stability) if the
// integration domain is limited to a finite range. The function
// setConvolutionWindow(mean,width,scale) allows to set a sliding
// window around the k value to be calculated taking a RooAbsReal
// expression for an offset and a width to be taken around the x
// value. These input expression can be RooFormulaVars or other
// function objects although the 3d 'scale' argument 'scale'
// multiplies the width RooAbsReal expression given in the 2nd
// argument, allowing for an appropriate window definition for most
// cases without need for a RooFormulaVar object: e.g. a Gaussian
// resolution PDF do setConvolutionWindow(gaussMean,gaussSigma,5)
// Note that for a 'wide' Gaussian the -inf to +inf integration
// may converge more quickly than that over a finite range!
// <p>
// The default numeric precision is 1e-7, i.e. the global default for
// numeric integration but you should experiment with this value to
// see if it is sufficient for example by studying the number of function
// calls that MINUIT needs to fit your function as function of the
// smearing precision.
// END_HTML

#include <cmath>
#include <cstring>
#include <iostream>
#include <algorithm>

#include <TMethodCall.h>
#include <RooFit.h>
#include <Riostream.h>
#include <TH2F.h>
#include <RooArgList.h>
#include <RooRealVar.h>
#include <RooFormulaVar.h>
#include <RooCustomizer.h>
#include <RooNumIntFactory.h>
#include <RooGenContext.h>
#include <RooConvGenContext.h>
#include <RooMsgService.h>
#include <RooGlobalFunc.h>
#include <TIterator.h>
#include <RooAbsAnaConvPdf.h>
#include <RooAbsCategory.h>

#include "B2DXFitters/RooGenSmearIntegrandBinding.h"
#include "B2DXFitters/RooGeneralisedSmearingBase.h"

//ClassImp(RooGeneralisedSmearingBase);

template <class BASE>
RooGeneralisedSmearingBase<BASE>::RooGeneralisedSmearingBase() :
    _init(kFALSE),
    _integrator(0),
    _integrand(0),
    _cloneVar(0),
    _clonePdf(0),
    _cloneModel(0),
    _binning(0),
    _binnedvarClone(0),
    _binnedcats("binnedcats"),
    _preferBinnedIntegrals(false),
    _useWindow(kFALSE),
    _windowScale(1)
{ /* default constructor for ROOT I/O */ }

template <class BASE>
RooGeneralisedSmearingBase<BASE>::RooGeneralisedSmearingBase(
	const char* name, const char* title, RooRealVar& k,
	RooAbsReal& inPdf, RooAbsReal& resmodel) :
    BASE(name, title),
    _init(kFALSE),
    _convIntConfig(RooNumIntConfig::defaultConfig()),
    _integrator(0),
    _integrand(0),
    _origVar("origVar","Original Convolution variable", this, k),
    _origPdf("origPdf","Original Input PDF", this, inPdf),
    _origModel("origModel","Original Resolution model", this, resmodel),
    _cloneVar(0),
    _clonePdf(0),
    _cloneModel(0),
    _binning(0),
    _binnedvarClone(0),
    _binnedcats("binnedcats"),
    _preferBinnedIntegrals(false),
    _useWindow(kFALSE),
    _windowScale(1),
    _windowParam("windowParam","Convolution window parameter", this, kFALSE)
{
    // Constructor of smearing operator PDF
    //
    // k       :  smearing variable (on which both pdf and resmodel should depend)
    // pdf     :  input 'physics' pdf
    // resmodel:  input 'smearing' pdf
    //
    // output is \int pdf(..., k, ...) resmodel (k) dk
    //

    // Use Adaptive Gauss-Kronrod integration by default for the smearing integral
    _convIntConfig.method1D().setLabel("RooAdaptiveGaussKronrodIntegrator1D");
    _convIntConfig.method1DOpen().setLabel("RooAdaptiveGaussKronrodIntegrator1D");
}

template <class BASE>
RooGeneralisedSmearingBase<BASE>::RooGeneralisedSmearingBase(
	const RooGeneralisedSmearingBase<BASE>& other, const char* name) :
    BASE(other, name),
    _init(kFALSE),
    _convIntConfig(other._convIntConfig),
    _integrator(0),
    _integrand(0),
    _origVar("origVar", this, other._origVar),
    _origPdf("origPdf", this, other._origPdf),
    _origModel("origModel", this, other._origModel),
    _cloneVar(0),
    _clonePdf(0),
    _cloneModel(0),
    _binning(0),
    _binnedvar("_binnedvar", this, other._binnedvar),
    _binnedvarClone(0),
    _binnedcats(other._binnedcats),
    _binnedcache(other._binnedcache),
    _preferBinnedIntegrals(false),
    _useWindow(other._useWindow),
    _windowScale(other._windowScale),
    _windowParam("windowParam", this, other._windowParam)
{
    /* Copy constructor */
    if (other._binning) {
	_binning = new char[1 + std::strlen(other._binning)];
	std::strcpy(_binning, other._binning);
    }
}

template <class BASE>
RooGeneralisedSmearingBase<BASE>& RooGeneralisedSmearingBase<BASE>::operator=(
	const RooGeneralisedSmearingBase<BASE>& other)
{
    if (&other == this) return *this;
    // drop all owned objects
    invalidateBinnedCache();
    invalidateAnaIntCache();
    invalidateIntegrandCache();
    delete[] _binning;
    base().operator=(other);
    _init = kFALSE;
    _convIntConfig = other._convIntConfig;
    _integrator = 0;
    _integrand = 0;
    _origVar = RooRealProxy("origVar", this, other._origVar);
    _origPdf = RooRealProxy("origPdf", this, other._origPdf);
    _origModel = RooRealProxy("origModel", this, other._origModel);
    _cloneVar = 0;
    _clonePdf = 0;
    _cloneModel = 0;
    _binning = 0;
    _binnedvar = RooRealProxy("_binnedvar", this, other._binnedvar);
    _binnedvarClone = 0;
    _binnedcats = other._binnedcats;
    _binnedcache = other._binnedcache;
    _preferBinnedIntegrals = false;
    _useWindow = other._useWindow;
    _windowScale = other._windowScale;
    _windowParam = RooListProxy("windowParam", this, other._windowParam);
    if (other._binning) {
	_binning = new char[1 + std::strlen(other._binning)];
	std::strcpy(_binning, other._binning);
    }
    return *this;
}

template <class BASE>
void RooGeneralisedSmearingBase<BASE>::initialize() const
{
    // One-time initialization of object

    // Initialization function -- create clone of k (k') and deep-copy clones
    // of pdf and model that are connected to k' rather than k

    // Start out clean
    invalidateAnaIntCache();
    invalidateBinnedCache();
    invalidateIntegrandCache();

    // Customize a copy of origPdf that is connected to k' rather than k
    // store all cloned components in _clonePdfSet as well as k' itself
    _cloneVar = new RooRealVar(Form("%s_prime", var().GetName()),
	    "Smearing Variable", 0);

    RooCustomizer mgr1(pdf(), "NumConv_PdfClone");
    mgr1.replaceArg(var(), *_cloneVar);
    _clonePdf = (RooAbsReal*) mgr1.build();

    RooCustomizer mgr2(model(), "NumConv_ModelClone");
    mgr2.replaceArg(var(), *_cloneVar);
    _cloneModel = (RooAbsReal*) mgr2.build();

     // Create Convolution integrand
    _integrand = new RooGenSmearIntegrandBinding(
	    *_clonePdf, *_cloneModel, *_cloneVar, 0);

    // Instantiate integrator for smearing integrand
    _integrator = RooNumIntFactory::instance().createIntegrator(
	    *_integrand, _convIntConfig, 1);
    _integrator->setUseIntegrandLimits(kFALSE);

    _init = kTRUE;

    // make sure everything gets propagated properly
    doEvaluate();
}

template <class BASE>
void RooGeneralisedSmearingBase<BASE>::invalidateAnaIntCache() const
{
    // drop anything we might have cached for analytical integrals
    if (!_integvarset.empty() || !_integratorset.empty() ||
	    !_integrandset.empty() || !_integralset.empty()) {
	ccoutD(Caching) << base().GetName() <<
	    "Dropping cached semi-analytical integrals" << std::endl;
    }
    for (std::map<std::string, std::map<Int_t, RooAbsReal*> >::iterator
	    it = _integralset.begin(); _integralset.end() != it; ++it) {
	for (std::map<Int_t, RooAbsReal*>::iterator jt = it->second.begin();
		it->second.end() != jt; ++jt) {
	    if (jt->second) {
		delete _integratorset[it->first][jt->first];
		delete _integrandset[it->first][jt->first];
		delete _integralset[it->first][jt->first];
	    }
	}
    }
    _integvarset.clear();
    _integratorset.clear();
    _integrandset.clear();
    _integralset.clear();
}

template <class BASE>
void RooGeneralisedSmearingBase<BASE>::invalidateBinnedCache() const
{
    if (_binning || !_binnedcache.empty() || _binnedcacheLastVal.empty()) {
	ccoutD(Caching) << base().GetName() <<
	    " Dropping binned eval cache" << std::endl;
    }
    // drop caches related to binned evaluation of pdf
    _binnedcache.clear();
    _binnedcacheLastVal.clear();
    // we never owned _binnedvarClone, so we don't delete it
    _binnedvarClone = 0;
}

template <class BASE>
void RooGeneralisedSmearingBase<BASE>::invalidateIntegrandCache() const
{
    if (_init) {
	ccoutD(Caching) << base().GetName() <<
	    " Dropping cached integrand" << std::endl;
    }
    // invalidate integrand cache - drop anything we may own
    delete _integrator;
    delete _integrand;
    delete _clonePdf;
    delete _cloneModel;
    delete _cloneVar;

    _integrator = 0;
    _integrand = 0;
    _cloneModel = 0;
    _clonePdf = 0;
    _cloneVar = 0;
    _init = kFALSE; // next use needs to reinitialise
}

template <class BASE>
RooGeneralisedSmearingBase<BASE>::~RooGeneralisedSmearingBase()
{
    // drop all owned objects
    invalidateBinnedCache();
    invalidateAnaIntCache();
    invalidateIntegrandCache();
    delete[] _binning;
}

template <class BASE>
void RooGeneralisedSmearingBase<BASE>::setBinnedCache(
	RooRealVar* var, const char* binningName,
	const RooArgSet* binnedcats,
	bool preferBinnedIntegrals)
{
    // with new binning, old cache is invalid
    invalidateBinnedCache();
    assert((var && binningName) || (!var && !binningName));
    if (_binning) delete[] _binning;
    _binning = new char[1 + std::strlen(binningName)];
    std::strcpy(_binning, binningName);
    _preferBinnedIntegrals = preferBinnedIntegrals;
    if (var) {
	assert(var->hasBinning(_binning));
	coutI(Caching) << base().GetName() <<
	    " Using binned evaluation '" << _binning << "'" <<
	    std::endl;
	_binnedvar = RooRealProxy("_binnedvar", "_binnedvar", this, *var);
    } else {
	coutI(Caching) << base().GetName() <<
	    " Using normal evaluation" << std::endl;
	_binnedvar = RooRealProxy();
    }
    _binnedvarClone = 0;
    _binnedcats.removeAll();
    if (binnedcats) _binnedcats.add(*binnedcats);
}

template <class BASE>
unsigned RooGeneralisedSmearingBase<BASE>::getIndex() const
{
    if (0 == _binnedcats.getSize()) return 0;
    unsigned cat = 0;
    TIterator* it = _binnedcats.createIterator();
    while (TObject* obj = it->Next()) {
	const RooAbsCategory* catvar = dynamic_cast<RooAbsCategory*>(obj);
	if (!catvar) continue;
	cat *= catvar->numTypes();
	const int cattype = catvar->getIndex();
	int k = 0;
	TIterator *jt = catvar->typeIterator();
	while (RooCatType* ty = reinterpret_cast<RooCatType*>(jt->Next())) {
	    if (cattype == ty->getVal()) break;
	    ++k;
	}
	delete jt;
	cat += k;
    }
    delete it;

    return cat;
}

template <class BASE>
unsigned RooGeneralisedSmearingBase<BASE>::getMaxIndex() const
{
    if (0 == _binnedcats.getSize()) return 1;
    unsigned cat = 1;
    TIterator* it = _binnedcats.createIterator();
    while (TObject* obj = it->Next()) {
	const RooAbsCategory* catvar = dynamic_cast<RooAbsCategory*>(obj);
	if (!catvar) continue;
	cat *= catvar->numTypes();
    }
    delete it;

    return cat;
}

template <class BASE>
Double_t RooGeneralisedSmearingBase<BASE>::doEvaluate() const
{
    const bool inhibit = base().inhibitDirty();
    base().setDirtyInhibit(true);
    // Calculate smearing integral
    // Propagate current normalization set to integrand
    _integrand->setNormalizationSet(_origVar.nset());

    // Adjust smearing integration window
    if (_useWindow) {
	Double_t center = ((RooAbsReal*)_windowParam.at(0))->getVal();
	Double_t width = _windowScale *
	    ((RooAbsReal*)_windowParam.at(1))->getVal();
	_integrator->setLimits(center - width, center + width);
    } else {
	_integrator->setLimits(-RooNumber::infinity(), RooNumber::infinity());
    }

    // Calculate smearing
    const double retVal = _integrator->integral();
    base().setDirtyInhibit(inhibit);
    return retVal;
}

template <class BASE>
void RooGeneralisedSmearingBase<BASE>::updateBinnedvarClone() const
{
    // binning variable: dig out the variable that is really used
    RooRealVar* newvar = _binnedvarClone;
    if (!newvar) {
	const RooAbsReal* deps[2] = { _clonePdf, _cloneModel };
	for (unsigned i = 0; i < 2; ++i) {
	    RooArgSet* s = deps[i]->getVariables();
	    TObject* obj = s->find(_binnedvar.arg());
	    if (obj) newvar = dynamic_cast<RooRealVar*>(obj);
	    delete s;
	    if (newvar) break;
	}
	if (!newvar) newvar = dynamic_cast<RooRealVar*>(
		const_cast<RooAbsReal*>(&_binnedvar.arg()));
	_binnedvarClone = newvar;
    }
}

template <class BASE>
void RooGeneralisedSmearingBase<BASE>::ensureBinnedCacheNotInvalid() const
{
    // check if we're dirty and need to invalidate the cache
    //
    // that'll happen if any input variable of either pdf or model has changed
    // from its previous value apart from the variable in which we cache and
    // the integration variable
    const unsigned idx = getIndex();
    if (_binnedcache.empty()) {
	const unsigned idxmax = getMaxIndex();
	_binnedcache.resize(idxmax, std::vector<double>());
	_binnedcacheLastVal.resize(idxmax, std::map<void*, double>());
    }
    bool isDirty = false;
    const RooAbsReal* deps[2] = { &clonePdf(), &cloneModel() };
    const RooAbsArg* whitelist[2] = { _binnedvarClone, &cloneVar() };
    for (unsigned i = 0; i < 2; ++i) {
	RooArgSet *s = deps[i]->getVariables();
	TIterator *it = s->createIterator();
	RooAbsArg* obj = 0;
	while ((obj = reinterpret_cast<RooAbsArg*>(it->Next()))) {
	    // variables in which we cache may change - that's what the
	    // cache is for
	    if (obj == whitelist[0] || obj == whitelist[1] ||
		    _binnedcats.find(*obj)) continue;
	    // if we don't know about obj, we have to force isDirty to true
	    const bool newobj = 
		(_binnedcacheLastVal[idx].end() ==
		 _binnedcacheLastVal[idx].find(obj));
	    // check if value of obj has changed
	    RooAbsReal* realvar = dynamic_cast<RooAbsReal*>(obj);
	    double& oldval = _binnedcacheLastVal[idx][obj];
	    if (realvar) {
		const double newval = realvar->getVal();
		if (oldval != newval || newobj) {
		    if (!isDirty && !newobj) {
			ccoutD(Caching) << base().GetName() <<
			    " invalidating cache because of " <<
			    realvar->GetName() << " " << oldval <<
			    " -> " << newval << std::endl;
		    }
		    oldval = newval;
		    isDirty = true;
		}
	    } else {
		RooAbsCategory* catvar = dynamic_cast<RooAbsCategory*>(obj);
		if (catvar) {
		    double newval = catvar->getIndex();
		    if (oldval != newval || newobj) {
			if (!isDirty && !newobj) {
			    ccoutD(Caching) << base().GetName() <<
				" invalidating cache because of " <<
				catvar->GetName() << " " << oldval <<
				" -> " << newval << std::endl;
			}
			oldval = newval;
			isDirty = true;
		    }
		}
	    }
	}
	delete it;
	delete s;
    }
    if (isDirty && !_binnedcache[idx].empty()) {
	// invalidate cached values
	_binnedcache[idx].clear();
	ccoutD(Caching) << base().GetName() <<
	    " Invalidating binned eval cache values" << std::endl;
    }
}

template <class BASE>
double RooGeneralisedSmearingBase<BASE>::getBinnedCacheValue(unsigned bin) const
{
    if (_binnedcache.empty()) {
	const unsigned idxmax = getMaxIndex();
	_binnedcache.resize(idxmax, std::vector<double>());
	_binnedcacheLastVal.resize(idxmax, std::map<void*, double>());
    }
    const unsigned idx = getIndex();
    const RooAbsBinning& binning =
	reinterpret_cast<const RooRealVar&>(_binnedvar.arg()).getBinning(_binning);
    if (_binnedcache[idx].empty())
	_binnedcache[idx].resize(binning.numBins() + 1, -1.);
    if (-0. > _binnedcache[idx][bin]) {
	// call the (private) inhibitDirty method through the ROOT runtine
	TMethodCall mycall(_binnedvarClone->Class(), "inhibitDirty", "");
	long inhibit = 0;
	mycall.Execute(_binnedvarClone, inhibit);
	// inhibit setting/propagating of dirty flags while we play
	// with _binnedvarClone
	_binnedvarClone->setDirtyInhibit(true);
	const double xold = _binnedvarClone->getVal();
	const double x = (bin == unsigned(binning.numBins())) ?
	    binning.binHigh(bin - 1) : binning.binLow(bin);
	_binnedvarClone->setVal(x);
	_binnedcache[idx][bin] = doEvaluate();
	_binnedvarClone->setVal(xold);
	_binnedvarClone->setDirtyInhibit(inhibit);
    }
    return _binnedcache[idx][bin];
}

template <class BASE>
Double_t RooGeneralisedSmearingBase<BASE>::evaluate() const
{
    // Check if deferred initialization has occurred
    if (!_init) initialize();
    // are we supposed to use a binned cache in a variable?
    if (!_binning) {
	// no, straightforward evaluation
	return doEvaluate();
    }
    // binned evaluation
    updateBinnedvarClone();
    ensureBinnedCacheNotInvalid();
    // work out in which bin we are
    const double x = _binnedvarClone->getVal();
    const RooAbsBinning& binning = reinterpret_cast<const RooRealVar&>(
		_binnedvar.arg()).getBinning(_binning);
    const unsigned bin = binning.binNumber(x);
    const double xlo = binning.binLow(bin);
    const double xhi = binning.binHigh(bin);
    // linear interpolation (extrapolation if out of range of binning)
    const double retVal =  ((x - xlo) * getBinnedCacheValue(bin + 1) +
	    (xhi - x) * getBinnedCacheValue(bin)) / (xhi - xlo);
    return std::max(retVal, 0.);
}

template <class BASE>
Bool_t RooGeneralisedSmearingBase<BASE>::redirectServersHook(
	const RooAbsCollection& /*newServerList*/, Bool_t /*mustReplaceAll*/,
	Bool_t /*nameChange*/, Bool_t /*isRecursive*/)
{
    // Intercept server redirects. Throw away cache, as figuring out
    // redirections on the cache is an unsolvable problem.
    if (_init) {
	ccoutD(Caching) << base().GetName() <<
	    " Dropping all caches in " << __func__ << std::endl;
    }
    _init = kFALSE;
    return kFALSE;
}

template <class BASE>
void RooGeneralisedSmearingBase<BASE>::clearConvolutionWindow()
{
    // Removes previously defined smearing window,
    // reverting to smearing from -inf to +inf
    invalidateBinnedCache();
    _useWindow = kFALSE;
    _windowParam.removeAll();
}

template <class BASE>
void RooGeneralisedSmearingBase<BASE>::setConvolutionWindow(RooAbsReal& centerParam,
	RooAbsReal& widthParam, Double_t widthScaleFactor)
{
    // Restrict smearing integral to finite range [ x - C - S*W, x - C + S*W ]
    // where x is current value of smearing variablem,
    // C = centerParam, W=widthParam and S = widthScaleFactor
    // Inputs centerParam and withParam can be function expressions
    // (RooAbsReal, RooFormulaVar) etc.
    invalidateBinnedCache();
    _useWindow = kTRUE;
    _windowParam.removeAll();
    _windowParam.add(centerParam);
    _windowParam.add(widthParam);
    _windowScale = widthScaleFactor;
}

template <class BASE>
void RooGeneralisedSmearingBase<BASE>::printCompactTreeHook(
	ostream& os, const char* indent)
{
    // Hook function to intercept printCompactTree() calls so that it can print
    // out the content of its private cache in the print sequence
    os << indent << "RooGeneralisedSmearingBase begin cache" << endl;
    if (_init) {
	_cloneVar->printCompactTree(os, Form("%s[Var]",indent));
	_clonePdf->printCompactTree(os, Form("%s[Pdf]",indent));
	_cloneModel->printCompactTree(os, Form("%s[Mod]",indent));
    }
    os << indent << "RooGeneralisedSmearingBase end cache" << endl;
}

template <class BASE>
Int_t RooGeneralisedSmearingBase<BASE>::getAnalyticalIntegral(
	RooArgSet& allVars, RooArgSet& integVars,
	const char* range) const
{
    // Check if deferred initialization has occurred
    if (!_init) initialize();
    RooArgSet candVars;
    TIterator *it = allVars.createIterator();
    RooAbsArg* obj = 0;
    while (0 != (obj = dynamic_cast<RooAbsArg*>(it->Next()))) {
	// 1. remove from allVars stuff which has overlap with k
	//    (we do the k integral anyhow)
	if (obj->overlaps(*_origVar.absArg())) continue;
	// 2. remove from allVars stuff which has overlap with model
	if (_origModel.absArg()->overlaps(*obj)) continue;
	candVars.add(*obj);
    }
    delete it;
    // 3. see if pdf can integrate what is left analytically
    Int_t code = pdf().getAnalyticalIntegralWN(
	    candVars, integVars, 0, range);
    // shift valid codes by one upwards
    if (code) ++code;
    // if we cannot do it analytically, but are using binned evaluation,
    // we can write down an analytical form for the binned approximation of the
    // integral, so use that
    if ((!code || _preferBinnedIntegrals) && _binning &&
	    candVars.contains(_binnedvar.arg())) {
	integVars.removeAll();
	integVars.add(_binnedvar.arg());
	code = 1;
    }
    if (code > 1) {
	// we need to keep track of the integration sets for use in
	// analyticalIntegral below
	const std::string rn((0 == range) ? "" : range);
	if (_integratorset[rn].end() != _integratorset[rn].find(code)) {
	    delete _integratorset[rn][code];
	    delete _integrandset[rn][code];
	    delete _integralset[rn][code];
	}
	_integratorset[rn][code] = 0;
	_integrandset[rn][code] = 0;
	_integralset[rn][code] = 0;
	// save set of variables to integrator over
	_integvarset[rn][code].removeAll();
	_integvarset[rn][code].add(integVars);
    }

    return code;
}

template <class BASE>
Double_t RooGeneralisedSmearingBase<BASE>::analyticalIntegral(
	Int_t code, const char* range) const
{
    if (!_init) initialize();
    const std::string rn((0 == range) ? "" : range);
    if (1 == code && _binning) {
	// perform "analytical" integration based on binned evaluation
	//
	// this does obviously not return the "correct" integral of the true
	// smearing, but the result is consistent with the approximation made
	// in binned evaluation
	
	// make sure we're in a valid state
	updateBinnedvarClone();
	ensureBinnedCacheNotInvalid();
	// loop over bins
	const RooRealVar* var =
	    dynamic_cast<const RooRealVar*>(&_binnedvar.arg());
	const RooAbsBinning& binning = var->getBinning(_binning);
	const double rmin = var->getMin(range);
	const double rmax = var->getMax(range);
	double sum = 0.; // accumulates integral
	const unsigned nBins = binning.numBins();
	for (unsigned bin = 0; bin < nBins; ++bin) {
	    // does bin contribute to sum?
	    const double xhi = binning.binHigh(bin);
	    if (xhi < rmin && bin != nBins - 1) continue;
	    const double xlo = binning.binLow(bin);
	    if (xlo > rmax && bin) break;
	    // yes, work out integration range
	    double a = std::max(xlo, rmin);
	    if (0 == bin) a = rmin;
	    double b = std::min(xhi, rmax);
	    if (nBins - 1 == bin) b = rmax;

	    // integrate analytically over bin
	    const double flo = getBinnedCacheValue(bin);
	    const double fhi = getBinnedCacheValue(bin + 1);
	    const double dx = xhi - xlo;
	    const double df = fhi - flo;
	    sum += (0.5 * df * (b + a) + fhi * xlo + flo * xhi) * (b - a) / dx;
	}
	return sum;
    }
    // verify we know about the code and range name
    assert(_integratorset.end() != _integratorset.find(rn));
    assert(_integratorset[rn].end() != _integratorset[rn].find(code));
    const bool inhibit = base().inhibitDirty();
    base().setDirtyInhibit(true);
    // get integral over pdf
    RooAbsIntegrator* integrator = _integratorset[rn][code];
    if (!integrator) {
	// integral over pdf, integrand, integrator over k not created yet, do
	// so now (on the fly)
	_integralset[rn][code] =
	    _clonePdf->createIntegral(_integvarset[rn][code], range);
	_integrandset[rn][code] = new RooGenSmearIntegrandBinding(
		*_integralset[rn][code], *_cloneModel, *_cloneVar, 0);
	_integratorset[rn][code] = integrator =
	    RooNumIntFactory::instance().createIntegrator(
		    *_integrandset[rn][code], _convIntConfig, 1);
    }

    integrator->setUseIntegrandLimits(kFALSE);
    // Adjust smearing integration window
    if (_useWindow) {
	Double_t center = ((RooAbsReal*)_windowParam.at(0))->getVal();
	Double_t width = _windowScale *
	    ((RooAbsReal*)_windowParam.at(1))->getVal();
	integrator->setLimits(center - width, center + width);
    } else {
	integrator->setLimits(-RooNumber::infinity(), RooNumber::infinity());
    }

    const double retVal = integrator->integral();
    base().setDirtyInhibit(inhibit);
    return retVal;
}

template<class BASE>
Int_t RooGeneralisedSmearingBase<BASE>::getMaxVal(const RooArgSet& vars) const
{
    const int modelcode = model().getMaxVal(vars);
    const int pdfcode = pdf().getMaxVal(vars);
    // make sure it's smaller than 2^15, or the sign bit in the combination
    // might be turned on
    assert(0 <= modelcode && modelcode < (1 << 15));
    assert(0 <= pdfcode && pdfcode < (1 << 15));
    if (!pdfcode || !modelcode) return 0;
    return (modelcode << 16) | pdfcode;
}

template<class BASE>
Double_t RooGeneralisedSmearingBase<BASE>::getMax(Int_t code) const
{
    const int pdfcode = code & 65536;
    const int modelcode = code >> 16;
    return model().maxVal(modelcode) * pdf().maxVal(pdfcode);
}

// force template instantiation
#ifndef __GCCXML__
template class RooGeneralisedSmearingBase<RooAbsReal>;
template class RooGeneralisedSmearingBase<RooAbsPdf>;
#endif
