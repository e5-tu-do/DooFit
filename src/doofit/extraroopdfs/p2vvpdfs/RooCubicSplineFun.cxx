/*****************************************************************************
 * Project: RooFit                                                           *
 * Package: RooFitModels                                                     *
 * @(#)root/roofit:$Id: RooBSpline.cxx 45780 2012-08-31 15:45:27Z moneta $
 * Authors:                                                                  *
 *   Gerhard Raven
 *                                                                           *
 *****************************************************************************/

//////////////////////////////////////////////////////////////////////////////
//
// BEGIN_HTML
// BSpline basis polynomials are positive-definite in the range [0,1].
// In this implementation, we extend [0,1] to be the range of the parameter.
// There are n+1 BSpline basis polynomials of degree n.
// Thus, by providing N coefficients that are positive-definite, there
// is a natural way to have well bahaved polynomail PDFs.
// For any n, the n+1 basis polynomials 'form a partition of unity', eg.
//  they sum to one for all values of x. See
// END_HTML
//
// STD & STL
#include <cmath>
#include <complex>
#include <iterator>
#include <algorithm>
#include <sstream>

// ROOT
#include "TMath.h"
#include "TH1.h"
#include "TGraph.h"
#include "TGraphErrors.h"

// RooFit
#include "RooFit.h"
#include "Riostream.h"
#include "RooMsgService.h"
#include "RooMath.h"
#include "RooAbsReal.h"
#include "RooRealVar.h"
#include "RooConstVar.h"
#include "RooArgList.h"

// P2VV
#include "P2VV/RooCubicSplineFun.h"
#include "P2VV/RooCubicSplineKnot.h"

using namespace std;

//_____________________________________________________________________________
void RooCubicSplineFun::init(const char* name,
                             const vector<double>& heights,
                             const vector<double>& errors,
                             double smooth, bool constCoeffs) {
   vector<double> values(heights);
   if ( smooth > 0 ) {
      assert(int(errors.size()) == _aux.size());
      _aux.smooth( values, errors, smooth );
   }
   _aux.computeCoefficients( values );
   for (unsigned int i=0;i<values.size();++i) {
      if (constCoeffs) {
         _coefList.add( RooFit::RooConst( values[i] ) );
      } else {
         stringstream name_str;
         name_str << name << "_smoothed_bin_" << i;
         string n(name_str.str());
         RooRealVar* coeff = new RooRealVar(n.c_str(), n.c_str(), values[i], 0.0, 1.0);
         if (i == 0 || i == values.size() - 1) coeff->setConstant(true);
         _coefList.add(*coeff);
         addOwnedComponents( *coeff );
      }
   }
}

//_____________________________________________________________________________
RooCubicSplineFun::RooCubicSplineFun()
    : _aux(0,0)
{
}

//_____________________________________________________________________________
RooCubicSplineFun::RooCubicSplineFun(const char* name, const char* title, RooRealVar& x,
                                     const vector<double>& knots,
                                     const vector<double>& values,
                                     const vector<double>& errors,
                                     double smooth, bool constCoeffs) :
   RooAbsGaussModelEfficiency(name, title),
   _x("x", "Dependent", this, x),
   _coefList("coefficients","List of coefficients",this),
   _aux(knots.begin(),knots.end())
{
   init(name, values, errors, smooth, constCoeffs);
}

//_____________________________________________________________________________
RooCubicSplineFun::RooCubicSplineFun(const char* name, const char* title,
                                     RooRealVar& x, const TGraph* graph, bool constCoeffs) :
  RooAbsGaussModelEfficiency(name, title),
  _x("x", "Dependent", this, x),
  _coefList("coefficients","List of coefficients",this),
  _aux(0,0)
{
    int nPoints = graph->GetN();
    vector<double> centres, values;
    for (int i=0;i<nPoints ;++i) {
        double x,y;
        graph->GetPoint(i,x,y);
        centres.push_back(x);
        values.push_back(y);
    }
   _aux = RooCubicSplineKnot( centres.begin(), centres.end() );
    vector<double> errs;
    init(name, values, errs, 0, constCoeffs);
}

//_____________________________________________________________________________
RooCubicSplineFun::RooCubicSplineFun(const char* name, const char* title,
                                     RooRealVar& x, const TH1* hist, double smooth,
                                     bool constCoeffs) :
  RooAbsGaussModelEfficiency(name, title),
  _x("x", "Dependent", this, x),
  _coefList("coefficients","List of coefficients",this),
  _aux(0,0)
{
    // bin 0 is underflow, and bin nBins + 1 is overflow...
    int nBins = hist->GetNbinsX();
    vector<double> centres, values;
    for (int i=0;i<nBins ;++i) {
        centres.push_back(hist->GetBinCenter(1+i));
        values.push_back(hist->GetBinContent(1+i));
    }
   _aux = RooCubicSplineKnot( centres.begin(), centres.end() );

    vector<double> errs;
    if (smooth>0) for (int i=0;i<nBins ;++i) errs.push_back(hist->GetBinError(1+i));

    init(name, values, errs, smooth, constCoeffs);
}
//_____________________________________________________________________________
RooCubicSplineFun::RooCubicSplineFun(const char* name, const char* title,
                                     RooRealVar& x, const TGraphErrors* graph, double smooth,
                                     bool constCoeffs) :
  RooAbsGaussModelEfficiency(name, title),
  _x("x", "Dependent", this, x),
  _coefList("coefficients","List of coefficients",this),
  _aux(0,0)
{
    int nPoints = graph->GetN();
    vector<double> centres, values;
    for (int i=0;i<nPoints ;++i) {
        double x,y;
        graph->GetPoint(i,x,y);
        centres.push_back(x);
        values.push_back(y);
    }
   _aux = RooCubicSplineKnot( centres.begin(), centres.end() );

    vector<double> errs;
    if (smooth>0) for (int i=0;i<nPoints ;++i) errs.push_back(graph->GetErrorY(i));

    init(name, values, errs, smooth, constCoeffs);
}

//_____________________________________________________________________________
RooCubicSplineFun::RooCubicSplineFun(const char* name, const char* title,
                                     RooRealVar& x, const char* knotBinningName,
                                     const RooArgList& coefList):
  RooAbsGaussModelEfficiency(name, title),
  _x("x", "Dependent", this, x),
  _coefList("coefficients", "List of coefficients", this),
  _aux(0,0)
{
  // TODO: verify coefList is consistent with knots as specified by the knotBinningName binning
  //    should be N+2 coefficients for N bins...
  const RooAbsBinning* binning = x.getBinningPtr(knotBinningName);
  assert( binning!=0);
  if ( coefList.getSize()!=2+binning->numBoundaries())  {
        cout << TString::Format( "you have specified %d coefficients for %d knots. The differentce should 2!",coefList.getSize(),binning->numBoundaries()) << endl;
        throw TString::Format( "you have specified %d coefficients for %d knots. The differentce should 2!",coefList.getSize(),binning->numBoundaries());
  }
  _coefList.add(coefList);

  Double_t* boundaries = binning->array();
  _aux = RooCubicSplineKnot( boundaries, boundaries + binning->numBoundaries() );
}

//_____________________________________________________________________________
RooCubicSplineFun::RooCubicSplineFun(const char* name, const char* title,
                                     RooRealVar& x, const vector<double>& knots,
                                     const RooArgList& coefList):
  RooAbsGaussModelEfficiency(name, title),
  _x("x", "Dependent", this, x),
  _coefList("coefficients", "List of coefficients", this),
   _aux(knots.begin(), knots.end())
{
   assert(size_t(coefList.getSize()) == 2 + knots.size());
   _coefList.add(coefList);
}

//_____________________________________________________________________________
RooCubicSplineFun::RooCubicSplineFun(const RooCubicSplineFun& other, const char* name) :
  RooAbsGaussModelEfficiency(other, name),
  _x("x", this, other._x),
  _coefList("coefList", this, other._coefList),
  _aux(other._aux)
{
}

//_____________________________________________________________________________
RooCubicSplineFun::~RooCubicSplineFun()
{
}

//_____________________________________________________________________________
Double_t RooCubicSplineFun::evaluate() const
{
  return _aux.evaluate(_x,_coefList);
}

//_____________________________________________________________________________
Int_t RooCubicSplineFun::getAnalyticalIntegral(RooArgSet& allVars, RooArgSet& analVars, const char* rangeName) const
{
  // No analytical calculation available (yet) of integrals over subranges
  if (_x.min(rangeName)!=_aux.knots().front() || _x.max(rangeName)!=_aux.knots().back() ) return 0;
  if (matchArgs(allVars, analVars, _x)) return 1;
  return 0;
}
//_____________________________________________________________________________
Double_t RooCubicSplineFun::analyticalIntegral(Int_t code, const char* /* rangeName */) const
{
  if (code != 1) {
    coutE(InputArguments) << "RooCubicSplineFun::analyticalIntegral(" << GetName()
        << "): argument \"code\" can only have value 1" << std::endl;
    assert(code==1) ;
  }
  return _aux.analyticalIntegral(_coefList);
}

//_____________________________________________________________________________
complex<double>  RooCubicSplineFun::gaussIntegralE(bool left, const RooGaussModelAcceptance::M_n<4U>& dM,
                                             const RooGaussModelAcceptance::K_n& K, double offset,
                                             double* sc) const
{
        RooCubicSplineKnot::S_edge s_jk( _aux.S_jk_edge( left, _coefList ), offset );
        return dM(0)*( s_jk(0,0) * K(0) * sc[0] + s_jk(0,1) * K(1) * sc[1] )
             + dM(1)*( s_jk(1,0) * K(0) * sc[1]                            );
}

//_____________________________________________________________________________
std::complex<double>
RooCubicSplineFun::productAnalyticalIntegral(Double_t umin, Double_t umax,
                                             Double_t scale, Double_t offset,
                                             const std::complex<double>& z) const
{
    RooGaussModelAcceptance::K_n K(z);
    assert(knotSize()>1);
    typedef RooGaussModelAcceptance::M_n<4U> M_n;
    std::vector<M_n> M; M.reserve( knotSize() );
    for (unsigned int i=0;i<knotSize();++i) {
        double x = (u(i)-offset)/scale ;
        M.push_back( M_n( x, z ) );
    }
    double sc[4]; for (int i=0;i<4;++i) sc[i] = pow(scale,i);
    double lo = scale*umin+offset;
    double hi = scale*umax+offset;
    std::complex<double> sum(0,0);
    //TODO: verify we remain within [lo,hi]
    assert(hi>=u(0)); // front only if hi>u(0)!!!
    if (lo<u(0)) sum += gaussIntegralE(true,  M.front()-M_n( umin,z), K, offset, sc);
    for (unsigned i=0;i<knotSize()-1 && u(i)<hi ;++i) {
        if (u(i+1)<lo) continue;
        // FIXME:TODO: we currently assume that u(0),u(knotSize()-1)] fully contained in [lo,hi]
        assert(lo<=u(i));
        assert(u(i+1)<=hi);
        M_n dM = M[i+1]-M[i]; // take M[i] if lo<=u(i) else M_n(lo) ; take M[i+1] if u(i+1)<=hi else M_n(hi)
        RooCubicSplineKnot::S_jk s_jk( _aux.S_jk_sum( i, _coefList ), offset );  // pass sc into S_jk, remove from loop
        for (int j=0;j<4;++j) for (int k=0;k<4-j;++k) sum += dM(j)*s_jk(j,k)*K(k)*sc[j+k];
    }
    assert(lo<=u(knotSize()-1));// back only if lo<u(knotsiwze()-1)!!!
    if (hi>u(knotSize()-1)) sum += gaussIntegralE(false, M_n(umax,z)-M.back(),   K, offset, sc);
    return sum;
}

//_____________________________________________________________________________
Int_t RooCubicSplineFun::getMaxVal(const RooArgSet& vars) const
{
    // check that vars only contains _x...
    return ( vars.getSize() == 1 && vars.contains( _x.arg() ) ) ? 1 : 0;
}
//_____________________________________________________________________________
Double_t RooCubicSplineFun::maxVal(Int_t code) const
{
    if (code != 1) {
      coutE(InputArguments) << "RooCubicSplineFun::maxVal(" << GetName()
          << "): argument \"code\" can only have value 1" << std::endl;
      assert(code==1) ;
    }
    RooFIter iter = _coefList.fwdIterator();
    RooAbsReal *c(0);
    double res = 0;
    while((c=(RooAbsReal*)iter.next())) {
          double x = fabs(c->getVal());
          if (x>res)  { res = x; }
    }
    return res;
}
