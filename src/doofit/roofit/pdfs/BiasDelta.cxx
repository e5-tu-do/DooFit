/*****************************************************************************
 * Project: RooFit                                                           *
 * Package: RooFitModels                                                     *
 * @(#)root/roofit:$Id: BiasDelta.cxx 25185 2008-08-20 14:00:42Z wouter $
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
// Delta distribution
// END_HTML
//

#include "RooFit.h"

#include "Riostream.h"
#include "Riostream.h"
#include <math.h>

#include "BiasDelta.h"
#include "RooAbsReal.h"
#include "RooRealVar.h"
#include "RooRandom.h"
#include "RooMath.h"

namespace doofit {
namespace roofit {
namespace pdfs {

//_____________________________________________________________________________
BiasDelta::BiasDelta(const char *name, const char *title,
			 RooAbsReal& _x, RooAbsReal& _mean) :
  RooAbsPdf(name,title),
  x("x","Observable",this,_x),
  mean("mean","Mean",this,_mean)
{
}



//_____________________________________________________________________________
BiasDelta::BiasDelta(const BiasDelta& other, const char* name) : 
  RooAbsPdf(other,name), x("x",this,other.x), mean("mean",this,other.mean)
{
}



//_____________________________________________________________________________
Double_t BiasDelta::evaluate() const
{
  if (x == mean) return 1;
  else return 0;
}



//_____________________________________________________________________________
Int_t BiasDelta::getAnalyticalIntegral(RooArgSet& allVars, RooArgSet& analVars, const char* /*rangeName*/) const 
{
  if (matchArgs(allVars,analVars,x)) return 1 ;
  return 0 ;
}



//_____________________________________________________________________________
Double_t BiasDelta::analyticalIntegral(Int_t code, const char* /*rangeName*/) const 
{
  assert(code==1) ;
  return (x.max()-x.min())/100.;

}




//_____________________________________________________________________________
Int_t BiasDelta::getGenerator(const RooArgSet& directVars, RooArgSet &generateVars, Bool_t /*staticInitOK*/) const
{
  if (matchArgs(directVars,generateVars,x)) return 1 ;  
  return 0 ;
}



//_____________________________________________________________________________
void BiasDelta::generateEvent(Int_t code)
{
  assert(code==1) ;
  // std::cout <<  "create new event with value " <<  mean   <<  std::endl;
  x = 1.0*mean;
}

} // namespace pdfs
} // namespace roofit
} // namespace doofit
