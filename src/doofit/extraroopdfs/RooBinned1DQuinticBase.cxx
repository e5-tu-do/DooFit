/**
 * @file RooBinned1DQuinticBase.cxx
 *
 * @author Manuel Tobias Schiller <manuel.schiller@nikhef.nl>
 * @date 2012-09-09
 */
#include <cmath>
#include <memory>
#include <limits>
#include <iostream>
#include <algorithm>

#include <TH1.h>
#include <RooArgSet.h>
#include <RooRealVar.h>

#include "B2DXFitters/RooBinned1DQuinticBase.h"

template<class BASE>
RooBinned1DQuinticBase<BASE>::BinSizeException::~BinSizeException() throw ()
{ }

template<class BASE>
const char* RooBinned1DQuinticBase<BASE>::BinSizeException::what() const throw ()
{ return "Variable-sized bins not supported!"; }

template<class BASE>
RooBinned1DQuinticBase<BASE>::~RooBinned1DQuinticBase()
{ }

template<class BASE>
RooBinned1DQuinticBase<BASE>::RooBinned1DQuinticBase(
	const RooBinned1DQuinticBase<BASE>& other,
	const char* name) :
    BASE(other, name),
    x("x", this, other.x), nBinsX(other.nBinsX), binSizeX(other.binSizeX),
    xmin(other.xmin), xmax(other.xmax), isIntegral(other.isIntegral),
    coeffs(other.coeffs)
{ }

template<class BASE>
RooBinned1DQuinticBase<BASE>& RooBinned1DQuinticBase<BASE>::operator=(
	const RooBinned1DQuinticBase<BASE>& other)
{
    if (&other == this) return *this;
    BASE::operator=(other);
    x = RooRealProxy("x", this, other.x);
    nBinsX = other.nBinsX;
    binSizeX = other.binSizeX;
    xmin = other.xmin;
    xmax = other.xmax;
    isIntegral = other.isIntegral;
    coeffs = other.coeffs;
    return *this;
}

template<class BASE>
inline double RooBinned1DQuinticBase<BASE>::histcont(
	const TH1& h, int xbin) const
{
    if (!isIntegral) {
	return (0 <= xbin && xbin < nBinsX - 1) ?
	    h.GetBinContent(1 + xbin) : 0.;
    } else {
	return h.GetBinContent(std::max(0, std::min(1 + xbin, nBinsX - 1)));
    }
}

template<class BASE>
inline double RooBinned1DQuinticBase<BASE>::dhistdx(
	const TH1& h, int xbin) const
{ return .5 * (histcont(h, xbin + 1) - histcont(h, xbin - 1)); }

template<class BASE>
inline double RooBinned1DQuinticBase<BASE>::d2histdx2(
	const TH1& h, int xbin) const
{ return (histcont(h, xbin + 1) - 2. * histcont(h, xbin) + histcont(h, xbin - 1)); }

template<class BASE>
RooBinned1DQuinticBase<BASE>::RooBinned1DQuinticBase(
	const char* name, const char* title, const TH1& h,
	RooAbsReal& xvar, bool integral) :
    BASE(name, title),
    x("x", "x", this, xvar),
    nBinsX((integral ? 3 : 1) + h.GetNbinsX()),
    binSizeX(h.GetXaxis()->GetBinWidth(1)),
    xmin(h.GetXaxis()->GetBinCenter(1) - (integral ? 2. : 1.) * binSizeX),
    xmax(h.GetXaxis()->GetBinCenter(h.GetNbinsX()) + (integral ? 2. : 1.) * binSizeX),
    isIntegral(integral), coeffs(CoeffRecLen * nBinsX)
{
    TAxis *xaxis = h.GetXaxis();
    const int nbins = h.GetNbinsX();
    // verify that all bins have same size
    for (int i = 1; i <= nbins; ++i) {
	if (std::abs(xaxis->GetBinWidth(i) / binSizeX - 1.) > 1e-9)
	    throw BinSizeException();
    }
    // if the histogram's contents are a function's integral over that bin, we
    // need to adjust things a bit - start by building a cumulative
    // distribution in *tmp
    std::auto_ptr<TH1> tmp(isIntegral ? static_cast<TH1*>(
		new TH1D(
		    (std::string(name) + "_" + h.GetName() + "_dummy").c_str(),
		    h.GetTitle(), nBinsX,
		    xmin + .5 * binSizeX,
		    xmax + .5 * binSizeX)) : 0);
    if (isIntegral) {
	tmp->SetDirectory(0);
	// copy h's contents, extend by one bin to right and left, taking
	// values from nearest valid bin in h
	
	// FIXME: fix PDFs with badly preprocessed input histograms
	const bool isPdf = 0 != dynamic_cast<RooAbsPdf*>(this);
	for (int i = 1; i <= nBinsX; ++i) {
	    const double binc = h.GetBinContent(
		    std::max(1, std::min(i - 1, nbins)));
	    tmp->SetBinContent(i, isPdf ? std::max(0., binc) : binc);
	}
	// build CDF of tmp
	for (int i = 1; i < nBinsX - 1; ++i) {
	    tmp->SetBinContent(i + 1,
		    tmp->GetBinContent(i + 1) + tmp->GetBinContent(i));
	}
    }
    const TH1& htmp(isIntegral ? *tmp : h);
    // ok, go through histogram to precalculate the interpolation coefficients
    // in rectangles between bin centres
    //
    // for that purpose, we map each of those rectangles to the unit square
    for (int i = -1; i < nBinsX - 1; ++i) {
	const double rhs[NCoeff] = {
	    // function values in bin centres
	    histcont(htmp, i),
	    histcont(htmp, i + 1),
	    // df/dx in bin centres (finite difference approximation)
	    dhistdx(htmp, i),
	    dhistdx(htmp, i + 1),
	    // d^2f/dx^2 in bin centres (finite difference approximation)
	    d2histdx2(htmp, i),
	    d2histdx2(htmp, i + 1)
	};
	// work out solution - strange array placement is due to the fact
	// that terms with x to high powers can be small, so they should
	// be added up first during evaluation to avoid cancellation
	// issues; at the same time you want to access them in order to
	// not confuse the CPU cache, so they're stored back to front
	coeff(i + 1, 5) = rhs[0];
	coeff(i + 1, 4) = rhs[2];
	coeff(i + 1, 3) = 0.5 * rhs[4];
	coeff(i + 1, 2) = -0.5 * (20. * (rhs[0] - rhs[1]) + 12. * rhs[2] +
		8. * rhs[3] + 3. * rhs[4] - rhs[5]);
	coeff(i + 1, 1) = 0.5 * (30. * (rhs[0] - rhs[1]) + 16. * rhs[2] +
		14. * rhs[3] + 3. * rhs[4] - 2. * rhs[5]);
	coeff(i + 1, 0) = -0.5 * (12. * (rhs[0] - rhs[1]) + 6. * (rhs[2] + rhs[3]) +
		rhs[4] - rhs[5]);
	// coeff(i + 1, N) contains integral of function over the
	// square in "unit square coordinates", i.e. neglecting bin widths
	// this is done to help speed up calculations of 2D integrals
	double sum = 0.;
	for (int k = 0; k < NCoeff; ++k)
	    sum += coeff(i + 1, k) / double(NCoeff - k);
	coeff(i + 1, NCoeff) = sum;
    }
}

template<class BASE>
RooBinned1DQuinticBase<BASE>* RooBinned1DQuinticBase<BASE>::clone(
	const char* newname) const
{ return new RooBinned1DQuinticBase<BASE>(*this, newname); }

template<class BASE>
Double_t RooBinned1DQuinticBase<BASE>::evaluate() const
{
    if (!isIntegral) {
	return eval(x);
    } else {
	// we need some gymnastics and shifting since the default
	// interpretation from the case where the histogram represents
	// function values in the middle of a bin does not apply in the
	// "integral" case, and we need to return the derivative instead of
	// the parametrisation
	const double xx = std::min(
		std::max(xmin + 1.5 * binSizeX, double(x)),
		xmax - 1.5 * binSizeX);
	return evalx(xx - 0.5 * binSizeX);
    }
}

template<>
Double_t RooBinned1DQuinticBase<RooAbsPdf>::evaluate() const
{
    if (!isIntegral) {
	// FIXME: Ugly hack for incorrectly preprocessed input shapes
	const double retVal = eval(x);
	return (0. < retVal) ? retVal : 0.;
    } else {
	// we need some gymnastics and shifting since the default
	// interpretation from the case where the histogram represents
	// function values in the middle of a bin does not apply in the
	// "integral" case, and we need to return the derivative instead of
	// the parametrisation
	const double xx = std::min(
		std::max(xmin + 1.5 * binSizeX, double(x)),
		xmax - 1.5 * binSizeX);
	// FIXME: Ugly hack for incorrectly preprocessed input shapes
	const double retVal = evalx(xx - 0.5 * binSizeX);
	return (0. < retVal) ? retVal : 0.;
    }
}

template <class BASE>
Int_t RooBinned1DQuinticBase<BASE>::getAnalyticalIntegral(
	RooArgSet& allVars, RooArgSet& integVars,
	const char* /* rangeName */) const
{
    Int_t retVal = 0;
    if (allVars.find(x.arg()) && dynamic_cast<RooRealVar*>(x.absArg())) {
	integVars.add(x.arg());
	retVal += 1;
    }
    return retVal;
}

template <class BASE>
Double_t RooBinned1DQuinticBase<BASE>::analyticalIntegral(
	Int_t code, const char* rangeName) const
{
    switch (code) {
	case 1:
	    if (!isIntegral) {
		return evalX(x.min(rangeName), x.max(rangeName));
	    } else {
		// we need some gymnastics and shifting since the default
		// interpretation from the case where the histogram represents
		// function values in the middle of a bin does not apply in
		// the "integral" case, and we need to return the
		// parametrisation itself, since it parametrises the integral
		const double x1 = std::min(
			std::max(xmin + 1.5 * binSizeX, x.min(rangeName)),
			xmax - 1.5 * binSizeX) - 0.5 * binSizeX;
		const double x2 = std::min(
			std::max(xmin + 1.5 * binSizeX, x.max(rangeName)),
			xmax - 1.5 * binSizeX) - 0.5 * binSizeX;
		return eval(x2) - eval(x1);
	    }
    };
    coutE(Integration) << base().GetName() <<
	": Unknown integration code " << code << " supplied!" << std::endl;
    return -1.;
}

template<class BASE>
double RooBinned1DQuinticBase<BASE>::eval(double xx) const
{
    // protect against NaN and out of range
    if (xx <= xmin || xx >= xmax || xx != xx) return 0.;
    // find the bin in question
    const int binx = int(double(nBinsX) * (xx - xmin) / (xmax - xmin));
    // get low edge of bin
    const double xlo = double(nBinsX - binx) / double(nBinsX) * xmin +
	double(binx) / double(nBinsX) * xmax;
    // normalise to coordinates in unit sqare
    const double hx = (xx - xlo) / binSizeX;
    // monomials
    const double hxton[6] = { hx * hx * hx * hx * hx, hx * hx * hx * hx,
	hx * hx * hx, hx * hx, hx, 1. };
    // sum up
    double retVal = 0.;
    for (int k = 0; k < NCoeff; ++k)
	retVal += coeff(binx, k) * hxton[k];

    return retVal;
}

template<class BASE>
double RooBinned1DQuinticBase<BASE>::evalx(double xx) const
{
    // protect against NaN and out of range
    if (xx <= xmin || xx >= xmax || xx != xx) return 0.;
    // find the bin in question
    const int binx = int(double(nBinsX) * (xx - xmin) / (xmax - xmin));
    // get low edge of bin
    const double xlo = double(nBinsX - binx) / double(nBinsX) * xmin +
	double(binx) / double(nBinsX) * xmax;
    // normalise to coordinates in unit sqare
    const double hx = (xx - xlo) / binSizeX;
    // monomials
    const double hxton[5] = { 5. * hx * hx * hx * hx, 4. * hx * hx * hx,
	3. * hx * hx, 2. * hx, 1. };
    // sum up
    double retVal = 0.;
    for (int k = 0; k < NCoeff - 1; ++k)
	retVal += coeff(binx, k) * hxton[k];

    // move from unit square coordinates to user coordinates
    return retVal / binSizeX;
}

template<class BASE>
double RooBinned1DQuinticBase<BASE>::evalX(double x1, double x2) const
{
    // protect against NaN
    if (x1 != x1 || x2 != x2) return 0.;
    // integral
    double sum = 0.;
    for (int binx = 0; binx < nBinsX; ++binx) {
	// get low/high edge of bin
	const double xhi = double(nBinsX - binx - 1) / double(nBinsX) * xmin +
	    double(binx + 1) / double(nBinsX) * xmax;
	if (xhi < x1) continue;
	const double xlo = double(nBinsX - binx) / double(nBinsX) * xmin +
	    double(binx) / double(nBinsX) * xmax;
	if (xlo > x2) break;
	// work out integration range
	const double a = ((xlo > x1) ? 0. : (x1 - xlo)) / binSizeX;
	const double b = ((xhi < x2) ? binSizeX : (x2 - xlo)) / binSizeX;
	const bool xFullyContained = std::abs(b - a - 1.0) < 1e-15;
	if (xFullyContained) {
	    // for fully contained bins, we have cached the integral
	    sum += coeff(binx, NCoeff);
	    continue;
	}
	// integrated monomials
	const double hxton[6] = {
	    (b * b * b * b * b * b - a * a * a * a * a * a) / 6.,
	    0.2 * (b * b * b * b * b - a * a * a * a * a),
	    0.25 * (b * b * b * b - a * a * a * a),
	    (b * b * b - a * a * a) / 3., 0.5 * (b * b - a * a), b - a };
	double lsum = 0.;
	for (int k = 0; k < NCoeff; ++k)
	    lsum += coeff(binx, k) * hxton[k];
	sum += lsum;
    }
    // move from unit square coordinates to user coordinates
    return sum * binSizeX;
}

template<class BASE>
Bool_t RooBinned1DQuinticBase<BASE>::isBinnedDistribution(
	const RooArgSet& vars) const
{ return 0 != vars.find(x.arg()) && 1 == vars.getSize(); }

template<class BASE>
std::list<Double_t>* RooBinned1DQuinticBase<BASE>::binBoundaries(
	RooAbsRealLValue& var, Double_t lo, Double_t hi) const
{
    if (dynamic_cast<RooAbsReal*>(&var) != &x.arg()) return 0;
    std::list<Double_t>* l = new std::list<Double_t>();
    assert(0 != l);
    for (int binx = (isIntegral ? 1 : 0);
	    binx < nBinsX - (isIntegral ? 2 : 1); ++binx) {
	const double xhi = double(nBinsX - binx - 1) / double(nBinsX) * xmin +
	    double(binx + 1) / double(nBinsX) * xmax + 0.5 * binSizeX;
	if (xhi < lo) continue;
	const double xlo = double(nBinsX - binx) / double(nBinsX) * xmin +
	    double(binx) / double(nBinsX) * xmax + 0.5 * binSizeX;
	if (xlo >= hi) break;
	if (l->empty()) l->push_back(xlo);
	l->push_back(xhi);
    }
    if (l->empty()) {
	delete l;
	return 0;
    }
    assert(l->size() >= 2);
    assert(l->front() <= l->back());
    return l;
}

template<class BASE>
std::list<Double_t>* RooBinned1DQuinticBase<BASE>::plotSamplingHint(
	RooAbsRealLValue& var, Double_t lo, Double_t hi) const
{
    if (dynamic_cast<RooAbsReal*>(&var) != &x.arg()) return 0;
    std::list<Double_t>* l = new std::list<Double_t>();
    assert(0 != l);
    for (int binx = (isIntegral ? 1 : 0);
	    binx < nBinsX - (isIntegral ? 2 : 1); ++binx) {
	const double xhi = double(nBinsX - binx - 1) / double(nBinsX) * xmin +
	    double(binx + 1) / double(nBinsX) * xmax + 0.5 * binSizeX;
	if (xhi < lo) continue;
	const double xlo = double(nBinsX - binx) / double(nBinsX) * xmin +
	    double(binx) / double(nBinsX) * xmax + 0.5 * binSizeX;
	if (xlo >= hi) break;
	l->push_back(xlo);
	l->push_back(xhi);
    }
    if (l->empty()) {
	delete l;
	return 0;
    }
    assert(l->size() >= 2);
    assert(0 == (l->size() & 1));
    assert(l->front() <= l->back());
    return l;
}

// instantiate the templates we want to have
#ifndef __GCCXML__
template class RooBinned1DQuinticBase<RooAbsReal>;
template class RooBinned1DQuinticBase<RooAbsPdf>;
#endif // __GCCXML__

// vim: ft=cpp:sw=4:tw=78
