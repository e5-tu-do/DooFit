/*****************************************************************************
 * Project: RooFit                                                           *
 * Package: RooFitModels                                                     *
 *    File: $Id: RooRelBreitWignerPdf.cc,v 1.1 2006/04/08 21:59:57 bevan Exp $
 * Authors:                                                                  *
 * Katherine George - University of Liverpool                                * 
 * Adrian Bevan QMUL (ported from RptBaseTools to Roo - 2009)               *
 *****************************************************************************/

// -- CLASS DESCRIPTION [PDF] --
// relativistic breit wigner with blatt weisskopf form factor dependence.

#include <iostream>
#include <cmath>

#include "P2VV/RooRelBreitWigner.h"
#include "RooAbsReal.h"
#include "RooRealVar.h"

RooRelBreitWigner::RooRelBreitWigner(const char *name, const char *title,
             RooAbsReal& _x, RooAbsReal& _mean,
             RooAbsReal& _width, RooAbsReal& _spin, RooAbsReal& _radius,
             RooAbsReal& _massa, RooAbsReal& _massb) :
  RooAbsPdf(name,title),
  x("x","Dependent",this,_x),
  mean("mean","Mean",this,_mean),
  width("width","Width",this,_width),
  spin("spin", "Spin", this, _spin),
  radius("radius", "radius", this, _radius),
  massa("massa", "mass of daughter A", this, _massa),
  massb("massb", "mass of daughter B", this, _massb)
{
}


RooRelBreitWigner::RooRelBreitWigner(const RooRelBreitWigner& other, const char* name) : 
  RooAbsPdf(other,name), x("x",this,other.x), mean("mean",this,other.mean),
  width("width",this,other.width),spin("spin", this, other.spin), radius("radius", this, other.radius), 
  massa("massa", this, other.massa),  massb("massb", this, other.massb) 
{
}


Double_t RooRelBreitWigner::evaluate() const
{
  Double_t val(0) ;
  if( x > (massa+massb) ) {
    // compute the mass-dependent width
    Double_t gammaf = gamma() ;
    
    // compute the BW, taking into account a phase-space factor
    Double_t arg = mean*mean - x*x ;
    val = x * gammaf / (arg*arg + mean * mean * gammaf * gammaf);

    // what we actually still missing is a term that truncates this on
    // the high end. In Bs->JpsiPhi that term would look like
    // p^2Leff+1 where p is the momentum of the j/psi or phi in the B
    // rest frame.
  }
  return val ;
}

Double_t RooRelBreitWigner::gamma() const
{  
  // first compute q(x) and q(m) where q is the momentum of daughters in mother rest frame
  Double_t qm = KFunction(mean);
  Double_t qx = KFunction(x) ;
  Double_t rq = qm>0 ? std::pow( qx/qm, rint( 2*spin + 1 ) ) : 1 ; // this is to 3rd power for spin 1
  // Now compute this radius dependent term
  Double_t rf = 1 ;
  if(radius>0) {
    Double_t fm = FFunction(radius*qm);
    Double_t fx = FFunction(radius*qx);
    rf = (fx/fm);
  } 
  Double_t gammaf = width * (rq * rf * mean / x) ;   
  return gammaf ;
}

Double_t RooRelBreitWigner::FFunction(Double_t X) const
{
  // Blatt-Weisskopf barrier factors.
  switch( int(rint(spin)) ) {
    case 0  : return 1.0;
    case 1  : return 1.0/(1 + X*X);
    case 2  : return 1.0/(9 + 3*X*X + X*X*X*X); 
  }
  return 1.0;
}

Double_t RooRelBreitWigner::KFunction(Double_t X) const
{
  // this returns q, the momentum of one of the daughters in
  // the rest frame
  Double_t ma = massa ;
  Double_t mb = massb ;
  Double_t q(0) ;
  if( X - ma - mb > 0 ) {
    Double_t ma2 = ma*ma ;
    Double_t mb2 = mb*mb ;
    Double_t Q2 = X*X - ma2 - mb2 ;
    q = std::sqrt( std::abs(Q2*Q2 - 4*ma2*mb2) ) / (2*X) ;
  }
  return q ;
}
