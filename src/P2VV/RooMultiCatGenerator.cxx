/*****************************************************************************
 * Project: RooFit                                                           *
 * Package: RooFitCore                                                       *
 * @(#)root/roofitcore:$Id$
 * Authors:                                                                  *
 *   GR, Gerhard Raven,  Vrije Universiteit Amsterdam & Nikhef               *
 *                                                                           *
 * Copyright (c) 2010, Vrije Universiteit Amsterdam & Nikhef                 *
 *                     All rights reserved.                                  *
 *                                                                           *
 * Redistribution and use in source and binary forms,                        *
 * with or without modification, are permitted according to the terms        *
 * listed in LICENSE (http://roofit.sourceforge.net/license.txt)             *
 *****************************************************************************/

//////////////////////////////////////////////////////////////////////////////
// 
// BEGIN_HTML
// Class RooMultiCatGenerator is a generic toy monte carlo generator that 
// generates discrete categories, and dispatches any real variables to 
// underlying generators -- one for each combination of category values.
// END_HTML
//


#include "RooFit.h"
#include "Riostream.h"

#include "P2VV/RooMultiCatGenerator.h"
#include "RooAbsReal.h"
#include "RooCategory.h"
#include "RooRealVar.h"
#include "RooDataSet.h"
#include "RooRandom.h"
#include "RooErrorHandler.h"

#include "TString.h"
#include "TIterator.h"
#include "RooMsgService.h"
#include "TClass.h"
#include "RooNumGenFactory.h"
#include "RooNumGenConfig.h"

#include <memory>
#include <utility>
using std::make_pair;
#include <iostream>
using std::endl;
#include <assert.h>

//_____________________________________________________________________________
void RooMultiCatGenerator::registerSampler(RooNumGenFactory& fact)
{
   // Register RooIntegrator1D, is parameters and capabilities with RooNumIntFactory
   fact.storeProtoSampler(new RooMultiCatGenerator,RooArgSet()) ;
}

namespace {
   const char *makeName(const char* name, const RooArgSet& terms ) {
      static TString pname;
      pname = name;
      pname.Append("_");
      std::auto_ptr<TIterator> i( terms.createIterator() );
      RooAbsArg *arg;
      Bool_t first(kTRUE);
      while((arg=(RooAbsArg*)i->Next())) {
         if (first) { first=kFALSE;}
         else pname.Append("_X_");
         pname.Append(arg->GetName());
      }
      return pname.Data();
   }

   RooSuperCategory makeSuper(const char* name, const RooArgSet& _catVars  ) {
      const char *catName =  makeName(name, _catVars );
      return RooSuperCategory(catName,catName,_catVars);
   }
}

//_____________________________________________________________________________
RooMultiCatGenerator::RooMultiCatGenerator(const RooAbsReal &func, const RooArgSet &genVars, const RooNumGenConfig& config, Bool_t verbose, const RooAbsReal* maxFuncVal) 
   : RooAbsNumGenerator(func,genVars,verbose,maxFuncVal)
   , _super( makeSuper( func.GetName(), _catVars ) )
{
   std::auto_ptr<RooAbsReal> marginal( _funcClone->createIntegral( _realVars ) );
   std::auto_ptr<TIterator> superIter( _super.MakeIterator() );
   // We first compute the marginalized fractions for the combinations of categories.
   // Next, we create, for each combination, a dedicated sampler for the real observables,
   // which is conditional on each of the combination of category values.
   while ( superIter->Next() ) {
      _super.setLabel( dynamic_cast<TObjString&>(***superIter).String() ); // this should assign _catVars...
      double n = marginal->getVal(); // fraction of events in this combination 
      if (!_realGenerators.empty()) n += _realGenerators.back().first;   // cumulative
      cxcoutI(Generation) << "RooMultiCatGenerator: creating sampler for " << _realVars << " given " << _catVars << " = "  << dynamic_cast<TObjString&>(***superIter).String() << " ( level = " << n << " )" << endl;
      _realGenerators.push_back(make_pair(n, RooNumGenFactory::instance().createSampler(*_funcClone,_realVars,RooArgSet(),config, true /*verbose*/ ))); 
   }
   // Given that above we properly marginalized, the next line should be a no-op.
   for (Generators::iterator i=_realGenerators.begin();i!=_realGenerators.end();++i) i->first /= _realGenerators.back().first; // normalize
}

//_____________________________________________________________________________
RooMultiCatGenerator::~RooMultiCatGenerator() 
{
   // Destructor
   for (Generators::iterator i=_realGenerators.begin();i!=_realGenerators.end();++i) delete i->second;
}

//_____________________________________________________________________________
const RooArgSet *RooMultiCatGenerator::generateEvent(UInt_t remaining, Double_t& resampleRatio) 
{
   // are we actually generating anything? (the cache always contains at least our function value)
   const RooArgSet *event= _cache->get();
   if(event->getSize() == 1) return event;

   Double_t r = RooRandom::uniform();
   std::auto_ptr<TIterator> superIter( _super.MakeIterator() );
   // find the right generator, and generate categories at the same time...
   Generators::iterator gen = _realGenerators.begin();
   while ( superIter->Next()!=0 && gen->first < r)  ++gen;
   _super.setLabel( dynamic_cast<TObjString&>(***superIter).String() ); // this should assign _catVars...

   // now that've assigned the categories, we can use the 'real' samplers
   // which are conditional on the categories...
   const RooArgSet* realEvent = gen->second->generateEvent( remaining, resampleRatio);
   _realVars.assignValueOnly( *realEvent );

   // calculate and store our function value at this new point
   Double_t val= _funcClone->getVal();
   _funcValPtr->setVal(val);

   // Transfer contents to dataset
   return _cache->get();
}
