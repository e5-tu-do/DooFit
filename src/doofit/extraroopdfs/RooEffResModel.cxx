/*****************************************************************************
 * Project: RooFit                                                           *
 * Package: RooFitModels                                                     *
 * @(#)root/roofit:$Id: RooEffResModel.cxx 44982 2012-07-10 08:36:13Z moneta $
 * Authors:                                                                  *
 *                                                                           *
 * Redistribution and use in source and binary forms,                        *
 * with or without modification, are permitted according to the terms        *
 * listed in LICENSE (http://roofit.sourceforge.net/license.txt)             *
 *
 * 2012-08-24 imported from G. Raven's p2vv git repository - M. Schiller     *
 * 2012-09-08 re-import from upstream, with some substantial modifications   *
 *	to ensure that the code does not leak memory (Manuel Schiller)       *
 * 2012-10-04 re-import from upstream, with some experimental changes, which *
 *	which are disabled by default for now...                             *
 *****************************************************************************/

//////////////////////////////////////////////////////////////////////////////
//
// BEGIN_HTML
// Class RooEffResModel implements a RooResolutionModel that models a EffResian
// distribution. Object of class RooEffResModel can be used
// for analytical convolutions with classes inheriting from RooAbsAnaConvPdf
// END_HTML
//

#include <string>
#include <memory>
#include <cstdio>
#include <limits>
#include <cstring>
#include <cassert>
#include <cmath>

#include <RooFit.h>
#include <Riostream.h>
#include <RooRealConstant.h>
#include <RooCustomizer.h>
#include <RooAddition.h>
#include <RooProduct.h>
#include <RooStringVar.h>
#include <RooAbsAnaConvPdf.h>
#include "RooEffConvGenContext.h"
#include "RooEffResModel.h"

namespace doofit {
namespace extraroopdfs {

//_____________________________________________________________________________
RooEffResModel::CacheElem::~CacheElem()
{
    delete _eff;
    delete _int; // _int owns _x, _xmin, _xmax, so no need to free them
}

//_____________________________________________________________________________
RooArgList RooEffResModel::CacheElem::containedArgs(Action)
{
    // Return list of all RooAbsArgs in cache element
    return RooArgList(*_int, *_eff);
}

//_____________________________________________________________________________

RooEffResModel::CacheElem::CacheElem(const RooEffResModel& parent, const RooArgSet& iset,
	const TNamed* rangeName) :
    _x(0), _eff(0), _xmin(0), _xmax(0), _int(0),
    _val(std::numeric_limits<Double_t>::quiet_NaN())
{
    RooRealVar& x = parent.convVar(); // binboundaries not const...
    RooAbsReal& eff = *parent.efficiency();
    RooAbsReal& model = parent.model();
    // the subset of iset on which the efficiency depends
    std::auto_ptr<const RooArgSet> effInt( eff.getObservables(iset) );

    assert(effInt->getSize() < 2); // for now, we only do 1D efficiency histograms...
    if (effInt->getSize() == 0) {
	_int = parent.model().createIntegral(iset, RooNameReg::str(rangeName));
	return;
    }

    const char* rn = (rangeName ? RooNameReg::str(rangeName) : "default");
    // get bin bounds
    const double rxmin = x.getMin(rn);
    const double rxmax = x.getMax(rn);

    std::auto_ptr<std::list<Double_t> > bounds(
	    eff.binBoundaries(x, x.getMin(), x.getMax()));
    assert(bounds->size() > 1);
    _bounds.reserve(bounds->size());
    for (std::list<Double_t>::const_iterator
	    lo = bounds->begin(), hi = ++(bounds->begin());
	    hi != bounds->end(); ++hi, ++lo) {
	if (*hi < rxmin) continue; // not there yet...
	if (*lo > rxmax) break;    // past the requested interval...
	const double xmin = std::max(*lo, rxmin);
	const double xmax = std::min(*hi, rxmax);
	if (_bounds.empty())
	    _bounds.push_back(xmin);
	_bounds.push_back(xmax);
    }
    assert(_bounds.size() > 1);
    // build integral ingredients
    _x = dynamic_cast<RooRealVar*>(x.clone((std::string(x.GetName()) + "_" +
		    rn).c_str()));
    assert(_x);
    _x->setConstant(true);
    RooCustomizer customizer2(model, (std::string(rn) + "_customizer2").c_str());
    customizer2.replaceArg(x, *_x);
    RooAbsReal* m = dynamic_cast<RooAbsReal*>(customizer2.build(false));
    assert(m);
    // build "customised" version of iset
    RooArgSet custiset(iset);
    custiset.replace(x, *_x);
    // working range
    _xmin = new RooRealVar("_xmin", "_xmin", _bounds.front());
    assert(_xmin);
    _xmin->setConstant(true);
    _xmax = new RooRealVar("_xmax", "_xmax", _bounds.back());
    assert(_xmax);
    _xmax->setConstant(true);
    std::string wrn(parent.GetName());
    wrn += "_"; wrn += x.GetName(); wrn += "_"; wrn += model.GetName();
    wrn += "_"; wrn += eff.GetName(); wrn += "_"; wrn += rn;
    wrn += "_workRange";
    assert(!_x->hasRange(wrn.c_str()));
    _x->setRange(wrn.c_str(), *_xmin, *_xmax);
    // build integral
    _int = m->createIntegral(custiset, wrn.c_str());
    assert(_int);
    _int->addOwnedComponents(*m);
    _int->addOwnedComponents(*_x);
    _int->addOwnedComponents(*_xmin);
    _int->addOwnedComponents(*_xmax);
    // build efficiency in bin middle
    RooCustomizer customizer(eff, (std::string(rn) + "_customizer").c_str());
    customizer.replaceArg(x, *_x);
    _eff = static_cast<RooAbsReal*>(customizer.build(false));
    assert(_eff);
}

Double_t RooEffResModel::CacheElem::getVal(const RooArgSet* nset) const
{
    if (0 == _x) {
	// handle trivial case
	return _int->getVal(nset);
    }
    // see if our cached value needs recalculating
    if (_val != _val || _eff->isValueOrShapeDirtyAndClear() ||
	    _int->isValueOrShapeDirtyAndClear()) {
	// yes, so iterate over subranges, and sum up integral contributions
	_val = 0.;
	for (unsigned i = 1; i < _bounds.size(); ++i) {
	    _xmin->setVal(_bounds[i - 1]);
	    _xmax->setVal(_bounds[i]);
	    _x->setVal(0.5 * (_bounds[i - 1] + _bounds[i]));
	    _val += _int->getVal() * _eff->getVal();
	}
    }

    return _val;
}

//_____________________________________________________________________________
RooEffResModel::RooEffResModel(const char *name, const char *title,
	RooResolutionModel& model, RooAbsReal& eff) :
    RooAbsEffResModel(name,title,model.convVar())
    , _observables("observables", "observables", this)
    , _model("!model","Original resolution model",this,model)
    , _eff("!efficiency","efficiency of convvar", this,eff)
    , _cacheMgr(this, 10)
{
    // FIXME: assert that efficiency is a function of convVar, and there are no overlaps...
    _observables.add(model.convVar());
}

//_____________________________________________________________________________
RooEffResModel::RooEffResModel(const RooEffResModel& other, const char* name) :
    RooAbsEffResModel(other,name)
    , _observables("observables", this, other._observables)
    , _model("!model", this, other._model)
    , _eff("!efficiency", this, other._eff)
    , _cacheMgr(other._cacheMgr, this)
{
    // copy constructor
}

RooEffResModel* RooEffResModel::clone(const char* newname) const
{ return new RooEffResModel(*this,newname); }

//_____________________________________________________________________________
RooEffResModel::~RooEffResModel()
{
    // Destructor
}

RooAbsReal* RooEffResModel::efficiency() const
{ return static_cast<RooAbsReal*>(_eff.absArg()); }

std::vector<RooAbsReal*> RooEffResModel::efficiencies() const
{ return std::vector<RooAbsReal*>(1, efficiency()); }

RooResolutionModel& RooEffResModel::model() const
{ return dynamic_cast<RooResolutionModel&>(*_model.absArg()); }

//_____________________________________________________________________________
RooEffResModel* RooEffResModel::convolution(
	RooFormulaVar* inBasis, RooAbsArg* owner) const
{
    // Instantiate a clone of this resolution model representing a convolution with given
    // basis function. The owners object name is incorporated in the clones name
    // to avoid multiple convolution objects with the same name in complex PDF structures.
    //
    // Note: The 'inBasis' formula expression must be a RooFormulaVar that encodes the formula
    // in the title of the object and this expression must be an exact match against the
    // implemented basis function strings (see derived class implementation of method basisCode()
    // for those strings

    // Check that primary variable of basis functions is our convolution variable
    if (inBasis->getParameter(0) != x.absArg()) {
	coutE(InputArguments) << "RooEffResModel::convolution(" << GetName() << "," << this
	    << ") convolution parameter of basis function and PDF don't match" << endl
	    << "basis->findServer(0) = " << inBasis->findServer(0) << endl
	    << "x.absArg()           = " << x.absArg() << endl;
	return 0;
    }

    if (basisCode(inBasis->GetTitle())==0) {
	coutE(InputArguments) << "RooEffResModel::convolution(" << GetName() << "," << this
	    << ") basis function '" << inBasis->GetTitle() << "' is not supported." << endl;
	return 0;
    }

    std::string newName(GetName());
    newName += "_conv_";
    newName += inBasis->GetName();
    newName += "_[";
    newName += owner->GetName();
    newName += "]";

    RooResolutionModel *conv = model().convolution(inBasis, owner);

    std::string newTitle(conv->GetTitle());
    newTitle += " convoluted with basis function ";
    newTitle += inBasis->GetName();
    conv->SetTitle(newTitle.c_str());

    RooAbsReal* eff = efficiency();
    RooEffResModel *effConv = new RooEffResModel(newName.c_str(), newTitle.c_str(), *conv, *eff);
    effConv->addOwnedComponents(*conv);
    effConv->changeBasis(inBasis);

    const char* cacheParamsStr = getStringAttribute("CACHEPARAMINT");
    if (cacheParamsStr && strlen(cacheParamsStr)) {
	//cout << "prior has CACHEPARAMINT : " << cacheParamsStr << endl;
	//const char* ecCacheParamsStr = effConv->getStringAttribute("CACHEPARAMINT");
	//if (ecCacheParamsStr && strlen(ecCacheParamsStr)) cout << "bound version has CACHEPARAMINT : " << ecCacheParamsStr << endl;
	effConv->setStringAttribute("CACHEPARAMINT",cacheParamsStr);
	//cout << "2nd time: bound version has CACHEPARAMINT : " << effConv->getStringAttribute("CACHEPARAMINT")  << endl;
	//cout << endl << endl << endl;
    }

    return effConv;
}

//_____________________________________________________________________________
Int_t RooEffResModel::basisCode(const char* name) const
{
    return model().basisCode(name);
}

//_____________________________________________________________________________
Double_t RooEffResModel::evaluate() const
{
    Double_t mod = model().getVal();
    // TODO: replace this by the discretized version, i.e. replace convVar by
    // customized middle of bin...  this in order to ensure evaluate &
    // analyticalIntegral are consistent (in case eff is not discretized!!!)
    Double_t eps = efficiency()->getVal();
    return eps * mod;
}

//_____________________________________________________________________________
Bool_t RooEffResModel::forceAnalyticalInt(const RooAbsArg& /*dep*/) const
{
    // Force RooRealIntegral to offer all observables for internal integration
    return kTRUE;
}

//_____________________________________________________________________________
Int_t RooEffResModel::getAnalyticalIntegral(
	RooArgSet& allVars, RooArgSet& analVars, const char* rangeName) const
{
    if (_forceNumInt) return 0;
    analVars.add(allVars);
    getCache(&analVars, RooNameReg::ptr(rangeName));
    return 1 + _cacheMgr.lastIndex();
}

//_____________________________________________________________________________
Double_t RooEffResModel::analyticalIntegral(
	Int_t code, const char* rangeName) const
{
    assert(code > 0);
    CacheElem* cache = static_cast<CacheElem*>(_cacheMgr.getObjByIndex(code - 1));
    if (!cache) {
	std::auto_ptr<RooArgSet> vars(getParameters(RooArgSet()));
	std::auto_ptr<RooArgSet> iset(
		_cacheMgr.nameSet1ByIndex(code - 1)->select(*vars));
	cache = getCache(iset.get(), RooNameReg::ptr(rangeName));
	assert(cache!=0);
    }
    const Double_t val = cache->getVal();
    return val;
}

//_____________________________________________________________________________
RooAbsGenContext* RooEffResModel::modelGenContext(
	const RooAbsAnaConvPdf& convPdf, const RooArgSet &vars,
	const RooDataSet *prototype, const RooArgSet* auxProto,
	Bool_t verbose) const
{
    return new RooEffConvGenContext(convPdf, vars, prototype, auxProto, verbose);
}

//_____________________________________________________________________________
Int_t RooEffResModel::getGenerator(
	const RooArgSet& directVars, RooArgSet &generateVars,
	Bool_t staticInitOK) const
{
    return model().getGenerator(directVars, generateVars, staticInitOK);
}

//_____________________________________________________________________________
void RooEffResModel::initGenerator(Int_t code)
{
    model().initGenerator(code);
}

//_____________________________________________________________________________
void RooEffResModel::generateEvent(Int_t code)
{
    // The hit-miss on the efficiency is done at the level of the GenContext.
    model().generateEvent(code);
}

//_____________________________________________________________________________
RooEffResModel::CacheElem* RooEffResModel::getCache(
	const RooArgSet *iset, const TNamed *rangeName) const
{
    Int_t sterileIndex(-1);
    CacheElem* cache = (CacheElem*) _cacheMgr.getObj(iset, &sterileIndex, rangeName);
    if (cache) return cache;
    _cacheMgr.setObj(iset, new CacheElem( *this,  *iset,  rangeName), rangeName);
    return getCache(iset, rangeName );
}

}
}