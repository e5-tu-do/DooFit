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
#ifndef ROO_MULTI_EFF_RES_MODEL
#define ROO_MULTI_EFF_RES_MODEL

#include <RooResolutionModel.h>
#include <RooRealProxy.h>
#include <RooObjCacheManager.h>

#include <P2VV/RooEffResModel.h>
#include <P2VV/RooAbsEffResModel.h>

class MultiHistEntry;
class RooSuperCategory;

class RooMultiEffResModel : public RooResolutionModel, public RooAbsEffResModel {
public:

   // Constructors, assignment etc
   inline RooMultiEffResModel()  { }

   RooMultiEffResModel(const char *name, const char *title,
                       std::vector<MultiHistEntry*> entries);
   RooMultiEffResModel(const RooMultiEffResModel& other, const char* name=0);
   virtual RooMultiEffResModel* clone(const char* newname) const { return new RooMultiEffResModel(*this,newname); }
   virtual ~RooMultiEffResModel();
  
   virtual Int_t basisCode(const char* name) const;
   virtual Int_t getAnalyticalIntegral(RooArgSet& allVars, RooArgSet& analVars, const char* rangeName=0) const;
   virtual Double_t analyticalIntegral(Int_t code, const char* rangeName) const;
   virtual Bool_t forceAnalyticalInt(const RooAbsArg& /*dep*/) const;

   virtual RooAbsGenContext* modelGenContext
   (const RooAbsAnaConvPdf& convPdf, const RooArgSet &vars, const RooDataSet *prototype,
    const RooArgSet* auxProto, Bool_t verbose) const;

   virtual Int_t getGenerator(const RooArgSet& directVars, RooArgSet &generateVars,
                              Bool_t staticInitOK=kTRUE) const;
   virtual void initGenerator(Int_t code);
   virtual void generateEvent(Int_t code);

   virtual const RooAbsReal* efficiency() const;
   virtual std::vector<const RooAbsReal*> efficiencies() const;

   const std::map<Int_t, MultiHistEntry*>& getEntries() const
   {
      return _entries;
   }
   
   const RooSuperCategory* getSuper() const
   {
      return _super;
   }

   // return pointer to observables because genreflex dictionaries can't handle value
   virtual RooArgSet* observables() const;

protected:

   virtual Double_t evaluate() const;
   virtual RooMultiEffResModel* convolution(RooFormulaVar* inBasis, RooAbsArg* owner) const;

private:

   typedef std::map<Int_t, MultiHistEntry*> HistEntries;

   // operator=
   RooMultiEffResModel* operator=(const RooMultiEffResModel& other);

   // Evaluation
   typedef std::list<double> BinBoundaries;
   HistEntries _entries;

   // Generation
   mutable RooArgSet _prodGenObs;
   mutable Int_t _prodGenCode;
   RooSuperCategory* _super;
   typedef std::vector<std::pair<double, TString> > Levels;
   Levels _levels;

   // Caching
   class CacheElem : public RooAbsCacheElement {
   public:
      CacheElem(const HistEntries& entries, const RooArgSet& iset, const TNamed* rangeName);
      virtual ~CacheElem();

      virtual Double_t getVal(const Int_t index) const;

      virtual RooArgList containedArgs(Action);

   private:
      // Payload
      typedef std::map<Int_t, RooAbsReal*> Integrals;
      Integrals _I;
      RooArgSet _iset;
   };

   mutable RooObjCacheManager _cacheMgr;

   ClassDef(RooMultiEffResModel, 1) // Resolution Model with Efficiency
};

#endif
