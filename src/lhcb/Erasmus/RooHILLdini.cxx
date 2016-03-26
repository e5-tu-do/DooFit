
#include "RooFit.h"
#include <iostream>
#include <math.h>

#include "RooHILLdini.h"
#include "RooAbsReal.h"
#include "RooRealVar.h"
#include "TMath.h"


  RooHILLdini::RooHILLdini(const char *name, const char *title,
				   RooAbsReal& _m, RooAbsReal& _a, RooAbsReal& _b, RooAbsReal& _csi, RooAbsReal& _shift, RooAbsReal& _sigma, RooAbsReal& _ratio_sigma, RooAbsReal& _fraction_sigma ) :


  RooAbsPdf(name, title),
  m("m", "Dependent", this, _m),
  a("a", "a", this, _a),
  b("b", "b", this, _b),
  csi("csi", "csi", this, _csi),
  shift("shift", "shift", this, _shift),
  sigma("sigma", "sigma", this, _sigma),
  ratio_sigma("ratio_sigma", "ratio_sigma", this, _ratio_sigma),
  fraction_sigma("fraction_sigma", "fraction_sigma", this, _fraction_sigma)
{
}


RooHILLdini::RooHILLdini(const RooHILLdini& other, const char* name) :
  RooAbsPdf(other, name), m("m", this, other.m), a("a", this, other.a), b("b", this, other.b), csi("csi", this, other.csi), shift("shift", this, other.shift), sigma("sigma", this, other.sigma),  ratio_sigma("ratio_sigma", this, other.ratio_sigma), fraction_sigma("fraction_sigma", this, other.fraction_sigma)
{
}





Double_t RooHILLdini::evaluate() const 
{

  ///////////////////////////////
  double a_new  = a;
  double b_new  = b;
  double sigma2 = sigma * ratio_sigma;
  ///////////////////////////////



  ///////////////////
  double firstG1  = (2*exp(-((a_new-(m-shift))*(a_new-(m-shift))/(2*(sigma*sigma))))*sigma*(b_new-(m-shift))+2*exp(-((b_new-(m-shift))*(b_new-(m-shift))/(2*(sigma*sigma))))*sigma*(-a_new+(m-shift))-sqrt(2*TMath::Pi())*(a_new*b_new+(sigma*sigma)-(a_new+b_new)*(m-shift)+((m-shift)*(m-shift)))*TMath::Erf((-a_new+(m-shift))/(sqrt(2)*sigma))+sqrt(2*TMath::Pi())*(a_new*b_new+(sigma*sigma)-(a_new+b_new)*(m-shift)+((m-shift)*(m-shift)))*TMath::Erf((-b_new+(m-shift))/(sqrt(2)*sigma)))/(2*sqrt(2*TMath::Pi()));
  //double  CURVEG1 =    fabs(   (1-csi)/(b_new-a_new)*(m-shift)  + (b_new*csi - a_new)/(b_new-a_new)  )*fabs(firstG1);
  double  CURVEG1 =    fabs(   (1-csi)/(b_new-a_new)*(m)  + (b_new*csi - a_new)/(b_new-a_new)  )*fabs(firstG1); 
  ///////////////////



  ///////////////////
  double firstG2  = (2*exp(-((a_new-(m-shift))*(a_new-(m-shift))/(2*(sigma2*sigma2))))*sigma2*(b_new-(m-shift))+2*exp(-((b_new-(m-shift))*(b_new-(m-shift))/(2*(sigma2*sigma2))))*sigma2*(-a_new+(m-shift))-sqrt(2*TMath::Pi())*(a_new*b_new+(sigma2*sigma2)-(a_new+b_new)*(m-shift)+((m-shift)*(m-shift)))*TMath::Erf((-a_new+(m-shift))/(sqrt(2)*sigma2))+sqrt(2*TMath::Pi())*(a_new*b_new+(sigma2*sigma2)-(a_new+b_new)*(m-shift)+((m-shift)*(m-shift)))*TMath::Erf((-b_new+(m-shift))/(sqrt(2)*sigma2)))/(2*sqrt(2*TMath::Pi()));
  //double  CURVEG2 =    fabs(   (1-csi)/(b_new-a_new)*(m-shift)  + (b_new*csi - a_new)/(b_new-a_new)  )*fabs(firstG2);
  double  CURVEG2 =    fabs(   (1-csi)/(b_new-a_new)*(m)  + (b_new*csi - a_new)/(b_new-a_new)  )*fabs(firstG2);
  ///////////////////


  return fabs(fraction_sigma*CURVEG1 + (1-fraction_sigma)*CURVEG2);

  

}
