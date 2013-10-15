/*****************************************************************************
 * Project: RooFit                                                           *
 * Package: RooFitModels                                                     *
 * @(#)root/roofit:$Id: RooEffResModel.cxx 44982 2012-07-10 08:36:13Z moneta $
 * Authors:                                                                  *
 *                                                                           *
 * Redistribution and use in source and binary forms,                        *
 * with or without modification, are permitted according to the terms        *
 * listed in LICENSE (http://roofit.sourceforge.net/license.txt)             *
 *****************************************************************************/

//////////////////////////////////////////////////////////////////////////////
//
// BEGIN_HTML
// Class RooEffResModel implements a RooResolutionModel that models a EffResian
// distribution. Object of class RooEffResModel can be used
// for analytical convolutions with classes inheriting from RooAbsAnaConvPdf
// END_HTML
//

#include <memory>

#include "RooFit.h"
#include "Riostream.h"
#include "P2VV/RooEffResModel.h"
#include "RooRealConstant.h"
#include "RooCustomizer.h"
#include "RooAddition.h"
#include "RooStringVar.h"
#include "RooAbsAnaConvPdf.h"
#include "P2VV/RooEffConvGenContext.h"
#include "RooCachedReal.h"

using namespace std;

//_____________________________________________________________________________
RooEffResModel::CacheElem::~CacheElem()
{
   delete _I;
}

//_____________________________________________________________________________
RooArgList RooEffResModel::CacheElem::containedArgs(Action) 
{
   // Return list of all RooAbsArgs in cache element
   return RooArgList(*_I);
}

//_____________________________________________________________________________
RooEffResModel::CacheElem::CacheElem(const RooEffResModel& parent, const RooArgSet& iset,
                                     const TNamed* rangeName)
   : _I(0)
{
   RooRealVar& x = parent.convVar(); // binboundaries not const...
   const RooAbsReal& eff = *parent.efficiency();
   const RooAbsReal& model = parent.model();
   // the subset of iset on which the efficiency depends
   std::auto_ptr<const RooArgSet> effInt( eff.getObservables(iset) ); 

   if (effInt->getSize()>1) {
        std::cout << " got efficiency iset with more than 1 AbsArg -- not yet supported" << std::endl;
        effInt->Print("V");
   }
   assert(effInt->getSize() < 2); // for now, we only do 1D efficiency histograms...
   //TODO: instead of the above, verify whether things factorize, i.e. 
   //      allow the case where the overlap of effInt and model is 1D, and
   //      all 'other' dependencies are from the efficiency only...
   //      That works because we can then ingrate the 'ceff' coefficient below
   //      over the remaining dependencies... 
   if (effInt->getSize()==0) {
      _I = model.createIntegral(iset,RooNameReg::str(rangeName)); 
      return;
   }

   RooArgList effList;
   RooArgList intList;

   const RooArgList& ranges = parent.getIntegralRanges(iset, RooNameReg::str(rangeName));
   RooFIter it = ranges.fwdIterator();
   while (RooStringVar* rangeVar = static_cast<RooStringVar*>(it.next())) {
      const char* range = rangeVar->getVal();
      assert(x.hasRange(range));

      Double_t xmin = x.getMin(range);
      Double_t xmax = x.getMax(range);
      
      intList.add(*model.createIntegral(iset, range));

      // create RooAbsReal for (average) efficiency in this range
      RooCustomizer customizer(eff, (TString(range) + "_customizer").Data());
      RooRealVar* cv = static_cast<RooRealVar*>(x.clone(TString(x.GetName()) + "_" + range) );
      cv->setVal((xmin + xmax) / 2.);
      cv->setConstant(true);
      customizer.replaceArg(x, *cv);
      RooAbsArg *ceff = customizer.build(kFALSE);
      ceff->addOwnedComponents(*cv);
      effList.add( *ceff );
   }
   TString iName = TString::Format("%s_I_%s", parent.GetName(),x.GetName());
   _I = new RooAddition(iName, iName, effList, intList, kTRUE);
}

//_____________________________________________________________________________
RooEffResModel::RooEffResModel(const char *name, const char *title, RooResolutionModel& model, RooAbsReal& eff) 
   : RooResolutionModel(name, title, model.convVar()),
     RooAbsEffResModel()
   , _observables("observables", "observables", this)
   , _model("!model","Original resolution model",this,model)
   , _eff("!efficiency","efficiency of convvar", this,eff)
   , _cacheMgr(this, 10)
{  
   // assert that efficiency is a function of convVar, and there are no overlaps...
   _observables.add(model.convVar());
}

//_____________________________________________________________________________
RooEffResModel::RooEffResModel(const RooEffResModel& other, const char* name) 
  : RooResolutionModel(other, name),
    RooAbsEffResModel()
  , _observables("observables", this, other._observables)
  , _model("!model",this,other._model)
  , _eff("!efficiency",this,other._eff)
  , _cacheMgr(other._cacheMgr,this)
{
   // Destructor
   for (RangeMap::const_iterator it = other._ranges.begin(), end = other._ranges.end();
        it != end; ++it) {
      RooArgList* ranges = new RooArgList(*it->second);
      _ranges.insert(make_pair(it->first, ranges));
   }
}

//_____________________________________________________________________________
RooEffResModel::~RooEffResModel()
{
   // Destructor
   for (RangeMap::const_iterator it = _ranges.begin(), end = _ranges.end();
        it != end; ++it) {
      delete it->second;
   }
}

//_____________________________________________________________________________
RooEffResModel* 
RooEffResModel::convolution(RooFormulaVar* inBasis, RooAbsArg* owner) const
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
    coutE(InputArguments) << "RooEffResModel::convolution(" << GetName() << "," << this
              << ") convolution parameter of basis function and PDF don't match" << endl
              << "basis->findServer(0) = " << inBasis->findServer(0) << endl
              << "x.absArg()           = " << x.absArg() << endl ;
    return 0 ;
  }

  if (basisCode(inBasis->GetTitle())==0) {
    coutE(InputArguments) << "RooEffResModel::convolution(" << GetName() << "," << this
              << ") basis function '" << inBasis->GetTitle() << "' is not supported." << endl ;
    return 0 ;
  }


  RooResolutionModel *conv = model().convolution(inBasis,owner);

  TString newName  = TString::Format("%s_conv_%s_[%s]", GetName(),inBasis->GetName(), owner->GetName());
  TString newTitle = TString::Format("%s convoluted with basis function %s",conv->GetTitle(),inBasis->GetName()) ;
  conv->SetTitle(newTitle.Data()) ;

  RooEffResModel *effConv = new RooEffResModel(newName,newTitle, *conv, *static_cast<RooAbsReal*>(_eff.absArg()));
  effConv->addOwnedComponents(*conv);
  effConv->changeBasis(inBasis) ;

  const char* cacheParamsStr = getStringAttribute("CACHEPARAMINT") ;
  if (cacheParamsStr && strlen(cacheParamsStr)) effConv->setStringAttribute("CACHEPARAMINT",cacheParamsStr);

  return effConv ;
}

//_____________________________________________________________________________
Int_t RooEffResModel::basisCode(const char* name) const 
{ 
   return model().basisCode(name);
} 

//_____________________________________________________________________________
Double_t RooEffResModel::evaluate() const 
{  
    Double_t mod = model().getVal();
    // TODO: replace this by the discretized version, i.e. replace convVar by customized middle of bin...
    //       this in order to ensure evaluate & analyticalIntegral are consistent (in case eff is not discretized!!!)
    Double_t eps = efficiency()->getVal(); 
    return eps * mod;
}

//_____________________________________________________________________________
Bool_t RooEffResModel::forceAnalyticalInt(const RooAbsArg& /*dep*/) const
{
  // Force RooRealIntegral to offer all observables for internal integration
   return kTRUE ;
}

//_____________________________________________________________________________
Int_t RooEffResModel::getAnalyticalIntegral(RooArgSet& allVars, RooArgSet& analVars, const char* rangeName) const 
{
    if (_forceNumInt) return 0;
    analVars.add(allVars);
    getCache(&analVars, RooNameReg::ptr(rangeName));
    return 1 + _cacheMgr.lastIndex();
}

//_____________________________________________________________________________
Double_t RooEffResModel::analyticalIntegral(Int_t code, const char* rangeName) const 
{
   assert(code > 0);
   CacheElem* cache = static_cast<CacheElem*>(_cacheMgr.getObjByIndex(code - 1));
   if (!cache) {
      std::auto_ptr<RooArgSet> vars(getParameters(RooArgSet()));
      std::auto_ptr<RooArgSet> iset( _cacheMgr.nameSet1ByIndex(code - 1)->select(*vars));
      cache = getCache(iset.get(), RooNameReg::ptr(rangeName) );
      assert(cache!=0);
   }
   return cache->getVal();
}

//_____________________________________________________________________________
RooAbsGenContext* RooEffResModel::modelGenContext
(const RooAbsAnaConvPdf& convPdf, const RooArgSet &vars, const RooDataSet *prototype,
 const RooArgSet* auxProto, Bool_t verbose) const
{
   return new RooEffConvGenContext(convPdf, vars, prototype, auxProto, verbose);
}

//_____________________________________________________________________________
Int_t RooEffResModel::getGenerator(const RooArgSet& directVars, RooArgSet &generateVars,
                                   Bool_t staticInitOK) const
{
   return model().getGenerator(directVars, generateVars, staticInitOK);
}

//_____________________________________________________________________________
void RooEffResModel::initGenerator(Int_t code)
{
   model().initGenerator(code);
}

//_____________________________________________________________________________
void RooEffResModel::generateEvent(Int_t code)
{
   // The hit-miss on the efficiency is done at the level of the GenContext.
   model().generateEvent(code);
}

//_____________________________________________________________________________
const RooArgList& RooEffResModel::getIntegralRanges(const RooArgSet& iset,
                                                    const char* rangeName) const
{
   rangeName = rangeName ? rangeName : "default";
   RangeMap::const_iterator it = _ranges.find(rangeName);
   if (it != _ranges.end()) return *it->second;

   RooRealVar& x = static_cast<RooRealVar&>(convVar());
   Double_t xmin = x.getMin(rangeName);
   Double_t xmax = x.getMax(rangeName);

   RooArgList* ranges = new RooArgList;
   std::auto_ptr<std::list<Double_t> > bounds(efficiency()->binBoundaries(x, x.getMin(), x.getMax()));
   if (bounds.get()==0) {
        TString err = TString::Format( "RooEffResModel(%s): specified efficiency %s does not provide binBoundaries..." , GetName(), efficiency()->GetName() );
        std::cout << err << std::endl;
        throw err;
   }
   std::list<Double_t>::const_iterator lo, hi = bounds->begin();
   for (unsigned int i = 0; i + 1 < bounds->size();++i) {
      lo = hi++;
      if (*hi < xmin) continue; // not there yet...
      if (*lo > xmax) break;    // past the requested interval...
      Double_t thisxmin = std::max(*lo, xmin);
      Double_t thisxmax = std::min(*hi, xmax);

      // add eff name, as it specifies the boundaries...
      TString trange = TString::Format("R%d_%s_%s", i, x.GetName(), efficiency()->GetName());

      // Add original rangeName if there is one
      if (rangeName) {
         trange.Append("_");
         trange.Append(rangeName);
      }

      trange.Append("_I_");
      RooNameSet ns(iset);
      trange.Append(ns.content());
      const char* range = trange.Data();

      // Create a new name for the range
      // check if already exists and matches..
      if (!x.hasRange(range)) {
         x.setRange(range, thisxmin, thisxmax);
      }
      assert(x.getMin(range)==thisxmin);
      assert(x.getMax(range)==thisxmax);
      RooStringVar* stringVar = new RooStringVar(trange.Data(), trange.Data(), trange.Data()); 
      ranges->addOwned(*stringVar);
   }
   pair<RangeMap::iterator, bool> r = _ranges.insert(make_pair(string(rangeName), ranges));
   assert(r.second);
   return *r.first->second;
}

//_____________________________________________________________________________
RooEffResModel::CacheElem* RooEffResModel::getCache(const RooArgSet *iset, const TNamed *rangeName) const 
{
   Int_t sterileIndex(-1);
   CacheElem* cache = (CacheElem*) _cacheMgr.getObj(iset, &sterileIndex, rangeName);
   if (cache) return cache;
   _cacheMgr.setObj(iset, new CacheElem( *this,  *iset,  rangeName), rangeName);
   return getCache(iset, rangeName );
}
