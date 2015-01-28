/**
 * @file DecRateCoeff.cxx
 *
 * cosh/sinh/cos/sin coefficients in decay rate equations
 *
 * @author Manuel Schiller <manuel.schiller@nikhef.nl>
 * @date 2012-10-24
 */

#include <cmath>
#include <limits>
#include <memory>
#include <cstdio>
#include <sstream>
#include <cassert>
#include <algorithm>
 
#include <RooRealVar.h>
#include <RooProduct.h>
#include <RooAddition.h>
#include <RooCategory.h>
#include <RooCustomizer.h>

#include "DecRateCoeff.h"

RooArgSet DecRateCoeff::s_emptyset;
RooConstVar DecRateCoeff::s_one("one", "1.0", 1.0);

DecRateCoeff::DecRateCoeff(
	const char* name, const char* title, Flags flags,
	RooAbsCategory& qf, RooAbsCategory& qt,
	RooAbsReal& Cf, RooAbsReal& Cfbar,
	RooAbsRealLValue& etaobs, RooAbsPdf& etapdf,
	RooAbsReal& tageff, RooAbsReal& eta,
	RooAbsReal& aprod, RooAbsReal& adet, RooAbsReal& atageff) :
    RooAbsReal(name, title),
    m_qf("qf", "qf", this, qf), m_qts("qts", "qts", this),
    m_Cf("Cf", "Cf", this, Cf), m_Cfbar("Cfbar", "Cfbar", this, Cfbar),
    m_etaobs("etaobs", "etaobs", this, etaobs),
    m_etapdfutinstance(
		(std::string(etapdf.GetName()) + "_untagged").c_str(),
		(std::string(etapdf.GetName()) + "_untagged").c_str(),
		RooArgSet(etaobs)),
    m_etapdfs("etapdfs", "etapdfs", this),
    m_tageffs("tageffs", "tageffs", this),
    m_etas("etas", "etas", this),
    m_aprod("aprod", "aprod", this, aprod),
    m_adet("adet", "adet", this, adet),
    m_atageffs("atageffs", "atageffs", this),
    m_cacheMgr(this), m_nset(0), m_nsethash(0), m_flags(flags)
{
    assert(checkDepsForConsistency(
		RooArgSet(qf, qt, etaobs),
		RooArgSet(Cf, Cfbar, etapdf, tageff,
		    eta, aprod, adet, atageff),
		&etaobs, RooArgSet(eta, etapdf)));
    m_qts.add(qt);
    initListProxies(
	    RooArgList(tageff), RooArgList(atageff),
	    RooArgList(eta), RooArgList(eta),
	    RooArgList(etapdf));
} 

DecRateCoeff::DecRateCoeff(
	const char* name, const char* title, Flags flags,
	RooAbsCategory& qf, RooAbsCategory& qt,
	RooAbsReal& Cf, RooAbsReal& Cfbar,
	RooAbsRealLValue& etaobs, RooAbsPdf& etapdf,
	RooAbsReal& tageff, RooAbsReal& eta, RooAbsReal& etabar,
	RooAbsReal& aprod, RooAbsReal& adet, RooAbsReal& atageff) :
    RooAbsReal(name, title),
    m_qf("qf", "qf", this, qf), m_qts("qts", "qts", this),
    m_Cf("Cf", "Cf", this, Cf), m_Cfbar("Cfbar", "Cfbar", this, Cfbar),
    m_etaobs("etaobs", "etaobs", this, etaobs),
    m_etapdfutinstance(
		(std::string(etapdf.GetName()) + "_untagged").c_str(),
		(std::string(etapdf.GetName()) + "_untagged").c_str(),
		RooArgSet(etaobs)),
    m_etapdfs("etapdfs", "etapdfs", this),
    m_tageffs("tageffs", "tageffs", this),
    m_etas("etas", "etas", this),
    m_aprod("aprod", "aprod", this, aprod),
    m_adet("adet", "adet", this, adet),
    m_atageffs("atageffs", "atageffs", this),
    m_cacheMgr(this), m_nset(0), m_nsethash(0), m_flags(flags)
{
    assert(checkDepsForConsistency(
		RooArgSet(qf, qt, etaobs),
		RooArgSet(Cf, Cfbar, etapdf, tageff,
		    eta, etabar, aprod, adet, atageff),
		&etaobs, RooArgSet(eta, etabar, etapdf)));
    m_qts.add(qt);
    initListProxies(
	    RooArgList(tageff), RooArgList(atageff),
	    RooArgList(eta), RooArgList(etabar),
	    RooArgList(etapdf));
} 

DecRateCoeff::DecRateCoeff(
	const char* name, const char* title, Flags flags,
	RooAbsCategory& qf, RooAbsCategory& qt,
	RooAbsReal& Cf, RooAbsReal& Cfbar,
	RooAbsReal& tageff, RooAbsReal& eta,
	RooAbsReal& aprod, RooAbsReal& adet,
	RooAbsReal& atageff) :
    RooAbsReal(name, title),
    m_qf("qf", "qf", this, qf), m_qts("qts", "qts", this),
    m_Cf("Cf", "Cf", this, Cf), m_Cfbar("Cfbar", "Cfbar", this, Cfbar),
    m_etapdfs("etapdfs", "etapdfs", this),
    m_tageffs("tageffs", "tageffs", this),
    m_etas("etas", "etas", this),
    m_aprod("aprod", "aprod", this, aprod),
    m_adet("adet", "adet", this, adet),
    m_atageffs("atageffs", "atageffs", this),
    m_cacheMgr(this), m_nset(0), m_nsethash(0), m_flags(flags)
{
    assert(checkDepsForConsistency(
		RooArgSet(qf, qt),
		RooArgSet(Cf, Cfbar, tageff, eta, aprod, adet, atageff)));
    m_qts.add(qt);
    initListProxies(
	    RooArgList(tageff), RooArgList(atageff),
	    RooArgList(eta));
}

DecRateCoeff::DecRateCoeff(
	const char* name, const char* title, Flags flags,
	RooAbsCategory& qf, RooAbsCategory& qt,
	RooAbsReal& Cf, RooAbsReal& Cfbar,
	RooAbsReal& tageff, RooAbsReal& eta, RooAbsReal& etabar,
	RooAbsReal& aprod, RooAbsReal& adet, RooAbsReal& atageff) :
    RooAbsReal(name, title),
    m_qf("qf", "qf", this, qf), m_qts("qts", "qts", this),
    m_Cf("Cf", "Cf", this, Cf), m_Cfbar("Cfbar", "Cfbar", this, Cfbar),
    m_etapdfs("etapdfs", "etapdfs", this),
    m_tageffs("tageffs", "tageffs", this),
    m_etas("etas", "etas", this),
    m_aprod("aprod", "aprod", this, aprod),
    m_adet("adet", "adet", this, adet),
    m_atageffs("atageffs", "atageffs", this),
    m_cacheMgr(this), m_nset(0), m_nsethash(0), m_flags(flags)
{
    assert(checkDepsForConsistency(
		RooArgSet(qf, qt),
		RooArgSet(Cf, Cfbar, tageff,
		    eta, etabar, aprod, adet, atageff)));
    m_qts.add(qt);
    initListProxies(
	    RooArgList(tageff), RooArgList(atageff),
	    RooArgList(eta), RooArgList(etabar));
} 

DecRateCoeff::DecRateCoeff(
	const char* name, const char* title, Flags flags,
	RooAbsCategory& qf, RooAbsCategory& qt,
	RooAbsReal& Cf, RooAbsReal& Cfbar,
	RooAbsRealLValue& etaobs, RooArgList& etapdfs,
	RooArgList& tageffs, RooArgList& etas,
	RooAbsReal& aprod, RooAbsReal& adet, RooArgList& atageffs) :
    RooAbsReal(name, title),
    m_qf("qf", "qf", this, qf), m_qts("qts", "qts", this),
    m_Cf("Cf", "Cf", this, Cf), m_Cfbar("Cfbar", "Cfbar", this, Cfbar),
    m_etaobs("etaobs", "etaobs", this, etaobs),
    m_etapdfutinstance(
		(std::string(etapdfs.at(0)->GetName()) + "_untagged").c_str(),
		(std::string(etapdfs.at(0)->GetName()) + "_untagged").c_str(),
		RooArgSet(etaobs)),
    m_etapdfs("etapdfs", "etapdfs", this),
    m_tageffs("tageffs", "tageffs", this),
    m_etas("etas", "etas", this),
    m_aprod("aprod", "aprod", this, aprod),
    m_adet("adet", "adet", this, adet),
    m_atageffs("atageffs", "atageffs", this),
    m_cacheMgr(this), m_nset(0), m_nsethash(0), m_flags(flags)
{
    RooArgSet params(Cf, Cfbar, aprod, adet);
    params.add(etapdfs); params.add(tageffs); params.add(etas);
    params.add(atageffs);
    RooArgSet etaset;
    etaset.add(etas);
    etaset.add(etapdfs);
    assert(checkDepsForConsistency(RooArgSet(qf, qt, etaobs), params,
		&etaobs, etaset));
    m_qts.add(qt);
    initListProxies(tageffs, atageffs, etas, etas, etapdfs);
} 

DecRateCoeff::DecRateCoeff(
	const char* name, const char* title, Flags flags,
	RooAbsCategory& qf, RooAbsCategory& qt,
	RooAbsReal& Cf, RooAbsReal& Cfbar,
	RooAbsRealLValue& etaobs, RooArgList& etapdfs,
	RooArgList& tageffs, RooArgList& etas, RooArgList& etabars,
	RooAbsReal& aprod, RooAbsReal& adet, RooArgList& atageffs) :
    RooAbsReal(name, title),
    m_qf("qf", "qf", this, qf), m_qts("qts", "qts", this),
    m_Cf("Cf", "Cf", this, Cf), m_Cfbar("Cfbar", "Cfbar", this, Cfbar),
    m_etaobs("etaobs", "etaobs", this, etaobs),
    m_etapdfutinstance(
		(std::string(etapdfs.at(0)->GetName()) + "_untagged").c_str(),
		(std::string(etapdfs.at(0)->GetName()) + "_untagged").c_str(),
		RooArgSet(etaobs)),
    m_etapdfs("etapdfs", "etapdfs", this),
    m_tageffs("tageffs", "tageffs", this),
    m_etas("etas", "etas", this),
    m_aprod("aprod", "aprod", this, aprod),
    m_adet("adet", "adet", this, adet),
    m_atageffs("atageffs", "atageffs", this),
    m_cacheMgr(this), m_nset(0), m_nsethash(0), m_flags(flags)
{
    RooArgSet params(Cf, Cfbar, aprod, adet);
    params.add(etapdfs); params.add(tageffs); params.add(etas);
    params.add(etabars), params.add(atageffs);
    RooArgSet etaset;
    etaset.add(etas);
    etaset.add(etabars);
    etaset.add(etapdfs);
    assert(checkDepsForConsistency(RooArgSet(qf, qt, etaobs), params,
		&etaobs, etaset));
    m_qts.add(qt);
    initListProxies(tageffs, atageffs, etas, etabars, etapdfs);
} 

DecRateCoeff::DecRateCoeff(
	const char* name, const char* title, Flags flags,
	RooAbsCategory& qf, RooAbsCategory& qt,
	RooAbsReal& Cf, RooAbsReal& Cfbar,
	RooArgList& tageffs, RooArgList& etas,
	RooAbsReal& aprod, RooAbsReal& adet,
	RooArgList& atageffs) :
    RooAbsReal(name, title),
    m_qf("qf", "qf", this, qf), m_qts("qts", "qts", this),
    m_Cf("Cf", "Cf", this, Cf), m_Cfbar("Cfbar", "Cfbar", this, Cfbar),
    m_etapdfs("etapdfs", "etapdfs", this),
    m_tageffs("tageffs", "tageffs", this),
    m_etas("etas", "etas", this),
    m_aprod("aprod", "aprod", this, aprod),
    m_adet("adet", "adet", this, adet),
    m_atageffs("atageffs", "atageffs", this),
    m_cacheMgr(this), m_nset(0), m_nsethash(0), m_flags(flags)
{
    RooArgSet params(Cf, Cfbar, aprod, adet);
    params.add(tageffs); params.add(etas);
    params.add(atageffs);
    assert(checkDepsForConsistency(RooArgSet(qf, qt), params));
    m_qts.add(qt);
    initListProxies(tageffs, atageffs, etas, etas);
}

DecRateCoeff::DecRateCoeff(
	const char* name, const char* title, Flags flags,
	RooAbsCategory& qf, RooAbsCategory& qt,
	RooAbsReal& Cf, RooAbsReal& Cfbar,
	RooArgList& tageffs, RooArgList& etas, RooArgList& etabars,
	RooAbsReal& aprod, RooAbsReal& adet, RooArgList& atageffs) :
    RooAbsReal(name, title),
    m_qf("qf", "qf", this, qf), m_qts("qts", "qts", this),
    m_Cf("Cf", "Cf", this, Cf), m_Cfbar("Cfbar", "Cfbar", this, Cfbar),
    m_etapdfs("etapdfs", "etapdfs", this),
    m_tageffs("tageffs", "tageffs", this),
    m_etas("etas", "etas", this),
    m_aprod("aprod", "aprod", this, aprod),
    m_adet("adet", "adet", this, adet),
    m_atageffs("atageffs", "atageffs", this),
    m_cacheMgr(this), m_nset(0), m_nsethash(0), m_flags(flags)
{
    RooArgSet params(Cf, Cfbar, aprod, adet);
    params.add(tageffs); params.add(etas); params.add(etabars);
    params.add(atageffs);
    assert(checkDepsForConsistency(RooArgSet(qf, qt), params));
    m_qts.add(qt);
    initListProxies(tageffs, atageffs, etas, etabars);
}

DecRateCoeff::DecRateCoeff(
	const DecRateCoeff& other, const char* name) : 
    RooAbsReal(other, name),
    m_qf("qf", this, other.m_qf), m_qts("qts", this, other.m_qts),
    m_Cf("Cf", this, other.m_Cf), m_Cfbar("Cfbar", this, other.m_Cfbar),
    m_etaobs("etaobs", this, other.m_etaobs),
    m_etapdfutinstance(other.m_etapdfutinstance),
    m_etapdfs("etapdfs", "etapdfs", this),
    m_tageffs("tageffs", this, other.m_tageffs),
    m_etas("etas", this, other.m_etas),
    m_aprod("aprod", this, other.m_aprod),
    m_adet("adet", this, other.m_adet),
    m_atageffs("atageffs", this, other.m_atageffs),
    m_cacheMgr(other.m_cacheMgr, this), m_nsets(other.m_nsets),
    m_nset(other.m_nset), m_nsethash(other.m_nsethash), m_flags(other.m_flags)
{
    // no need to verify constraints as in other constructors - we make a copy
    // of something that has already been verified
    if (m_etaobs.absArg()) {
	RooFIter it = other.m_etapdfs.fwdIterator();
	// skip other.m_etapdfutinstance;
	it.next();
	m_etapdfs.add(m_etapdfutinstance);
	for (RooAbsArg* arg = it.next(); arg; arg = it.next())
	    m_etapdfs.add(*arg);
    }
} 

DecRateCoeff::~DecRateCoeff()
{ }

TObject* DecRateCoeff::clone(const char* newname) const
{ return new DecRateCoeff(*this, newname); }

bool DecRateCoeff::checkDepsForConsistency(
	const RooArgSet& obs, const RooArgSet& params,
	const RooAbsArg* etaobs, const RooArgSet& etas) const
{
    // observables must not overlap with one another, or be constant
    RooFIter it = obs.fwdIterator();
    for (RooAbsArg* o1 = it.next(); o1; o1 = it.next()) {
	if (o1->isConstant()) continue;
	RooFIter jt = it;
	for (RooAbsArg* o2 = jt.next(); o2; o2 = jt.next()) {
	    if (o1->overlaps(*o2) && !o2->isConstant()) return false;
	}
    }

    // paramers must not overlap with observables, be constant or inherit from
    // TaggingCat
    it = params.fwdIterator();
    for (RooAbsArg* o1 = it.next(); o1; o1 = it.next()) {
	if (o1->isConstant()) continue;
	if (o1->InheritsFrom("TaggingCat")) continue;
	const bool isMistag = (0 != etas.find(*o1));
	// check out overlap with observables
	RooFIter jt = obs.fwdIterator();
	for (RooAbsArg* o2 = jt.next(); o2; o2 = jt.next()) {
	    if (isMistag && o2 == etaobs) continue;
	    if (o1->overlaps(*o2) && !o2->isConstant()) return false;
	}
    }

    return true;
}

unsigned DecRateCoeff::getMaxQt() const
{
    assert(1 == m_qts.getSize());
    TIterator *it =
	dynamic_cast<const RooAbsCategory&>(*m_qts.at(0)).typeIterator();
    unsigned maxqt = 0u;
    for (RooCatType* cat = dynamic_cast<RooCatType*>(it->Next()); cat;
	    cat = dynamic_cast<RooCatType*>(it->Next())) {
	const unsigned idx = std::abs(cat->getVal());
	if (idx > maxqt) maxqt = idx;
    }
    delete it;
    return maxqt;
}

void DecRateCoeff::fillListProxy(RooListProxy& proxy,
	const RooArgList& list, const RooArgList& listbar,
	const RooAbsArg& zeroelem) const
{
    const unsigned idxend = 1u + (2u * getMaxQt());
    for (unsigned idx = 0; idx < idxend; ++idx) {
	const int qt = qtFromIdx(idx);
	const int absqt = std::abs(qt);
	if (!qt) {
	    proxy.add(zeroelem);
	} else if (qt < 0) {
	    const int listidx = std::min(absqt - 1, listbar.getSize() - 1);
	    proxy.add(*listbar.at(listidx));
	} else {
	    const int listidx = std::min(absqt - 1, list.getSize() - 1);
	    proxy.add(*list.at(listidx));
	}
    }
}

void DecRateCoeff::initListProxies(
	const RooArgList& tageffs, const RooArgList& atageffs,
	const RooArgList& etas, const RooArgList& etabars,
	const RooArgList& etapdfs)
{
   // nothing in slot for (qt = 0)
   fillListProxy(m_tageffs, tageffs, tageffs, s_one);
   // last one is dummy (tagging efficiency asymmetry makes no sense for
   // qt = 0)
   fillListProxy(m_atageffs, atageffs, atageffs, s_one);
   // last one is dummy (calibrated mistag makes no sense for qt = 0)
   fillListProxy(m_etas, etas, etabars.getSize() ? etabars : etas, s_one);
   if (etapdfs.getSize()) {
       fillListProxy(m_etapdfs, etapdfs, etapdfs, m_etapdfutinstance);
   }
}

Double_t DecRateCoeff::getValV(const RooArgSet* nset) const
{
    assert(1 == m_qts.getSize());
    const UInt_t nsethash(nset ? hash(*nset) : 0);
    if (m_nsethash != nsethash) {
	// update hash for current nset
	m_nsethash = nsethash;
	if (nset && 0 != nset->getSize()) {
	    // non-empty nset, so form our own copy with just the contents we
	    // need
	    RooArgSet& ns(m_nsets[nsethash]);
	    if (0 != ns.getSize()) ns.removeAll();
	    matchArgs(*nset, ns, m_qf.arg());
	    matchArgs(*nset, ns, *m_qts.at(0));
	    if (m_etaobs.absArg())
		matchArgs(*nset, ns, m_etaobs.arg());
	    // set up pointer to current normalisation set
	    m_nset = (0 != ns.getSize()) ? &ns : 0;
	} else {
	    // set up pointer to current normalisation set (empty)
	    m_nset = 0;
	}
    }
    return RooAbsReal::getValV(m_nset);
}

Double_t DecRateCoeff::evaluate() const
{
    const double oep((m_flags & Minus ? -1. : 1.) *
	    (m_flags & CPOdd ? double(int(m_qf)) : 1.));
    const double oem((m_flags & CPOdd) ? (-oep) : oep);
    const double Cf = (m_flags & AvgDelta) ?
	(double(m_Cf) + double(m_Cfbar)) : double(m_Cf);
    const double Cfbar = (m_flags & AvgDelta) ?
	(double(m_Cf) - double(m_Cfbar)) : double(m_Cfbar);
    const double alphapp(oep * Cf), alphapm(oem * Cf);
    const double alphamp(oep * Cfbar), alphamm(oem * Cfbar);
    // get value
    const double val = getCache(s_emptyset, m_nset, RooNameReg::ptr(0)).first->eval(
	    alphapp, alphapm, alphamp, alphamm);
    if (!m_nset) return val;
    // normalise if we have to
    const char *nrange = 0; /* (_normRangeOverride.Length() ?
			       _normRangeOverride.Data() :
			       ((_normRange.Length()) ? _normRange.Data() : 0)); */
    return val / getCache(*m_nset, m_nset,
	    RooNameReg::ptr(nrange)).first->eval(1., 1., 1., 1.);
}

Bool_t DecRateCoeff::forceAnalyticalInt(const RooAbsArg& dep) const
{
    assert(1 == m_qts.getSize());
    if (&dep == m_qf.absArg()) return kTRUE;
    if (&dep == m_qts.at(0)) return kTRUE;
    if (&dep == m_etaobs.absArg()) return kTRUE;
    return kFALSE;
}

Int_t DecRateCoeff::getAnalyticalIntegral(
	RooArgSet& allVars, RooArgSet& anaIntVars,
	const char* rangeName) const
{ return getAnalyticalIntegralWN(allVars, anaIntVars, m_nset, rangeName); }

Double_t DecRateCoeff::analyticalIntegral(
	Int_t code, const char* rangeName) const
{ return analyticalIntegralWN(code, m_nset, rangeName); }

Int_t DecRateCoeff::getAnalyticalIntegralWN(
	RooArgSet& allVars, RooArgSet& anaIntVars,
	const RooArgSet* nset, const char* rangeName) const
{
    assert(1 == m_qts.getSize());
    // perform analytical integrals
    // we know how to integrate over etaobs, qf, qt
    if (m_etaobs.absArg()) {
	matchArgs(allVars, anaIntVars, m_etaobs);
    }
    matchArgs(allVars, anaIntVars, m_qf);
    matchArgs(allVars, anaIntVars, *m_qts.at(0));
    // create the integral object
    unsigned icode = 1 + getCache(
	    anaIntVars, nset, RooNameReg::ptr(rangeName)).second;
    assert(icode < 32767);
    unsigned ncode = 0;
    // create normalisation object (if needed)
    if (nset) {
	const char *nrange = 0; /* (_normRangeOverride.Length() ?
		_normRangeOverride.Data() :
		((_normRange.Length()) ? _normRange.Data() : 0)); */
	ncode = 1 + getCache(
		*nset, nset, RooNameReg::ptr(nrange)).second;
	assert(ncode < 32767);
    }
    return (ncode << 15) | icode;
}

Double_t DecRateCoeff::analyticalIntegralWN(
	Int_t code, const RooArgSet* nset, const char* /* rangeName */) const
{
    if (!code) return getValV(nset);
    assert(code > 0);
    const unsigned icode(unsigned(code) & 32767);
    const unsigned ncode((unsigned(code) >> 15) & 32767);
    assert(Int_t(icode | (ncode << 15)) == code);

    CacheElem* ic =
	static_cast<CacheElem*>(m_cacheMgr.getObjByIndex(icode - 1));
    const int iqf(m_qf);
    const double oep((m_flags & Minus ? -1. : 1.) *
	    (m_flags & CPOdd ? double(iqf) : 1.));
    const double oem((m_flags & CPOdd) ? (-oep) : oep);
    const double Cf = (m_flags & AvgDelta) ?
	(double(m_Cf) + double(m_Cfbar)) : double(m_Cf);
    const double Cfbar = (m_flags & AvgDelta) ?
	(double(m_Cf) - double(m_Cfbar)) : double(m_Cfbar);
    const double alphapp(oep * Cf), alphapm(oem * Cf);
    const double alphamp(oep * Cfbar), alphamm(oem * Cfbar);
    const double ival = ic->eval(alphapp, alphapm, alphamp, alphamm);
    if (ncode) {
	CacheElem* nc =
	    static_cast<CacheElem*>(m_cacheMgr.getObjByIndex(ncode - 1));
	const double nval = nc->eval(1., 1., 1., 1.);
	return ival / nval;
    } else {
	return ival;
    }
}

DecRateCoeff::CacheElemPair DecRateCoeff::getCache(
	const RooArgSet& iset, const RooArgSet* nset,
	const TNamed* rangeName) const
{
    Int_t idx = -1;
    CacheElem* cache = reinterpret_cast<CacheElem*>(
	    m_cacheMgr.getObj(nset, &iset, &idx, rangeName));
    if (cache) {
	idx = m_cacheMgr.lastIndex();
	assert(idx >= 0);
	return CacheElemPair(cache, idx);
    }
    // ok, integral not in cache yet
    cache = new CacheElem(*this, iset, nset, rangeName);
    assert(cache);
    // put it in cache
    idx = m_cacheMgr.setObj(nset, &iset, cache, rangeName);
    assert(idx >= 0);
    return CacheElemPair(cache, idx);
}

UInt_t DecRateCoeff::hash(const RooArgSet& s) const
{
    // empty set maps to zero hash value
    UInt_t sz = s.getSize();
    if (0 == sz) return 0;
    // use FNV1a to hash things - fast and easy to implement
    UInt_t uhash = 2166136261u;
    // hash size
    for (unsigned i = sizeof(sz); i--; sz >>= 8) {
	uhash ^= sz & 0xffu;
	uhash *= 16777619u;
    }
    // hash name, pointer of each member of the set
    for (RooFIter it = s.fwdIterator(); const RooAbsArg* arg = it.next(); ) {
	std::ptrdiff_t ptr = reinterpret_cast<const char*>(arg) -
	    reinterpret_cast<const char*>(0);
	for (unsigned i = sizeof(ptr); i--; ptr >>= 8) {
	    uhash ^= ptr & 0xffu;
	    uhash *= 16777619u;
	}
	for (const unsigned char* cptr =
		reinterpret_cast<const unsigned char*>(arg->GetName());
		*cptr; ++cptr) {
	    uhash ^= *cptr;
	    uhash *= 16777619u;
	}
    }
    // protect against zero hash - reserved for empty set
    if (!uhash) ++uhash;
    return uhash;
}

DecRateCoeff::CacheElem::CacheElem(const DecRateCoeff& parent,
	const RooArgSet& iset, const RooArgSet* nset,
	const TNamed* rangeName) :
    m_etapdfs(1u + 2u * parent.getMaxQt(), 0),
    m_prodetapdfs(1u + 2u * parent.getMaxQt(), 0),
    m_rangeName(rangeName ? rangeName->GetName() : 0),
    m_parent(parent),
    m_workRanges(1u + 2u * parent.getMaxQt(),
	    std::pair<RooAbsRealLValue*, RooAbsRealLValue*>(0, 0)),
    m_etabins(1u + 2u * parent.getMaxQt(), std::vector<double>()),
    m_prodcachedvals(1u + 2u * parent.getMaxQt(),
	    std::numeric_limits<double>::quiet_NaN()),
    m_qtmax(parent.getMaxQt()),
    m_flags(None)
{
    assert(m_qtmax);
    assert(1 == parent.m_qts.getSize());
    m_workRangeNames.resize(1u + 2u * m_qtmax);
    // set flag for qf/qt integration
    if (iset.find(parent.m_qf.arg()))
	m_flags = static_cast<Flags>(m_flags | IntQf);
    if (iset.find(*parent.m_qts.at(0)))
	m_flags = static_cast<Flags>(m_flags | IntQt);

    // initialise mistag (eta) part of cache element (if needed)
    if (parent.m_etaobs.absArg()) {
	// case with per-event mistag
	setupForPerEventEta(iset, nset);
    } else {
	// case with average mistag
	for (int qt = -m_qtmax; int(m_qtmax) >= qt; ++qt) {
	    const unsigned idx = DecRateCoeff::idxFromQt(qt);
	    RooAbsReal* eta = dynamic_cast<RooAbsReal*>(m_parent.m_etas.at(idx));
	    assert(eta);
	    m_prodetapdfs[idx] = eta;
	}
    }
}

RooArgSet DecRateCoeff::CacheElem::setupISetAndNSet(
	const RooArgSet& iset, const RooArgSet* nset)
{
    // massage integration set, so we do the right thing
    RooArgSet etaiset;
    if (iset.find(m_parent.m_etaobs.arg())) {
	etaiset.add(m_parent.m_etaobs.arg());
	m_flags = static_cast<Flags>(m_flags | IntEta);
	bool allBinned = true;
	RooFIter it = m_parent.m_etapdfs.fwdIterator();
	it.next(); // skip untagged events - they're always unbinned
	for (RooAbsReal* arg = dynamic_cast<RooAbsReal*>(it.next());
		arg; arg = dynamic_cast<RooAbsReal*>(it.next())) {
	    if (!arg->isBinnedDistribution(etaiset)) {
		allBinned = false;
		break;
	    }
	}
	if (allBinned)
	    m_flags = static_cast<Flags>(m_flags | ProdIntBinned);
    }
    // massage normalisation set as well (if present)
    if (nset && nset->find(m_parent.m_etaobs.arg())) {
	m_nset.add(m_parent.m_etaobs.arg());
	m_flags = static_cast<Flags>(m_flags | NormEta);
    }
    return etaiset;
}

void DecRateCoeff::CacheElem::setupForPerEventEta(const RooArgSet& iset,
	const RooArgSet* nset)
{
    const RooArgSet etaiset(setupISetAndNSet(iset, nset));

    for (int qt = -m_qtmax; int(m_qtmax) >= qt; ++qt) {
	const unsigned idx = DecRateCoeff::idxFromQt(qt);
	std::ostringstream os;
	os << m_parent.GetName() << "_" <<
	    m_parent.hash(RooArgSet(m_parent)) <<
	    "_INT_" << m_parent.hash(iset) <<
	    "_NORM_" << m_parent.hash(nset ? *nset : m_parent.s_emptyset) <<
	    "_RANGE_" << (m_rangeName ? m_rangeName : "") << "_IDX_" << idx;
	const std::string newname(os.str() + "_prod_eta");
	RooAbsPdf* etapdf = dynamic_cast<RooAbsPdf*>(
		m_parent.m_etapdfs.at(idx));
	assert(etapdf);
	m_etapdfs[idx] = etapdf;
	if (m_flags & IntEta) {
	    // integrate over eta
	    m_etapdfs[idx] = etapdf->createIntegral(
		    etaiset, nset ? &m_nset : 0, 0, m_rangeName);
	    assert(m_etapdfs[idx]);
	}
	if (0 == qt) continue;
	RooAbsReal* eta = dynamic_cast<RooAbsReal*>(m_parent.m_etas.at(idx));
	assert(eta);
	RooAbsReal* prod = new RooProduct(newname.c_str(), newname.c_str(),
		RooArgList(*etapdf, *eta));
	assert(prod);
	m_prodetapdfs[idx] = prod;
	if (m_flags & IntEta) {
	    // integrate over eta
	    m_prodetapdfs[idx] = prod->createIntegral(
		    etaiset, nset ? &m_nset : 0, 0, m_rangeName);
	    assert(m_prodetapdfs[idx]);
	}
	if (qt && (m_flags & ProdIntBinned)) {
	    // patch configuration: set up binned evaluation of product
	    // integral
	    setupBinnedProductIntegral(
		    m_prodetapdfs[idx], *etapdf, *eta, idx);
	}
    }
    //print();
}

void DecRateCoeff::CacheElem::setupBinBoundaries(unsigned idx)
{
    // get binning
    const double rxmin = m_parent.m_etaobs.min(m_rangeName);
    const double rxmax = m_parent.m_etaobs.max(m_rangeName);
    if (m_etabins[idx].empty()) {
	// bin vector can be shared between B/Bbar
	std::auto_ptr<std::list<Double_t> > bins(
		dynamic_cast<const RooAbsPdf&>(
		    *m_parent.m_etapdfs.at(idx)).binBoundaries(
		    const_cast<RooAbsRealLValue&>(
			dynamic_cast<const RooAbsRealLValue&>(
			    m_parent.m_etaobs.arg())),
		    m_parent.m_etaobs.min(), m_parent.m_etaobs.max()));
	m_etabins[idx].reserve(bins->size());
	for (std::list<Double_t>::const_iterator
		lo = bins->begin(), hi = ++(bins->begin());
		bins->end() != hi; ++hi, ++lo) {
	    if (*hi < rxmin) continue;
	    if (*lo > rxmax) break;
	    const double xmin = std::max(*lo, rxmin);
	    const double xmax = std::min(*hi, rxmax);
	    if (m_etabins[idx].empty()) m_etabins[idx].push_back(xmin);
	    m_etabins[idx].push_back(xmax);
	}
	assert(m_etabins[idx].size() >= 2);
	assert(std::abs(m_etabins[idx].front() - rxmin) <
		16. * std::numeric_limits<double>::epsilon() *
		std::max(std::abs(rxmin), 1.));
	assert(std::abs(m_etabins[idx].back() - rxmax) <
		16. * std::numeric_limits<double>::epsilon() *
		std::max(std::abs(rxmax), 1.));
    }
}

void DecRateCoeff::CacheElem::setupBinnedProductIntegral(
	RooAbsReal* &prod, const RooAbsPdf& etapdf,
	const RooAbsReal& eta, int idx)
{
    // define name of working range
    m_workRangeNames[idx] = prod->GetName();
    m_workRangeNames[idx] += "_workRange";
    // get binning
    const double rxmin = m_parent.m_etaobs.min(m_rangeName);
    const double rxmax = m_parent.m_etaobs.max(m_rangeName);
    setupBinBoundaries(idx);
    // set up required binning
    m_workRanges[idx].first = new RooRealVar(
	    (m_workRangeNames[idx] + "_min").c_str(),
	    (m_workRangeNames[idx] + "_min").c_str(), rxmin, rxmin, rxmax);
    m_workRanges[idx].second = new RooRealVar(
	    (m_workRangeNames[idx] + "_max").c_str(),
	    (m_workRangeNames[idx] + "_max").c_str(), rxmax, rxmin, rxmax);
    // we need to operate on a clone of etaobs with our custom binning, or
    // things die a horrible death when using more than one CPU
    RooRealVar* etaobs = dynamic_cast<RooRealVar*>(
	    m_parent.m_etaobs.arg().clone(
		(m_workRangeNames[idx] + "_eta").c_str()));
    assert(etaobs);
    etaobs->setRange(m_workRangeNames[idx].c_str(),
	    *m_workRanges[idx].first, *m_workRanges[idx].second);
    // ok get rid of prod, and set to what is needed for binned evaluation
    std::string prodname(prod->GetName());
    prodname += "_binned";
    delete prod;
    RooArgSet etaiset(*etaobs);
    RooAbsPdf* myetapdf = 0;
    {
	std::string str(etapdf.GetName());
	str += "_for_";
	str += m_workRangeNames[idx];
	RooCustomizer cust(etapdf, str.c_str());
	cust.replaceArg(m_parent.m_etaobs.arg(), *etaobs);
	myetapdf = dynamic_cast<RooAbsPdf*>(cust.build());
	assert(myetapdf);
    }
    RooAbsReal* etaa = 0;
    {
	std::string str(eta.GetName());
	str += "_for_";
	str += m_workRangeNames[idx];
	RooCustomizer cust(eta, str.c_str());
	cust.replaceArg(m_parent.m_etaobs.arg(), *etaobs);
	etaa = dynamic_cast<RooAbsReal*>(cust.build());
	assert(etaa);
    }

    RooArgSet nset(*etaobs);
    RooAbsReal* pdf = myetapdf->createIntegral(etaiset,
	    (m_flags & NormEta) ? &nset : 0, 0,
	    m_workRangeNames[idx].c_str());
    assert(pdf);
    RooAbsReal* etai = etaa->createIntegral(etaiset,
	    (m_flags & NormEta) ? &nset : 0, 0,
	    m_workRangeNames[idx].c_str());
    assert(etai);
    prod = new RooProduct(
	    prodname.c_str(), prodname.c_str(), RooArgList(*pdf, *etai));
    assert(prod);
    // make sure we do not leak
    prod->addOwnedComponents(*pdf);
    prod->addOwnedComponents(*etai);
    prod->addOwnedComponents(*myetapdf);
    if (etaa != m_parent.m_etaobs.absArg()) prod->addOwnedComponents(*etaa);
    prod->addOwnedComponents(*etaobs);
    // all done
}

DecRateCoeff::CacheElem::~CacheElem()
{
    if (m_etapdfs.front()) {
	// per-event mistag case
	// we own the (integral of the) product of mistag pdf and per-event
	// mistag in any case
	for (unsigned idx = 0; idx < m_prodetapdfs.size(); ++idx)
	    delete m_prodetapdfs[idx];
	// work out if we own the stuff in m_etapdfs, too - that is only the
	// case if integrate over mistag
	if (m_flags & IntEta) {
	    for (unsigned idx = 0; idx < m_etapdfs.size(); ++idx)
		delete m_etapdfs[idx];
	}
	for (unsigned idx = 0; idx < m_workRanges.size(); ++idx) {
	    delete m_workRanges[idx].first;
	    delete m_workRanges[idx].second;
	}
    } else {
	// we're in the average mistag case, so we do not own the pointers and
	// must not delete them...
    }
}

RooArgList DecRateCoeff::CacheElem::containedArgs(Action)
{
    assert(1 == m_parent.m_qts.getSize());
    RooArgList retVal;
    for (unsigned idx = 0; idx < m_etapdfs.size(); ++idx) {
	if (!m_etapdfs[idx]) continue;
	if (!retVal.find(*m_etapdfs[idx]))
	    retVal.add(*m_etapdfs[idx]);
    }
    for (unsigned idx = 0; idx < m_prodetapdfs.size(); ++idx) {
	if (!m_prodetapdfs[idx]) continue;
	if (!retVal.find(*m_prodetapdfs[idx]))
	    retVal.add(*m_prodetapdfs[idx]);
    }
    for (unsigned idx = 0; idx < m_workRanges.size(); ++idx) {
	if (!m_workRanges[idx].first) continue;
	if (!retVal.find(*m_workRanges[idx].first))
	    retVal.add(*m_workRanges[idx].first);
	if (!retVal.find(*m_workRanges[idx].second))
	    retVal.add(*m_workRanges[idx].second);
    }
    for (unsigned idx = 0; idx < m_etapdfs.size(); ++idx) {
	if (!retVal.find(*m_parent.m_tageffs.at(idx)))
	    retVal.add(*m_parent.m_tageffs.at(idx));
	if (!retVal.find(*m_parent.m_etas.at(idx)))
	    retVal.add(*m_parent.m_etas.at(idx));
	if (!retVal.find(*m_parent.m_atageffs.at(idx)))
	    retVal.add(*m_parent.m_atageffs.at(idx));
    }
    if (!(m_flags & IntQf)) retVal.add(m_parent.m_qf.arg());
    if (!(m_flags & IntQt)) retVal.add(*m_parent.m_qts.at(0));
    if (!(m_flags & IntEta)) {
	if (m_parent.m_etaobs.absArg()) retVal.add(m_parent.m_etaobs.arg());
	for (unsigned idx = 0; idx < m_etapdfs.size(); ++idx) {
	    if (!m_etapdfs[idx]) continue;
	    if (!retVal.find(*m_parent.m_etapdfs.at(idx)))
		retVal.add(*m_parent.m_etapdfs.at(idx));
	}
    }
    retVal.add(m_parent.m_aprod.arg());
    retVal.add(m_parent.m_adet.arg());
    return retVal;
}

double DecRateCoeff::CacheElem::qfpdf(const int qf) const
{
    assert(std::abs(qf) == 1);
    return 1. + double(qf) * double(m_parent.m_adet);
}

double DecRateCoeff::CacheElem::qtetapdf(const int qf, const int qt,
	const double alphapp, const double alphapm,
	const double alphamp, const double alphamm) const
{
    assert(std::abs(qf) == 1);
    const unsigned qtabs = std::abs(qt);
    assert(qtabs <= m_qtmax);
    const unsigned idx = DecRateCoeff::idxFromQt(qt);
    const double cp((qf > 0) ? alphapp : alphamp);
    const double cm((qf > 0) ? alphapm : alphamm);
    const double ap(m_parent.m_aprod);
    if (qt < 0) {
	const double at(((RooAbsReal*) m_parent.m_atageffs.at(idx))->getVal());
	const double eps(((RooAbsReal*) m_parent.m_tageffs.at(idx))->getVal());
	const double m(etaintpdf(qtabs));
	const double mpp(etaintprodpdf(+qtabs));
	const double mpm(etaintprodpdf(-qtabs));
	return
	    (1. + ap) * eps * (1. + at) * mpp * cp +
	    (1. - ap) * eps * (1. - at) * (m - mpm) * cm;
    } else if (0 == qt) {
	double epsb = 1., epsbbar = 1.;
	for (unsigned i = 1; i <= m_qtmax; ++i) {
	    const unsigned idx2 = DecRateCoeff::idxFromQt(i);
	    const double at(((RooAbsReal*)
			m_parent.m_atageffs.at(idx2))->getVal());
	    const double eps(((RooAbsReal*)
			m_parent.m_tageffs.at(idx2))->getVal());
	    epsb -= eps * (1. + at);
	    epsbbar -= eps * (1. - at);
	}
	return etaintpdf(qt) * (
		(1. + ap) * epsb * cp + (1. - ap) * epsbbar * cm);
    } else if (qt > 0) {
	const double at(((RooAbsReal*) m_parent.m_atageffs.at(idx))->getVal());
	const double eps(((RooAbsReal*) m_parent.m_tageffs.at(idx))->getVal());
	const double m(etaintpdf(qtabs));
	const double mpp(etaintprodpdf(+qtabs));
	const double mpm(etaintprodpdf(-qtabs));
	return
	    (1. + ap) * eps * (1. + at) * (m - mpp) * cp +
	    (1. - ap) * eps * (1. - at) * mpm * cm;
    }
    // must never reach this point
    assert(1 == 0);
    return std::numeric_limits<double>::quiet_NaN();
}

double DecRateCoeff::CacheElem::eval(
	const double alphapp, const double alphapm,
	const double alphamp, const double alphamm) const
{
	assert(1 == m_parent.m_qts.getSize());
    if (!(m_flags & (IntQf | IntQt))) {
	const int qt = dynamic_cast<RooAbsCategory&>(
		*m_parent.m_qts.at(0)).getIndex();
	return qfpdf(int(m_parent.m_qf)) *
	    qtetapdf(int(m_parent.m_qf), qt,
		    alphapp, alphapm, alphamp, alphamm);
    } else if ((m_flags & IntQf) && !(m_flags & IntQt)) {
	// loop over qf states
	const RooCategory& qf(
		dynamic_cast<const RooCategory&>(m_parent.m_qf.arg()));
	std::auto_ptr<TIterator> qfit(qf.typeIterator());
	if (alphapp == alphamp && alphapm == alphamm) {
	    // can optimise a bit, since qtetapdf will not depend on qf with
	    // this choice of alpha{m,p}{m,p}
	    double retVal = 0.;
	    while (RooCatType* qfty =
		    reinterpret_cast<RooCatType*>(qfit->Next())) {
		// skip qf states not in range if the category has a range of
		// that name (if there's no range of rangeName in qf, we use
		// all qf states)
		if (qf.hasRange(m_rangeName) &&
			!qf.isStateInRange(m_rangeName, qfty->GetName()))
		    continue;
		retVal += qfpdf(qfty->getVal());
	    }
	    const int qt = dynamic_cast<RooAbsCategory&>(
		    *m_parent.m_qts.at(0)).getIndex();
	    return retVal * qtetapdf(1, qt,
		    alphapp, alphapm, alphamp, alphamm);
	} else {
	    const int iqt = dynamic_cast<RooAbsCategory&>(
		    *m_parent.m_qts.at(0)).getIndex();
	    double retVal = 0.;
	    while (RooCatType* qfty =
		    reinterpret_cast<RooCatType*>(qfit->Next())) {
		// skip qf states not in range if the category has a range of
		// that name (if there's no range of rangeName in qf, we use
		// all qf states)
		if (qf.hasRange(m_rangeName) &&
			!qf.isStateInRange(m_rangeName, qfty->GetName()))
		    continue;
		const int iqf = int(qfty->getVal());
		retVal += qfpdf(iqf) *
		    qtetapdf(iqf, iqt, alphapp, alphapm, alphamp, alphamm);
	    }
	    return retVal;
	}
    } else if (!(m_flags & IntQf) && (m_flags & IntQt)) {
	double retVal = 0.;
	const int iqf(m_parent.m_qf);
	// loop over qt states
	const RooCategory& qt(
		dynamic_cast<const RooCategory&>(*m_parent.m_qts.at(0)));
	std::auto_ptr<TIterator> qtit(qt.typeIterator());
	while (RooCatType* qtty =
		reinterpret_cast<RooCatType*>(qtit->Next())) {
	    // skip qt states not in range if the category has a range of that
	    // name (if there's no range of rangeName in qt, we use all qt
	    // states)
	    if (qt.hasRange(m_rangeName) &&
		    !qt.isStateInRange(m_rangeName, qtty->GetName()))
		continue;
	    retVal +=
		qtetapdf(iqf, qtty->getVal(),
			alphapp, alphapm, alphamp, alphamm);
	}
	return retVal * qfpdf(iqf);
    } else if ((m_flags & IntQf) && (m_flags & IntQt)) {
	const RooCategory& qf(
		dynamic_cast<const RooCategory&>(m_parent.m_qf.arg()));
	std::auto_ptr<TIterator> qfit(qf.typeIterator());
	if (alphapp == alphamp && alphapm == alphamm) {
	    // can optimise a bit, since qtetapdf will not depend on qf with
	    // this choice of alpha{m,p}{m,p}
	    double qfint = 0., qtint = 0.;
	    while (RooCatType* qfty =
		    reinterpret_cast<RooCatType*>(qfit->Next())) {
		// skip qf states not in range if the category has a range of
		// that name (if there's no range of rangeName in qf, we use
		// all qf states)
		if (qf.hasRange(m_rangeName) &&
			!qf.isStateInRange(m_rangeName, qfty->GetName()))
		    continue;
		qfint += qfpdf(qfty->getVal());
	    }
	    // loop over qt states
	    const RooCategory& qt(
		    dynamic_cast<const RooCategory&>(*m_parent.m_qts.at(0)));
	    std::auto_ptr<TIterator> qtit(qt.typeIterator());
	    while (RooCatType* qtty =
		    reinterpret_cast<RooCatType*>(qtit->Next())) {
		// skip qt states not in range if the category has a range of
		// that name (if there's no range of rangeName in qt, we use
		// all qt states)
		if (qt.hasRange(m_rangeName) &&
			!qt.isStateInRange(m_rangeName, qtty->GetName()))
		    continue;
		qtint +=
		    qtetapdf(1, qtty->getVal(),
			    alphapp, alphapm, alphamp, alphamm);
	    }
	    return qfint * qtint;
	} else {
	    double retVal = 0.;
	    // loop over qf states
	    while (RooCatType* qfty =
		    reinterpret_cast<RooCatType*>(qfit->Next())) {
		const int iqf = qfty->getVal();
		// skip qf states not in range if the category has a range of
		// that name (if there's no range of rangeName in qf, we use
		// all qf states)
		if (qf.hasRange(m_rangeName) &&
			!qf.isStateInRange(m_rangeName, qfty->GetName()))
		    continue;
		// loop over qt states
		const RooCategory& qt(
			dynamic_cast<const RooCategory&>(*m_parent.m_qts.at(0)));
		std::auto_ptr<TIterator> qtit(qt.typeIterator());
		while (RooCatType* qtty =
			reinterpret_cast<RooCatType*>(qtit->Next())) {
		    // skip qt states not in range if the category has a range
		    // of that name (if there's no range of rangeName in qt,
		    // we use all qt states)
		    if (qt.hasRange(m_rangeName) &&
			    !qt.isStateInRange(m_rangeName, qtty->GetName()))
			continue;
		    retVal += qfpdf(iqf) *
			qtetapdf(iqf, qtty->getVal(),
				alphapp, alphapm, alphamp, alphamm);
		}
	    }
	    return retVal;
	}
    }
    // must never come here
    assert(1 == 0);
    return std::numeric_limits<double>::quiet_NaN();
}

double DecRateCoeff::CacheElem::etaintprodpdf(const int qt) const
{
    const unsigned idx = DecRateCoeff::idxFromQt(qt);
    if (m_prodetapdfs.empty() || !m_prodetapdfs[idx] || !qt) return 1.0;
    RooAbsReal* prodint = m_prodetapdfs[idx];
    const RooArgSet* nset = (m_flags & NormEta) ? &m_nset : 0;
    if (!(m_flags & ProdIntBinned))
	return prodint->getValV(nset);
    // use binned approximation to integral
    if (m_prodcachedvals[idx] == m_prodcachedvals[idx] &&
	    !prodint->isValueOrShapeDirtyAndClear()) {
	return m_prodcachedvals[idx];
    }
    RooAbsRealLValue* low = m_workRanges[idx].first;
    RooAbsRealLValue* high = m_workRanges[idx].second;
    double retVal = 0.;
    for (std::vector<double>::const_iterator it =
	    m_etabins[idx].begin() + 1;
	    m_etabins[idx].end() != it; ++it) {
	const double xmin(*(it - 1)), xmax(*it);
	low->setVal(xmin);
	high->setVal(xmax);
	retVal += prodint->getValV(nset);
    }
    return (m_prodcachedvals[idx] = retVal);
}

void DecRateCoeff::CacheElem::print() const
{
    std::printf("\nDEBUG: DecRateCoeff::CacheElem at %p\n", this);
    std::printf("\tm_parent %p (%s)\n", &m_parent, m_parent.GetName());
    std::printf("\trangeName %s, qtmax %u, flags 0x%02x",
	    m_rangeName, m_qtmax, m_flags);
    std::printf("\tnset: "); m_nset.Print();
    const RooArgSet* nset = (m_flags & NormEta) ? &m_nset : 0;
    RooArgList tmp = const_cast<DecRateCoeff::CacheElem&>(
	    *this).containedArgs(RooAbsCacheElement::OperModeChange);
    std::printf("\tcontainedArgs: "); tmp.Print();
    for (int qt = -m_qtmax; qt <= int(m_qtmax); ++qt) {
	const unsigned idx = DecRateCoeff::idxFromQt(qt);
	std::printf("\tDUMPING FOR QT = % d (IDX %u)\n", qt, idx);
	if (!m_workRangeNames[idx].empty())
	    std::printf("\t\tm_workRangeNames[% d]: %s\n",
		    idx, m_workRangeNames[idx].c_str());
	if (m_etapdfs[idx])
	    std::printf("\t\tm_etapdfs[% d]: %p (%s) %g\n", idx,
		    m_etapdfs[idx], m_etapdfs[idx]->GetName(),
		    m_etapdfs[idx]->getVal(nset));
	if (m_prodetapdfs[idx])
	    std::printf("\t\tm_prodetapdfs[% d]: %p (%s) %g\n",
		    idx, m_prodetapdfs[idx], m_prodetapdfs[idx]->GetName(),
		    m_prodetapdfs[idx]->getVal(nset));
	if (m_workRanges[idx].first)
	    std::printf("\t\tm_workRanges[% d]: %p (%s)\n", idx,
		    m_workRanges[idx].first,
		    m_workRanges[idx].first->GetName());
	if (m_workRanges[idx].second)
	    std::printf("\t\tm_workRanges[% d]: %p (%s)\n", idx,
		    m_workRanges[idx].second,
		    m_workRanges[idx].second->GetName());
	std::printf("\t\tm_prodcachevals[% d]: %g\n", idx,
		m_prodcachedvals[idx]);
	std::printf("\t\tm_etabins: (size %u): [",
		unsigned(m_etabins[idx].size()));
	for (unsigned j = 0; j < m_etabins[idx].size(); ++j) {
	    std::printf(" %g", m_etabins[idx][j]);
	}
	std::printf(" ]\n");
    }
}

// vim: sw=4:tw=78:ft=cpp
