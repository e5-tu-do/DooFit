/*****************************************************************************
 * Project: RooFit                                                           *
 * Package: RooFitModels                                                     *
 *    File: $Id$
 * Authors:                                                                  *
 *   JvL, Jeroen van Leerdam, Nikhef,      j.van.leerdam@nikhef.nl           *
 *                                                                           *
 * Copyright (c) 2012, Nikhef. All rights reserved.                          *
 *                                                                           *
 * Redistribution and use in source and binary forms,                        *
 * with or without modification, are permitted according to the terms        *
 * listed in LICENSE (http://roofit.sourceforge.net/license.txt)             *
 *****************************************************************************/

#ifndef ROO_TAG_DECISION_WRAPPER
#define ROO_TAG_DECISION_WRAPPER

#include "RooAbsCategory.h"
#include "RooCategoryProxy.h"

class RooTagDecisionWrapper : public RooAbsCategory
{

public:
  inline RooTagDecisionWrapper() {}

  RooTagDecisionWrapper(const char *name, const char *title,
      RooAbsCategory& tagDecision);

  RooTagDecisionWrapper(const RooTagDecisionWrapper& other,
      const char* name = 0);

  virtual TObject* clone(const char* newname) const 
  { 
    return new RooTagDecisionWrapper(*this, newname);
  }

  ~RooTagDecisionWrapper();

private:
  const RooCategoryProxy _tagDecision;

  virtual RooCatType evaluate() const;

  ClassDef(RooTagDecisionWrapper, 1) // wrapper for a "tag decision" flavour tagging category
};

#endif

