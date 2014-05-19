/*****************************************************************************
 * Project: RooFit                                                           *
 * Package: RooFitModels                                                     *
 *    File: $Id: RooGaussModel.h,v 1.21 2007/05/11 09:13:07 verkerke Exp $
 * Authors:                                                                  *
 *
 * Redistribution and use in source and binary forms,                        *
 * with or without modification, are permitted according to the terms        *
 * listed in LICENSE (http://roofit.sourceforge.net/license.txt)             *
 *****************************************************************************/
#ifndef DOOFIT_SCHILLER_ROO_ABS_EFF_RES_MODEL
#define DOOFIT_SCHILLER_ROO_ABS_EFF_RES_MODEL

#include <RooResolutionModel.h>
#include <RooRealProxy.h>
#include <RooObjCacheManager.h>
#include <RooSetProxy.h>

namespace doofit {
namespace schiller {

class RooAbsEffResModel : public RooResolutionModel  {
public:
   // Constructors, assignment etc
   inline RooAbsEffResModel() { }
   RooAbsEffResModel(const char *name, const char *title, RooRealVar& convVar);
   RooAbsEffResModel(const RooAbsEffResModel& other, const char* name=0);

   virtual ~RooAbsEffResModel();
  
   virtual RooAbsReal* efficiency() const = 0;
   virtual std::vector<RooAbsReal*> efficiencies() const = 0;

private:

   ClassDef(RooAbsEffResModel, 0) // EffResian Resolution Model
};

}// namespace schiller
}// namespace doofit

#endif
