/*****************************************************************************
 * Project: RooFit                                                           *
 * Package: RooFitModels                                                     *
 *    File: $Id: RooEffProd.h,v 1.2 2007/05/11 10:14:56 verkerke Exp $
 * Authors:                                                                  *
 *   GR, Gerhard Raven, NIKHEF/VU                                            *
 *                                                                           *
 * Redistribution and use in source and binary forms,                        *
 * with or without modification, are permitted according to the terms        *
 * listed in LICENSE (http://roofit.sourceforge.net/license.txt)             *
 *
 *                                                                           *
 * 2012-07-02 imported into B2DXFitters package by M. Schiller, Nikhef,      *
 * manuel.schiller@nikhef.nl; some changes have been made during the import, *
 * so anything that does not work is probably my fault                       *
 * 2012-08-21 re-import from upstream - Manuel Schiller                      *
 *****************************************************************************/
#ifndef ROO_EFFHIST_PROD
#define ROO_EFFHIST_PROD

#include "RooAbsPdf.h"
#include "RooAbsReal.h"
#include "RooRealVar.h"
#include "RooRealProxy.h"
#include "RooSetProxy.h"
#include "RooObjCacheManager.h"

#include <exception>

namespace EffHistProd {
class Exception : public std::exception
{
public:
   Exception(const std::string& message)
      : m_message(message)
   {

   }

   virtual ~Exception() throw()
   {
      
   }

   virtual const char* what() const throw()
   {
      return m_message.c_str();
   }

private:
   const std::string m_message;
   
};
}

class RooAbsReal;
class RooSuperCategory;
class RooCustomizer;

class RooEffHistProd: public RooAbsPdf {
public:
   // Constructors, assignment etc
   inline RooEffHistProd()   { };
   virtual ~RooEffHistProd();
   RooEffHistProd(const char *name, const char *title, 
                  RooAbsPdf& pdf, RooAbsReal& efficiency);
   RooEffHistProd(const RooEffHistProd& other, const char* name = 0);

   virtual Bool_t forceAnalyticalInt(const RooAbsArg& /*dep*/) const { 
      // Return kTRUE to force RooRealIntegral to offer all observables for internal integration
      return kTRUE ; 
   }

   virtual TObject* clone(const char* name) const {
      return new RooEffHistProd(*this, name);
   }

   virtual RooAbsGenContext* genContext(const RooArgSet &vars, const RooDataSet *prototype,
                                        const RooArgSet* auxProto, Bool_t verbose) const;

   virtual Int_t getGenerator(const RooArgSet& dv, RooArgSet &gv, Bool_t si) const;
   virtual void initGenerator(Int_t code);
   virtual void generateEvent(Int_t code);

   // virtual Int_t getAnalyticalIntegralWN(RooArgSet& allDeps, RooArgSet& analDeps, 
   //                                       const RooArgSet* normSet, const char* rangeName) const;
   virtual Int_t getAnalyticalIntegral(RooArgSet& allVars, RooArgSet& analVars, const char* rangeName=0) const;
   virtual Double_t analyticalIntegral(Int_t code,const char* rangeName=0) const ;

   virtual void selectNormalization(const RooArgSet*,Bool_t);
   virtual ExtendMode extendMode() const;

   virtual Double_t expectedEvents(const RooArgSet* nset) const;

   // Function evaluation
   virtual Double_t getValV(const RooArgSet* set = 0) const;

   RooAbsPdf* pdf() const { 
      // Return pointer to pdf in product
      return static_cast<RooAbsPdf*>(_pdf.absArg());
   }

   RooAbsReal* efficiency() const { 
      // Return pointer to pdf in product
      return static_cast<RooAbsReal*>(_eff.absArg());
   }

   const RooArgSet* observables() const { 
      // Return pointer to pdf in product
      return static_cast<const RooArgSet*>(&_observables);
   }

   const RooArgList& getIntegralBins(const RooArgSet* iset, const char* rangeName = 0) const;

protected:

   virtual Double_t evaluate() const;


   typedef std::vector<double> BinBoundaries;
   BinBoundaries _binboundaries;

private:

   const char* makeFPName(const TString& prefix, const RooArgSet& iset, const RooArgSet *nset,
                          const TString& postfix) const;

   // Pointers to our underlying components
   RooRealProxy _pdf;     // Probability Density function
   RooRealProxy _eff;     // Probability Density function

   // Observables
   RooSetProxy _observables;

   // Data for event generation
   mutable RooArgSet _pdfGenVars;
   mutable Int_t _pdfGenCode;
   Double_t _maxEff;

   typedef std::vector<std::pair<double, TString> > Levels;
   Levels _levels; // 

   // Data for integration
   typedef std::map<std::string, RooArgSet*> argMap_t;
   mutable argMap_t _pdfObs;
   mutable const RooArgSet* _pdfNormSet;
   mutable const RooArgSet* _fixedNormSet;

   class CacheElem : public RooAbsCacheElement {
   public:
      CacheElem(const RooEffHistProd* parent,const RooArgSet& iset, const RooArgSet* nset,
                const char *rangeName);
      CacheElem() : _clone(0), _I(0) {}
      virtual ~CacheElem();

      virtual RooArgList containedArgs(Action) ;
      Double_t getVal() { 
          return _I->getVal(_intObs);
      }
      
      bool trivial() const { return _trivial; }

      void setClone(RooEffHistProd* cl) { _clone = cl; }

      RooEffHistProd* clone() { return _clone; }

      RooArgSet& intObs() { return _intObs; }

      const RooAbsReal* integral() const { return _I; }

   private:
      friend class RooEffHistProd;
      // Payload
      RooArgSet _intObs ;
      RooEffHistProd* _clone;
      RooAbsReal* _I;
      bool _trivial;
   };
   
   friend class CacheElem;
   CacheElem *getCache(const RooArgSet* nset, const RooArgSet* iset, 
                       const char* rangeName = 0, const bool makeClone = false) const;

   std::string setName(const RooArgSet* set) const;
   RooArgSet* normSet(const RooArgSet* input) const;
   bool allFalse() const;

   mutable RooObjCacheManager _cacheMgr;
   
   ClassDef(RooEffHistProd, 1) // Product operator p.d.f of (PDF x efficiency) implementing optimized generator context
};

#endif
