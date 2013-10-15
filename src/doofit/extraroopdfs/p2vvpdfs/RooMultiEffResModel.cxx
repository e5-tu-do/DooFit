/*****************************************************************************
 * Project: RooFit                                                           *
 * Package: RooFitModels                                                     *
 * @(#)root/roofit:$Id: RooMultiEffResModel.cxx 44982 2012-07-10 08:36:13Z moneta $
 * Authors:                                                                  *
 *                                                                           *
 * Redistribution and use in source and binary forms,                        *
 * with or without modification, are permitted according to the terms        *
 * listed in LICENSE (http://roofit.sourceforge.net/license.txt)             *
 *****************************************************************************/

//////////////////////////////////////////////////////////////////////////////
//
// BEGIN_HTML
// Class RooMultiEffResModel implements a RooResolutionModel that models a EffResian
// distribution. Object of class RooMultiEffResModel can be used
// for analytical convolutions with classes inheriting from RooAbsAnaConvPdf
// END_HTML
//

#include <memory>
#include <sstream>
#include <iostream>
#include <utility>

#include "RooFit.h"
#include "Riostream.h"
#include "RooRealConstant.h"
#include "RooCustomizer.h"
#include "RooAddition.h"
#include "RooSuperCategory.h"
#include "RooRandom.h"

#include "P2VV/RooMultiEffResModel.h"
#include "P2VV/RooEffConvGenContext.h"
#include "P2VV/MultiHistEntry.h"

namespace {
   TString makeName(const char* name, const RooArgSet& terms ) {
      TString pname;
      pname = name;
      pname.Append("_");
      RooFIter i = terms.fwdIterator();
      RooAbsArg *arg;
      bool first = true;
      while((arg = i.next())) {
         if (first) {
            first= false;
         } else {
            pname.Append("_X_");
         }
         pname.Append(arg->GetName());
      }
      return pname;
   }

   RooSuperCategory* makeSuper(const char* name, const RooArgSet& _catVars) {
      TString catName = makeName(name, _catVars );
      return new RooSuperCategory(catName.Data(), catName, _catVars);
   }

   using std::string;
   using std::stringstream;
   using std::vector;
   using std::list;
   using std::map;
   using std::pair;
   using std::auto_ptr;
   using std::endl;
   using std::make_pair;
}

//_____________________________________________________________________________
RooMultiEffResModel::CacheElem::~CacheElem()
{
   for (Integrals::const_iterator it = _I.begin(), end = _I.end(); it != end;
        ++it) {
      delete it->second;
   }
}

//_____________________________________________________________________________
RooArgList RooMultiEffResModel::CacheElem::containedArgs(Action) 
{
   // Return list of all RooAbsArgs in cache element
   RooArgList l;
   for (Integrals::const_iterator it = _I.begin(), end = _I.end(); it != end;
        ++it) {
      l.add(*(it->second));
   }
   l.add(_iset);
   return l;
}

//_____________________________________________________________________________
RooMultiEffResModel::CacheElem::CacheElem(const HistEntries& entries,
                                          const RooArgSet& iset, const TNamed* rangeName)
   : _iset(iset)
{
   bool cats = false;
   RooArgSet categories;
   entries.begin()->second->addCategories(categories);
   RooArgSet observables(iset);
   RooAbsArg* cat = categories.first();
   if (iset.contains(*cat)) {
      // Integral over only categories
      cats = true;
      observables.remove(categories);
   }

   for(HistEntries::const_iterator it = entries.begin(), end = entries.end();
       it != end; ++it) {
      Int_t index = it->first;
      const MultiHistEntry* entry = it->second;
      if (cats && iset.getSize() == categories.getSize()) {
         stringstream s;
         string name;
         s << "integral_" << index;
         s >> name;
         _I.insert(make_pair(index, new RooConstVar(name.c_str(), name.c_str(), 1.)));
      } else {
         RooAbsReal* I = entry->effModel()->createIntegral(observables, RooNameReg::str(rangeName));
         _I.insert(make_pair(index, I));
      }
   }
}

//_____________________________________________________________________________
Double_t RooMultiEffResModel::CacheElem::getVal(const Int_t index) const
{
   Integrals::const_iterator it = _I.find(index);
   assert(it != _I.end());
   double val = it->second->getVal(_iset);
   // cout << "CacheElem::getVal " << it->second->GetName() << " = " << val << endl;
   return val;
}

//_____________________________________________________________________________
RooMultiEffResModel::RooMultiEffResModel(const char *name, const char *title,
                                         std::vector<MultiHistEntry*> entries)
   : RooResolutionModel(name,title, entries.front()->effModel()->convVar()),
     RooAbsEffResModel(),
     _prodGenCode(0),
     _super(0),
     _cacheMgr(this, 10)
{  
   RooArgSet observables;
   RooArgSet categories;
   for(vector<MultiHistEntry*>::const_iterator it = entries.begin(),
          end = entries.end(); it != end; ++it) {
      MultiHistEntry* entry = *it;
      if (observables.getSize() == 0) {
         observables.add(*entry->efficiency()->observables());
         entry->addCategories(categories);
      } else {
         assert(observables.equals(*entry->efficiency()->observables()));
         RooArgSet tmp;
         entry->addCategories(tmp);
         assert(categories.equals(tmp));
      }
   }
   
   // Build entries.
   _super = makeSuper(GetName(), categories);

   vector<MultiHistEntry*> ownedEntries;
   for(vector<MultiHistEntry*>::const_iterator it = entries.begin(),
          end = entries.end(); it != end; ++it) {
      MultiHistEntry* entry = new MultiHistEntry(**it);
      entry->setParent(this);
      ownedEntries.push_back(entry);
   }
   TString current = _super->getLabel();

   for(vector<MultiHistEntry*>::const_iterator it = ownedEntries.begin(),
          end = ownedEntries.end(); it != end; ++it) {
      MultiHistEntry* entry = *it;
      entry->select();
      Int_t index = _super->getIndex();
      pair<HistEntries::iterator, bool> r = _entries.insert(make_pair(index, entry));
      if (!r.second) {
         coutE(InputArguments) << "RooMultiEffResModel::ctor(" << GetName()
             << "): failed to insert MultiHistEntry" << std::endl;
         assert(r.second);
      }
   }
   _super->setLabel(current.Data());
}

//_____________________________________________________________________________
RooMultiEffResModel::RooMultiEffResModel(const RooMultiEffResModel& other, const char* name) 
   : RooResolutionModel(other,name),
     RooAbsEffResModel(),
     _prodGenObs(other._prodGenObs),
     _prodGenCode(other._prodGenCode),
     _levels(other._levels),
     _cacheMgr(other._cacheMgr,this)
{
   // Copy constructor
   _super = new RooSuperCategory(*other._super);

   for (HistEntries::const_iterator it = other._entries.begin(), end = other._entries.end();
        it != end; ++it) {
      MultiHistEntry* entry = new MultiHistEntry(*(it->second), this);
      _entries.insert(make_pair(it->first, entry));
   }
}

//_____________________________________________________________________________
RooMultiEffResModel::~RooMultiEffResModel()
{
   // Destructor
   if (_super) delete _super;
}

//_____________________________________________________________________________
RooMultiEffResModel* 
RooMultiEffResModel::convolution(RooFormulaVar* inBasis, RooAbsArg* owner) const
{
   // Instantiate a clone of this resolution model representing a convolution with given
   // basis function. The owners object name is incorporated in the clones name
   // to avoid multiple convolution objects with the same name in complex PDF structures.
   // 
   // Note: The 'inBasis' formula expression must be a RooFormulaVar that encodes the formula
   // in the title of the object and this expression must be an exact match against the
   // implemented basis function strings (see derived class implementation of method basisCode()
   // for those strings

   // Check that primary variable of basis functions is our convolution variable  
   if (inBasis->getParameter(0) != x.absArg()) {
      coutE(InputArguments) << "RooMultiEffResModel::convolution(" << GetName() << "," << this
                            << ") convolution parameter of basis function and PDF don't match" << endl
                            << "basis->findServer(0) = " << inBasis->findServer(0) << endl
                            << "x.absArg()           = " << x.absArg() << endl ;
      return 0 ;
   }

   if (basisCode(inBasis->GetTitle())==0) {
      coutE(InputArguments) << "RooMultiEffResModel::convolution(" << GetName() << "," << this
                            << ") basis function '" << inBasis->GetTitle() << "' is not supported." << endl ;
      return 0 ;
   }

   const char* cacheParamsStr = getStringAttribute("CACHEPARAMINT") ;
   if (cacheParamsStr && !strlen(cacheParamsStr)) cacheParamsStr=0;

   vector<MultiHistEntry*> entries;
   vector<RooResolutionModel*> models;
   for (HistEntries::const_iterator it = _entries.begin(), end = _entries.end();
        it != end; ++it) {
      RooResolutionModel *conv = it->second->effModel()->convolution(inBasis, owner);
      if (cacheParamsStr) conv->setStringAttribute("CACHEPARAMINT",cacheParamsStr);
      models.push_back(conv);

      RooAbsEffResModel* effModel = dynamic_cast<RooAbsEffResModel*>(conv);
      if (!effModel) {
         cout << conv->GetName() << " is not a RooAbsEffResModel!" << endl;
         assert(false);
      }
      MultiHistEntry* entry = new MultiHistEntry(*(it->second), effModel);
      entries.push_back(entry);
   }

   TString newName = TString::Format("%s_conv_%s_[%s]", GetName(),inBasis->GetName(), owner->GetName());

   RooMultiEffResModel *effConv = new RooMultiEffResModel(newName, GetTitle(), entries);
   for (vector<RooResolutionModel*>::iterator it = models.begin(), end = models.end();
        it != end; ++it) {
      effConv->addOwnedComponents(**it);
   }
   effConv->changeBasis(inBasis) ;

   for (vector<MultiHistEntry*>::const_iterator it = entries.begin(),
           end = entries.end(); it != end; ++it) {
      delete *it;
   }

   return effConv ;
}

//_____________________________________________________________________________
Int_t RooMultiEffResModel::basisCode(const char* name) const 
{ 
   return _entries.begin()->second->effModel()->basisCode(name);
} 


//_____________________________________________________________________________
Double_t RooMultiEffResModel::evaluate() const 
{  
   bool onlyCats = false;
   bool onlyVars = false;

   // Calculate the raw value of this p.d.f
   RooArgSet categories;
   _entries.begin()->second->addCategories(categories);
   RooAbsArg* cat = categories.first();
   if (_normSet && _normSet->getSize() == categories.getSize() && 
       _normSet->contains(*cat)) {
      onlyCats = true;
   } else if (_normSet && !_normSet->contains(*cat)) {
      onlyVars = true;
   }

   double val = 0;
   for (HistEntries::const_iterator it = _entries.begin(), end = _entries.end();
        it != end; ++it) {
      if (it->second->thisEntry()) {
         if (onlyCats) {
            val = it->second->relative()->getVal();
         } else if (onlyVars) {
            val = it->second->relative()->getVal() * it->second->effModel()->getVal();
         } else {
            val = it->second->relative()->getVal() * it->second->effModel()->getVal();
         }
         // cout << "RooMultiEffResModel::evaluate " 
         //      << it->second->efficiency()->GetName() << " case = " << code << " " 
         //      << " norm " << (_normSet ? *_normSet : RooArgSet()) << " = " << val << endl;
      }
   }
   return val;
}


//_____________________________________________________________________________
Bool_t RooMultiEffResModel::forceAnalyticalInt(const RooAbsArg& /*dep*/) const
{
   // Force RooRealIntegral to offer all observables for internal integration
   return kTRUE ;
}

//_____________________________________________________________________________
Int_t RooMultiEffResModel::getAnalyticalIntegral(RooArgSet& allVars, RooArgSet& analVars, const char* rangeName) const 
{
   if (_forceNumInt) return 0;

   RooArgSet categories;
   for (HistEntries::const_iterator it = _entries.begin(), end = _entries.end();
        it != end; ++it) {
      it->second->addCategories(categories);
   }

   bool all = true;
   bool none = true;
   RooFIter iter = categories.fwdIterator();
   while (RooAbsArg* cat = iter.next()) {
      if (!allVars.find(*cat)) {
         all &= false;
      } else {
         none &= false;
      }
   }
   if (!(all ^ none)) {
      return 0;
   }

   // we always do things ourselves -- actually, always delegate further down the line ;-)
   analVars.add(allVars);
   // The underlying pdf does not depend on the categories
   // analVars.remove(categories);

   bool vars = (!all && analVars.getSize() != 0) || (all && analVars.getSize() > categories.getSize());

   // check if we already have integrals for this combination of factors
   Int_t sterileIndex(-1);
   CacheElem* cache = (CacheElem*) _cacheMgr.getObj(&analVars, &analVars, &sterileIndex,
                                                    RooNameReg::ptr(rangeName));
   if (cache != 0) {
      Int_t code = _cacheMgr.lastIndex();
      code = (code + 1) << 1;
      code |= vars;
      code = code << 1;
      code |= all;
      return code;
   }

   // we don't, so we make it right here....
   RooArgSet pdfVars(analVars);
   pdfVars.remove(categories);
   cache = new CacheElem(_entries, analVars, RooNameReg::ptr(rangeName));
   
   Int_t code = _cacheMgr.setObj(&analVars, &analVars, cache, RooNameReg::ptr(rangeName));
   // cout << "getAnalyticalIntegral cache code = " << code << endl;
   code = (code + 1) << 1;
   code |= vars;
   code = code << 1;
   code |= all;
   // cout << "getAnalyticalIntegral return code = " << code << endl;
   return code;
}

//_____________________________________________________________________________
Double_t RooMultiEffResModel::analyticalIntegral(Int_t code, const char* rangeName) const 
{
   assert(code > 0);
   // Calculate integral internally from appropriate integral cache
   // note: rangeName implicit encoded in code: see _cacheMgr.setObj in getPartIntList...
   bool cats = code & 0x1;
   bool vars = code & 0x2;
   Int_t cacheCode = (code >> 2);

   // cout << "analyticalIntegral code = " << code << " cachecode = " << cacheCode << endl;
 
   CacheElem *cache = static_cast<CacheElem*>(_cacheMgr.getObjByIndex(cacheCode - 1));
   
   if (cache==0) {
      // cache got sterilized, trigger repopulation of this slot, then try again...
      auto_ptr<RooArgSet> params(getParameters(RooArgSet()));
      auto_ptr<RooArgSet> iset(_cacheMgr.nameSet2ByIndex(cacheCode - 1)->select(*params));
      RooArgSet dummy;
      Int_t code2 = getAnalyticalIntegral(*iset, dummy, rangeName);
      // must have revived the right (sterilized) slot...
      // cout << "code = " << code << " code2 = " << code2 << endl;
      assert(code == code2);
      return analyticalIntegral(code2, rangeName);
   }
   assert(cache != 0);

   double sum = 0;
   for (HistEntries::const_iterator it = _entries.begin(), end = _entries.end();
        it != end; ++it) {
      if (!cats && vars && it->second->thisEntry()) {
         double entry = it->second->relative()->getVal() * cache->getVal(it->first);
         // cout << "AnalyticalIntegral: entry = " << entry << endl;
         return entry;
      } else if (cats) {
         sum += it->second->relative()->getVal() * cache->getVal(it->first);
      }
   }
   // cout << "AnalyticalIntegral: sum = " << sum << endl;
   return sum;
}

//_____________________________________________________________________________
RooAbsGenContext* RooMultiEffResModel::modelGenContext
(const RooAbsAnaConvPdf& convPdf, const RooArgSet &vars, const RooDataSet *prototype,
 const RooArgSet* auxProto, Bool_t verbose) const
{
   return new RooEffConvGenContext(convPdf, vars, prototype, auxProto, verbose);
}

//_____________________________________________________________________________
Int_t RooMultiEffResModel::getGenerator(const RooArgSet& directVars, RooArgSet &generateVars,
                                        Bool_t staticInitOK) const
{
   bool all = true;
   bool none = true;

   _prodGenObs.removeAll();

   RooArgSet categories;
   for (HistEntries::const_iterator it = _entries.begin(), end = _entries.end();
        it != end; ++it) {
      it->second->addCategories(categories);
   }
   RooFIter iter = categories.fwdIterator();
   while (RooAbsArg* cat = iter.next()) {
      if (!directVars.find(*cat)) {
         all &= false;
      } else {
         none &= false;
      }
   }
   if (!(all ^ none)) {
      return 0;
   }

   RooArgSet testVars(directVars);
   testVars.remove(categories);

   Int_t prodGenCode = 0;
   RooArgSet genVars;
   for (HistEntries::const_iterator it = _entries.begin(), end = _entries.end();
        it != end; ++it) {
      const RooResolutionModel* resModel = it->second->effModel();
      if (genVars.getSize() == 0) {
         prodGenCode = resModel->getGenerator(testVars, genVars, staticInitOK);
      } else {
         RooArgSet prodGenVars;
         Int_t code = resModel->getGenerator(testVars, prodGenVars, staticInitOK);
         if (!(prodGenVars.equals(genVars) && prodGenCode == code)) {
            assert(false);
         }
      }
   }

   generateVars.add(genVars);
   _prodGenCode = prodGenCode;
   _prodGenObs.add(testVars);
   if (none) {
      return 1;
   } else {
      iter = categories.fwdIterator();
      while (RooAbsArg* cat = iter.next()) {
         generateVars.add(*cat);
      }
      return 2;
   }
}

//_____________________________________________________________________________
void RooMultiEffResModel::initGenerator(Int_t code)
{
   // Forward one-time initialization call to component generation initialization
   // methods.
   for (HistEntries::iterator it = _entries.begin(), end = _entries.end();
        it != end; ++it) {
      it->second->effModel()->initGenerator(_prodGenCode);
   }
   if (code == 1) {
      return;
   }
   _levels.clear();

   RooArgSet categories;
   for (HistEntries::const_iterator it = _entries.begin(), end = _entries.end();
        it != end; ++it) {
      it->second->addCategories(categories);
   }
   _super->recursiveRedirectServers(categories);

   // RooSuperCategory* super = dynamic_cast<RooSuperCategory*>(_super->absArg());
   std::auto_ptr<TIterator> superIter(_super->MakeIterator());

   TString current = _super->getLabel();
   while (TObjString* label = static_cast<TObjString*>(superIter->Next())) {
      _super->setLabel(label->String());
      Int_t index = _super->getIndex();
      HistEntries::const_iterator it = _entries.find(index);
      if (it == _entries.end()) {
         // Skip the combination for which there is no shape (all false).
         continue;
      }
      double n = it->second->relative()->getVal();
      if (!_levels.empty()) n += _levels.back().first; // cumulative
      cxcoutD(Generation) << "RooMultiHistEfficiency creating sampler for " << _prodGenObs
                          << " given " << categories
                          << " = "  << label->String() << " (level = " << n << ")" << endl;
      _levels.push_back(make_pair(n, label->String()));
   }

   // Normalise just in case, but it should be a noop.
   for (Levels::iterator i = _levels.begin(); i != _levels.end(); ++i) 
      i->first /= _levels.back().first; // normalize
   _super->setLabel(current);
}

//_____________________________________________________________________________
void RooMultiEffResModel::generateEvent(Int_t code)
{
   if (code == 2) {
      // Only generate categories if code == 2. 
      Double_t r = RooRandom::uniform();
      // RooSuperCategory* super = dynamic_cast<RooSuperCategory*>(_super->absArg());
      std::auto_ptr<TIterator> superIter(_super->MakeIterator());

      Levels::const_iterator itLevel = _levels.begin();
      // find the right generator, and generate categories at the same time...
      while (itLevel != _levels.end() && itLevel->first < r) {
         ++itLevel;
      }
      // this assigns the categories.
      _super->setLabel(itLevel->second);
   }

   // Otherwise assume the categories are OK and just get the current index.
   Int_t index = _super->getIndex();
   HistEntries::const_iterator itEntry = _entries.find(index);
   assert(itEntry != _entries.end());

   // now that've assigned the categories, we can use the 'real' samplers
   // which are conditional on the categories.
   itEntry->second->effModel()->generateEvent(_prodGenCode);
}

//_____________________________________________________________________________
const RooAbsReal* RooMultiEffResModel::efficiency() const {
   Int_t index = _super->getIndex();
   HistEntries::const_iterator it = _entries.find(index);
   assert(it != _entries.end());
   return it->second->effModel();
}

//_____________________________________________________________________________
std::vector<const RooAbsReal*> RooMultiEffResModel::efficiencies() const { 
   std::vector<const RooAbsReal*> effs;
      // Return pointer to pdf in product
   for (HistEntries::const_iterator it = _entries.begin(), end = _entries.end();
        it != end; ++it) {
      effs.push_back(it->second->effModel());
   }
   return effs;
}

//_____________________________________________________________________________
RooArgSet* RooMultiEffResModel::observables() const {
   RooArgSet* observables = new RooArgSet;
   for(HistEntries::const_iterator it = _entries.begin(),
          end = _entries.end(); it != end; ++it) {
      MultiHistEntry* entry = it->second;
      RooArgSet obs(*entry->efficiency()->observables());
      if (observables->getSize() == 0) {
         observables->add(obs);
      } else {
         assert(observables->equals(obs));
      }
   }
   return observables;
}
