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
#include <cassert>
#include <algorithm>
 
#include <RooRealVar.h>
#include <RooProduct.h>
#include <RooCategory.h>
#include <RooCustomizer.h>

#include "DecRateCoeff.h"

namespace doofit {
namespace extraroopdfs {

RooArgSet DecRateCoeff::s_emptyset;

DecRateCoeff::DecRateCoeff(
	const char* name, const char* title, Flags flags,
	RooAbsCategory& qf, RooAbsCategory& qt,
	RooAbsReal& Cf, RooAbsReal& Cfbar,
	RooAbsRealLValue& etaobs, RooAbsPdf& etapdf,
	RooAbsReal& tageff, RooAbsReal& eta,
	RooAbsReal& aprod, RooAbsReal& adet, RooAbsReal& atageff) :
    RooAbsReal(name, title),
    m_qf("qf", "qf", this, qf), m_qt("qt", "qt", this, qt),
    m_Cf("Cf", "Cf", this, Cf), m_Cfbar("Cfbar", "Cfbar", this, Cfbar),
    m_etaobs("etaobs", "etaobs", this, etaobs),
    m_etapdf("etapdf", "etapdf", this, etapdf),
    m_etapdfutinstance(
		(std::string(etapdf.GetName()) + "_untagged").c_str(),
		(std::string(etapdf.GetName()) + "_untagged").c_str(),
		RooArgSet(etaobs)),
    m_etapdfut("etapdfut", "etapdfut", this, m_etapdfutinstance),
    m_tageff("tageff", "tageff", this, tageff),
    m_eta("eta", "eta", this, eta),
    m_aprod("aprod", "aprod", this, aprod),
    m_adet("adet", "adet", this, adet),
    m_atageff("atageff", "atageff", this, atageff),
    m_cacheMgr(this), m_nset(0), m_nsethash(0), m_flags(flags)
{
    // make sure there are no crazy dependencies which would make analytic
    // integrals impossible
    if (!qf.isConstant()) {
	assert(!qf.overlaps(qt) || qt.isConstant());
	assert(!qf.overlaps(Cf) || Cf.isConstant());
	assert(!qf.overlaps(Cfbar) || Cfbar.isConstant());
	assert(!qf.overlaps(etaobs) || etaobs.isConstant());
	assert(!qf.overlaps(etapdf) || etapdf.isConstant());
	assert(!qf.overlaps(tageff) || tageff.isConstant());
	assert(!qf.overlaps(eta) || eta.isConstant());
	assert(!qf.overlaps(aprod) || aprod.isConstant());
	assert(!qf.overlaps(adet) || adet.isConstant());
	assert(!qf.overlaps(atageff) || atageff.isConstant());
    }

    if (!qt.isConstant()) {
	assert(!qt.overlaps(qf) || qf.isConstant());
	assert(!qt.overlaps(Cf) || Cf.isConstant());
	assert(!qt.overlaps(Cfbar) || Cfbar.isConstant());
	assert(!qt.overlaps(etaobs) || etaobs.isConstant());
	assert(!qt.overlaps(etapdf) || etapdf.isConstant());
	assert(!qt.overlaps(tageff) || tageff.isConstant());
	assert(!qt.overlaps(eta) || eta.isConstant());
	assert(!qt.overlaps(aprod) || aprod.isConstant());
	assert(!qt.overlaps(adet) || adet.isConstant());
	assert(!qt.overlaps(atageff) || atageff.isConstant());
    }

    if (!Cf.isConstant()) {
	assert(!Cf.overlaps(qf) || qf.isConstant());
	assert(!Cf.overlaps(qt) || qt.isConstant());
	assert(!Cf.overlaps(etaobs) || etaobs.isConstant());
	assert(!Cf.overlaps(etapdf) || etapdf.isConstant());
	assert(!Cf.overlaps(tageff) || tageff.isConstant());
	assert(!Cf.overlaps(eta) || eta.isConstant());
	assert(!Cf.overlaps(aprod) || aprod.isConstant());
	assert(!Cf.overlaps(adet) || adet.isConstant());
	assert(!Cf.overlaps(atageff) || atageff.isConstant());
    }

    if (!Cfbar.isConstant()) {
	assert(!Cfbar.overlaps(qf) || qf.isConstant());
	assert(!Cfbar.overlaps(qt) || qt.isConstant());
	assert(!Cfbar.overlaps(etaobs) || etaobs.isConstant());
	assert(!Cfbar.overlaps(etapdf) || etapdf.isConstant());
	assert(!Cfbar.overlaps(tageff) || tageff.isConstant());
	assert(!Cfbar.overlaps(eta) || eta.isConstant());
	assert(!Cfbar.overlaps(aprod) || aprod.isConstant());
	assert(!Cfbar.overlaps(adet) || adet.isConstant());
	assert(!Cfbar.overlaps(atageff) || atageff.isConstant());
    }

    if (!etaobs.isConstant()) {
	assert(!etaobs.overlaps(qf) || qf.isConstant());
	assert(!etaobs.overlaps(qt) || qt.isConstant());
	assert(!etaobs.overlaps(Cf) || Cf.isConstant());
	assert(!etaobs.overlaps(Cfbar) || Cfbar.isConstant());
	assert(etaobs.overlaps(etapdf));
	assert(!etaobs.overlaps(tageff) || tageff.isConstant());
	assert(etaobs.overlaps(eta));
	assert(!etaobs.overlaps(aprod) || aprod.isConstant());
	assert(!etaobs.overlaps(adet) || adet.isConstant());
	assert(!etaobs.overlaps(atageff) || atageff.isConstant());
    }

    if (!etapdf.isConstant()) {
	assert(!etapdf.overlaps(qf) || qf.isConstant());
	assert(!etapdf.overlaps(qt) || qt.isConstant());
	assert(!etapdf.overlaps(Cf) || Cf.isConstant());
	assert(!etapdf.overlaps(Cfbar) || Cfbar.isConstant());
	assert(etapdf.overlaps(etaobs));
	assert(!etapdf.overlaps(tageff) || tageff.isConstant());
	assert(!etapdf.overlaps(aprod) || aprod.isConstant());
	assert(!etapdf.overlaps(adet) || adet.isConstant());
	assert(!etapdf.overlaps(atageff) || atageff.isConstant());
    }

    if (!tageff.isConstant()) {
	assert(!tageff.overlaps(qf) || qf.isConstant());
	assert(!tageff.overlaps(qt) || qt.isConstant());
	assert(!tageff.overlaps(Cf) || Cf.isConstant());
	assert(!tageff.overlaps(Cfbar) || Cfbar.isConstant());
	assert(!tageff.overlaps(etaobs) || etaobs.isConstant());
	assert(!tageff.overlaps(etapdf) || etapdf.isConstant());
	assert(!tageff.overlaps(eta) || eta.isConstant());
	assert(!tageff.overlaps(aprod) || aprod.isConstant());
	assert(!tageff.overlaps(adet) || adet.isConstant());
	assert(!tageff.overlaps(atageff) || atageff.isConstant());
    }

    if (!eta.isConstant()) {
	assert(!eta.overlaps(qf) || qf.isConstant());
	assert(!eta.overlaps(qt) || qt.isConstant());
	assert(!eta.overlaps(Cf) || Cf.isConstant());
	assert(!eta.overlaps(Cfbar) || Cfbar.isConstant());
	assert(eta.overlaps(etaobs));
	assert(!eta.overlaps(tageff) || tageff.isConstant());
	assert(!eta.overlaps(aprod) || aprod.isConstant());
	assert(!eta.overlaps(adet) || adet.isConstant());
	assert(!eta.overlaps(atageff) || atageff.isConstant());
    }

    if (!aprod.isConstant()) {
	assert(!aprod.overlaps(qf) || qf.isConstant());
	assert(!aprod.overlaps(qt) || qt.isConstant());
	assert(!aprod.overlaps(Cf) || Cf.isConstant());
	assert(!aprod.overlaps(Cfbar) || Cfbar.isConstant());
	assert(!aprod.overlaps(etaobs) || etaobs.isConstant());
	assert(!aprod.overlaps(etapdf) || etapdf.isConstant());
	assert(!aprod.overlaps(tageff) || tageff.isConstant());
	assert(!aprod.overlaps(eta) || eta.isConstant());
	assert(!aprod.overlaps(adet) || adet.isConstant());
	assert(!aprod.overlaps(atageff) || atageff.isConstant());
    }

    if (!adet.isConstant()) {
	assert(!adet.overlaps(qf) || qf.isConstant());
	assert(!adet.overlaps(qt) || qt.isConstant());
	assert(!adet.overlaps(Cf) || Cf.isConstant());
	assert(!adet.overlaps(Cfbar) || Cfbar.isConstant());
	assert(!adet.overlaps(etaobs) || etaobs.isConstant());
	assert(!adet.overlaps(etapdf) || etapdf.isConstant());
	assert(!adet.overlaps(tageff) || tageff.isConstant());
	assert(!adet.overlaps(eta) || eta.isConstant());
	assert(!adet.overlaps(aprod) || aprod.isConstant());
	assert(!adet.overlaps(atageff) || atageff.isConstant());
    }

    if (!atageff.isConstant()) {
	assert(!atageff.overlaps(qf) || qf.isConstant());
	assert(!atageff.overlaps(qt) || qt.isConstant());
	assert(!atageff.overlaps(Cf) || Cf.isConstant());
	assert(!atageff.overlaps(Cfbar) || Cfbar.isConstant());
	assert(!atageff.overlaps(etaobs) || etaobs.isConstant());
	assert(!atageff.overlaps(etapdf) || etapdf.isConstant());
	assert(!atageff.overlaps(tageff) || tageff.isConstant());
	assert(!atageff.overlaps(eta) || eta.isConstant());
	assert(!atageff.overlaps(aprod) || aprod.isConstant());
	assert(!atageff.overlaps(adet) || adet.isConstant());
    }
} 

DecRateCoeff::DecRateCoeff(
	const char* name, const char* title, Flags flags,
	RooAbsCategory& qf, RooAbsCategory& qt,
	RooAbsReal& Cf, RooAbsReal& Cfbar,
	RooAbsRealLValue& etaobs, RooAbsPdf& etapdf,
	RooAbsReal& tageff, RooAbsReal& eta, RooAbsReal& etabar,
	RooAbsReal& aprod, RooAbsReal& adet, RooAbsReal& atageff) :
    RooAbsReal(name, title),
    m_qf("qf", "qf", this, qf), m_qt("qt", "qt", this, qt),
    m_Cf("Cf", "Cf", this, Cf), m_Cfbar("Cfbar", "Cfbar", this, Cfbar),
    m_etaobs("etaobs", "etaobs", this, etaobs),
    m_etapdf("etapdf", "etapdf", this, etapdf),
    m_etapdfutinstance(
		(std::string(etapdf.GetName()) + "_untagged").c_str(),
		(std::string(etapdf.GetName()) + "_untagged").c_str(),
		RooArgSet(etaobs)),
    m_etapdfut("etapdfut", "etapdfut", this, m_etapdfutinstance),
    m_tageff("tageff", "tageff", this, tageff),
    m_eta("eta", "eta", this, eta),
    m_etabar("etabar", "etabar", this, etabar),
    m_aprod("aprod", "aprod", this, aprod),
    m_adet("adet", "adet", this, adet),
    m_atageff("atageff", "atageff", this, atageff),
    m_cacheMgr(this), m_nset(0), m_nsethash(0), m_flags(flags)
{
    // make sure there are no crazy dependencies which would make analytic
    // integrals impossible
    if (!qf.isConstant()) {
	assert(!qf.overlaps(qt) || qt.isConstant());
	assert(!qf.overlaps(Cf) || Cf.isConstant());
	assert(!qf.overlaps(Cfbar) || Cfbar.isConstant());
	assert(!qf.overlaps(etaobs) || etaobs.isConstant());
	assert(!qf.overlaps(etapdf) || etapdf.isConstant());
	assert(!qf.overlaps(tageff) || tageff.isConstant());
	assert(!qf.overlaps(eta) || eta.isConstant());
	assert(!qf.overlaps(etabar) || etabar.isConstant());
	assert(!qf.overlaps(aprod) || aprod.isConstant());
	assert(!qf.overlaps(adet) || adet.isConstant());
	assert(!qf.overlaps(atageff) || atageff.isConstant());
    }

    if (!qt.isConstant()) {
	assert(!qt.overlaps(qf) || qf.isConstant());
	assert(!qt.overlaps(Cf) || Cf.isConstant());
	assert(!qt.overlaps(Cfbar) || Cfbar.isConstant());
	assert(!qt.overlaps(etaobs) || etaobs.isConstant());
	assert(!qt.overlaps(etapdf) || etapdf.isConstant());
	assert(!qt.overlaps(tageff) || tageff.isConstant());
	assert(!qt.overlaps(eta) || eta.isConstant());
	assert(!qt.overlaps(etabar) || etabar.isConstant());
	assert(!qt.overlaps(aprod) || aprod.isConstant());
	assert(!qt.overlaps(adet) || adet.isConstant());
	assert(!qt.overlaps(atageff) || atageff.isConstant());
    }

    if (!Cf.isConstant()) {
	assert(!Cf.overlaps(qf) || qf.isConstant());
	assert(!Cf.overlaps(qt) || qt.isConstant());
	assert(!Cf.overlaps(etaobs) || etaobs.isConstant());
	assert(!Cf.overlaps(etapdf) || etapdf.isConstant());
	assert(!Cf.overlaps(tageff) || tageff.isConstant());
	assert(!Cf.overlaps(eta) || eta.isConstant());
	assert(!Cf.overlaps(etabar) || etabar.isConstant());
	assert(!Cf.overlaps(aprod) || aprod.isConstant());
	assert(!Cf.overlaps(adet) || adet.isConstant());
	assert(!Cf.overlaps(atageff) || atageff.isConstant());
    }

    if (!Cfbar.isConstant()) {
	assert(!Cfbar.overlaps(qf) || qf.isConstant());
	assert(!Cfbar.overlaps(qt) || qt.isConstant());
	assert(!Cfbar.overlaps(etaobs) || etaobs.isConstant());
	assert(!Cfbar.overlaps(etapdf) || etapdf.isConstant());
	assert(!Cfbar.overlaps(tageff) || tageff.isConstant());
	assert(!Cfbar.overlaps(eta) || eta.isConstant());
	assert(!Cfbar.overlaps(etabar) || etabar.isConstant());
	assert(!Cfbar.overlaps(aprod) || aprod.isConstant());
	assert(!Cfbar.overlaps(adet) || adet.isConstant());
	assert(!Cfbar.overlaps(atageff) || atageff.isConstant());
    }

    if (!etaobs.isConstant()) {
	assert(!etaobs.overlaps(qf) || qf.isConstant());
	assert(!etaobs.overlaps(qt) || qt.isConstant());
	assert(!etaobs.overlaps(Cf) || Cf.isConstant());
	assert(!etaobs.overlaps(Cfbar) || Cfbar.isConstant());
	assert(etaobs.overlaps(etapdf));
	assert(!etaobs.overlaps(tageff) || tageff.isConstant());
	assert(etaobs.overlaps(eta));
	assert(etaobs.overlaps(etabar));
	assert(!etaobs.overlaps(aprod) || aprod.isConstant());
	assert(!etaobs.overlaps(adet) || adet.isConstant());
	assert(!etaobs.overlaps(atageff) || atageff.isConstant());
    }

    if (!etapdf.isConstant()) {
	assert(!etapdf.overlaps(qf) || qf.isConstant());
	assert(!etapdf.overlaps(qt) || qt.isConstant());
	assert(!etapdf.overlaps(Cf) || Cf.isConstant());
	assert(!etapdf.overlaps(Cfbar) || Cfbar.isConstant());
	assert(etapdf.overlaps(etaobs));
	assert(!etapdf.overlaps(tageff) || tageff.isConstant());
	assert(!etapdf.overlaps(aprod) || aprod.isConstant());
	assert(!etapdf.overlaps(adet) || adet.isConstant());
	assert(!etapdf.overlaps(atageff) || atageff.isConstant());
    }

    if (!tageff.isConstant()) {
	assert(!tageff.overlaps(qf) || qf.isConstant());
	assert(!tageff.overlaps(qt) || qt.isConstant());
	assert(!tageff.overlaps(Cf) || Cf.isConstant());
	assert(!tageff.overlaps(Cfbar) || Cfbar.isConstant());
	assert(!tageff.overlaps(etaobs) || etaobs.isConstant());
	assert(!tageff.overlaps(etapdf) || etapdf.isConstant());
	assert(!tageff.overlaps(eta) || eta.isConstant());
	assert(!tageff.overlaps(etabar) || etabar.isConstant());
	assert(!tageff.overlaps(aprod) || aprod.isConstant());
	assert(!tageff.overlaps(adet) || adet.isConstant());
	assert(!tageff.overlaps(atageff) || atageff.isConstant());
    }

    if (!eta.isConstant()) {
	assert(!eta.overlaps(qf) || qf.isConstant());
	assert(!eta.overlaps(qt) || qt.isConstant());
	assert(!eta.overlaps(Cf) || Cf.isConstant());
	assert(!eta.overlaps(Cfbar) || Cfbar.isConstant());
	assert(eta.overlaps(etaobs));
	assert(!eta.overlaps(tageff) || tageff.isConstant());
	assert(!eta.overlaps(aprod) || aprod.isConstant());
	assert(!eta.overlaps(adet) || adet.isConstant());
	assert(!eta.overlaps(atageff) || atageff.isConstant());
    }

    if (!etabar.isConstant()) {
	assert(!etabar.overlaps(qf) || qf.isConstant());
	assert(!etabar.overlaps(qt) || qt.isConstant());
	assert(!etabar.overlaps(Cf) || Cf.isConstant());
	assert(!etabar.overlaps(Cfbar) || Cfbar.isConstant());
	assert(etabar.overlaps(etaobs));
	assert(!etabar.overlaps(tageff) || tageff.isConstant());
	assert(!etabar.overlaps(aprod) || aprod.isConstant());
	assert(!etabar.overlaps(adet) || adet.isConstant());
	assert(!etabar.overlaps(atageff) || atageff.isConstant());
    }

    if (!aprod.isConstant()) {
	assert(!aprod.overlaps(qf) || qf.isConstant());
	assert(!aprod.overlaps(qt) || qt.isConstant());
	assert(!aprod.overlaps(Cf) || Cf.isConstant());
	assert(!aprod.overlaps(Cfbar) || Cfbar.isConstant());
	assert(!aprod.overlaps(etaobs) || etaobs.isConstant());
	assert(!aprod.overlaps(etapdf) || etapdf.isConstant());
	assert(!aprod.overlaps(tageff) || tageff.isConstant());
	assert(!aprod.overlaps(eta) || eta.isConstant());
	assert(!aprod.overlaps(etabar) || etabar.isConstant());
	assert(!aprod.overlaps(adet) || adet.isConstant());
	assert(!aprod.overlaps(atageff) || atageff.isConstant());
    }

    if (!adet.isConstant()) {
	assert(!adet.overlaps(qf) || qf.isConstant());
	assert(!adet.overlaps(qt) || qt.isConstant());
	assert(!adet.overlaps(Cf) || Cf.isConstant());
	assert(!adet.overlaps(Cfbar) || Cfbar.isConstant());
	assert(!adet.overlaps(etaobs) || etaobs.isConstant());
	assert(!adet.overlaps(etapdf) || etapdf.isConstant());
	assert(!adet.overlaps(tageff) || tageff.isConstant());
	assert(!adet.overlaps(eta) || eta.isConstant());
	assert(!adet.overlaps(etabar) || etabar.isConstant());
	assert(!adet.overlaps(aprod) || aprod.isConstant());
	assert(!adet.overlaps(atageff) || atageff.isConstant());
    }

    if (!atageff.isConstant()) {
	assert(!atageff.overlaps(qf) || qf.isConstant());
	assert(!atageff.overlaps(qt) || qt.isConstant());
	assert(!atageff.overlaps(Cf) || Cf.isConstant());
	assert(!atageff.overlaps(Cfbar) || Cfbar.isConstant());
	assert(!atageff.overlaps(etaobs) || etaobs.isConstant());
	assert(!atageff.overlaps(etapdf) || etapdf.isConstant());
	assert(!atageff.overlaps(tageff) || tageff.isConstant());
	assert(!atageff.overlaps(eta) || eta.isConstant());
	assert(!atageff.overlaps(etabar) || etabar.isConstant());
	assert(!atageff.overlaps(aprod) || aprod.isConstant());
	assert(!atageff.overlaps(adet) || adet.isConstant());
    }
} 

DecRateCoeff::DecRateCoeff(
	const char* name, const char* title, Flags flags,
	RooAbsCategory& qf, RooAbsCategory& qt,
	RooAbsReal& Cf, RooAbsReal& Cfbar,
	RooAbsReal& tageff, RooAbsReal& eta,
	RooAbsReal& aprod, RooAbsReal& adet,
	RooAbsReal& atageff) :
    RooAbsReal(name, title),
    m_qf("qf", "qf", this, qf), m_qt("qt", "qt", this, qt),
    m_Cf("Cf", "Cf", this, Cf), m_Cfbar("Cfbar", "Cfbar", this, Cfbar),
    m_tageff("tageff", "tageff", this, tageff),
    m_eta("eta", "eta", this, eta),
    m_aprod("aprod", "aprod", this, aprod),
    m_adet("adet", "adet", this, adet),
    m_atageff("atageff", "atageff", this, atageff),
    m_cacheMgr(this), m_nset(0), m_nsethash(0), m_flags(flags)
{
    // make sure there are no crazy dependencies which would make analytic
    // integrals impossible
    if (!qf.isConstant()) {
	assert(!qf.overlaps(qt) || qt.isConstant());
	assert(!qf.overlaps(Cf) || Cf.isConstant());
	assert(!qf.overlaps(Cfbar) || Cfbar.isConstant());
	assert(!qf.overlaps(tageff) || tageff.isConstant());
	assert(!qf.overlaps(eta) || eta.isConstant());
	assert(!qf.overlaps(aprod) || aprod.isConstant());
	assert(!qf.overlaps(adet) || adet.isConstant());
	assert(!qf.overlaps(atageff) || atageff.isConstant());
    }

    if (!qt.isConstant()) {
	assert(!qt.overlaps(qf) || qf.isConstant());
	assert(!qt.overlaps(Cf) || Cf.isConstant());
	assert(!qt.overlaps(Cfbar) || Cfbar.isConstant());
	assert(!qt.overlaps(tageff) || tageff.isConstant());
	assert(!qt.overlaps(eta) || eta.isConstant());
	assert(!qt.overlaps(aprod) || aprod.isConstant());
	assert(!qt.overlaps(adet) || adet.isConstant());
	assert(!qt.overlaps(atageff) || atageff.isConstant());
    }

    if (!Cf.isConstant()) {
	assert(!Cf.overlaps(qf) || qf.isConstant());
	assert(!Cf.overlaps(qt) || qt.isConstant());
	assert(!Cf.overlaps(tageff) || tageff.isConstant());
	assert(!Cf.overlaps(eta) || eta.isConstant());
	assert(!Cf.overlaps(aprod) || aprod.isConstant());
	assert(!Cf.overlaps(adet) || adet.isConstant());
	assert(!Cf.overlaps(atageff) || atageff.isConstant());
    }

    if (!Cfbar.isConstant()) {
	assert(!Cfbar.overlaps(qf) || qf.isConstant());
	assert(!Cfbar.overlaps(qt) || qt.isConstant());
	assert(!Cfbar.overlaps(tageff) || tageff.isConstant());
	assert(!Cfbar.overlaps(eta) || eta.isConstant());
	assert(!Cfbar.overlaps(aprod) || aprod.isConstant());
	assert(!Cfbar.overlaps(adet) || adet.isConstant());
	assert(!Cfbar.overlaps(atageff) || atageff.isConstant());
    }

    if (!tageff.isConstant()) {
	assert(!tageff.overlaps(qf) || qf.isConstant());
	assert(!tageff.overlaps(qt) || qt.isConstant());
	assert(!tageff.overlaps(Cf) || Cf.isConstant());
	assert(!tageff.overlaps(Cfbar) || Cfbar.isConstant());
	assert(!tageff.overlaps(eta) || eta.isConstant());
	assert(!tageff.overlaps(aprod) || aprod.isConstant());
	assert(!tageff.overlaps(adet) || adet.isConstant());
	assert(!tageff.overlaps(atageff) || atageff.isConstant());
    }

    if (!eta.isConstant()) {
	assert(!eta.overlaps(qf) || qf.isConstant());
	assert(!eta.overlaps(qt) || qt.isConstant());
	assert(!eta.overlaps(Cf) || Cf.isConstant());
	assert(!eta.overlaps(Cfbar) || Cfbar.isConstant());
	assert(!eta.overlaps(tageff) || tageff.isConstant());
	assert(!eta.overlaps(aprod) || aprod.isConstant());
	assert(!eta.overlaps(adet) || adet.isConstant());
	assert(!eta.overlaps(atageff) || atageff.isConstant());
    }

    if (!aprod.isConstant()) {
	assert(!aprod.overlaps(qf) || qf.isConstant());
	assert(!aprod.overlaps(qt) || qt.isConstant());
	assert(!aprod.overlaps(Cf) || Cf.isConstant());
	assert(!aprod.overlaps(Cfbar) || Cfbar.isConstant());
	assert(!aprod.overlaps(tageff) || tageff.isConstant());
	assert(!aprod.overlaps(eta) || eta.isConstant());
	assert(!aprod.overlaps(adet) || adet.isConstant());
	assert(!aprod.overlaps(atageff) || atageff.isConstant());
    }

    if (!adet.isConstant()) {
	assert(!adet.overlaps(qf) || qf.isConstant());
	assert(!adet.overlaps(qt) || qt.isConstant());
	assert(!adet.overlaps(Cf) || Cf.isConstant());
	assert(!adet.overlaps(Cfbar) || Cfbar.isConstant());
	assert(!adet.overlaps(tageff) || tageff.isConstant());
	assert(!adet.overlaps(eta) || eta.isConstant());
	assert(!adet.overlaps(aprod) || aprod.isConstant());
	assert(!adet.overlaps(atageff) || atageff.isConstant());
    }

    if (!atageff.isConstant()) {
	assert(!atageff.overlaps(qf) || qf.isConstant());
	assert(!atageff.overlaps(qt) || qt.isConstant());
	assert(!atageff.overlaps(Cf) || Cf.isConstant());
	assert(!atageff.overlaps(Cfbar) || Cfbar.isConstant());
	assert(!atageff.overlaps(tageff) || tageff.isConstant());
	assert(!atageff.overlaps(eta) || eta.isConstant());
	assert(!atageff.overlaps(aprod) || aprod.isConstant());
	assert(!atageff.overlaps(adet) || adet.isConstant());
    }
}

DecRateCoeff::DecRateCoeff(
	const char* name, const char* title, Flags flags,
	RooAbsCategory& qf, RooAbsCategory& qt,
	RooAbsReal& Cf, RooAbsReal& Cfbar,
	RooAbsReal& tageff, RooAbsReal& eta, RooAbsReal& etabar,
	RooAbsReal& aprod, RooAbsReal& adet, RooAbsReal& atageff) :
    RooAbsReal(name, title),
    m_qf("qf", "qf", this, qf), m_qt("qt", "qt", this, qt),
    m_Cf("Cf", "Cf", this, Cf), m_Cfbar("Cfbar", "Cfbar", this, Cfbar),
    m_tageff("tageff", "tageff", this, tageff),
    m_eta("eta", "eta", this, eta),
    m_etabar("etabar", "etabar", this, etabar),
    m_aprod("aprod", "aprod", this, aprod),
    m_adet("adet", "adet", this, adet),
    m_atageff("atageff", "atageff", this, atageff),
    m_cacheMgr(this), m_nset(0), m_nsethash(0), m_flags(flags)
{
    // make sure there are no crazy dependencies which would make analytic
    // integrals impossible
    if (!qf.isConstant()) {
	assert(!qf.overlaps(qt) || qt.isConstant());
	assert(!qf.overlaps(Cf) || Cf.isConstant());
	assert(!qf.overlaps(Cfbar) || Cfbar.isConstant());
	assert(!qf.overlaps(tageff) || tageff.isConstant());
	assert(!qf.overlaps(eta) || eta.isConstant());
	assert(!qf.overlaps(etabar) || etabar.isConstant());
	assert(!qf.overlaps(aprod) || aprod.isConstant());
	assert(!qf.overlaps(adet) || adet.isConstant());
	assert(!qf.overlaps(atageff) || atageff.isConstant());
    }

    if (!qt.isConstant()) {
	assert(!qt.overlaps(qf) || qf.isConstant());
	assert(!qt.overlaps(Cf) || Cf.isConstant());
	assert(!qt.overlaps(Cfbar) || Cfbar.isConstant());
	assert(!qt.overlaps(tageff) || tageff.isConstant());
	assert(!qt.overlaps(eta) || eta.isConstant());
	assert(!qt.overlaps(etabar) || etabar.isConstant());
	assert(!qt.overlaps(aprod) || aprod.isConstant());
	assert(!qt.overlaps(adet) || adet.isConstant());
	assert(!qt.overlaps(atageff) || atageff.isConstant());
    }

    if (!Cf.isConstant()) {
	assert(!Cf.overlaps(qf) || qf.isConstant());
	assert(!Cf.overlaps(qt) || qt.isConstant());
	assert(!Cf.overlaps(tageff) || tageff.isConstant());
	assert(!Cf.overlaps(eta) || eta.isConstant());
	assert(!Cf.overlaps(etabar) || etabar.isConstant());
	assert(!Cf.overlaps(aprod) || aprod.isConstant());
	assert(!Cf.overlaps(adet) || adet.isConstant());
	assert(!Cf.overlaps(atageff) || atageff.isConstant());
    }

    if (!Cfbar.isConstant()) {
	assert(!Cfbar.overlaps(qf) || qf.isConstant());
	assert(!Cfbar.overlaps(qt) || qt.isConstant());
	assert(!Cfbar.overlaps(tageff) || tageff.isConstant());
	assert(!Cfbar.overlaps(eta) || eta.isConstant());
	assert(!Cfbar.overlaps(etabar) || etabar.isConstant());
	assert(!Cfbar.overlaps(aprod) || aprod.isConstant());
	assert(!Cfbar.overlaps(adet) || adet.isConstant());
	assert(!Cfbar.overlaps(atageff) || atageff.isConstant());
    }

    if (!tageff.isConstant()) {
	assert(!tageff.overlaps(qf) || qf.isConstant());
	assert(!tageff.overlaps(qt) || qt.isConstant());
	assert(!tageff.overlaps(Cf) || Cf.isConstant());
	assert(!tageff.overlaps(Cfbar) || Cfbar.isConstant());
	assert(!tageff.overlaps(eta) || eta.isConstant());
	assert(!tageff.overlaps(etabar) || etabar.isConstant());
	assert(!tageff.overlaps(aprod) || aprod.isConstant());
	assert(!tageff.overlaps(adet) || adet.isConstant());
	assert(!tageff.overlaps(atageff) || atageff.isConstant());
    }

    if (!eta.isConstant()) {
	assert(!eta.overlaps(qf) || qf.isConstant());
	assert(!eta.overlaps(qt) || qt.isConstant());
	assert(!eta.overlaps(Cf) || Cf.isConstant());
	assert(!eta.overlaps(Cfbar) || Cfbar.isConstant());
	assert(!eta.overlaps(tageff) || tageff.isConstant());
	assert(!eta.overlaps(aprod) || aprod.isConstant());
	assert(!eta.overlaps(adet) || adet.isConstant());
	assert(!eta.overlaps(atageff) || atageff.isConstant());
    }

    if (!etabar.isConstant()) {
	assert(!etabar.overlaps(qf) || qf.isConstant());
	assert(!etabar.overlaps(qt) || qt.isConstant());
	assert(!etabar.overlaps(Cf) || Cf.isConstant());
	assert(!etabar.overlaps(Cfbar) || Cfbar.isConstant());
	assert(!etabar.overlaps(tageff) || tageff.isConstant());
	assert(!etabar.overlaps(aprod) || aprod.isConstant());
	assert(!etabar.overlaps(adet) || adet.isConstant());
	assert(!etabar.overlaps(atageff) || atageff.isConstant());
    }

    if (!aprod.isConstant()) {
	assert(!aprod.overlaps(qf) || qf.isConstant());
	assert(!aprod.overlaps(qt) || qt.isConstant());
	assert(!aprod.overlaps(Cf) || Cf.isConstant());
	assert(!aprod.overlaps(Cfbar) || Cfbar.isConstant());
	assert(!aprod.overlaps(tageff) || tageff.isConstant());
	assert(!aprod.overlaps(eta) || eta.isConstant());
	assert(!aprod.overlaps(etabar) || etabar.isConstant());
	assert(!aprod.overlaps(adet) || adet.isConstant());
	assert(!aprod.overlaps(atageff) || atageff.isConstant());
    }

    if (!adet.isConstant()) {
	assert(!adet.overlaps(qf) || qf.isConstant());
	assert(!adet.overlaps(qt) || qt.isConstant());
	assert(!adet.overlaps(Cf) || Cf.isConstant());
	assert(!adet.overlaps(Cfbar) || Cfbar.isConstant());
	assert(!adet.overlaps(tageff) || tageff.isConstant());
	assert(!adet.overlaps(eta) || eta.isConstant());
	assert(!adet.overlaps(etabar) || etabar.isConstant());
	assert(!adet.overlaps(aprod) || aprod.isConstant());
	assert(!adet.overlaps(atageff) || atageff.isConstant());
    }

    if (!atageff.isConstant()) {
	assert(!atageff.overlaps(qf) || qf.isConstant());
	assert(!atageff.overlaps(qt) || qt.isConstant());
	assert(!atageff.overlaps(Cf) || Cf.isConstant());
	assert(!atageff.overlaps(Cfbar) || Cfbar.isConstant());
	assert(!atageff.overlaps(tageff) || tageff.isConstant());
	assert(!atageff.overlaps(eta) || eta.isConstant());
	assert(!atageff.overlaps(etabar) || etabar.isConstant());
	assert(!atageff.overlaps(aprod) || aprod.isConstant());
	assert(!atageff.overlaps(adet) || adet.isConstant());
    }
} 

DecRateCoeff::DecRateCoeff(
	const DecRateCoeff& other, const char* name) : 
    RooAbsReal(other, name),
    m_qf("qf", this, other.m_qf), m_qt("qt", this, other.m_qt),
    m_Cf("Cf", this, other.m_Cf), m_Cfbar("Cfbar", this, other.m_Cfbar),
    m_etaobs("etaobs", this, other.m_etaobs),
    m_etapdf("etapdf", this, other.m_etapdf),
    m_etapdfutinstance(other.m_etapdfutinstance),
    m_etapdfut("etapdfut", this, other.m_etapdfut),
    m_tageff("tageff", this, other.m_tageff),
    m_eta("eta", this, other.m_eta), m_etabar("etabar", this, other.m_etabar),
    m_aprod("aprod", this, other.m_aprod),
    m_adet("adet", this, other.m_adet),
    m_atageff("atageff", this, other.m_atageff),
    m_cacheMgr(other.m_cacheMgr, this), m_nsets(other.m_nsets),
    m_nset(other.m_nset), m_nsethash(other.m_nsethash), m_flags(other.m_flags)
{
    // no need to verify constraints as in other constructors - we make a copy
    // of something that has already been verified
    if (m_etaobs.absArg()) {
	m_etapdfut.setArg(m_etapdfutinstance);
    }
} 

DecRateCoeff::~DecRateCoeff()
{ }

TObject* DecRateCoeff::clone(const char* newname) const
{ return new DecRateCoeff(*this, newname); }

Double_t DecRateCoeff::getValV(const RooArgSet* nset) const
{
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
	    matchArgs(*nset, ns, m_qt.arg());
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
    if (&dep == m_qf.absArg()) return kTRUE;
    if (&dep == m_qt.absArg()) return kTRUE;
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
    // perform analytical integrals
    // we know how to integrate over etaobs, qf, qt
    if (m_etaobs.absArg()) {
	matchArgs(allVars, anaIntVars, m_etaobs);
    }
    matchArgs(allVars, anaIntVars, m_qf);
    matchArgs(allVars, anaIntVars, m_qt);
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
    m_etaintpdftagged(0),
    m_etaintprodpdfmistagtaggedplus(0),
    m_etaintprodpdfmistagtaggedminus(0),
    m_etaintpdfuntagged(0),
    m_rangeName(rangeName ? rangeName->GetName() : 0),
    m_parent(parent), m_flags(None)
{
    m_prodcachedval[0] = m_prodcachedval[1] =
	std::numeric_limits<double>::quiet_NaN();
    m_workRange[0] = m_workRange[1] =
	std::make_pair<RooRealVar*, RooRealVar*>(0, 0);
    // set flag for qf/qt integration
    if (iset.find(parent.m_qf.arg()))
	m_flags = static_cast<Flags>(m_flags | IntQf);
    if (iset.find(parent.m_qt.arg()))
	m_flags = static_cast<Flags>(m_flags | IntQt);

    // initialise mistag (eta) part of cache element (if needed)
    if (parent.m_etaobs.absArg()) {
	// case with per-event mistag
	//
	// massage integration set, so we do the right thing
	RooArgSet etaiset;
	if (iset.find(parent.m_etaobs.arg())) {
	    etaiset.add(parent.m_etaobs.arg());
	    m_flags = static_cast<Flags>(m_flags | IntEta);
	    if (m_parent.m_etapdf.arg().isBinnedDistribution(etaiset))
		m_flags = static_cast<Flags>(m_flags | ProdIntBinned);
	}
	// massage normalisation set as well (if present)
	if (nset && nset->find(m_parent.m_etaobs.arg())) {
	    m_nset.add(m_parent.m_etaobs.arg());
	    m_flags = static_cast<Flags>(m_flags | NormEta);
	}
	const std::string newname(std::string(parent.GetName()) + "_prod_eta");
	RooAbsReal* prod = new RooProduct(newname.c_str(), newname.c_str(),
		RooArgList(parent.m_etapdf.arg(), parent.m_eta.arg()));
	assert(prod);
	RooAbsReal* prod2 = 0;
	if (m_parent.m_etabar.absArg()) {
	    // different calibrations for B/Bbar, need the second product
	    const std::string newname2(newname + "bar");
	    prod2 = new RooProduct(newname2.c_str(), newname2.c_str(),
		    RooArgList(parent.m_etapdf.arg(), parent.m_etabar.arg()));
	    assert(prod2);
	}
	if (m_flags & IntEta) {
	    // integrate over eta
	    m_etaintpdftagged = parent.m_etapdf.arg().createIntegral(
		    etaiset, nset ? &m_nset : 0, 0, m_rangeName);
	    assert(m_etaintpdftagged);
	    m_etaintprodpdfmistagtaggedplus = prod->createIntegral(
		    etaiset, nset ? &m_nset : 0, 0, m_rangeName);
	    assert(m_etaintprodpdfmistagtaggedplus);
	    m_etaintprodpdfmistagtaggedplus->addOwnedComponents(*prod);
	    if (prod2) {
		m_etaintprodpdfmistagtaggedminus = prod2->createIntegral(
			etaiset, nset ? &m_nset : 0, 0, m_rangeName);
		assert(m_etaintprodpdfmistagtaggedminus);
		m_etaintprodpdfmistagtaggedminus->addOwnedComponents(*prod2);
	    }
	    m_etaintpdfuntagged = parent.m_etapdfut.arg().createIntegral(
		    etaiset, nset ? &m_nset : 0, 0, m_rangeName);
	    assert(m_etaintpdfuntagged);
	} else {
	    m_etaintpdftagged = const_cast<RooAbsReal*>(
		    &parent.m_etapdf.arg());
	    m_etaintprodpdfmistagtaggedplus = prod;
	    m_etaintprodpdfmistagtaggedminus = prod2;
	    m_etaintpdfuntagged = const_cast<RooAbsReal*>(
		    &parent.m_etapdfut.arg());
	}
    } else {
	// case with average mistag
	m_etaintprodpdfmistagtaggedplus = const_cast<RooAbsReal*>(
		&parent.m_eta.arg());
	if (m_parent.m_etabar.absArg()) {
	    // different calibrations for B/Bbar
	    m_etaintprodpdfmistagtaggedminus = const_cast<RooAbsReal*>(
		    &parent.m_etabar.arg());
	}
    }
    if (m_flags & ProdIntBinned) {
	// patch configuration: set up binned evaluation of product integral
	setupBinnedProductIntegral(
		m_etaintprodpdfmistagtaggedplus, m_parent.m_eta.arg(), 0);
	if (m_etaintprodpdfmistagtaggedminus)
	    setupBinnedProductIntegral(
		    m_etaintprodpdfmistagtaggedminus, m_parent.m_etabar.arg(), 1);
    }
}

void DecRateCoeff::CacheElem::setupBinnedProductIntegral(
	RooAbsReal* &prod, const RooAbsReal& eta, int idx)
{
    // define name of working range
    m_workRangeName[idx] += prod->GetName();
    m_workRangeName[idx] += "_workRange";
    // get binning
    const double rxmin = m_parent.m_etaobs.min(m_rangeName);
    const double rxmax = m_parent.m_etaobs.max(m_rangeName);
    if (m_etabins.empty()) {
	// bin vector can be shared between B/Bbar
	std::auto_ptr<std::list<Double_t> > bins(
		dynamic_cast<const RooAbsPdf&>(
		    m_parent.m_etapdf.arg()).binBoundaries(
		    const_cast<RooAbsRealLValue&>(
			dynamic_cast<const RooAbsRealLValue&>(
			    m_parent.m_etaobs.arg())),
		    m_parent.m_etaobs.min(), m_parent.m_etaobs.max()));
	m_etabins.reserve(bins->size());
	for (std::list<Double_t>::const_iterator
		lo = bins->begin(), hi = ++(bins->begin());
		bins->end() != hi; ++hi, ++lo) {
	    if (*hi < rxmin) continue;
	    if (*lo > rxmax) break;
	    const double xmin = std::max(*lo, rxmin);
	    const double xmax = std::min(*hi, rxmax);
	    if (m_etabins.empty()) m_etabins.push_back(xmin);
	    m_etabins.push_back(xmax);
	}
	assert(m_etabins.size() >= 2);
	assert(std::abs(m_etabins.front() - rxmin) <
		16. * std::numeric_limits<double>::epsilon() *
		std::max(std::abs(rxmin), 1.));
	assert(std::abs(m_etabins.back() - rxmax) <
		16. * std::numeric_limits<double>::epsilon() *
		std::max(std::abs(rxmax), 1.));
    }
    // set up required binning
    m_workRange[idx].first = new RooRealVar(
	    (m_workRangeName[idx] + "_min").c_str(),
	    (m_workRangeName[idx] + "_min").c_str(), rxmin, rxmin, rxmax);
    m_workRange[idx].second = new RooRealVar(
	    (m_workRangeName[idx] + "_max").c_str(),
	    (m_workRangeName[idx] + "_max").c_str(), rxmax, rxmin, rxmax);
    // we need to operate on a clone of etaobs with our custom binning, or
    // things die a horrible death when using more than one CPU
    RooRealVar* etaobs = dynamic_cast<RooRealVar*>(
	    m_parent.m_etaobs.arg().clone(0));
    assert(etaobs);
    etaobs->setRange(
	    m_workRangeName[idx].c_str(),
	    *m_workRange[idx].first, *m_workRange[idx].second);
    // ok get rid of prod, and set to what is needed for binned evaluation
    std::string prodname(prod->GetName());
    delete prod;
    RooArgSet etaiset(*etaobs);
    RooAbsPdf* etapdf = 0;
    {
	std::string str(m_parent.m_etapdf.arg().GetName());
	str += "_for_";
	str += m_workRangeName[idx];
	RooCustomizer cust(m_parent.m_etapdf.arg(), str.c_str());
	cust.replaceArg(m_parent.m_etaobs.arg(), *etaobs);
	etapdf = dynamic_cast<RooAbsPdf*>(cust.build());
	assert(etapdf);
    }
    RooAbsReal* etaa = 0;
    {
	std::string str(eta.GetName());
	str += "_for_";
	str += m_workRangeName[idx];
	RooCustomizer cust(eta, str.c_str());
	cust.replaceArg(m_parent.m_etaobs.arg(), *etaobs);
	etaa = dynamic_cast<RooAbsReal*>(cust.build());
	assert(etaa);
    }

    RooAbsReal* pdf = etapdf->createIntegral(etaiset,
	    (m_flags & NormEta) ? &m_nset : 0, 0, m_workRangeName[idx].c_str());
    assert(pdf);
    RooAbsReal* etai = etaa->createIntegral(etaiset,
	    (m_flags & NormEta) ? &m_nset : 0, 0, m_workRangeName[idx].c_str());
    assert(etai);
    prod = new RooProduct(
	    prodname.c_str(), prodname.c_str(), RooArgList(*pdf, *etai));
    assert(prod);
    // make sure we do not leak
    prod->addOwnedComponents(*pdf);
    prod->addOwnedComponents(*etai);
    prod->addOwnedComponents(*etapdf);
    prod->addOwnedComponents(*etaa);
    prod->addOwnedComponents(*etaobs);
    // all done
}

DecRateCoeff::CacheElem::~CacheElem()
{
    if (m_etaintpdftagged) {
	// per-event mistag case
	// we own the (integral of the) product of mistag pdf and per-event
	// mistag in any case
	delete m_etaintprodpdfmistagtaggedplus;
	delete m_etaintprodpdfmistagtaggedminus;
	// work out if we own m_etaintpdftagged and m_etaintpdfuntagged -
	// that is only the case if integrate over mistag
	if (m_flags & IntEta) {
	    delete m_etaintpdftagged;
	    delete m_etaintpdfuntagged;
	}
	delete m_workRange[0].first;
	delete m_workRange[0].second;
	delete m_workRange[1].first;
	delete m_workRange[1].second;
    } else {
	// we're in the average mistag case, so we do not own the pointers and
	// must not delete them...
    }
}

RooArgList DecRateCoeff::CacheElem::containedArgs(Action)
{
    RooArgList retVal;
    if (m_etaintpdftagged)
	retVal.add(*m_etaintpdftagged);
    if (m_etaintpdfuntagged)
	retVal.add(*m_etaintpdfuntagged);
    if (m_etaintprodpdfmistagtaggedplus)
	retVal.add(*m_etaintprodpdfmistagtaggedplus);
    if (m_etaintprodpdfmistagtaggedminus)
	retVal.add(*m_etaintprodpdfmistagtaggedminus);
    if (m_workRange[0].first)
	retVal.add(*m_workRange[0].first);
    if (m_workRange[0].second)
	retVal.add(*m_workRange[0].second);
    if (m_workRange[1].first)
	retVal.add(*m_workRange[1].first);
    if (m_workRange[1].second)
	retVal.add(*m_workRange[1].second);
    if (!(m_flags & IntQf)) retVal.add(m_parent.m_qf.arg());
    if (!(m_flags & IntQt)) retVal.add(m_parent.m_qt.arg());
    if (!(m_flags & IntEta)) {
	if (m_parent.m_etaobs.absArg()) retVal.add(m_parent.m_etaobs.arg());
	if (m_parent.m_etapdf.absArg()) retVal.add(m_parent.m_etapdf.arg());
	if (m_parent.m_etapdfut.absArg()) retVal.add(m_parent.m_etapdfut.arg());
    }
    retVal.add(m_parent.m_tageff.arg());
    retVal.add(m_parent.m_eta.arg());
    if (m_parent.m_etabar.absArg()) retVal.add(m_parent.m_etabar.arg());
    retVal.add(m_parent.m_aprod.arg());
    retVal.add(m_parent.m_adet.arg());
    retVal.add(m_parent.m_atageff.arg());
    return retVal;
}

double DecRateCoeff::CacheElem::qfpdf(const int qf) const
{
    assert(std::abs(qf) == 1);
    return 1. + double(qf) * double(m_parent.m_aprod);
}

double DecRateCoeff::CacheElem::qtetapdf(const int qf, const int qt,
	const double alphapp, const double alphapm,
	const double alphamp, const double alphamm) const
{
    assert(std::abs(qf) == 1);
    assert(std::abs(qt) <= 1);
    const double cp((qf > 0) ? alphapp : alphamp);
    const double cm((qf > 0) ? alphapm : alphamm);
    const double ap(m_parent.m_aprod), at(m_parent.m_atageff);
    const double eps(m_parent.m_tageff);
    switch (qt) {
	case -1:
	    {
		const double m(etaintpdftagged());
		const double mpp(etaintprodpdfmistagtagged(+1));
		const double mpm(etaintprodpdfmistagtagged(-1));
		return
		    (1. + ap) * eps * (1. + at) * mpp * cp +
		    (1. - ap) * eps * (1. - at) * (m - mpm) * cm;
	    }
	case 0:
	    return etaintpdfuntagged() * (
		    (1. + ap) * (1. - eps * (1. + at)) * cp +
		    (1. - ap) * (1. - eps * (1. - at)) * cm);
	case +1:
	    {
		const double m(etaintpdftagged());
		const double mpp(etaintprodpdfmistagtagged(+1));
		const double mpm(etaintprodpdfmistagtagged(-1));
		return
		    (1. + ap) * eps * (1. + at) * (m - mpp) * cp +
		    (1. - ap) * eps * (1. - at) * mpm * cm;
	    }
	default:
	    // should never get here
	    assert(1 == 0);
	    return std::numeric_limits<double>::quiet_NaN();
    }
}

double DecRateCoeff::CacheElem::eval(
	const double alphapp, const double alphapm,
	const double alphamp, const double alphamm) const
{
    if (!(m_flags & (IntQf | IntQt))) {
	return qfpdf(int(m_parent.m_qf)) *
	    qtetapdf(int(m_parent.m_qf), int(m_parent.m_qt),
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
	    return retVal * qtetapdf(1, int(m_parent.m_qt),
		    alphapp, alphapm, alphamp, alphamm);
	} else {
	    const int iqf = int(m_parent.m_qf);
	    double retVal = 0.;
	    while (RooCatType* qfty =
		    reinterpret_cast<RooCatType*>(qfit->Next())) {
		// skip qf states not in range if the category has a range of
		// that name (if there's no range of rangeName in qf, we use
		// all qf states)
		if (qf.hasRange(m_rangeName) &&
			!qf.isStateInRange(m_rangeName, qfty->GetName()))
		    continue;
		retVal += qfpdf(iqf) *
		    qtetapdf(iqf, qfty->getVal(),
			    alphapp, alphapm, alphamp, alphamm);
	    }
	    return retVal;
	}
    } else if (!(m_flags & IntQf) && (m_flags & IntQt)) {
	double retVal = 0.;
	const int iqf(m_parent.m_qf);
	// loop over qt states
	const RooCategory& qt(
		dynamic_cast<const RooCategory&>(m_parent.m_qt.arg()));
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
		    dynamic_cast<const RooCategory&>(m_parent.m_qt.arg()));
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
			dynamic_cast<const RooCategory&>(m_parent.m_qt.arg()));
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

double DecRateCoeff::CacheElem::etaintprodpdfmistagtagged(int qt) const
{
    assert(0 != qt);
    if (!m_etaintprodpdfmistagtaggedplus)
	return 1.0;
    RooAbsReal* prodint = (m_etaintprodpdfmistagtaggedminus && qt < 0) ?
	m_etaintprodpdfmistagtaggedminus : m_etaintprodpdfmistagtaggedplus;
    const int cacheidx = (m_etaintprodpdfmistagtaggedminus && qt < 0) ? 1 : 0;
    const RooArgSet* nset = (m_flags & NormEta) ? &m_nset : 0;
    if (!(m_flags & ProdIntBinned))
	return prodint->getValV(nset);
    // use binned approximation to integral
    if (m_prodcachedval[cacheidx] == m_prodcachedval[cacheidx] &&
	    !prodint->isValueOrShapeDirtyAndClear())
	return m_prodcachedval[cacheidx];
    double retVal = 0.;
    for (std::vector<double>::const_iterator it = m_etabins.begin() + 1;
	    m_etabins.end() != it; ++it) {
	const double xmin(*(it - 1)), xmax(*it);
	m_workRange[cacheidx].first->setVal(xmin);
	m_workRange[cacheidx].second->setVal(xmax);
	retVal += prodint->getValV(nset);
    }
    return (m_prodcachedval[cacheidx] = retVal);
}
}
}

// vim: sw=4:tw=78:ft=cpp
