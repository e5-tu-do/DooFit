/*****************************************************************************
 * Project: RooFit                                                           *
 * Package: RooFitCore                                                       *
 *    File: $Id$
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
#ifndef ROO_MULTICAT_GENERATOR
#define ROO_MULTICAT_GENERATOR

#include "RooAbsNumGenerator.h"
#include "RooPrintable.h"
#include "RooArgSet.h"
#include "RooSuperCategory.h"
#include <vector>
#include <utility>

class RooAbsReal;
class RooRealVar;
class RooDataSet;

class RooNumGenFactory ;

class RooMultiCatGenerator : public RooAbsNumGenerator {
public:
  RooMultiCatGenerator() {}
  RooMultiCatGenerator(const RooAbsReal &func, const RooArgSet &genVars, const RooNumGenConfig& config, Bool_t verbose=kFALSE, const RooAbsReal* maxFuncVal=0);
  RooAbsNumGenerator* clone(const RooAbsReal& func, const RooArgSet& genVars, const RooArgSet& /*condVars*/, 
			    const RooNumGenConfig& config, Bool_t verbose=kFALSE, const RooAbsReal* maxFuncVal=0) const {
    return new RooMultiCatGenerator(func,genVars,config,verbose,maxFuncVal) ;
  }
  virtual ~RooMultiCatGenerator();

  const RooArgSet *generateEvent(UInt_t remaining, Double_t& resampleRatio);

  virtual Bool_t canSampleConditional() const { return kFALSE ; }
  virtual Bool_t canSampleCategories() const { return kTRUE ; }
 
protected:
public:

  friend class RooNumGenFactory ;
  static void registerSampler(RooNumGenFactory& fact) ;	

protected:
  typedef std::vector<std::pair<double,RooAbsNumGenerator*> > Generators;
  Generators _realGenerators;
  RooSuperCategory _super;

  ClassDef(RooMultiCatGenerator,0) // Context for generating a dataset from a PDF 
};

#endif
