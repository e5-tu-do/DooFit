/**
 * @file DecRateCoeff.h
 *
 * cosh/sinh/cos/sin coefficients in decay rate equations
 *
 * @author Manuel Schiller <manuel.schiller@nikhef.nl>
 * @date 2012-10-24
 */
#ifndef DECRATECOEFF
#define DECRATECOEFF

#include <map>
#include <string>
#include <vector>
#include <utility>

#include <RooAbsReal.h>
#include <RooAbsPdf.h>
#include <RooUniform.h>
#include <RooConstVar.h>
#include <RooCategory.h>
#include <RooRealProxy.h>
#include <RooCategoryProxy.h>

/**@class DecRateCoeff
 *
 * @brief cosh/sinh/cos/sin coefficients in decay rate equations
 *
 * This class calculates the coefficients which go in front of the
 * cosh/sinh/cos/sin terms in the decay rate equations (RooBDecay). By forming
 * a suitable sum over the true initial state flavour inside the class
 * implementation, it is possible to treat production, detection and tagging
 * efficiency asymmetries from first principles. Mistag asymmetries are
 * implemented using either different average mistags for true initial B and
 * Bbar, or different mistag calibrations for true initial B and Bbar. For
 * simplicity, we first review how these coefficients look in case of a single
 * tagger:
 *
 * The coefficient can be either CP even or CP odd (the latter has the
 * property that, with perfect tagging, it changes sign under CP), and it can
 * be different depending on final state charge (@f$C_f@f$ for @f$q_f=+1@f$,
 * @f$C_{\bar{f}}@f$ for @f$q_f=-1@f$).
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
 * Instead of using @f$C_f@f$ and @f$C_{\bar{f}}@f$ directly, it is also
 * possible to use the average @f$\langle C\rangle=(C_f+C_{\bar{f}})/2@f$
 * difference @f$\Delta C=(C_f-C_{\bar{f}})/2@f$ of @f$C_f@f$ and
 * @f$C_{\bar{f}}@f$. The advantage to fitting for @f$\langle C\rangle@f$ and
 * @f$\Delta C@f$ are smaller correlations in the typical case, see e.g.
 *
 * http://de.arxiv.org/abs/1208.6463
 *
 * for details.
 *
 * This class can also handle several mutually exclusive taggers. To this end,
 * the meaning of the tagging decision @f$q_t@f$ is generalised: @f$q_t=0@f$
 * means the event is untagged, @f$|q_t| = k@f$ means the event was tagged by
 * tagger @f$k@f$, and the sign of @f$q_t@f$ inicates if the event was tagged
 * as a B (positive) or B bar (negative). Each tagger has its own tagging
 * efficiency, tagging efficiency asymmetry, mistag @f$\eta_c(\eta)@f$
 * (and @f$\overline{\eta_c}(\eta)@f$ in case the mistags are different for B
 * and B bar) and mistag distribution, if applicable.
 *
 * This construction also allows to treat uncorrelated taggers. By using the
 * convention @f$|q_t|=\sum_{k=1}^N |d_k|\cdot 2^k@f$ where @f$d_k@f$ is the
 * decision of the k-th tagger, we get mutually exclusive "tagging categories"
 * which allow the formalism described in the last paragraph to work. For
 * example, for two independent taggers OS (k=1) and SSK (k =2), we would get
 * @f$|q_t| = 0@f$ for untagged events, @f$|q_t|=1@f$ for events tagged only by
 * OS and not by SSK, @f$|q_t|=2@f$ for events tagged only by SSK and not by
 * OS, and @f$|q_t|=3@f$ for events tagged only by both OS and SSK.
 *
 * @author Manuel Schiller <manuel.schiller@nikhef.nl>
 * @date 2012-10-24
 * * initial release
 *
 * @author Manuel Schiller <manuel.schiller@nikhef.nl>
 * @date 2013-11-11
 * * major code refactoring to allow to switch between different taggers
 *   depending on the value of qt
 * * the new version of the class is source compatible with the old version -
 *   existing code need to be changed to get the old functionality
 * * however, the persistent form of the class is incompatible with the old
 *   version, so that old RooFit workspaces which contain DecRateCoeff
 *   instances can no longer be read
 * * put in the necessary ground work to extend the functionality of the class
 *   to do the combination of taggers inside the DecRateCoeff class at some
 *   point in the future without breaking persistency again
 */
class DecRateCoeff : public RooAbsReal
{
    private:
	class CacheElem; // forward decl.
	friend class CacheElem;
	/// pair of cache element and corresponding slot number in cache
	typedef std::pair<DecRateCoeff::CacheElem*, Int_t> CacheElemPair;

    public:
	/// flags
	typedef enum {
	    CPEven = 0,	  ///< nothing set for CP even coefficients
	    CPOdd = 1,	  ///< bit 0 set for CP odd coefficient
	    Minus = 2,	  ///< bit 1 set to apply an overall minus sign
	    AvgDelta = 4, ///< bit 2 set if Cf/Cfbar refer to average and Delta C
	} Flags;

	/// default constructor for ROOT I/O
	DecRateCoeff() : m_nset(0), m_nsethash(0) { }

	/** @brief constructor
	 *
	 * initialise pdf to average over initial flavour when using per-event
	 * mistag
	 *
	 * @param name		name of the pdf
	 * @param title		title of the pdf
	 * @param flags		flags
	 * @param qf		final state charge (-1, +1)
	 * @param qt		tagging decision (-1, 0, 1)
	 * @param Cf		coefficient for qf = +1
	 * @param Cfbar		coefficient for qf = -1
	 * @param etaobs	mistag observable
	 * @param etapdf	per-event-mistag distribution for tagged events
	 * @param tageff	tagging efficiency
	 * @param eta		(calibrated) per-event mistag
	 * @param aprod		production asymmetry
	 * @param adet		detection asymmetry
	 * @param atageff	tagging efficiency asymmetry (as function of qi)
	 *
	 * If flags has AvgDelta set, we interpret the parameter Cf to be the
	 * average C, @f$\langle C\rangle=(C_f+C_{\bar{f}})/2@f$, and Cfbar to
	 * be the difference in C, @f$\Delta C=(C_f-C_{\bar{f}})/2@f$. The
	 * advantage to fitting for @f$\langle C\rangle@f$ and @f$\Delta C@f$
	 * are smaller correlations in the typical case, see e.g.
	 *
	 * http://de.arxiv.org/abs/1208.6463
	 *
	 * for details.
	 */

	DecRateCoeff(const char* name, const char* title, Flags flags,
		RooAbsCategory& qf, RooAbsCategory& qt,
		RooAbsReal& Cf, RooAbsReal& Cfbar,
		RooAbsRealLValue& etaobs, RooAbsPdf& etapdf,
		RooAbsReal& tageff, RooAbsReal& eta,
		RooAbsReal& aprod, RooAbsReal& adet,
		RooAbsReal& atageff);

	/** @brief constructor
	 *
	 * initialise pdf to average over initial flavour when using per-event
	 * mistag (with different mistags for true initial flavours)
	 *
	 * @param name		name of the pdf
	 * @param title		title of the pdf
	 * @param flags		flags
	 * @param qf		final state charge (-1, +1)
	 * @param qt		tagging decision (-1, 0, 1)
	 * @param Cf		coefficient for qf = +1
	 * @param Cfbar		coefficient for qf = -1
	 * @param etaobs	mistag observable
	 * @param etapdf	per-event-mistag distribution for tagged events
	 * @param tageff	tagging efficiency
	 * @param eta		(calibrated) per-event mistag for true B
	 * @param etabar	(calibrated) per-event mistag for true Bbar
	 * @param aprod		production asymmetry
	 * @param adet		detection asymmetry
	 * @param atageff	tagging efficiency asymmetry (as function of qi)
	 *
	 * If flags has AvgDelta set, we interpret the parameter Cf to be the
	 * average C, @f$\langle C\rangle=(C_f+C_{\bar{f}})/2@f$, and Cfbar to
	 * be the difference in C, @f$\Delta C=(C_f-C_{\bar{f}})/2@f$. The
	 * advantage to fitting for @f$\langle C\rangle@f$ and @f$\Delta C@f$
	 * are smaller correlations in the typical case, see e.g.
	 *
	 * http://de.arxiv.org/abs/1208.6463
	 *
	 * for details.
	 */
	DecRateCoeff(const char* name, const char* title, Flags flags,
		RooAbsCategory& qf, RooAbsCategory& qt,
		RooAbsReal& Cf, RooAbsReal& Cfbar,
		RooAbsRealLValue& etaobs, RooAbsPdf& etapdf,
		RooAbsReal& tageff, RooAbsReal& eta, RooAbsReal& etabar,
		RooAbsReal& aprod, RooAbsReal& adet,
		RooAbsReal& atageff);

	/** @brief constructor
	 *
	 * initialise pdf to average over initial flavour when using an
	 * average mistag
	 *
	 * @param name		name of the pdf
	 * @param title		title of the pdf
	 * @param flags		flags
	 * @param qf		final state charge (-1, +1)
	 * @param qt		tagging decision (-1, 0, 1)
	 * @param Cf		coefficient for qf = +1
	 * @param Cfbar		coefficient for qf = -1
	 * @param tageff	tagging efficiency
	 * @param eta		average mistag
	 * @param aprod		production asymmetry
	 * @param adet		detection asymmetry
	 * @param atageff	tagging efficiency asymmetry (as function of qi)
	 *
	 * If flags has AvgDelta set, we interpret the parameter Cf to be the
	 * average C, @f$\langle C\rangle=(C_f+C_{\bar{f}})/2@f$, and Cfbar to
	 * be the difference in C, @f$\Delta C=(C_f-C_{\bar{f}})/2@f$. The
	 * advantage to fitting for @f$\langle C\rangle@f$ and @f$\Delta C@f$
	 * are smaller correlations in the typical case, see e.g.
	 *
	 * http://de.arxiv.org/abs/1208.6463
	 *
	 * for details.
	 */

	DecRateCoeff(const char* name, const char* title, Flags flags,
		RooAbsCategory& qf, RooAbsCategory& qt,
		RooAbsReal& Cf, RooAbsReal& Cfbar,
		RooAbsReal& tageff, RooAbsReal& eta,
		RooAbsReal& aprod, RooAbsReal& adet,
		RooAbsReal& atageff);

	/** @brief constructor
	 *
	 * initialise pdf to average over initial flavour when using an
	 * average mistag (with different mistags for true initial flavours)
	 *
	 * @param name		name of the pdf
	 * @param title		title of the pdf
	 * @param flags		flags
	 * @param qf		final state charge (-1, +1)
	 * @param qt		tagging decision (-1, 0, 1)
	 * @param Cf		coefficient for qf = +1
	 * @param Cfbar		coefficient for qf = -1
	 * @param tageff	tagging efficiency
	 * @param eta		average mistag for true B
	 * @param etabar	average mistag for true Bbar
	 * @param aprod		production asymmetry
	 * @param adet		detection asymmetry
	 * @param atageff	tagging efficiency asymmetry (as function of qi)
	 *
	 * If flags has AvgDelta set, we interpret the parameter Cf to be the
	 * average C, @f$\langle C\rangle=(C_f+C_{\bar{f}})/2@f$, and Cfbar to
	 * be the difference in C, @f$\Delta C=(C_f-C_{\bar{f}})/2@f$. The
	 * advantage to fitting for @f$\langle C\rangle@f$ and @f$\Delta C@f$
	 * are smaller correlations in the typical case, see e.g.
	 *
	 * http://de.arxiv.org/abs/1208.6463
	 *
	 * for details.
	 */
	DecRateCoeff(const char* name, const char* title, Flags flags,
		RooAbsCategory& qf, RooAbsCategory& qt,
		RooAbsReal& Cf, RooAbsReal& Cfbar,
		RooAbsReal& tageff, RooAbsReal& eta, RooAbsReal& etabar,
		RooAbsReal& aprod, RooAbsReal& adet,
		RooAbsReal& atageff);

	/** @brief constructor
	 *
	 * initialise pdf to average over initial flavour when using per-event
	 * mistag for N mutually exclusive taggers
	 *
	 * @param name		name of the pdf
	 * @param title		title of the pdf
	 * @param flags		flags
	 * @param qf		final state charge (-1, +1)
	 * @param qt		tagging decision (-N, ..., -1, 0, 1, ..., N)
	 * @param Cf		coefficient for qf = +1
	 * @param Cfbar		coefficient for qf = -1
	 * @param etaobs	mistag observable
	 * @param etapdfs	per-event-mistag distributions for tagged events
	 * @param tageffs	tagging efficiencies
	 * @param etas		(calibrated) per-event mistags
	 * @param aprod		production asymmetry
	 * @param adet		detection asymmetry
	 * @param atageffs	tagging efficiency asymmetries (as function of qi)
	 *
	 * If flags has AvgDelta set, we interpret the parameter Cf to be the
	 * average C, @f$\langle C\rangle=(C_f+C_{\bar{f}})/2@f$, and Cfbar to
	 * be the difference in C, @f$\Delta C=(C_f-C_{\bar{f}})/2@f$. The
	 * advantage to fitting for @f$\langle C\rangle@f$ and @f$\Delta C@f$
	 * are smaller correlations in the typical case, see e.g.
	 *
	 * http://de.arxiv.org/abs/1208.6463
	 *
	 * for details.
	 */

	DecRateCoeff(const char* name, const char* title, Flags flags,
		RooAbsCategory& qf, RooAbsCategory& qt,
		RooAbsReal& Cf, RooAbsReal& Cfbar,
		RooAbsRealLValue& etaobs, RooArgList& etapdfs,
		RooArgList& tageffs, RooArgList& etas,
		RooAbsReal& aprod, RooAbsReal& adet,
		RooArgList& atageffs);

	/** @brief constructor
	 *
	 * initialise pdf to average over initial flavour when using per-event
	 * mistag for N mutually exclusive taggers (with different mistags for
	 * true initial flavours)
	 *
	 * @param name		name of the pdf
	 * @param title		title of the pdf
	 * @param flags		flags
	 * @param qf		final state charge (-1, +1)
	 * @param qt		tagging decision (-N, ..., -1, 0, 1, ..., N)
	 * @param Cf		coefficient for qf = +1
	 * @param Cfbar		coefficient for qf = -1
	 * @param etaobs	mistag observable
	 * @param etapdfs	per-event-mistag distributions for tagged events
	 * @param tageffs	tagging efficiencies
	 * @param etas		(calibrated) per-event mistags for true B
	 * @param etabars	(calibrated) per-event mistags for true Bbar
	 * @param aprod		production asymmetry
	 * @param adet		detection asymmetry
	 * @param atageffs	tagging efficiency asymmetries (as function of qi)
	 *
	 * If flags has AvgDelta set, we interpret the parameter Cf to be the
	 * average C, @f$\langle C\rangle=(C_f+C_{\bar{f}})/2@f$, and Cfbar to
	 * be the difference in C, @f$\Delta C=(C_f-C_{\bar{f}})/2@f$. The
	 * advantage to fitting for @f$\langle C\rangle@f$ and @f$\Delta C@f$
	 * are smaller correlations in the typical case, see e.g.
	 *
	 * http://de.arxiv.org/abs/1208.6463
	 *
	 * for details.
	 */
	DecRateCoeff(const char* name, const char* title, Flags flags,
		RooAbsCategory& qf, RooAbsCategory& qt,
		RooAbsReal& Cf, RooAbsReal& Cfbar,
		RooAbsRealLValue& etaobs, RooArgList& etapdfs,
		RooArgList& tageffs, RooArgList& etas, RooArgList& etabars,
		RooAbsReal& aprod, RooAbsReal& adet,
		RooArgList& atageffs);

	/** @brief constructor
	 *
	 * initialise pdf to average over initial flavour when using an
	 * average mistag for N mutually exclusive taggers
	 *
	 * @param name		name of the pdf
	 * @param title		title of the pdf
	 * @param flags		flags
	 * @param qf		final state charge (-1, +1)
	 * @param qt		tagging decision (-N, ..., -1, 0, 1, ..., N)
	 * @param Cf		coefficient for qf = +1
	 * @param Cfbar		coefficient for qf = -1
	 * @param tageffs	tagging efficiencies
	 * @param etas		average mistags
	 * @param aprod		production asymmetry
	 * @param adet		detection asymmetry
	 * @param atageffs	tagging efficiency asymmetries (as function of qi)
	 *
	 * If flags has AvgDelta set, we interpret the parameter Cf to be the
	 * average C, @f$\langle C\rangle=(C_f+C_{\bar{f}})/2@f$, and Cfbar to
	 * be the difference in C, @f$\Delta C=(C_f-C_{\bar{f}})/2@f$. The
	 * advantage to fitting for @f$\langle C\rangle@f$ and @f$\Delta C@f$
	 * are smaller correlations in the typical case, see e.g.
	 *
	 * http://de.arxiv.org/abs/1208.6463
	 *
	 * for details.
	 */

	DecRateCoeff(const char* name, const char* title, Flags flags,
		RooAbsCategory& qf, RooAbsCategory& qt,
		RooAbsReal& Cf, RooAbsReal& Cfbar,
		RooArgList& tageffs, RooArgList& etas,
		RooAbsReal& aprod, RooAbsReal& adet,
		RooArgList& atageffs);

	/** @brief constructor
	 *
	 * initialise pdf to average over initial flavour when using an
	 * average mistag for N mutually exclusive taggers (with different
	 * mistags for true initial flavours)
	 *
	 * @param name		name of the pdf
	 * @param title		title of the pdf
	 * @param flags		flags
	 * @param qf		final state charge (-1, +1)
	 * @param qt		tagging decision (-N, ..., -1, 0, 1, ..., N)
	 * @param Cf		coefficient for qf = +1
	 * @param Cfbar		coefficient for qf = -1
	 * @param tageffs	tagging efficiencies
	 * @param etas		average mistags for true B
	 * @param etabars	average mistags for true Bbar
	 * @param aprod		production asymmetry
	 * @param adet		detection asymmetry
	 * @param atageffs	tagging efficiency asymmetries (as function of qi)
	 *
	 * If flags has AvgDelta set, we interpret the parameter Cf to be the
	 * average C, @f$\langle C\rangle=(C_f+C_{\bar{f}})/2@f$, and Cfbar to
	 * be the difference in C, @f$\Delta C=(C_f-C_{\bar{f}})/2@f$. The
	 * advantage to fitting for @f$\langle C\rangle@f$ and @f$\Delta C@f$
	 * are smaller correlations in the typical case, see e.g.
	 *
	 * http://de.arxiv.org/abs/1208.6463
	 *
	 * for details.
	 */
	DecRateCoeff(const char* name, const char* title, Flags flags,
		RooAbsCategory& qf, RooAbsCategory& qt,
		RooAbsReal& Cf, RooAbsReal& Cfbar,
		RooArgList& tageffs, RooArgList& etas, RooArgList& etabars,
		RooAbsReal& aprod, RooAbsReal& adet,
		RooArgList& atageffs);

	/** @brief copy constructor
	 *
	 * @param other	pdf to copy
	 * @param name	name for the copy of other
	 */
	DecRateCoeff(
		const DecRateCoeff& other, const char* name = 0);

	/** @brief clone
	 *
	 * @param newname	name for the copy of other
	 * @returns		pointer to copy of this object
	 */
	virtual TObject* clone(const char* newname) const;

	/// destructor
	virtual ~DecRateCoeff();

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

	/** @brief consistency check to be used in constructor
	 *
	 * @param obs		set of observables
	 * @param params	set of other parameters
	 * @param etaobs	mistag observable (if set)
	 * @param etas		calibrated mistags (if non-empty)
	 *
	 * @returns true if sets valid input for construction of DecRateCoeff
	 *
	 * observables must not overlap with one another, and parameters must
	 * not overlap with observables, be constant or inherit from
	 * TaggingCat.
	 *
	 * tagging calibrations may (obviously) depend on the mistag
	 * observables.
	 */
	bool checkDepsForConsistency(
		const RooArgSet& obs, const RooArgSet& params,
		const RooAbsArg* etaobs = 0,
		const RooArgSet& etas = RooArgSet()) const;

	/// get maximial value of the index in qt
	unsigned getMaxQt() const;

	/// convert qt = -N, -N + 1, ..., -1, 0, 1, ..., N - 1, N to index
	static inline unsigned idxFromQt(int qt)
	{ return (qt < 0) ? (2u * (-qt) - 1u) : (2u * qt); }

	/// convert index to qt
	static inline int qtFromIdx(unsigned idx)
	{
	    const unsigned absqt = (1u + idx) / 2u;
	    return (idx & 1) ? -absqt : absqt;
	}

	/** @brief fill ListProxies, respecting conventions
	 *
	 * @param proxy		set proxy to fill
	 * @param list		what to fill with (qt > 0)
	 * @param listbar	what to fill with (qt < 0)
	 * @param zeroelem	what to fill with (qt == 0)
	 */
	void fillListProxy(RooListProxy& proxy,
		const RooArgList& list, const RooArgList& listbar,
		const RooAbsArg& zeroelem) const;

	/** @brief initialise our member list proxies with the RIGHT STUFF
	 *
	 * this routine factors out common functionality needed in the
	 * constructors
	 * 
	 * @param tageffs	tagging efficiencies (|qt| = 1, ...)
	 * @param atageffs	tagging efficiency asymmetries
	 * @param etas		mistags (for B, also for Bbar if etabars empty)
	 * @param etabars	mistags (for Bbar)
	 * @param etapdfs	mistag distributions
	 */
	void initListProxies(
		const RooArgList& tageffs, const RooArgList& atageffs,
		const RooArgList& etas,
		const RooArgList& etabars = RooArgList(),
		const RooArgList& etapdfs = RooArgList());

    private:
	static RooArgSet s_emptyset;	///< empty RooArgSet
	static RooConstVar s_one;	///< +1 (constant)
	RooCategoryProxy m_qf;		///< final state charge
	RooListProxy m_qts;		///< tagging decision(s)
	RooRealProxy m_Cf;		///< coefficient for qf = +1
	RooRealProxy m_Cfbar;		///< coefficient for qf = -1
	RooRealProxy m_etaobs;		///< mistag observable
	/// instance of per-event mistag pdf for untagged events (flat)
	RooUniform m_etapdfutinstance;
	RooListProxy m_etapdfs;		///< mistag distributions
	RooListProxy m_tageffs;		///< tagging efficiencies
	RooListProxy m_etas;		///< per-event/average mistags
	RooRealProxy m_aprod;		///< production asymmetry
	RooRealProxy m_adet;		///< detection asymmetry
	RooListProxy m_atageffs;	///< tagging efficiency asymmetries
	/// integral cache
	mutable RooObjCacheManager m_cacheMgr; //! transient member
	/// place to keep normalisation sets
	mutable std::map<UInt_t, RooArgSet> m_nsets; //! transient member
	/// current normalisation set
	mutable const RooArgSet* m_nset; //! transient member
	/// hash of current normalisation set
	mutable UInt_t m_nsethash; //! transient member
        Flags m_flags;			///< flags

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
		CacheElem(const DecRateCoeff& parent,
			const RooArgSet& iset, const RooArgSet* nset = 0,
			const TNamed* rangeName = 0);

		/// destructor
		virtual ~CacheElem();
		
		/// return list of contained RooFit variables
		virtual RooArgList containedArgs(Action);
		
		/// return value of cache element
		double eval(const double alphapp, const double alphapm,
			const double alphamp, const double alphamm) const;

		/// print what the CacheElem contains for debugging (stdout)
		void print() const;
	
	    protected:
		/// set up binned evaluation of integral of eta product
		void setupBinnedProductIntegral(
			RooAbsReal* &prod, const RooAbsPdf& etapdf,
			const RooAbsReal& eta, int idx);

		/// return integral over mistag pdf (tagged events)
		inline double etaintpdf(const int qt) const
		{
		   const unsigned idx = DecRateCoeff::idxFromQt(qt);
		    return (m_etapdfs.empty() || !m_etapdfs[idx]) ? 1.0 :
			m_etapdfs[idx]->getValV(
				(m_flags & NormEta) ? &m_nset : 0);
		}

		/// return integral over mistag pdf times mistag (tagged events)
		double etaintprodpdf(const int qt) const;
		
		/// return value of qf pdf
		double qfpdf(const int qf) const;
		
		/// return value of (q_t, eta) pdf
		double qtetapdf(const int qf, const int qt,
			const double alphapp, const double alphapm,
			const double alphamp, const double alphamm) const;

		/// set up integration and normalisation sets, return iset
		RooArgSet setupISetAndNSet(
			const RooArgSet& iset, const RooArgSet* nset = 0);

		/// set things up for per event mistag
		void setupForPerEventEta(
			const RooArgSet& iset, const RooArgSet* nset = 0);

		/// set up bin boundaries for binned evaluation
		void setupBinBoundaries(unsigned idx);

	    private:
		/// (integrals over) mistag pdfs
		std::vector<RooAbsReal*> m_etapdfs;
		/// (integrals over) mistag pdfs times mistags
		std::vector<RooAbsReal*> m_prodetapdfs;
		/// normalisation set to use for mistag pdfs
		RooArgSet m_nset;
		/// integration range
		const char* m_rangeName;
		/// parent
		const DecRateCoeff& m_parent;

		// for binned eta pdfs, we sum up our P(eta) * eta_c(eta)
		// product integrals ourselves; we need a couple of extra
		// members for that
		
		/// name of the working range
		std::vector<std::string> m_workRangeNames;
		/// two RooRealProxyVars so we can move the range over eta
		std::vector<std::pair<RooAbsRealLValue*, RooAbsRealLValue*> >
		    m_workRanges;
		/// a vector of eta bin boundaries
		std::vector<std::vector<double> > m_etabins;
		/// cached value of product integral
		mutable std::vector<double>
		    m_prodcachedvals; //! transient data member

		/// maximal value of qt
		const unsigned m_qtmax;
		
		/// flags
		enum Flags {
		    None = 0,
		    IntQf = 1,		/// integrate over qf
		    IntQt = 2,		/// integrate over qt
		    IntEta = 4,		/// integrate over eta
		    NormEta = 8,	/// eta is in normalisation set
		    ProdIntBinned = 16	/// binned product: m_etapdf and m_eta
		} m_flags;
	};

	ClassDef(DecRateCoeff, 3);
};

#endif // DECRATECOEFF

// vim: sw=4:tw=78:ft=cpp
