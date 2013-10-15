/*****************************************************************************
 * Project: RooFit                                                           *
 * Package: RooFitModels                                                     *
 * Authors:                                                                  *
 *   WV, Wouter Verkerke, UC Santa Barbara, verkerke@slac.stanford.edu       *
 *   DK, David Kirkby,    UC Irvine,         dkirkby@uci.edu                 *
 *                                                                           *
 * Copyright (c) 2000-2005, Regents of the University of California          *
 *                          and Stanford University. All rights reserved.    *
 *                                                                           *
 * Redistribution and use in source and binary forms,                        *
 * with or without modification, are permitted according to the terms        *
 * listed in LICENSE (http://roofit.sourceforge.net/license.txt)             *
 *****************************************************************************/

//////////////////////////////////////////////////////////////////////////////
//
// BEGIN_HTML
// Single sided decay function that can be analytically convolved
// with any RooResolutionModel implementation
// 
// Special 
// END_HTML
//
// STL
#include <algorithm>

// RooFit
#include "RooFit.h"
#include "Riostream.h"
#include "Riostream.h"
#include "RooRealVar.h"
#include "RooRandom.h"
#include "RooGaussian.h"

// Local
#include "P2VV/RooTPDecay.h"

using namespace std;

namespace {
   const double c_light = 0.299792458; // mm / ps

   bool sort_tps(const pair<double, double>& tp1, const pair<double, double>& tp2) {
      return tp1.first < tp2.first;
   }
}


//_____________________________________________________________________________
RooTPDecay::RooTPDecay(const char *name, const char *title, 
                       RooRealVar& t, RooAbsReal& tau, RooArgList& tps,
                       const RooResolutionModel& model, const Roo1DTable& nPV,
                       RooAbsReal& z, RooAbsPdf& zPDF)
   : RooDecay(name, title, t, tau, model, RooDecay::SingleSided),
     _tps("tps", "turning points", this),
     _nPV(nPV),
     _z("z", "z coordinate", this, z, kFALSE, kFALSE),
     _zPDF("zPDF", "Z distribution PDF", this, zPDF, kFALSE, kFALSE),
     _d("TPDecay_distance", "TPDecay_distance", -50, 50),
     _mean("TPDecay_mean", "TPDecay_mean", 0.1),
     _gmean("TPDecay_gmean", "TPDecay_gmean", 0.0),
     _sigma("TPDecay_smearing", "TPDecay_smearing", 0.1)
{
   _nPV.Print("v");
   _tps.add(tps);
   _min = t.getMin();
   _max = t.getMax();
   assert(_tps.getSize() > 0 && _tps.getSize() % 2 == 0);

   _gauss = new RooGaussian("TPDecay_gauss", "TPDecay_gauss", _d, _gmean, _sigma);

}


//_____________________________________________________________________________
RooTPDecay::RooTPDecay(const RooTPDecay& other, const char* name)
   : RooDecay(other, name), 
     _tps("tps", this, other._tps),
     _nPV(other._nPV),
     _z("z", this, other._z),
     _zPDF("zPDF", this, other._zPDF),
     _d(other._d),
     _mean(other._mean),
     _gmean(other._gmean),
     _sigma(other._sigma),
     _min(other._min),
     _max(other._max)
{
   // Copy constructor
   const char* gname = other._gauss->GetName();
   _gauss = new RooGaussian(gname, gname, _d, _mean, _sigma);
}



//_____________________________________________________________________________
RooTPDecay::~RooTPDecay()
{
   // Destructor
   delete _gauss;
}



//_____________________________________________________________________________
Int_t RooTPDecay::getGenerator(const RooArgSet& directVars, RooArgSet &generateVars, Bool_t /*staticInitOK*/) const
{
   RooArgSet allVars(_tps);
   allVars.add(_t.arg());
   if (matchArgs(directVars, generateVars, allVars)) {
      RooArgSet z(_z.arg());
      RooArgSet genVars;
      _zCode = static_cast<const RooAbsPdf*>(_zPDF.absArg())->getGenerator(z, genVars);
      _gCode = _gauss->getGenerator(RooArgSet(_d), genVars);
      return 1 ;
   } else if (matchArgs(directVars, generateVars, _t)) {
      return 2 ;
   } else {
      return 0;
   }
}


//_____________________________________________________________________________
void RooTPDecay::initGenerator(Int_t /*code*/)
{
   RooArgSet z(_z.arg());
   _zPDF.absArg()->recursiveRedirectServers(z);
   static_cast<RooAbsPdf*>(_zPDF.absArg())->initGenerator(_zCode);
   _gauss->initGenerator(_gCode);
}

#ifdef STANDALONE
//_____________________________________________________________________________
void RooTPDecay::generateEvent(Int_t code)
{
   
   assert(code == 1 || code == 2) ;
   // Generate decay time
   RooDecay::generateEvent(code);

   if (code == 2) return;

   vector<double> PVz;
   PVz.reserve(10);

   vector<pair<double, double> > tps;
   vector<double> tmp_tps;
   vector<double> new_tps;

   // Generate delta-t dependent
   while(1) {
      // First generate the decay time.
      RooDecay::generateEvent(code);

      // Then the TPs
      Double_t rand = RooRandom::uniform();
      int n = 1;
      double f = 0;
      while(true) {
         f += _nPV.getFrac(n);
         if (f > rand) break;
         ++n;
      }
      for (int i = 0; i < n; ++i) {
         RooAbsPdf* zPDF = static_cast<RooAbsPdf*>(_zPDF.absArg());
         zPDF->generateEvent(_zCode);
         PVz.push_back(_z);
      }
      
      // Pick an origin PV
      rand = RooRandom::uniform();
      unsigned int j = 0;
      f = 1 / double(n);
      while (f < rand) {
         f += 1 / double(n);
         j++;
      }

      // Add the decay time at all turning points. The first TP is from true to false
      // and the last from false to true.
      for (size_t i = 0; i < PVz.size(); ++i) {
         double z = PVz[i];

         // Smear the turning point distance
         _gauss->generateEvent(_gCode);
         double d = fabs(_d.getVal()) + _mean.getVal();
         tps.push_back(make_pair((PVz[j] - (z + d)) / c_light, (PVz[j] - (z - d)) / c_light));
      }

      std::sort(tps.begin(), tps.end(), sort_tps);

      // Check for overlaps
      for (size_t i = 0; i < tps.size(); ++i) {
         if (i != 0 && tps[i - 1].second > tps[i].first) {
            // If there is an overlap, merge
            vector<pair<double, double> >::iterator it = tps.begin() + i;
            tps[i - 1].first  = tps[i - 1].first  < tps[i].first  ? tps[i - 1].first  : tps[i].first;
            tps[i - 1].second = tps[i - 1].second > tps[i].second ? tps[i - 1].second : tps[i].second;
            tps.erase(it);
            --i;
         }
      }

      // Check if the lifetime / momentum combination yields a candidate inside the acceptance.
      bool bad = false;
      for (size_t i = 0; i < tps.size() - 1; ++i) {
         if (_t > tps[i].first && _t < tps[i + 1].second) {
            bad = true;
            break;
         }
      }

      // Try again
      if (bad) {
         tps.clear();
         PVz.clear();
         continue;
      }

      // Copy to a regular vector
      for (size_t i = 0; i < tps.size(); ++i) {
         tmp_tps.push_back(tps[i].first);
         tmp_tps.push_back(tps[i].second);
      }      

      // Clip temporary TPs with lifetime range.
      for (size_t i = 0; i < tmp_tps.size(); ++i) {
         if (tmp_tps[i] < _min) {
            continue;
         } else if (new_tps.empty() && (i % 2 == 0)) {
            // if the first TP is true -> false, add the start of the interval first
            new_tps.push_back(_min);
         } else if (tmp_tps[i] > _max) {
            // if the last TP is false -> true, add the end of the interval
            if (i % 2 == 0) new_tps.push_back(_max);
            break;
         } 
         new_tps.push_back(tmp_tps[i]);
         // If we're done at this moment, we're in a true interval and 
         // need to add the max.
         if (i == (tmp_tps.size() - 1)) {
            new_tps.push_back(_max);
         }
      }

      assert(new_tps.size() != 0 &&  new_tps.size() % 2 == 0);

      if (new_tps.size() > size_t(_tps.getSize())) {
         coutW(Generation) << "Number of turning points is larger than number "
                           << "of available variables." << endl;
         tps.clear();
         PVz.clear();
         tmp_tps.clear();
         new_tps.clear();
         continue;
      }
      
      tmp_tps.clear();
      // Distribute left-over TPs over TP variables
      unsigned int nTP = new_tps.size() / 2;
      unsigned int div = _tps.getSize() / 2 - nTP + 1;
      unsigned int last = new_tps.size() - 1;
      double d = (new_tps[last] - new_tps[last - 1]) / double(div);
      for (unsigned int i = 0; i < last - 1; ++i) {
         tmp_tps.push_back(new_tps[i]);
      }
      for (unsigned int i = last - 1; i <= last - 1 + div; ++i) {
         double tp = new_tps[last - 1] + (i - last + 1) * d;
         tmp_tps.push_back(tp);
         if (i != last - 1 && i !=  last - 1 + div) {
            tmp_tps.push_back(tp);
         }
      }

      // Set output RealVars to generated values
      for (size_t i = 0; i < tmp_tps.size(); ++i) {
         RooAbsRealLValue* tp = dynamic_cast<RooAbsRealLValue*>(_tps.at(i));
         tp->setVal(tmp_tps[i]);
      }
      break;
   }  
}
#else
//_____________________________________________________________________________
void RooTPDecay::generateEvent(Int_t /* code */)
{
   
}
#endif
