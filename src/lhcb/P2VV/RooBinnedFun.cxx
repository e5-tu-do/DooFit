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
// Piecewise constant function, suitable for use in RooGaussEfficiencyModel
// as efficiency
// END_HTML
//
// STD & STL
#include <cmath>
#include <complex>

// ROOT
#include "TMath.h"
#include "TH1.h"

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
#include "P2VV/RooBinnedFun.h"

using namespace std;

//_____________________________________________________________________________
RooBinnedFun::RooBinnedFun()
{
}

//_____________________________________________________________________________
RooBinnedFun::RooBinnedFun(const char* name, const char* title,
                           RooRealVar& x, const TH1* hist,
                           bool constCoeffs) :
  RooAbsGaussModelEfficiency(name, title),
  _x("x", "Dependent", this, x),
  _coefList("coefficients","List of coefficients",this)
{
    int nBins = hist->GetNbinsX();
    _u.reserve(nBins+1);
    for (int i=0;i<nBins ;++i) {
        _u.push_back(  hist->GetBinLowEdge(1+i) );
        double value = hist->GetBinContent(1+i) ;
        if (constCoeffs) {
           _coefList.add( RooFit::RooConst( value ) );
        } else {
           TString n = TString::Format("%s_bin_%d",name,i);
           RooRealVar* coeff = new RooRealVar(n, n, value, 0.0, 1.0 );
           _coefList.add(*coeff);
           addOwnedComponents( *coeff );
        }
    }
    _u.push_back( hist->GetBinLowEdge(nBins)+hist->GetBinWidth(nBins) );
}

//_____________________________________________________________________________
RooBinnedFun::RooBinnedFun(const char* name, const char* title,
                           RooRealVar& x, const char *binningName,
                           const RooArgList& coefList):
  RooAbsGaussModelEfficiency(name, title),
  _x("x", "Dependent", this, x),
  _coefList("coefficients","List of coefficients",this)
{
  const RooAbsBinning* binning = x.getBinningPtr(binningName);
  assert( binning!=0);
  if ( coefList.getSize()+1!=binning->numBoundaries())  {
        cout << TString::Format( "you have specified %d coefficients for %d boundaries. The differentce should 1!",coefList.getSize(),binning->numBoundaries()) << endl;
        throw TString::Format( "you have specified %d coefficients for %d boundaries. The differentce should 1!",coefList.getSize(),binning->numBoundaries());
  }
  _coefList.add(coefList);
  Double_t* boundaries = binning->array();
  _u.insert(_u.end(), boundaries, boundaries + binning->numBoundaries() );
}

//_____________________________________________________________________________
RooBinnedFun::RooBinnedFun(const RooBinnedFun& other, const char* name) :
  RooAbsGaussModelEfficiency(other, name),
  _x("x", this, other._x),
  _coefList("coefList", this, other._coefList),
  _u(other._u)
{
}

//_____________________________________________________________________________
RooBinnedFun::~RooBinnedFun()
{
}

namespace {
  Double_t get(const RooArgList& b,int i) { return ((RooAbsReal&)b[i]).getVal() ; }
}
//_____________________________________________________________________________
Double_t RooBinnedFun::evaluate() const
{
    if (_x<_u.front()) get(_coefList,0);
    if (_x>_u.back())  get(_coefList,_u.size()-2);
    std::vector<double>::const_iterator i = --std::upper_bound(_u.begin(),_u.end()-1,_x);
    return get(_coefList,std::distance(_u.begin(),i));
}

//_____________________________________________________________________________
Int_t RooBinnedFun::getAnalyticalIntegral(RooArgSet& allVars, RooArgSet& analVars, const char* rangeName) const
{
  // No analytical calculation available (yet) of integrals over subranges
  if (_x.min(rangeName)!=_u.front() || _x.max(rangeName)!=_u.back()) return 0 ;
  // check that range goes from first to last knot...
  if (matchArgs(allVars, analVars, _x)) return 1;
  return 0;
}
//_____________________________________________________________________________
Double_t RooBinnedFun::analyticalIntegral(Int_t code, const char* /* rangeName */) const
{
  if (code != 1) {
    coutE(InputArguments) << "RooBinnedFun::analyticalIntegral(" << GetName()
        << "): argument \"code\" can only have value 1" << std::endl;
    assert(code==1) ;
  }
  Double_t result = 0;
  for(unsigned i=0;i<_u.size()-1;++i) {
        // TODO: check range...
        result += ( _u[i+1]-_u[i] ) * get(_coefList,i);
  }
  return result;
}



//_____________________________________________________________________________
std::complex<double>
RooBinnedFun::productAnalyticalIntegral(Double_t umin, Double_t umax,
                                             Double_t scale, Double_t offset,
                                             const std::complex<double>& z) const
{
    RooGaussModelAcceptance::K_n K(z);
    typedef RooGaussModelAcceptance::M_n<1U> M_n;
    std::vector<M_n> M; M.reserve( _u.size() );
    for (unsigned int i=0;i<_u.size();++i) {
        double x = (_u[i]-offset)/scale ;
        M.push_back( M_n( x, z ) );
    }
    double lo = scale*umin+offset;
    double hi = scale*umax+offset;
    //TODO: verify we remain within [lo,hi]
    assert(fabs(lo-_u.front())<1e-7*fabs(lo+_u.front()));
    assert(fabs(hi-_u.back())<1e-7*fabs(hi+_u.back()));
    std::complex<double> sum(0,0);
    for (unsigned i=0; i<_u.size()-1 && _u[i]<hi ;++i) {
        if (_u[i+1]<lo) continue;
        // FIXME:TODO: we currently assume that u(0),u(knotSize()-1)] fully contained in [lo,hi]
        M_n dM = M[i+1]-M[i]; // take M[i] if lo<=u(i) else M_n(lo) ; take M[i+1] if u(i+1)<=hi else M_n(hi)
        sum += dM(0)*K(0) * get(_coefList, i ) ;
    }
    return sum;
}

//_____________________________________________________________________________
Int_t RooBinnedFun::getMaxVal(const RooArgSet& vars) const
{
    // check that vars only contains _x...
    return ( vars.getSize() == 1 && vars.contains( _x.arg() ) ) ? 1 : 0;
}
//_____________________________________________________________________________
Double_t RooBinnedFun::maxVal(Int_t code) const
{
    if (code != 1) {
      coutE(InputArguments) << "RooBinnedFun::maxVal(" << GetName()
          << "): argument \"code\" can only have value 1" << std::endl;
      assert(code==1);
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

std::list<Double_t>* RooBinnedFun::binBoundaries(RooAbsRealLValue& obs,
    Double_t xlo, Double_t xhi) const
{
   // Check that we have observable, if not no binning is returned
   if ( _x.arg().GetName() != obs.GetName()) return 0;

   std::list<Double_t>* bounds = new list<Double_t>;
   for (size_t i=0; i<_u.size(); ++i) {
      if (_u[i]>=xlo && _u[i]<=xhi) bounds->push_back(_u[i]);
   }
   return bounds;
}
