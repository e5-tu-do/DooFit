/*****************************************************************************
 * Project: RooFit                                                           *
 * Package: RooFitCore                                                       *
 * @(#)root/roofitcore:$Id: RooEffHistProd.cxx 25184 2008-08-20 13:59:55Z wouter $
 * Authors:                                                                  *
 *   WV, Wouter Verkerke, NIKHEF
 *   GR, Gerhard Raven, NIKHEF/VU                                            *
 *                                                                           *
 * Redistribution and use in source and binary forms,                        *
 * with or without modification, are permitted according to the terms        *
 * listed in LICENSE (http://roofit.sourceforge.net/license.txt)             *
 *                                                                           *
 * 2012-07-02 imported into B2DXFitters package by M. Schiller, Nikhef,      *
 * manuel.schiller@nikhef.nl; some changes have been made during the import, *
 * so anything that does not work is probably my fault                       *
 * 2012-08-21 re-import from upstream - Manuel Schiller                      *
 *****************************************************************************/


/////////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML
// The class RooEffHistProd implements the product of a PDF with an efficiency function.
// The normalization integral of the product is calculated numerically, but the
// event generation is handled by a specialized generator context that implements
// the event generation in a more efficient for cases where the PDF has an internal
// generator that is smarter than accept reject. 
// END_HTML
//

#include "RooFit.h"
#include "B2DXFitters/RooEffHistProd.h"
#include "RooEffGenContext.h"
#include "RooNameReg.h"
#include "RooRealVar.h"
#include "RooRandom.h"
#include "RooCategory.h"
#include "RooCatType.h"
#include "TString.h"
#include <RooAbsArg.h>
#include <RooCustomizer.h>
#include <RooConstVar.h>
#include <RooAddition.h>
#include <RooSuperCategory.h>
#include "B2DXFitters/RooBinnedPdf.h"
#include <RooGenContext.h>

#include <memory>
#include <algorithm>
#include <exception>

//ClassImp(RooEffHistProd);

namespace EffHistProd {
//_____________________________________________________________________________
void cloneRanges(const RooArgSet& observables, const RooArgSet& iset,
            const RooArgSet* nset, const char* rangeName,
            const char* newName)
{
   assert(rangeName != 0);
   assert(newName != rangeName);
   // Create a set which is the union of iset and nset to find other variables with the same range
   RooArgSet uni(iset);
   uni.add(*nset, kTRUE);

   // Skip the observables for which we are explicitely modifying the range
   uni.remove(observables);

   std::auto_ptr<TIterator> iter(uni.createIterator());
   RooAbsArg* arg = 0;
   while ((arg = static_cast<RooAbsArg*>(iter->Next()))) {
      if (arg->hasRange(rangeName)) {
         if (RooRealVar* real = dynamic_cast<RooRealVar*>(arg)) {
            const RooAbsBinning& range = real->getBinning(rangeName);
            if (range.isParameterized()) {
               // Deal with parametrized ranges
               real->setRange(newName, *range.lowBoundFunc(), *range.highBoundFunc());
            } else {
               // Regular range
               real->setRange(newName, range.lowBound(), range.highBound());
            }
         } else if (RooCategory* cat = dynamic_cast<RooCategory*>(arg)) {
            std::auto_ptr<TIterator> it(cat->typeIterator());
            TString states;
            const RooCatType* ct = 0;
            while ((ct = static_cast<const RooCatType*>(it->Next()))) {
               if (cat->isStateInRange(rangeName, ct->GetName())) {
                  states.Append(ct->GetName());
                  states.Append(",");
               }
            }
            states.Remove(TString::kTrailing, ',');
            cat->setRange(newName, states.Data());
         } else {
            throw EffHistProd::Exception("Got type for which a range cannot be cloned");
         }
      }
   }
}
}

namespace {
   using std::string;
   using std::cout;
   using std::endl;
}

//_____________________________________________________________________________
RooEffHistProd::CacheElem::CacheElem(const RooEffHistProd* parent, const RooArgSet& iset,
                                     const RooArgSet* nset, const char *rangeName)
   : _clone(0), _I(0)
{
   // create a function object for the corresponding integral of the underlying PDF.
   // i.e. if we have  eps(x)f(x,y)  and we get (x,y) as allVars, 
   // construct I_i,j = int_xmin,i^xmax,j dx int dy f(x,y)
   // so that later we can do sum_i eps( (x_i+x_i+1)/2 ) * I(x_i,x_i+1)
   RooArgSet x_;
   const RooArgSet *effObs = parent->efficiency()->getObservables(iset); // the subset of iset on which _eff depends
   RooFIter iter = iset.fwdIterator();
   while(RooAbsArg* arg = iter.next()) {
      if (effObs->contains(*arg)) {
         x_.add(*arg);
      }
   }
   delete effObs;

   _intObs.addClone(*nset);

   const BinBoundaries& bounds = parent->_binboundaries;
   assert(bounds.size() > 0);

   assert(x_.getSize() < 2); // for now, we only do 1D efficiency histograms...
   _trivial = x_.getSize()==0;

   if (!trivial()) {
      RooRealVar* x = static_cast<RooRealVar*>(x_.first());
      Double_t xmin = x->getMin(rangeName);
      Double_t xmax = x->getMax(rangeName);

      RooArgList effList;
      RooArgList intList;

      for(unsigned int i = 0; i < bounds.size() - 1; ++i) {
         const char* newRange = rangeName;
         Double_t low = bounds[i];
         Double_t high = bounds[i + 1];
         if (high < xmin) continue; // not there yet...
         if (low  > xmax) break;    // past the requested interval...

         Double_t thisxmin = std::max(low,  xmin);
         Double_t thisxmax = std::min(high, xmax);

         TString range = TString::Format("R_%d", i);
         // Add original rangeName if there is one
         if (0 != newRange) range.Replace(3, 8, newRange);
      
         // Create a new name for the range
         newRange = parent->makeFPName(parent->GetName(), iset, nset, range);
         x->setRange(newRange, thisxmin, thisxmax);

         if (0 != rangeName) {
            EffHistProd::cloneRanges(x_, iset, nset, rangeName, newRange);
         }
         TString suffix = "bin_"; suffix += i;
         TString name = x->GetName(); name += "_"; name += suffix; name += "_customizer";
         TString binName = x->GetName(); binName += "_"; binName += suffix;
         RooCustomizer customizer(*parent->efficiency(), name.Data());
         RooRealVar* cv = static_cast<RooRealVar*>(x->clone(binName.Data()));
         cv->setVal((thisxmin + thisxmax) / 2.);
         cv->setConstant(true);
         customizer.replaceArg(*x, *cv);
         // FIXME: Use the nset here once we get need the special case!
         RooAbsReal* I = parent->pdf()->createIntegral(iset, newRange);
         RooAbsArg* built = customizer.build(kFALSE);
         effList.add(*built);
         intList.add(*I);
      }
      _I = new RooAddition("integral", "integral", effList, intList, kTRUE);
   } else {
         // FIXME: Use the nset here once we get need the special case!
      _I = parent->pdf()->createIntegral(iset, rangeName);
   }
}

//_____________________________________________________________________________
RooArgList RooEffHistProd::CacheElem::containedArgs(Action) 
{
   // Return list of all RooAbsArgs in cache element
   RooArgList l(_intObs);
   l.add(*_I);
   l.add(*_clone);
   return l;
}

//_____________________________________________________________________________
RooEffHistProd::CacheElem::~CacheElem() 
{
   delete _I; _I = 0;
   delete _clone; _clone = 0;
}

//_____________________________________________________________________________
RooEffHistProd::RooEffHistProd(const char *name, const char *title, 
                               RooAbsPdf& inPdf, RooAbsReal& eff)
   : RooAbsPdf(name, title),
     _pdf("pdf", "pre-efficiency pdf", this, inPdf),
     _eff("efficiency", "efficiency", this, eff),
     _observables("observables", "observables", this),
     _maxEff(1.),
     _pdfNormSet(0),
     _fixedNormSet(0),
     _cacheMgr(this, 10)
{  
   // to figure out what the observable is, we look at the overlap of
   // the variables of efficiency function and pdf.
  
   std::auto_ptr<RooArgSet> pdfpars(inPdf.getVariables());
   std::auto_ptr<RooArgSet> effpars(eff.getVariables());
   std::auto_ptr<TIterator> iter(effpars->createIterator());

   RooAbsArg *effelem(0);
   while((effelem = (RooAbsArg*)iter->Next())) {
      if(pdfpars->find(effelem->GetName()))  _observables.add(*effelem);
   }

   if(_observables.getSize() == 0) {
      throw std::string("WARNING: RooEffHistProd: PDF and Efficiency function "
                        "factorise. Please use RooProd");
   } else if(_observables.getSize() > 1) {
      throw std::string("WARNING: RooEffHistProd not yet implemented for more than 1D efficiency" );
   }

   RooAbsRealLValue* x = static_cast<RooAbsRealLValue*>(_observables.first());

   // an interesting hack. need to discuss with wouter. one idea: let
   // every function that is 'binned' (discrete,quantized,..) add a 
   // special binning object to its dependents.
#if ROOT_VERSION_CODE >= ROOT_VERSION(5,32,0)
   std::list<Double_t>* binboundaries = eff.binBoundaries(*x, x->getMin(), x->getMax());
   // for(std::list<Double_t>::const_iterator it = binboundaries->begin();
   //     it != binboundaries->end(); ++it) {
   //    cout << *it << endl;
   // }
   std::copy(binboundaries->begin(), binboundaries->end(), std::back_inserter(_binboundaries));
#else
   std::list<Double_t>* binboundaries = eff.plotSamplingHint(*x, x->getMin(), x->getMax());
   if (binboundaries) {
      for(std::list<Double_t>::const_iterator it = binboundaries->begin();
           it != binboundaries->end(); ++it) {
         double x1 = *it++, x2 = *it;
         _binboundaries.push_back(0.5 * (x1 + x2));
      }
   }
#endif
   delete binboundaries;

   // double eps = 1e-4;
   // for(std::list<Double_t>::const_iterator it = _binboundaries.begin();
   //     it != _binboundaries.end(); ++it) {
   //    assert(*it > eps && (1 - *it) > eps);
   // }
}

//_____________________________________________________________________________
RooEffHistProd::RooEffHistProd(const RooEffHistProd& other, const char* name):
   RooAbsPdf(other, name),
   _binboundaries(other._binboundaries),
   _pdf("pdf", this, other._pdf),
   _eff("eff", this, other._eff),
   _observables("observables", this, other._observables),
   _maxEff(other._maxEff),
   _pdfNormSet(0), _fixedNormSet(0),
   _cacheMgr(other._cacheMgr, this)
{
   string otherName = other._pdfNormSet ? setName(other._pdfNormSet) : "";
   string otherFixedName = other._fixedNormSet ? setName(other._fixedNormSet) : "";
   for(argMap_t::const_iterator it = other._pdfObs.begin(), end = other._pdfObs.end();
       it != end; ++it) {
      RooArgSet* sn = static_cast<RooArgSet*>(it->second->snapshot(kTRUE));
      _pdfObs[it->first] = sn;
      if (otherName == it->first) {
         _pdfNormSet = sn;
      }
      if (otherFixedName == it->first) {
         _fixedNormSet = sn;
      }
   }
}

//_____________________________________________________________________________
RooEffHistProd::~RooEffHistProd() 
{
   // Destructor
   for(argMap_t::iterator it = _pdfObs.begin(), end = _pdfObs.end();
       it != end; ++it) {
      delete it->second;
      it->second = 0;
   }
}

//_____________________________________________________________________________
Double_t RooEffHistProd::getValV(const RooArgSet* ns) const 
{  
   // Return p.d.f. value normalized over given set of observables
   // cout << "RooEffHistProd::getValV " << (normSet ? *normSet : RooArgSet()) << endl;
   _pdfNormSet = _fixedNormSet ? _fixedNormSet : normSet(ns);
   return RooAbsPdf::getValV(ns);
}

//_____________________________________________________________________________
const RooArgList& RooEffHistProd::getIntegralBins(const RooArgSet* iset,
                                                  const char* rangeName) const
{
   const CacheElem* cache = getCache(iset, iset, rangeName);
   const RooAddition* addition = dynamic_cast<const RooAddition*>(cache->integral());
   assert(addition);
   return addition->list();
}

//_____________________________________________________________________________
Double_t RooEffHistProd::evaluate() const
{
   // cout << "RooEffHistProd " << GetName() << "::evaluate norm = " 
   //      << (_pdfNormSet ? *_pdfNormSet : RooArgSet()) <<endl;
   // Calculate and return 'raw' unnormalized value of p.d.f
   // double pdfVal = pdf()->getVal(*_pdfNormSet);
   // TEST THIS
   // double pdfVal(_pdf);

   // These have to be the _unnormalised_ values!!
   // FIXME: This needs to be fixed for the special case where the pdf shape depends on the 
   // normalisation set.
   double pdfVal = pdf()->getVal();

   double effVal = efficiency()->getVal();
   // cout << "RooEffHistProd::evaluate: " << effVal << " " << pdfVal << " "  << endl;
   return effVal * pdfVal;
}

//_____________________________________________________________________________
const char* RooEffHistProd::makeFPName(const TString& prefix,const RooArgSet& iset, 
                                       const RooArgSet* nset, const TString& binName) const
{
   static TString pname;
   pname = binName;
   if (pname.Sizeof()) pname.Append("_");
   pname.Append( prefix );
   if (pname.Sizeof()) pname.Append("_");
   std::auto_ptr<TIterator> i(iset.createIterator());
   RooAbsArg *arg;
   Bool_t first(kTRUE);
   pname.Append("I_");
   while((arg=(RooAbsArg*)i->Next())) {
      if (first) { 
         first = kFALSE;
      } else {
         pname.Append("_X_");
      }
      pname.Append(arg->GetName());
   }
   if (nset) {
      pname.Append("_N_");
      std::auto_ptr<TIterator> it(nset->createIterator());
      first = kTRUE;
      while ((arg = (RooAbsArg*)it->Next())) {
         if (first) {
            first=kFALSE;
         } else {
            pname.Append("_X_");
         }
         pname.Append(arg->GetName());
      }
   }
   return RooNameReg::str(RooNameReg::ptr(pname.Data()));
}

//_____________________________________________________________________________
void RooEffHistProd::selectNormalization(const RooArgSet* nset,Bool_t force) {
   RooAbsPdf::selectNormalization(nset,force);
}

//_____________________________________________________________________________
RooAbsPdf::ExtendMode RooEffHistProd::extendMode() const
{
   // If this product contains exactly one extendable p.d.f return the extension abilities of
   // that p.d.f, otherwise return CanNotBeExtended
   return pdf()->extendMode();
}

//_____________________________________________________________________________
Double_t RooEffHistProd::expectedEvents(const RooArgSet* nset) const
{
   // Return the expected number of events associated with the extentable input p.d.f
   // in the product. If there is no extendable term, return zero and issue and error
   return pdf()->expectedEvents(nset);
}

//_____________________________________________________________________________
RooAbsGenContext* RooEffHistProd::genContext(const RooArgSet &vars, const RooDataSet *prototype,
                                             const RooArgSet* auxProto, Bool_t verbose) const
{
   std::auto_ptr<RooArgSet> pdfObs(pdf()->getObservables(vars));
   // Return specialized generator context for RooEffHistProds that implements generation
   // in a more efficient way than can be done for generic correlated products
   assert(pdf() != 0);
   assert(efficiency() != 0);
   return new RooGenContext(*this, vars, prototype, auxProto, verbose, pdfObs.get());
}

//_____________________________________________________________________________
Int_t RooEffHistProd::getGenerator
(const RooArgSet& directVars, RooArgSet &generateVars, Bool_t staticInitOK) const
{
   _pdfGenVars.removeAll();
   RooArgSet pdfVars(directVars);
   Int_t pdfCode = pdf()->getGenerator(pdfVars, _pdfGenVars, staticInitOK) ;
   if (pdfCode != 0 && _pdfGenVars.getSize() != 0) {
      generateVars.add(_pdfGenVars);
      _pdfGenCode = pdfCode;
      return 1;
   } else {
      return 0;
   }
}

//_____________________________________________________________________________
void RooEffHistProd::initGenerator(Int_t code) 
{
   assert(code != 0);

   // Check if PDF supports maximum finding
   Int_t maxCode = efficiency()->getMaxVal(_pdfGenVars);
   if (!maxCode) {
      _maxEff = 1.;
   } else {
      _maxEff = efficiency()->maxVal(code);
   }
}

//_____________________________________________________________________________
void RooEffHistProd::generateEvent(Int_t code)
{
   // generate values for the variables corresponding to the generation code
   assert(code != 0);

   while (true) {
      // use the pdf to generate the observables.
      pdf()->generateEvent(_pdfGenCode);
      double val = efficiency()->getVal();
      // FIXME: use proper getMaxVal/maxVal
      if (val > _maxEff && !efficiency()->getMaxVal(_pdfGenVars)) {
         coutE(Generation) << ClassName() << "::" << GetName() 
              << ":generateEvent: value of efficiency is larger than assumed maximum of 1."  << endl;
         continue;
      }
      if (val > RooRandom::uniform() * _maxEff) break;
   }
}

//_____________________________________________________________________________
RooEffHistProd::CacheElem* RooEffHistProd::getCache(const RooArgSet* nset,
                                                    const RooArgSet* iset,
                                                    const char* rangeName,
                                                    const bool makeClone) const 
{
   Int_t sterileIndex(-1);
   CacheElem* cache = (CacheElem*) _cacheMgr.getObj(nset, iset, &sterileIndex,
                                                    RooNameReg::ptr(rangeName));
   if (cache) {
      return cache;
   }

   string clone_name = GetName(); clone_name += "_clone";
   const RooEffHistProd* parent = makeClone ? static_cast<const RooEffHistProd*>
      (clone(clone_name.c_str())) : this;
   
   cache = new CacheElem(parent, *iset, nset, rangeName);
   _cacheMgr.setObj(nset, iset, cache, RooNameReg::ptr(rangeName));

   if (makeClone) {
      cache->setClone(const_cast<RooEffHistProd*>(parent));
      cache->clone()->_fixedNormSet = &cache->intObs();
   }
   return getCache(nset, iset, rangeName, makeClone);
}

// //_____________________________________________________________________________
// Int_t RooEffHistProd::getAnalyticalIntegralWN(RooArgSet& allDeps, RooArgSet& analDeps, 
//                                               const RooArgSet* ns, const char* rangeName) const
// {
//    // Variant of getAnalyticalIntegral that is also passed the normalization set
//    // that should be applied to the integrand of which the integral is request.
//    // For certain operator p.d.f it is useful to overload this function rather
//    // than analyticalIntegralWN() as the additional normalization information
//    // may be useful in determining a more efficient decomposition of the
//    // requested integral
   
//    // No special handling required if a normalization set is given
//    if (ns && ns->getSize() > 0) {
//       _pdfNormSet = normSet(ns);
//       cout << "RooEffHistProd::getAnalyticalIntegralWN 0 " << allDeps << " " << analDeps << " "
//            << (ns ? *ns : RooArgSet()) << " " << (rangeName ? rangeName : "<none>")  << endl;
//       Int_t code = _forceNumInt ? 0 : getAnalyticalIntegral(allDeps, analDeps, rangeName);
//       return code;
//    } else if (_fixedNormSet) {    
//       _pdfNormSet = _fixedNormSet;
//       cout << "RooEffHistProd::getAnalyticalIntegralWN 1 " << allDeps << " " << analDeps << " "
//            << (ns ? *ns : RooArgSet()) << " " << (rangeName ? rangeName : "<none>") << endl;
//       Int_t code = _forceNumInt ? 0 : getAnalyticalIntegral(allDeps, analDeps, rangeName);
//       return code;
//    } else {
//       // No normSet passed
//       // Declare that we can analytically integrate all requested observables
//       std::auto_ptr<RooArgSet> pdfObs(pdf()->getObservables(allDeps));
//       analDeps.add(*pdfObs.get());

//       // Construct cache with clone of p.d.f that has fixed normalization set that is 
//       // passed to input pdf.
//       cout << "RooEffHistProd::getAnalyticalIntegralWN 2 " << allDeps << " " 
//            << analDeps << " " << (ns ? *ns : RooArgSet()) << " " 
//            << (rangeName ? rangeName : "<none>") << endl;

//       getCache(pdfObs.get(), pdfObs.get(), rangeName, true);
//       Int_t code = _cacheMgr.lastIndex();
//       return 1 + code;
//    }
// }

//_____________________________________________________________________________
Int_t RooEffHistProd::getAnalyticalIntegral(RooArgSet& allVars, RooArgSet& iset,
                                            const char* rangeName) const 
{
   if (_forceNumInt) return 0;

   // std::auto_ptr<RooArgSet> pdfObs(pdf()->getObservables(allVars));
   // if (pdfObs->getSize() == 0) {
   //    return 0;
   // } else {
   //    iset.add(*pdfObs.get());
   // }
   
   // We delegate further down by claiming to be able to do all of the variables
   iset.add(allVars);

   // getCache((_pdf.nset() ? _pdf.nset() : _pdfNormSet), &iset, rangeName);
   getCache(&iset, &iset, rangeName);
   Int_t code = _cacheMgr.lastIndex();
   return 1 + code;
}

//_____________________________________________________________________________
Double_t RooEffHistProd::analyticalIntegral(Int_t code, const char* rangeName) const 
{
   assert(code > 0);

   CacheElem* cache = static_cast<CacheElem*>(_cacheMgr.getObjByIndex(code - 1));
   if (!cache) {
      std::auto_ptr<RooArgSet> vars(getParameters(RooArgSet()));
      std::auto_ptr<RooArgSet> nset( _cacheMgr.nameSet1ByIndex(code - 1)->select(*vars));
      std::auto_ptr<RooArgSet> iset( _cacheMgr.nameSet2ByIndex(code - 1)->select(*vars));
      // const RooArgSet* normSet = _pdfNormSet ? _pdfNormSet : vars.get();
      // cache = getCache(nset.get(), iset.get(), rangeName, (_pdfNormSet == 0));
      cache = getCache(nset.get(), iset.get(), rangeName, false);
   }
   return cache->getVal();
}

//_____________________________________________________________________________
std::string RooEffHistProd::setName(const RooArgSet* set) const {
   string s;
   RooFIter it = set->fwdIterator();
   bool first = true;
   RooAbsArg* item = 0;
   while((item = it.next())) {
      if (first) {
         first = false;
      } else {
         s += ":";
      }
      s += item->GetName();
   }
   return s;
}

//_____________________________________________________________________________
RooArgSet* RooEffHistProd::normSet(const RooArgSet* input) const {
   if (!input) return 0;

   string s = setName(input);
   argMap_t::const_iterator i = _pdfObs.find(s);
   RooArgSet* r;
   if (i == _pdfObs.end()) {
      r = static_cast<RooArgSet*>(input->snapshot(kTRUE));
      _pdfObs.insert(make_pair(s, r));
   } else {
      r = i->second;
   }
   return r;
}
