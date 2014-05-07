/*****************************************************************************
 * Project: RooFit                                                           *
 * Package: RooFitModels                                                     *
 *    File: $Id$
 * Authors:                                                                  *
 *   DMS, Diego Martinez Santos, Nikhef, Diego.Martinez.Santos@cern.ch       *
 *                                                                           *
 * Copyright (c) 2013, Nikhef. All rights reserved.                          *
 *                                                                           *
 * Redistribution and use in source and binary forms,                        *
 * with or without modification, are permitted according to the terms        *
 * listed in LICENSE (http://roofit.sourceforge.net/license.txt)             *
 *****************************************************************************/

#include "TMath.h"
#include "Math/SpecFunc.h"

#include "RooAbsReal.h" 
#include "RooIpatia2.h" 

const Double_t sq2pi = TMath::Sqrt(2.*TMath::ACos(-1.));
const Double_t sq2pi_inv = 1./sq2pi;
const Double_t logsq2pi = TMath::Log(sq2pi);
const Double_t log_de_2 = TMath::Log(2.);

Double_t low_x_BK(Double_t nu,Double_t x){
  return TMath::Gamma(nu)*TMath::Power(2.,nu-1.)*TMath::Power(x,-nu);
}


Double_t low_x_LnBK(Double_t nu, Double_t x){
  return TMath::Log(TMath::Gamma(nu)) + (nu-1.)*log_de_2 - nu * TMath::Log(x);
}

Double_t BK(Double_t ni, Double_t x) {
  Double_t nu = TMath::Abs(ni);
  if ( x < 1.e-06 && nu > 0.) return low_x_BK(nu,x);
  if ( x < 1.e-04 && nu > 0. && nu < 55.) return low_x_BK(nu,x);
  if ( x < 0.1 && nu >= 55.) return low_x_BK(nu,x);

  //return gsl_sf_bessel_Knu(nu, x);
  return ROOT::Math::cyl_bessel_k(nu, x);
}

Double_t LnBK(double ni, double x) {
  Double_t nu = TMath::Abs(ni);
  if ( x < 1.e-06 && nu > 0.) return low_x_LnBK(nu,x);
  if ( x < 1.e-04 && nu > 0. && nu < 55.) return low_x_LnBK(nu,x);
  if ( x < 0.1 && nu >= 55.) return low_x_LnBK(nu,x);

  //return gsl_sf_bessel_lnKnu(nu, x);
  return TMath::Log(ROOT::Math::cyl_bessel_k(nu, x));
}


Double_t LogEval(Double_t d, Double_t l, Double_t alpha, Double_t beta, Double_t delta) {
  //Double_t d = x-mu;
  //Double_t sq2pi = TMath::Sqrt(2*TMath::ACos(-1));
  Double_t gamma = alpha;//TMath::Sqrt(alpha*alpha-beta*beta);
  Double_t dg = delta*gamma;
  Double_t thing = delta*delta + d*d;
  Double_t logno = l*TMath::Log(gamma/delta) - logsq2pi -LnBK(l, dg);
  
  return TMath::Exp(logno + beta*d +(0.5-l)*(TMath::Log(alpha)-0.5*TMath::Log(thing)) + LnBK(l-0.5,alpha*TMath::Sqrt(thing)));// + TMath::Log(TMath::Abs(beta)+0.0001) );

}


Double_t diff_eval(Double_t d, Double_t l, Double_t alpha, Double_t beta, Double_t delta){
  //Double_t sq2pi = TMath::Sqrt(2*TMath::ACos(-1));
  //Double_t cons1 = 1./sq2pi;
  Double_t gamma = alpha;// TMath::Sqrt(alpha*alpha-beta*beta);
  Double_t dg = delta*gamma;
  //Double_t mu_ = mu;// - delta*beta*BK(l+1,dg)/(gamma*BK(l,dg));
  //Double_t d = x-mu;
  Double_t thing = delta*delta + d*d;
  Double_t sqthing = TMath::Sqrt(thing);
  Double_t alphasq = alpha*sqthing;
  Double_t no = TMath::Power(gamma/delta,l)/BK(l,dg)*sq2pi_inv;
  Double_t ns1 = 0.5-l;
  //Double_t cheat = TMath::Exp(beta*d);//*(TMath::Abs(beta) + 1e-04);
  //Double_t cheat = TMath::Exp(beta*d);//*(TMath::Abs(beta) + 0.0001);

  //no =  no*TMath::Power(alpha, ns1 )*TMath::Power(thing, 0.5*l - 5.0/4.0)*0.5*cheat;//TMath::Exp(beta*d);

  //return no*(-alphasq*d* (BK(l - 3.0/2.0, alphasq) - BK(l + 0.5, alphasq)) + (2*beta*thing + 2*d*l - d)*BK(-ns1, alphasq));
  //return no*TMath::Power(alpha, -l + 1.0/2.0)*TMath::Power(thing, l/2 - 5.0/4.0)*(-d*alphasq*BK(l - 3.0/2.0, alphasq) - d*alphasq*BK(l + 1.0/2.0, alphasq) + 2*beta*thing*BK(l - 0.5, alphasq) + 2*d*l*BK(l - 0.5, alphasq) - d*BK(l - 0.5, alpha*sqthing))*TMath::Exp(beta*d)/2;
  
  return no*TMath::Power(alpha, ns1)*TMath::Power(thing, l/2. - 1.25)*(-d*alphasq*(BK(l - 1.5, alphasq) + BK(l + 0.5, alphasq)) + (2.*(beta*thing + d*l) - d)*BK(ns1, alphasq))*TMath::Exp(beta*d)/2.;
}

// Double_t diff_eval_check(Double_t d, Double_t l, Double_t alpha, Double_t beta, Double_t delta){
  
//   Double_t gamma = alpha;//TMath::Sqrt(alpha*alpha-beta*beta);
//   Double_t dg = delta*gamma;
  
//   Double_t thing = delta*delta + d*d;
//   Double_t sqthing = TMath::Sqrt(thing);
//   Double_t alphasq = alpha*sqthing;
//   Double_t no = TMath::Power(gamma/delta,l)/BK(l,dg)*sq2pi_inv;

//   return no*TMath::Power(alpha, -l + 1.0/2.0)*TMath::Power(TMath::Power(d, 2) + TMath::Power(delta, 2), l/2 - 5.0/4.0)*(-alpha*d*TMath::Sqrt(TMath::Power(d, 2) + TMath::Power(delta, 2))*BK(l - 3.0/2.0, alpha*TMath::Sqrt(TMath::Power(d, 2) + TMath::Power(delta, 2))) - alpha*d*TMath::Sqrt(TMath::Power(d, 2) + TMath::Power(delta, 2))*BK(l + 1.0/2.0, alpha*TMath::Sqrt(TMath::Power(d, 2) + TMath::Power(delta, 2))) + 2*beta*(TMath::Power(d, 2) + TMath::Power(delta, 2))*BK(l - 1.0/2.0, alpha*TMath::Sqrt(TMath::Power(d, 2) + TMath::Power(delta, 2))) + 2*d*l*BK(l - 1.0/2.0, alpha*TMath::Sqrt(TMath::Power(d, 2) + TMath::Power(delta, 2))) - d*BK(l - 1.0/2.0, alpha*TMath::Sqrt(TMath::Power(d, 2) + TMath::Power(delta, 2))))*TMath::Exp(beta*d)/2;
// }

 

 RooIpatia2::RooIpatia2(const char *name, const char *title, 
                        RooAbsReal& _x,
                        RooAbsReal& _l,
                        RooAbsReal& _zeta,
                        RooAbsReal& _fb,
                        RooAbsReal& _sigma,
                        RooAbsReal& _mu,
                        RooAbsReal& _a,
                        RooAbsReal& _n,
                        RooAbsReal& _a2,
                        RooAbsReal& _n2) :
   RooAbsPdf(name,title), 
   x("x","x",this,_x),
   l("l","l",this,_l),
   zeta("zeta","zeta",this,_zeta),
   fb("fb","fb",this,_fb),
   sigma("sigma","sigma",this,_sigma),
   mu("mu","mu",this,_mu),
   a("a","a",this,_a),
   n("n","n",this,_n),
   a2("a2","a2",this,_a2),
   n2("n2","n2",this,_n2)
 { 
 } 


 RooIpatia2::RooIpatia2(const RooIpatia2& other, const char* name) :  
   RooAbsPdf(other,name), 
   x("x",this,other.x),
   l("l",this,other.l),
   zeta("zeta",this,other.zeta),
   fb("fb",this,other.fb),
   sigma("sigma",this,other.sigma),
   mu("mu",this,other.mu),
   a("a",this,other.a),
   n("n",this,other.n),
   a2("a2",this,other.a2),
   n2("n2",this,other.n2)
 { 
 } 



 Double_t RooIpatia2::evaluate() const 
 { 
   Double_t d = x-mu;
   Double_t cons0 = TMath::Sqrt(zeta);
   Double_t alpha, beta, delta,  cons1, phi, A, B, k1, k2;
   Double_t asigma = a*sigma;
   Double_t a2sigma = a2*sigma;
   Double_t out = 0.;
   if (zeta!= 0.) {
     phi = BK(l+1.,zeta)/BK(l,zeta); // careful if zeta -> 0. You can implement a function for the ratio, but carefull again that |nu + 1 | != |nu| + 1 so you jave to deal wiht the signs
     cons1 = sigma/TMath::Sqrt(phi);
     alpha  = cons0/cons1;//*TMath::Sqrt((1 - fb*fb));
     beta = fb;//*alpha;
     delta = cons0*cons1;
     
     if (d < -asigma){
       //printf("-_-\n");
       //printf("alpha %e\n",alpha);
       //printf("beta %e\n",beta);
       //printf("delta %e\n",delta);
       
       k1 = LogEval(-asigma,l,alpha,beta,delta);
       k2 = diff_eval(-asigma,l,alpha,beta,delta); 
       B = -asigma + n*k1/k2;
       A = k1*TMath::Power(B+asigma,n);
       //printf("k1 is %e\n",k1);
       //printf("k2 is %e\n",k2);
       //printf("A is%e\n",A);
       //printf("B is%e\n",B);
       out = A*TMath::Power(B-d,-n);
     }
     else if (d>a2sigma) {
       //printf("uoeo\n");
       k1 = LogEval(a2sigma,l,alpha,beta,delta);
       k2 = diff_eval(a2sigma,l,alpha,beta,delta);
       
       B = -a2sigma - n2*k1/k2;
       
       A = k1*TMath::Power(B+a2sigma,n2);
       
       out =  A*TMath::Power(B+d,-n2);
       
     }
     else {
       //printf("HERE\n");
       out = LogEval(d,l,alpha,beta,delta);
     }
     


   }
   else if (l < 0.) {
     beta = fb;
     cons1 = -2.*l;
     delta = sigma;
     if (d < -asigma ) {
       cons1 = TMath::Exp(-beta*asigma);
       phi = 1. + a*a;
       k1 = cons1*TMath::Power(phi,l-0.5);
       k2 = beta*k1- cons1*(l-0.5)*TMath::Power(phi,l-1.5)*2*a/delta;
       B = -asigma + n*k1/k2;
       A = k1*TMath::Power(B+asigma,n);
       out = A*TMath::Power(B-d,-n);
     }
     else if (d > a2sigma) {
       cons1 = TMath::Exp(beta*a2sigma);
       phi = 1. + a2*a2;
       k1 = cons1*TMath::Power(phi,l-0.5);
       k2 = beta*k1+ cons1*(l-0.5)*TMath::Power(phi,l-1.5)*2.*a2/delta;
       B = -a2sigma - n2*k1/k2;
       A = k1*TMath::Power(B+a2sigma,n2);
       out =  A*TMath::Power(B+d,-n2);
       
     }
     else { out = TMath::Exp(beta*d)*TMath::Power(1. + d*d/(delta*delta),l-0.5);}
   }
   else {
     //printf("zeta = 0 only suported for l < 0, while l = %e\n",0);
   }
   //printf("result is %e\n",out);
   return out;
 }
