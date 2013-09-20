/**
 * @file RooBinned1DQuinticBase.h
 *
 * @author Manuel Tobias Schiller <manuel.schiller@nikhef.nl>
 * @date 2012-09-09
 */
#ifndef _ROOBINNED1DQUINTICBASE
#define _ROOBINNED1DQUINTICBASE

#include <list>
#include <exception>

#include <RooAbsReal.h>
#include <RooAbsPdf.h>
#include <RooRealProxy.h>

#include "SharedArray.h"

class TH1;
class RooArgSet;

/** @brief smoothly interpolate a TH1 for use in RooFit
 *
 * @author Manuel Tobias Schiller <manuel.schiller@nikhef.nl>
 * @date 2012-09-09
 *
 * Smoothly interpolate between bin midpoints of a 1D histogram with
 * equal-sized bins, or between integrals of a function over each bin
 *
 * In case the histogram bins represent the values of the function in the
 * middle of each bin, the interpolation function coincides with the saved
 * histogram values in bin midpoints; a quintic polynomial is used. Bins must
 * all have the same size.  The interpolation polynomial in a "cell" between
 * four bin midpoints is:
 *
 * \f[ p(x) = \sum_{i=0}^5 a_{i} x^i \f]
 *
 * The coefficients \f$a_{i}\f$ are determined by the requirement that
 * function value, first and second derivatives must agree with that of the
 * binned function at the four bin midpoints at the edges of the cell.
 * Symmetric finite differences are used to approximate these derivatives.
 *
 * For each cell, the coefficients are determined at constuction time. The
 * object also keeps a cache of 1D integrals over complete cells, such that 1D
 * integrations can be done analytically in a reasonably short amount of time.
 *
 * If histogram bins represent the integral of the function over that bin, the
 * procedure is similar, but the parametrisation above i used to parametrise
 * the function integral instead, so that the bin-wise integrals of the
 * resulting interpolating function coincide with the contents of the original
 * histogram. The interpretation of the interpolation polynomial changes, but
 * the code handles that transparently, and user code can remain unaware of
 * the "gymnastics" behind the scenes (as the parametrisation itself is not
 * exposed to user code).
 */
template <class BASE>
class RooBinned1DQuinticBase : public BASE
{
    private:
	/// length of coefficient record in array
	enum { NCoeff = 6, CoeffRecLen = 7 };
	/// exception to throw in case of variable-sized bins
	class BinSizeException : public std::exception
        {
	    public:
		/// constructor
		BinSizeException() throw () { }
		/// destructor
		virtual ~BinSizeException() throw ();
		/// description
		virtual const char* what() const throw ();
	};

    public:
	/// constructor for ROOT I/O (ROOT does not care)
	RooBinned1DQuinticBase() : coeffs(0) { }
	/** @brief constructor from histogram
	 *
	 * @param name		name of new object
	 * @param title		title of new object
	 * @param h		histogram which contains data for interpolation
	 * @param xvar		variable in which to do the lookup
	 * @param integral	if true, histogram bins contain integral of
	 * 			underlying function/distribution over that
	 * 			bin, if false, histogram bins represent
	 * 			function/distribution value at bin centre
	 *
	 * note: histogram bins must be uniform in size
	 *
	 * The interpolation polynomial is a quintic polynomial in each bin
	 */
	RooBinned1DQuinticBase(
		const char* name, const char* title, const TH1& h,
		RooAbsReal& xvar, bool integral = false);
	/// copy constructor
	RooBinned1DQuinticBase(
		const RooBinned1DQuinticBase<BASE>& other, const
		char* name = 0);
	/// assignment operator
	RooBinned1DQuinticBase<BASE>& operator=(
		const RooBinned1DQuinticBase<BASE>& other);
	/// clone method
	virtual RooBinned1DQuinticBase<BASE>* clone(
		const char* newname = 0) const;

	/// destructor
	virtual ~RooBinned1DQuinticBase();

	/// evaluation of function
	virtual Double_t evaluate() const;
	/// advertise analytical integrals
	virtual Int_t getAnalyticalIntegral(
		RooArgSet& allVars, RooArgSet& integVars,
		const char* rangeName = 0) const;
	/// evaluate advertised analytical integral
        virtual Double_t analyticalIntegral(
		Int_t code, const char* rangeName = 0) const;

	/// return true is vars contains the parametrisation variable
	virtual Bool_t isBinnedDistribution(const RooArgSet& vars) const;
	/// return list with bin boundaries
	virtual std::list<Double_t>* binBoundaries(
		RooAbsRealLValue& var, Double_t lo, Double_t hi) const;
	/// return list of pairs of lo, hi bin boundaries
	virtual std::list<Double_t>* plotSamplingHint(
		RooAbsRealLValue& var, Double_t lo, Double_t hi) const;

    private:
	/// proxy for RooAbsReals
	RooRealProxy x;
	/// number of bins
	int nBinsX;
	/// bin size in x and y directions
	double binSizeX;
	/// x and y range
	double xmin, xmax;
	/// are coefficients for function of integral of function over bin?
	bool isIntegral;
	/// coefficients of interpolation polynomials
	SharedArray<double> coeffs;

	/// helper to deal with TH1 bin contents
	double histcont(const TH1& h, int xbin) const;
	/// d/dx finite differences of histogram
	double dhistdx(const TH1& h, int xbin) const;
	/// d^3/dx^3 finite differences of histogram
	double d2histdx2(const TH1& h, int xbin) const;

	/// const convenience access to base class
	inline const BASE& base() const
	{ return *reinterpret_cast<const BASE*>(this); }
	/// convenience access to base class
	inline BASE& base()
	{ return *reinterpret_cast<BASE*>(this); }

	/// const access to coefficients
	inline SharedArray<double>::RWProxy coeff(int binx, int ncoeff) const
	{ return coeffs[ncoeff + CoeffRecLen * binx]; }
	/// access to coefficients
	inline SharedArray<double>::RWProxy coeff(int binx, int ncoeff)
	{ return coeffs[ncoeff + CoeffRecLen * binx]; }

	/// evaluate parametrisation at given point
	double eval(double x) const;
	/// evaluate parametrisation derivative at given point
	double evalx(double x) const;
	/// evaluate parametrisation integral over x from x1 to x2
	double evalX(double x1, double x2) const;

	ClassDef(RooBinned1DQuinticBase, 2);
};

// help genreflex
#ifdef __GCCXML__
template class RooBinned1DQuinticBase<RooAbsReal>;
template class RooBinned1DQuinticBase<RooAbsPdf>;
#endif // __GCCXML__

typedef RooBinned1DQuinticBase<RooAbsReal> RooBinned1DQuintic;
typedef RooBinned1DQuinticBase<RooAbsPdf> RooBinned1DQuinticPdf;

#endif // _ROOBINNED1DQUINTICBASE

// vim: ft=cpp:sw=4:tw=78
