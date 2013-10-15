/*****************************************************************************
 * Project: RooFit                                                           *
 *                                                                           *
 * Authors:                                                                  *
 *   JvL, Jeroen van Leerdam, Nikhef, j.van.leerdam@nikhef.nl                *
 *                                                                           *
 * Copyright (c) 2013, Nikhef. All rights reserved.                          *
 *                                                                           *
 * Redistribution and use in source and binary forms,                        *
 * with or without modification, are permitted according to the terms        *
 * listed in LICENSE (http://roofit.sourceforge.net/license.txt)             *
 *****************************************************************************/

#ifndef ROO_CATEGORY_VAR
#define ROO_CATEGORY_VAR

#include <map>
#include "RooCategoryProxy.h"
#include "RooListProxy.h"

class RooAbsReal;
class RooAbsCategory;
 
class RooCategoryVar : public RooAbsReal
{
public:
  RooCategoryVar() {}; 

  RooCategoryVar(const char *name, const char *title,
      RooAbsCategory& category, const RooArgList& variables);

  RooCategoryVar(const RooCategoryVar& other, const char* name = 0);
  virtual RooCategoryVar* clone(const char* name) const
  { 
    return new RooCategoryVar(*this, name);
  }

  virtual ~RooCategoryVar() { }

  inline const RooAbsCategory& category() const {return _cat.arg();}
  inline RooArgList variables() const {return RooArgList(_vars, "variables");}

protected:
  RooCategoryProxy _cat;
  RooListProxy     _vars;

  mutable std::map<Int_t,Int_t> _varIndices; //!
  
  Double_t evaluate() const;
  void initVarsMap() const;

private:
  ClassDef(RooCategoryVar, 1)
};
 
#endif
