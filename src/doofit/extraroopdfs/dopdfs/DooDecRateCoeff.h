/**
 * @file DooDecRateCoeff.h
 *
 * cosh/sinh/cos/sin coefficients in decay rate equations
 *
 * @author Manuel Schiller <manuel.schiller@nikhef.nl> (and Frank Meier (simplification))
 * @date 2012-10-24
 */
#ifndef DOOFIT_EXTRAROOPDFS_SCHILLERPDFS_DOODECRATECOEFF
#define DOOFIT_EXTRAROOPDFS_SCHILLERPDFS_DOODECRATECOEFF

#include <map>
#include <string>
#include <vector>
#include <utility>

#include <RooAbsReal.h>
#include <RooAbsPdf.h>
#include <RooUniform.h>
#include <RooCategory.h>
#include <RooRealProxy.h>
#include <RooCategoryProxy.h>

/** @brief cosh/sinh/cos/sin coefficients in decay rate equations
 *
 * This class calculates the coefficients which go in front of the
 * cosh/sinh/cos/sin terms in the decay rate equations (RooBDecay). By forming
 * a suitable sum over the true initial state flavour inside the class
 * implementation, it is possible to treat production, detection and tagging
 * efficiency asymmetries from first principles. Mistag asymmetries are
 * implemented using either different average mistags for true initial B and
 * Bbar, or different mistag calibrations for true initial B and Bbar.
 *
 * The coefficient can be either CP even or CP odd (the latter has the
 * property that, with perfect tagging, it changes sign under CP).
 *
 * These "coefficients" are hermaphrodites in the sense that they are not
 * proper PDFs as such, but have to be normalised in a similar manner to
 * proper PDFs: If it were not for the coefficients which we need to put into
 * the decay rate equations, we would write down a PDF @f$P(q_f)\cdot
 * P(q_t,\eta)@f$, and this is the expression that needs to be used to
 * normalise the coefficients correctly. However, to be usable in the decay
 * rate equations, the proper prefactors have to be included which adds four
 * parameters @f$\alpha(q_i,q_f)@f$, so that the total PDF is @f$P(q_f)\cdot
 * P(q_t,\eta;\alpha(+1,q_f),\alpha(-1,q_f))@f$. The component PDFs are
 * written down below:
 *
 * @f[P(q_f)=1+q_f\cdot a_{det}@f]
 * @f[P(q_t,\eta;\alpha(+1,q_f),\alpha(-1,q_f))=
 * \left\{\begin{array}{l l}
 * (1+a_{prod}) \cdot \left(1-\epsilon (1+a_{tageff})\right) \cdot
 * U(\eta) \cdot \alpha(+1, q_f) +
 * (1-a_{prod}) \cdot \left(1-\epsilon (1-a_{tageff})\right) \cdot
 * U(\eta) \cdot \alpha(-1, q_f) &
 * \textrm{if $qt=0$} \\
 *
 * (1+a_{prod}) \cdot \epsilon \cdot (1+a_{tageff}) \cdot P(\eta)
 * \cdot \left(1-\eta_c(\eta)\right) \cdot
 * \alpha(+1, q_f) +
 * (1-a_{prod}) \cdot \epsilon \cdot (1-a_{tageff}) \cdot P(\eta)
 * \cdot {\overline{\eta_c}}(\eta) \cdot
 * \alpha(-1, q_f) &
 * \textrm{if $qt=+1$} \\
 *
 * (1+a_{prod}) \cdot \epsilon \cdot (1+a_{tageff}) \cdot P(\eta)
 * \cdot \eta_c(\eta) \cdot
 * \alpha(+1, q_f) +
 * (1-a_{prod}) \cdot \epsilon \cdot (1-a_{tageff}) \cdot P(\eta)
 * \cdot \left(1 - {\overline{\eta_c}}(\eta)\cdot\right) \cdot
 * \alpha(-1, q_f) &
 * \textrm{if $qt=-1$} \\
 * \end{array}\right.@f]
 *
 * The asymmetries given above (@f$a_{prod}@f$, @f$a_{det}@f$,
 * @f$a_{tageff}@f$) are assumed to be independent of any observables (at
 * least for the time being).
 *
 * Convenience constructors are provided if there is no mistag asymmetry (i.e.
 * @f$\eta_c(\eta)=\overline{\eta_c}(\eta)@f$ or the equivalent for the average
 * mistag case).
 *
 * The coefficients @f$\alpha(q_i, q_f)@f$ are put to 1 to calculate the
 * normalisation of the coefficients, their unnormalised values are calculated
 * as follows:
 *
 * For CP-even coefficients, we have
 * @f[ \alpha(q_i, q_f) = \left\{\begin{array}{l l}
 * C_f & \textrm{for $q_f = +1$} \\
 * C_{\bar{f}} & \textrm{for $q_f = -1$} \\
 * \end{array}\right. @f]
 *
 * For CP-odd coefficients, we have
 * @f[ \alpha(q_i, q_f) = q_i \cdot q_f \cdot \left\{\begin{array}{l l}
 * C_f & \textrm{for $q_f = +1$} \\
 * C_{\bar{f}} & \textrm{for $q_f = -1$} \\
 * \end{array}\right. @f]
 *
 * Moreover, an overall minus sign can be prepended in the definition of the
 * @f$\alpha(q_i,q_f)@f$.
 *
 * @author Manuel Schiller <manuel.schiller@nikhef.nl>
 * @date 2012-10-24
 */

namespace doofit {
namespace extraroopdfs {

class DooDecRateCoeff : public RooAbsReal
{
    private:
	class CacheElem; // forward decl.
	friend class CacheElem;
	/// pair of cache element and corresponding slot number in cache
	typedef std::pair<DooDecRateCoeff::CacheElem*, Int_t> CacheElemPair;

    public:
	/// default constructor for ROOT I/O
	DooDecRateCoeff() : m_nset(0), m_nsethash(0) { }

	/** @brief constructor
	 *
	 * initialise pdf to average over initial flavour when using per-event
	 * mistag
	 *
	 * @param name		name of the pdf
	 * @param title		title of the pdf
	 * @param qt      tagging decision (-1, 0, 1)
	 * @param Cf      coefficient
	 * @param etaobs	mistag observable
	 * @param etapdf	per-event-mistag distribution for tagged events
	 * @param eta     (calibrated) per-event mistag
	 * @param aprod		production asymmetry
	 * @param atageff	tagging efficiency asymmetry (as function of qi)
	 *
	 */

	DooDecRateCoeff(const char* name, const char* title,
		RooAbsCategory& qt, RooAbsReal& Cf,
    RooAbsRealLValue& etaobs, RooAbsPdf& etapdf,
		RooAbsReal& eta, RooAbsReal& aprod, RooAbsReal& atageff);

	/** @brief constructor
	 *
	 * initialise pdf to average over initial flavour when using per-event
	 * mistag (with different mistags for true initial flavours)
	 *
	 * @param name		name of the pdf
	 * @param title		title of the pdf
	 * @param qt      tagging decision (-1, 0, 1)
	 * @param Cf      coefficient
	 * @param etaobs	mistag observable
	 * @param etapdf	per-event-mistag distribution for tagged events
	 * @param eta     (calibrated) per-event mistag for true B
	 * @param etabar	(calibrated) per-event mistag for true Bbar
	 * @param aprod		production asymmetry
	 * @param atageff	tagging efficiency asymmetry (as function of qi)
	 *
	 */
	DooDecRateCoeff(const char* name, const char* title,
		RooAbsCategory& qt,	RooAbsReal& Cf,
		RooAbsRealLValue& etaobs, RooAbsPdf& etapdf,
		RooAbsReal& eta, RooAbsReal& etabar,
		RooAbsReal& aprod, RooAbsReal& atageff);

	/** @brief constructor
	 *
	 * initialise pdf to average over initial flavour when using an
	 * average mistag
	 *
	 * @param name		name of the pdf
	 * @param title		title of the pdf
	 * @param qt      tagging decision (-1, 0, 1)
	 * @param Cf      coefficient
	 * @param eta     average mistag
	 * @param aprod		production asymmetry
	 * @param atageff	tagging efficiency asymmetry (as function of qi)
	 *
	 */

	DooDecRateCoeff(const char* name, const char* title,
		RooAbsCategory& qt,	RooAbsReal& Cf,
		RooAbsReal& eta, RooAbsReal& aprod, RooAbsReal& atageff);

	/** @brief constructor
	 *
	 * initialise pdf to average over initial flavour when using an
	 * average mistag (with different mistags for true initial flavours)
	 *
	 * @param name		name of the pdf
	 * @param title		title of the pdf
	 * @param qt      tagging decision (-1, 0, 1)
	 * @param Cf      coefficient
	 * @param eta     average mistag for true B
	 * @param etabar	average mistag for true Bbar
	 * @param aprod		production asymmetry
	 * @param atageff	tagging efficiency asymmetry (as function of qi)
	 *
	 */
	DooDecRateCoeff(const char* name, const char* title,
		RooAbsCategory& qt,	RooAbsReal& Cf,
		RooAbsReal& eta, RooAbsReal& etabar,
		RooAbsReal& aprod, RooAbsReal& atageff);

	/** @brief copy constructor
	 *
	 * @param other	pdf to copy
	 * @param name	name for the copy of other
	 */
	DooDecRateCoeff(
		const DooDecRateCoeff& other, const char* name = 0);

	/** @brief clone
	 *
	 * @param newname	name for the copy of other
	 * @returns		pointer to copy of this object
	 */
	virtual TObject* clone(const char* newname) const;

	/// destructor
	virtual ~DooDecRateCoeff();

	/// get value of function
	virtual Double_t getValV(const RooArgSet* nset) const;

	/// force all integrals to be treated analytically
	virtual Bool_t forceAnalyticalInt(const RooAbsArg& dep) const;

	/** @brief anounce analytical integrals
	 *
	 * @param allVars	variables over which integration is desired
	 * @param anaIntVars	variables for which we can perform analytical integral
	 * @param rangeName	range over which to integrate
	 * @returns		code for that particular integration
	 *
	 * Note: since this is not a proper PDF, it uses the default range to
	 * normalise the coefficient. The coefficient is normalised over the
	 * current normalisation set.
	 */
	virtual Int_t getAnalyticalIntegral(
		RooArgSet& allVars, RooArgSet& anaIntVars,
		const char* rangeName = 0) const;

	/** @brief perform analytical integrals
	 *
	 * @param code		code for integral to be performed
	 * @param rangeName	range over which to integrate
	 * @returns		value of the integral
	 *
	 * Note: since this is not a proper PDF, it uses the default range to
	 * normalise the coefficient. The coefficient is normalised over the
	 * current normalisation set.
	 */
	virtual Double_t analyticalIntegral(
		Int_t code, const char* rangeName = 0) const;

	/** @brief anounce analytical integrals
	 *
	 * @param allVars	variables over which integration is desired
	 * @param anaIntVars	variables for which we can perform analytical integral
	 * @param nset		normalisation set
	 * @param rangeName	range over which to integrate
	 * @returns		code for that particular integration
	 *
	 * Note: since this is not a proper PDF, it uses the default range to
	 * normalise the coefficient.
	 */
	virtual Int_t getAnalyticalIntegralWN(
		RooArgSet& allVars, RooArgSet& anaIntVars,
		const RooArgSet* nset = 0, const char* rangeName = 0) const;
	
	/** @brief perform analytical integrals
	 *
	 * @param code		code for integral to be performed
	 * @param nset		normalisation set
	 * @param rangeName	range over which to integrate
	 * @returns		value of the integral
	 *
	 * Note: since this is not a proper PDF, it uses the default range to
	 * normalise the coefficient.
	 */
	virtual Double_t analyticalIntegralWN(
		Int_t code, const RooArgSet* nset = 0,
		const char* rangeName = 0) const;

    protected:
	/// return value of coefficient
	Double_t evaluate() const;
	
	/// return (integral) cache element
	CacheElemPair getCache(
		const RooArgSet& iset, const RooArgSet* nset = 0,
		const TNamed* irangeName = 0) const;

	/// hash the contents of a RooArgSet
	UInt_t hash(const RooArgSet& s) const;

    private:
	static RooArgSet s_emptyset;	///< empty RooArgSet
	RooCategoryProxy m_qt;		///< tagging decision
	RooRealProxy m_Cf;		///< coefficient for qf = +1
	RooRealProxy m_etaobs;		///< mistag observable
	RooRealProxy m_etapdf;		///< per-event mistag pdf
	/// instance of per-event mistag pdf for untagged events (flat)
	RooUniform m_etapdfutinstance;
	/// per-event mistag pdf for untagged events (flat)
	RooRealProxy m_etapdfut;
	RooRealProxy m_eta;		///< per-event/average mistag (B)
	RooRealProxy m_etabar;		///< per-event/average mistag (Bbar)
	RooRealProxy m_aprod;		///< production asymmetry
	RooRealProxy m_atageff;		///< tagging efficiency asymmetry
	/// integral cache
	mutable RooObjCacheManager m_cacheMgr; //! transient member
	/// place to keep normalisation sets
	mutable std::map<UInt_t, RooArgSet> m_nsets; //! transient member
	/// current normalisation set
	mutable const RooArgSet* m_nset; //! transient member
	/// hash of current normalisation set
	mutable UInt_t m_nsethash; //! transient member

	/** @brief hold cached integrals
	 *
	 * technically, this may not integrate at all, depending on the
	 * integration set you supply at construction
	 *
	 * @author Manuel Schiller <manuel.schiller@nikhef.nl>
	 * @date 2012-11-08
	 */
	class CacheElem : public RooAbsCacheElement
        {
	    public:
		/// constructor
		CacheElem(const DooDecRateCoeff& parent,
			const RooArgSet& iset, const RooArgSet* nset = 0,
			const TNamed* rangeName = 0);

		/// destructor
		virtual ~CacheElem();
		
		/// return list of contained RooFit variables
		virtual RooArgList containedArgs(Action);
		
		/// return value of cache element
		double eval(const double alphapp, const double alphapm) const;
	
	    protected:
		/// set up binned evaluation of integral of eta product
		void setupBinnedProductIntegral(
			RooAbsReal* &prod, const RooAbsReal& eta, int idx);

		/// return integral over mistag pdf (tagged events)
		inline double etaintpdftagged() const
		{
		    return m_etaintpdftagged ?
			m_etaintpdftagged->getValV(
				(m_flags & NormEta) ? &m_nset : 0) : 1.0;
		}

		/// return integral over mistag pdf times mistag (tagged events)
		double etaintprodpdfmistagtagged(int qt) const;
		
		/// retun integral over mistag pdf (untagged events)
		inline double etaintpdfuntagged() const
		{
		    return m_etaintpdfuntagged ? 
			m_etaintpdfuntagged->getValV(
				(m_flags & NormEta) ? &m_nset : 0) : 1.0;
		}
				
		/// return value of (q_t, eta) pdf
		double qtetapdf(const int qt,
			const double alphapp, const double alphapm) const;

	    private:
		/// integral over mistag pdf (tagged events)
		RooAbsReal *m_etaintpdftagged;
		/// integral over mistag pdf times mistag (tagged as B)
		RooAbsReal *m_etaintprodpdfmistagtaggedplus;
		/// integral over mistag pdf times mistag (tagged as Bbar)
		RooAbsReal *m_etaintprodpdfmistagtaggedminus;
		/// integral over mistag pdf (untagged events)
		RooAbsReal *m_etaintpdfuntagged;
		/// normalisation set to use for mistag pdfs
		RooArgSet m_nset;
		/// integration range
		const char* m_rangeName;
		/// parent
		const DooDecRateCoeff& m_parent;

		// for binned eta pdfs, we sum up our P(eta) * eta_c(eta)
		// product integrals ourselves; we need a couple of extra
		// members for that
		
		/// name of the working range
		std::string m_workRangeName[2];
		/// two RooRealProxyVars so we can move the range over eta
		std::pair<RooRealVar*, RooRealVar*> m_workRange[2];
		/// a vector of eta bin boundaries
		std::vector<double> m_etabins;
		/// cached value of product integral
		mutable double m_prodcachedval[2]; //! transient data member
		
		/// flags
		enum Flags {
		    None = 0,
		    IntQt = 2,		/// integrate over qt
		    IntEta = 4,		/// integrate over eta
		    NormEta = 8,	/// eta is in normalisation set
		    ProdIntBinned = 16	/// binned product: m_etapdf and m_eta
		} m_flags;
	};

	ClassDef(DooDecRateCoeff, 2);
};

}
}

#endif // DOODECRATECOEFF

// vim: sw=4:tw=78:ft=cpp
