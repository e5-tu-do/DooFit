/*****************************************************************************
 * Project: RooFit                                                           *
 * Package: RooFitModels                                                     *
 * @(#)root/roofit:$Id: RooGaussModel.cxx 44982 2012-07-10 08:36:13Z moneta $
 * Authors:                                                                  *
 *   WV, Wouter Verkerke, UC Santa Barbara, verkerke@slac.stanford.edu       *
 *   DK, David Kirkby,    UC Irvine,         dkirkby@uci.edu                 *
 *                                                                           *
 * Copyright (c) 2000-2005, Regents of the University of California          *
 *                          and Stanford University. All rights reserved.    *
 *                                                                           *
 * Redistribution and use in source and binary forms,                        *
 * with or without modification, are permitted according to the terms        *
 * listed in LICENSE (http://roofit.sourceforge.net/license.txt)             *
 *****************************************************************************/

//////////////////////////////////////////////////////////////////////////////
//
// BEGIN_HTML
// Class RooGaussModel implements a RooResolutionModel that models a Gaussian
// distribution. Object of class RooGaussModel can be used
// for analytical convolutions with classes inheriting from RooAbsAnaConvPdf
// After  the convolution  is applied,  the result is multiplied with an
// efficiency, which can be any implementation of the RooAbsGaussModelEfficiency
// interface
// END_HTML
//
#include <memory>

#include "RooFit.h"

#include "TMath.h"
#include "Riostream.h"
#include "RooMsgService.h"
#include "P2VV/RooGaussEfficiencyModel.h"
#include "P2VV/RooEffConvGenContext.h"
#include "RooMath.h"
#include "RooRealConstant.h"
#include "RooRandom.h"

using namespace std;

namespace {
    enum basisType { noBasis=0  ,  expBasis= 3
                   , sinBasis=13,  cosBasis=23
                   , sinhBasis=63, coshBasis=53 };
    static const Double_t rootpi(sqrt(TMath::Pi())) ;

    std::complex<double> evalApprox(Double_t x, const std::complex<double>& z) {
      // compute exp(-x^2)cwerf(-i(z-x)), cwerf(z) = exp(-z^2)erfc(-iz)
      // use the approximation: erfc(z) = exp(-z*z)/(sqrt(pi)*z)
      // to explicitly cancel the divergent exp(y*y) behaviour of
      // CWERF for z = x + i y with large negative y
      static const std::complex<double> mi(0,-1);
      std::complex<double> zp  = mi*(z-x);
      std::complex<double> zsq = zp*zp;
      std::complex<double> v = -zsq - x*x;
      std::complex<double> iz(z.imag()+x,z.real()-x); // ???
      return exp(v)*(exp(zsq)/(iz*rootpi) + 1.)*2. ;
    }

    // Calculate Re[exp(-x^2) cwerf(i (z-x) )], taking care of numerical instabilities
    Double_t evalRe(Double_t x, const std::complex<double>& z) {
      Double_t re =  z.real()-x;
      return (re>-5.0) ? RooMath::faddeeva_fast(std::complex<double>(-z.imag(),re)).real()*exp(-x*x)
                       : evalApprox(x,z).real() ;
    }

    // Calculate Im[exp(-x^2) cwerf(i(z-x))], taking care of numerical instabilities
    Double_t evalIm(Double_t x, const std::complex<double>& z) {
      Double_t re = z.real()-x;
      return (re>-5.0) ? RooMath::faddeeva_fast(std::complex<double>(-z.imag(),re)).imag()*exp(-x*x)
                       : evalApprox(x,z).imag() ;
    }

}


//_____________________________________________________________________________
RooGaussEfficiencyModel::RooGaussEfficiencyModel(const char *name, const char *title
                , RooRealVar& x, RooAbsGaussModelEfficiency& _eff
			    , RooAbsReal& _mean, RooAbsReal& _sigma )
   : RooResolutionModel(name, title, x),
     RooAbsEffResModel(),
     _flatSFInt(kFALSE),
     eff("eff","Spline describing efficiency",this,_eff),
     mean("mean","Mean",this,_mean),
     sigma("sigma","Width",this,_sigma),
     msf("msf","Mean Scale Factor",this,RooRealConstant::value(1.0)),
     ssf("ssf","Sigma Scale Factor",this,RooRealConstant::value(1.0))
{
   // make sure 'x' matches the eff argument!
   std::auto_ptr<RooArgSet> svar( eff.arg().getVariables() );
   assert( svar->contains( convVar() ) );
}

//_____________________________________________________________________________
RooGaussEfficiencyModel::RooGaussEfficiencyModel(const char *name, const char *title
                                                   , RooRealVar& x, RooAbsGaussModelEfficiency& _eff
                                                   , RooAbsReal& _mean, RooAbsReal& _sigma
                                                   , RooAbsReal& _meanSF, RooAbsReal& _sigmaSF)
   : RooResolutionModel(name,title,x),
     RooAbsEffResModel(),
     _flatSFInt(kFALSE),
     eff("eff","Spline describing efficiency",this,_eff),
     mean("mean","Mean",this,_mean),
     sigma("sigma","Width",this,_sigma),
     msf("msf","Mean Scale Factor",this,_meanSF),
     ssf("ssf","Sigma Scale Factor",this,_sigmaSF)
{
   // make sure 'x' matches the spline argument!
   std::auto_ptr<RooArgSet> svar( eff.arg().getVariables() );
   assert( svar->contains( convVar() ) );
}

//_____________________________________________________________________________
RooGaussEfficiencyModel::RooGaussEfficiencyModel(const RooGaussEfficiencyModel& other,
                                                   const char* name)
   : RooResolutionModel(other,name),
     RooAbsEffResModel(),
     _flatSFInt(other._flatSFInt),
     eff("eff",this,other.eff),
     mean("mean",this,other.mean),
     sigma("sigma",this,other.sigma),
     msf("msf",this,other.msf),
     ssf("ssf",this,other.ssf)
{
}

//_____________________________________________________________________________
RooGaussEfficiencyModel::~RooGaussEfficiencyModel()
{
  // Destructor
}

//_____________________________________________________________________________
Int_t RooGaussEfficiencyModel::basisCode(const char* name) const
{
  if (!TString("exp(-@0/@1)"              ).CompareTo(name)) return expBasis;
  if (!TString("exp(-@0/@1)*sin(@0*@2)"   ).CompareTo(name)) return sinBasis;
  if (!TString("exp(-@0/@1)*cos(@0*@2)"   ).CompareTo(name)) return cosBasis;
  if (!TString("exp(-@0/@1)*cosh(@0*@2/2)").CompareTo(name)) return coshBasis;
  if (!TString("exp(-@0/@1)*sinh(@0*@2/2)").CompareTo(name)) return sinhBasis;
  return 0 ;
}

//_____________________________________________________________________________
const RooAbsReal* RooGaussEfficiencyModel::efficiency() const
{
    return &eff.arg();
}

//_____________________________________________________________________________
RooArgSet* RooGaussEfficiencyModel::observables() const {
   // Return pointer to pdf in product
   // verify whether efficiency depends on additional observables!!!
   return new RooArgSet(convVar());
}

//_____________________________________________________________________________
std::complex<double> RooGaussEfficiencyModel::evalInt(Double_t umin, Double_t umax,
                                                       Double_t scale, Double_t offset,
                                                       const std::complex<double>& z) const
{
    const RooAbsGaussModelEfficiency &sp = dynamic_cast<const RooAbsGaussModelEfficiency&>( eff.arg() );
    return sp.productAnalyticalIntegral( umin, umax, scale, offset, z) ;
}

//_____________________________________________________________________________
Double_t RooGaussEfficiencyModel::evaluate() const
{
  basisType basisCode = (basisType) _basisCode ;
  Double_t tau    = (basisCode!=noBasis)                           ? ((RooAbsReal*)basis().getParameter(1))->getVal() : 0 ;
  Double_t omega  = (basisCode==sinBasis  || basisCode==cosBasis)  ? ((RooAbsReal*)basis().getParameter(2))->getVal() : 0 ;
  Double_t dGamma = (basisCode==sinhBasis || basisCode==coshBasis) ? ((RooAbsReal*)basis().getParameter(2))->getVal() : 0 ;
  if (basisCode  == coshBasis && basisCode!=noBasis && dGamma==0 ) basisCode = expBasis;

  Double_t scale = sigma*ssf*TMath::Sqrt2();
  Double_t u = (x-mean*msf)/scale;
  // *** 1st form: Straight Gaussian, used for unconvoluted PDF or expBasis with 0 lifetime ***
  if (basisCode==noBasis || ((basisCode==expBasis || basisCode==cosBasis) && tau==0)) {
    if (verboseEval()>2) cout << "RooGaussEfficiencyModel::evaluate(" << GetName() << ") 1st form" << endl ;
    Double_t eff=efficiency()->getVal();
    if (TMath::IsNaN(eff))
       cxcoutE(Tracing) << "RooGaussEfficiencyModel::evaluate(" << GetName()
                        << ") got nan during efficiency " << endl;
    return eff * exp(-u*u)/(scale*rootpi) ; // ???
  }

  // *** 2nd form: 0, used for sinBasis, linBasis, and quadBasis with tau=0 ***
  if (tau==0) {
    if (verboseEval()>2) cout << "RooGaussEfficiencyModel::evaluate(" << GetName() << ") 2nd form" << endl ;
    return 0. ;
  }
  std::complex<double> z( double(1)/tau, -omega ); z*=0.5*scale;

  Double_t val(0);
  if (verboseEval()>2) cout << "RooGaussEfficiencyModel::evaluate(" << GetName() << ") basisCode = " <<  basisCode << " z = " << z << ", u = " << u << endl ;

  switch (basisCode) {
    case expBasis:
    case cosBasis:
        val +=             evalRe(u,z);
        break;
    case sinBasis:
        val += z.imag()!=0 ? evalIm(u,z) : 0;
        break;
    case coshBasis:
    case sinhBasis: {
        std::complex<double> y( scale * dGamma / 4 , 0 );
        val += (                                      evalRe(u,z-y)
               + ( basisCode == coshBasis ? +1 : -1 )*evalRe(u,z+y) )/2;
        break;
    }
    default:
        assert(0);
  }
  if (TMath::IsNaN(val))
     cxcoutE(Tracing) << "RooGaussEfficiencyModel::evaluate(" << GetName()
                      << ") got nan during basisCode = " << basisCode << endl;
  Double_t _eff=eff;
  if (TMath::IsNaN(_eff))
     cxcoutE(Tracing) << "RooGaussEfficiencyModel::evaluate(" << GetName()
                      << ") got nan during efficiency " << endl;
  return _eff*val;
}

//_____________________________________________________________________________
Int_t RooGaussEfficiencyModel::getAnalyticalIntegral(RooArgSet& allVars, RooArgSet& analVars, const char* /*rangeName*/) const
{
  switch(_basisCode) {
  // Analytical integration capability of raw PDF
  case noBasis:
    // Optionally advertise flat integral over sigma scale factor
    if (_flatSFInt && matchArgs(allVars,analVars,RooArgSet(convVar(),ssf.arg()))) return 2 ;
    if (matchArgs(allVars,analVars,convVar())) return 1 ;
    break ;

  // Analytical integration capability of convoluted PDF
  case expBasis:
  case sinBasis:
  case cosBasis:
  case coshBasis:
  case sinhBasis:
    // Optionally advertise flat integral over sigma scale factor
    if (_flatSFInt && matchArgs(allVars,analVars,RooArgSet(convVar(),ssf.arg()))) return 2 ;
    if (matchArgs(allVars,analVars,convVar())) return 1 ;
    break ;
  }
  return 0 ;
}

//_____________________________________________________________________________
Double_t RooGaussEfficiencyModel::analyticalIntegral(Int_t code, const char* rangeName) const
{
  // Code must be 1 or 2
  assert(code==1||code==2) ;
  Double_t ssfInt( code==2 ? (ssf.max(rangeName)-ssf.min(rangeName)) : 1.0 );

  basisType basisCode = (basisType) _basisCode ;
  Double_t tau    = (basisCode!=noBasis)                           ? ((RooAbsReal*)basis().getParameter(1))->getVal() : 0 ;
  Double_t omega  = (basisCode==sinBasis  || basisCode==cosBasis)  ? ((RooAbsReal*)basis().getParameter(2))->getVal() : 0 ;
  Double_t dGamma = (basisCode==sinhBasis || basisCode==coshBasis) ? ((RooAbsReal*)basis().getParameter(2))->getVal() : 0 ;
  if (basisCode == coshBasis && basisCode!=noBasis && dGamma==0 ) basisCode = expBasis;

  Double_t scale  = sigma*ssf*TMath::Sqrt2();
  Double_t offset = mean*msf;
  Double_t umin = (x.min(rangeName)-offset)/scale;
  Double_t umax = (x.max(rangeName)-offset)/scale;

  if (basisCode==noBasis || ((basisCode==expBasis || basisCode==cosBasis) && tau==0)) {
    if (verboseEval()>0) cout << "RooGaussEfficiencyModel::analyticalIntegral(" << GetName() << ") 1st form" << endl ;
    Double_t result =  0.5*(RooMath::erf( umax )-RooMath::erf( umin )) ;
    if (TMath::IsNaN(result)) { cxcoutE(Tracing) << "RooGaussEfficiencyModel::analyticalIntegral(" << GetName() << ") got nan during case 1 " << endl; }
    return result*ssfInt ; // ???
  }
  if (tau==0) {
    if (verboseEval()>0) cout << "RooGaussEfficiencyModel::analyticalIntegral(" << GetName() << ") 2nd form" << endl ;
    return 0. ;
  }

  std::complex<double> z( double(1)/tau, -omega ); z=0.5*z*scale;

  if (verboseEval()>0) cout << "RooGaussEfficiencyModel::analyticalIntegral(" << GetName() << ") basisCode = " << basisCode << " z = " << z << endl ;

  Double_t result(0);
  switch (basisCode) {
    case expBasis:
    case cosBasis:
        result +=               evalInt(umin,umax,scale,offset,z).real();
        break;
    case sinBasis:
        result += z.imag()!=0 ? evalInt(umin,umax,scale,offset,z).imag() : 0 ;
        break;
    case coshBasis:
    case sinhBasis: {
        std::complex<double> y( scale * dGamma / 4 , 0 );
        result += 0.5 * (                                      evalInt(umin,umax,scale,offset,z-y).real()
                        + ( basisCode == coshBasis ? +1 : -1 )*evalInt(umin,umax,scale,offset,z+y).real() );
        break;
    }
    default:
        assert(0) ;
  }
  if (TMath::IsNaN(result)) { cxcoutE(Tracing) << "RooGaussEfficiencyModel::analyticalIntegral("
                                               << GetName() << ") got nan for basisCode = "
                                               << basisCode << endl;
  }
  return scale*result*ssfInt;
}


//_____________________________________________________________________________
RooAbsGenContext* RooGaussEfficiencyModel::modelGenContext
(const RooAbsAnaConvPdf& convPdf, const RooArgSet &vars, const RooDataSet *prototype,
 const RooArgSet* auxProto, Bool_t verbose) const
{
   return new RooEffConvGenContext(convPdf, vars, prototype, auxProto, verbose);
}

//_____________________________________________________________________________
Bool_t RooGaussEfficiencyModel::isDirectGenSafe(const RooAbsArg& arg) const
{
   return (!TString(convVar().GetName()).CompareTo(arg.GetName())) 
       || RooResolutionModel::isDirectGenSafe(arg);
}

//_____________________________________________________________________________
Int_t RooGaussEfficiencyModel::getGenerator(const RooArgSet& directVars, RooArgSet &generateVars, Bool_t /*staticInitOK*/) const
{
  if (matchArgs(directVars,generateVars,x)) { return 1 ;  }
  return 0 ;
}

//_____________________________________________________________________________
void RooGaussEfficiencyModel::generateEvent(Int_t code)
{
  if (code != 1) {
    coutE(InputArguments) << "RooGaussEfficiencyModel::generateEvent("
        << GetName()<< "): argument \"code\" can only have value 1"
        << std::endl;
    assert(code==1);
  }

  Double_t xmin = x.min();
  Double_t xmax = x.max();
  Double_t m = mean*msf;
  Double_t s = sigma*ssf;
  TRandom *generator = RooRandom::randomGenerator();
  while(true) {
    Double_t xgen = generator->Gaus(m,s);
    if (xgen<xmax && xgen>xmin) {
      x = xgen ; return ;
    }
  }
}

