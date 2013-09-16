/*****************************************************************************
 * Project: RooFit                                                           *
 * Package: RooFitCore                                                       *
 *    File: $Id: RooGeneralisedSmearingBase.h,v 1.4 2007/05/11 09:11:30 verkerke Exp $
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
#ifndef ROO_GENERALISED_SMEARING_BASE
#define ROO_GENERALISED_SMEARING_BASE

#include <map>
#include <vector>
#include <string>

#include <RooAbsPdf.h>
#include <RooRealProxy.h>
#include <RooSetProxy.h>
#include <RooListProxy.h>
#include <RooNumIntConfig.h>

class RooAbsIntegrator;
class TH2;
class RooGenSmearIntegrandBinding;

template<class BASE> class RooGeneralisedSmearingBase : public BASE
{
    public:

	RooGeneralisedSmearingBase();

	RooGeneralisedSmearingBase(const char* name, const char* title,
		RooRealVar& k, RooAbsReal& pdf, RooAbsReal& resmodel);

	RooGeneralisedSmearingBase(
		const RooGeneralisedSmearingBase<BASE>& other,
		const char* name=0);
	RooGeneralisedSmearingBase& operator=(
		const RooGeneralisedSmearingBase<BASE>& other);

	virtual TObject* clone(const char* newname) const
	{ return new RooGeneralisedSmearingBase(*this,newname); }
	
	virtual ~RooGeneralisedSmearingBase();

	// activate evaluation using 1D binning in variable var, optionally
	// the binned cache is kept for each of the possible values of the
	// categories given in binnedcats
	//
	// if perferBinnedIntegrals is true, integrals over the variable var
	// will be evaluated using the cache
	void setBinnedCache(
		RooRealVar* var = 0,
		const char* binningName = 0,
		const RooArgSet* binnedcats = 0,
		bool preferBinnedIntegrals = false);

	virtual Double_t evaluate() const;

	RooNumIntConfig& convIntConfig()
	{ _init = kFALSE; return _convIntConfig; }
	
	const RooNumIntConfig& convIntConfig() const
	{ _init = kFALSE; return _convIntConfig; }

	void clearConvolutionWindow();
	void setConvolutionWindow(RooAbsReal& centerParam,
		RooAbsReal& widthParam, Double_t widthScaleFactor=1);

	// "analytical integral" - not really, but if the pdf can be integrated
	// analytically and that integral commutes with the integration over
	// the smearing variable, take advantage of the analytical integration
	// of the pdf
	virtual Int_t getAnalyticalIntegral(
		RooArgSet& allVars, RooArgSet& integVars,
		const char* rangeName = 0) const;

	// this routine does the actual work of performing the "analytical"
	// integral
	virtual Double_t analyticalIntegral(
		Int_t code, const char* rangeName = 0) const;
	
	// tell the RooFit framework that we most likely know better about
	// which analytical integrals we can perform than it does
	virtual Bool_t forceAnalyticalInt(const RooAbsArg& /*dep*/) const
	{ return kTRUE; }

	virtual Int_t getMaxVal(const RooArgSet& vars) const;
	virtual Double_t getMax(Int_t code) const;

    protected:

	void initialize() const;
	Bool_t redirectServersHook(const RooAbsCollection& newServerList,
		Bool_t mustReplaceAll, Bool_t nameChange, Bool_t isRecursive);
	virtual void printCompactTreeHook(ostream& os, const char* indent="");
	RooRealVar& cloneVar() const
	{ if (!_init) initialize(); return (RooRealVar&) *_cloneVar; }
	RooAbsReal& clonePdf() const
	{ if (!_init) initialize(); return (RooAbsReal&)  *_clonePdf; }
	RooAbsReal& cloneModel() const
	{ if (!_init) initialize(); return (RooAbsReal&)  *_cloneModel; }
	Double_t doEvaluate() const;
	void invalidateAnaIntCache() const;
	void invalidateBinnedCache() const;
	void invalidateIntegrandCache() const;
	// Access components
	const RooRealVar&  var() const { return (RooRealVar&) _origVar.arg(); }
        const RooAbsReal&  pdf() const { return (RooAbsReal&) _origPdf.arg(); }
	const RooAbsReal&  model() const { return (RooAbsReal&) _origModel.arg(); }

	void updateBinnedvarClone() const;
	void ensureBinnedCacheNotInvalid() const;
	double getBinnedCacheValue(unsigned bin) const;

	unsigned getIndex() const;
	unsigned getMaxIndex() const;

	const BASE& base() const
	{ return *reinterpret_cast<const BASE*>(this); }
	BASE& base()
	{ return *reinterpret_cast<BASE*>(this); }

	mutable Bool_t _init;

	RooNumIntConfig _convIntConfig; // Configuration of numeric smearing integral;
	mutable RooAbsIntegrator* _integrator;  //! Numeric integrator of smearing integrand
	mutable RooGenSmearIntegrandBinding* _integrand; //! binding of smearing integrand

	RooRealProxy _origVar;   // Original smearing variable
	RooRealProxy _origPdf;   // Original input PDF
	RooRealProxy _origModel; // Original resolution model

	// keep working with our own copies of model, pdf, smearing variable
	mutable RooAbsReal*  _cloneVar;   // Pointer to cloned smearing variable
	mutable RooAbsReal*  _clonePdf;   // Pointer to cloned PDF
	mutable RooAbsReal*  _cloneModel; // Pointer to cloned model

	// keep cache of ingredients for analytical integrals
	mutable std::map<std::string, std::map<Int_t, RooArgSet> >
	    _integvarset; //! cache mapping range name, code to variable set
	mutable std::map<std::string, std::map<Int_t, RooAbsReal*> >
	    _integralset; //! cache mapping range name, code to integral over pdf
	mutable std::map<std::string, std::map<Int_t, RooGenSmearIntegrandBinding*> >
	    _integrandset; //! cache mapping range name, code to integrand
	mutable std::map<std::string, std::map<Int_t, RooAbsIntegrator*> >
	    _integratorset; //! cache mapping range name, code to integrator
	char* _binning; // binning (name) to use
	RooRealProxy _binnedvar; // variable in which to bin
	mutable RooRealVar* _binnedvarClone; //!
	RooArgSet _binnedcats; // categories to use when binning
	mutable std::vector<std::vector<double> > _binnedcache; //! values at bin edges
	mutable std::vector<std::map<void*, double> > _binnedcacheLastVal; //! last values
	bool _preferBinnedIntegrals;

	Bool_t       _useWindow  ;     // Switch to activate window smearing
	Double_t     _windowScale;     // Scale factor for window parameter
	RooListProxy _windowParam;     // Holder for optional smearing integration window scaling parameter

	ClassDef(RooGeneralisedSmearingBase,2)   // Operator PDF implementing numeric smearing of 2 input functions
};

// help genreflex
#ifdef __GCCXML__
template class RooGeneralisedSmearingBase<RooAbsReal>;
template class RooGeneralisedSmearingBase<RooAbsPdf>;
#endif

typedef RooGeneralisedSmearingBase<RooAbsReal> RooNumGenSmear;
typedef RooGeneralisedSmearingBase<RooAbsPdf> RooNumGenSmearPdf;

#endif
