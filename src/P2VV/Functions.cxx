// STD & STL
#include <memory>
#include <iostream>
#include <cmath>
#include <string>
#include <sstream>
#include <set>
#include <vector>
#include <list>

// ROOT
#include <TH1.h>
#include <TTree.h>
#include <TMatrixT.h>
#include <TEntryList.h>
#include <TDirectory.h>
#include <TBranch.h>
#include <TLeaf.h>
#include <TObjArray.h>

// RooFit
#include <RooFit.h>
#include <RooRealVar.h>
#include <RooCategory.h>
#include <RooDataSet.h>
#include <RooArgSet.h>
#include <RooLinkedListIter.h>

namespace {
   using std::cout;
   using std::endl;
   using std::list;
}

double sigmaFromFT( const TH1& h1, const double dMs, const double dMsErr, std::ostream& out)
{
   double sum(0), sumdt(0), sumdterr(0), sumcos(0), sumx2(0), error(0) ;
   for(int i=1; i<=h1.GetNbinsX() ; ++i) {
      double c = h1.GetBinContent( i ) ;
      double e = h1.GetBinError( i ) ;
      double x = h1.GetBinCenter( i ) ;
      double dt = h1.GetBinWidth( i ) ;
      sum += c ;
      sumdt += c * dt ;
      sumdterr += e * e * dt * dt;

      double cosv = cos( - dMs * x );
      sumcos += c * cosv * dt ;
      sumx2 += c * x * x ;

      double coserr = fabs(sin(dMsErr)) * dMsErr;
      error += (c * c * coserr * coserr +  cosv * cosv * e * e) * dt * dt;
   }

   error = sqrt(error / (sumdt * sumdt) + sumdterr * sumcos * sumcos / pow(sumdt, 4));

   double rms = sqrt(sumx2/sum) ;
   double D = sumcos / sumdt ;
   double sigma = sqrt( -2*log(D) ) / dMs ;

   out << sum << " " << sumdt << " " << sumcos << " " << sumx2 << std::endl;
   out << "RMS of input histogram: " << rms<< std::endl
       << "If distribution were Gaussian, dilution is: " << exp(-0.5*rms*rms*dMs*dMs) << std::endl 
       << "Dilution from FT: " << D << " +- " << error << std::endl
       << "Corresponding Gaussian resolution: " << sigma << std::endl;

   return D;
}


void addSWeightToTree(const std::vector<double>& weights, TTree& tree, const std::string& branch_name)
{
   assert(Long64_t(weights.size()) == tree.GetEntries());

   Double_t w = 0;
   //#       Create the output Branch
   std::string branch_def = branch_name + "/D";
   TBranch* branch = tree.Branch(branch_name.c_str(), &w, branch_def.c_str());

   for (Long64_t i = 0; i < tree.GetEntries(); ++i) {
      w = weights[i];
      branch->Fill();
   }
   tree.FlushBaskets();

}

void addSWeightToTree(double* weights, size_t n, TTree& tree, const std::string& branch_name)
{
   std::vector<double> w(weights, weights + n);
   addSWeightToTree(w, tree, branch_name);
}

void addSWeightToTree(const RooDataSet& ds, TTree& tree, const std::string& branch_name,
                      const std::string& cut)
{
   tree.Draw(">>elist", cut.c_str(), "entrylist");
   TEntryList *cut_list = static_cast<TEntryList*>(gDirectory->Get( "elist" ));

   Long64_t nds(ds.numEntries());
   if (nds != cut_list->GetN()) {
      std::cout << "addSWeightToTree(): ERROR: number of entries in data set does not match number of selected entries in tree"
            << std::endl;
      assert(nds == cut_list->GetN());
   }

   std::vector<Double_t> weights(tree.GetEntries(), 0.);

   Int_t j = 0;
   for (Long64_t i = 0; i < tree.GetEntries(); ++i) {
      if (cut_list->Contains(i)) {
         ds.get(j++);
         weights[i] = ds.weight();
      }
   }
   addSWeightToTree(weights, tree, branch_name);
}

void addRunPeriodToTree(TTree& tree, Int_t period, const char* branchName) {
  TString branchNameStr(branchName);
  TBranch* branch = tree.Branch(branchNameStr, &period, branchNameStr + "/I");
  for (Long64_t it = 0; it < tree.GetEntries(); ++it) branch->Fill();
  tree.FlushBaskets();
}

void addVertexErrors(TTree* tree, const std::list<RooDataSet*>& dss, const std::string& cut) {

   cout << "Reading tree " << tree->GetName() << " to get vertex errors." << endl;

   tree->Draw(">>vxerr_elist", cut.c_str(), "entrylist");
   TEntryList *cut_list = static_cast<TEntryList*>(gDirectory->Get( "vxerr_elist" ));

   Double_t px = 0., py = 0., pz = 0.;
   Double_t m = 5366.7;
   Double_t p = 0.;
   Float_t cov_sv[3][3];
   Float_t cov_pv[3][3];
   Float_t cov_jpsi[3][3];

   Double_t pvx = 0., pvy = 0., pvz = 0.;

   tree->SetBranchAddress("B_s0_OWNPV_X", &pvx);
   tree->SetBranchAddress("B_s0_OWNPV_Y", &pvy);
   tree->SetBranchAddress("B_s0_OWNPV_Z", &pvz);

   tree->SetBranchAddress("B_s0_PX", &px);
   tree->SetBranchAddress("B_s0_PY", &py);
   tree->SetBranchAddress("B_s0_PZ", &pz);

   tree->SetBranchAddress("B_s0_P", &p);
   
   tree->SetBranchAddress("B_s0_ENDVERTEX_COV_", &cov_sv);
   tree->SetBranchAddress("B_s0_OWNPV_COV_", &cov_pv);
   tree->SetBranchAddress("J_psi_1S_ENDVERTEX_COV_", &cov_jpsi);

   TMatrixT<float> P(3, 1);
   TMatrixT<float> dx(3, 1);
   TMatrixT<float> r(1, 1);
   TMatrixT<float> tmp(3, 1);

   RooRealVar* sv_err = new RooRealVar("sv_err", "sv_err", 0, 0.1);
   RooRealVar* pv_err = new RooRealVar("pv_err", "pv_err", 0, 0.1);
   RooRealVar* psi_err = new RooRealVar("jpsi_vx_err", "jpsi_vx_err", 0, 0.1);

   RooDataSet* ds = new RooDataSet("vertex_errors", "vertex_errors", RooArgSet(*sv_err, *pv_err, *psi_err));
   const RooArgSet* obs = ds->get();
   sv_err = static_cast<RooRealVar*>(obs->find(sv_err->GetName()));
   pv_err = static_cast<RooRealVar*>(obs->find(pv_err->GetName()));
   psi_err = static_cast<RooRealVar*>(obs->find(psi_err->GetName()));

   Long64_t n = tree->GetEntries();
   for (Long64_t i = 0; i < n; ++i) {
      if (i != 0 && i != n && i % (n / 20) == 0) {
         cout << int(double(i + 20) / double(n) * 100) << "% ";
         cout.flush();
      }
      if (!cut_list->Contains(i)) {
         continue;
      } else {
         tree->GetEntry(i);
      }

      P(0, 0) = px;
      P(1, 0) = py;
      P(2, 0) = pz;
      P *= m / (p * p);
         
      TMatrixT<float> P_T(P);
      P_T.T();

      TMatrixT<float> csv(3, 3, &cov_sv[0][0]);
      TMatrixT<float> cpv(3, 3, &cov_pv[0][0]);
      TMatrixT<float> cjpsi(3, 3, &cov_jpsi[0][0]);

      tmp.Mult(csv, P);
      r.Mult(P_T, tmp);

      // Result is (c * tau)^2, set value in ps
      sv_err->setVal(sqrt(r(0, 0)) / 0.299792458);

      tmp.Mult(cpv, P);
      r.Mult(P_T, tmp);

      // Result is (c * tau)^2, set value in ps
      pv_err->setVal(sqrt(r(0, 0)) / 0.299792458);

      tmp.Mult(cjpsi, P);
      r.Mult(P_T, tmp);

      // Result is (c * tau)^2, set value in ps
      psi_err->setVal(sqrt(r(0, 0)) / 0.299792458);
      ds->fill();
   }
   cout << endl;

   cout << "Adding vertex errors to RooDataSets" << endl;
   for (list<RooDataSet*>::const_iterator it = dss.begin(), end = dss.end(); it != end; ++it) {
      (*it)->merge(ds);
   }
}

TTree* RooDataSetToTree(const RooDataSet& dataSet, const char* name,
      const char* title, const char* weightName, const char* branchList,
      Bool_t RooFitFormat)
{
  // get branch names
  std::set<TString> branches;
  TString branchStr(branchList);
  while (branchStr.Length() > 0) {
    Ssiz_t sep = branchStr.First(',');
    if (sep < 0) sep = branchStr.Length();

    TString brName(((TString)branchStr(0, sep)).Strip(TString::kBoth));
    if (brName.Length() > 0) branches.insert(brName);
    branchStr = branchStr(sep + 1, branchStr.Length() - sep - 1);
  }

  // get tree name and title
  TString treeName(name);
  TString treeTitle(title);
  if (treeName.Length() < 1) treeName = dataSet.GetName();
  if (treeTitle.Length() < 1) treeTitle = dataSet.GetTitle();

  // create tree
  TTree* tree = new TTree(treeName, treeTitle);

  // create branch in tree for event weights
  TString weightNameStr(weightName);
  Double_t* weightVarAdd = 0;
  if (weightNameStr.Length() > 0) {
    weightVarAdd = new Double_t(0.);
    tree->Branch(weightNameStr, weightVarAdd, weightNameStr + "/D");
  }

  // create branches in tree for data set columns
  std::vector<RooRealVar*>  realVars;
  std::vector<RooCategory*> categories;
  std::vector<Double_t*>    realVarAdds;
  std::vector<char*>        catLabelAdds;
  std::vector<Int_t*>       catIndexAdds;

  std::auto_ptr<TIterator> varIter(dataSet.get(0)->createIterator());
  RooAbsArg* arg = 0;
  while ((arg = static_cast<RooAbsArg*>(varIter->Next()))) {
    TString name(arg->GetName());
    if (branches.size() > 0 && branches.count(name) == 0) continue;

    RooRealVar*  realVar  = dynamic_cast<RooRealVar*>(arg);
    RooCategory* category = dynamic_cast<RooCategory*>(arg);
    if (realVar != 0) {
      // create RooRealVar branch
      Double_t* var = new Double_t(0.);
      tree->Branch(name, var, name + "/D");

      realVars.push_back(realVar);
      realVarAdds.push_back(var);
    } else if (category != 0) {
      // create RooCategory branch
      char* catLabel = new char[64];
      Int_t* catInd  = new Int_t(0);
      if (RooFitFormat) {
        tree->Branch(name + "_lbl", catLabel, name + "_lbl/C");
        tree->Branch(name + "_idx", catInd, name + "_idx/I");
        catIndexAdds.push_back(catInd);
        catLabelAdds.push_back(catLabel);
      } else {
        tree->Branch(name, catInd, name + "/I");
        catIndexAdds.push_back(catInd);
      }

      categories.push_back(category);
    }
  }

  // loop over events in data set
  for (Int_t event = 0; event < dataSet.numEntries(); ++event) {
    dataSet.get(event);

    // set weight
    if (weightVarAdd != 0) {
      *weightVarAdd = dataSet.weight();
    }

    // set RooRealVar values
    for (Int_t realVarIter = 0; realVarIter < (Int_t)realVars.size();
        ++realVarIter) {
      *realVarAdds.at(realVarIter) = realVars.at(realVarIter)->getVal();
    }

    // set RooCategory values
    for (Int_t catIter = 0; catIter < (Int_t)categories.size(); ++catIter) {
      if (RooFitFormat) {
        char* labelAdd = catLabelAdds.at(catIter);
        TString label(categories.at(catIter)->getLabel());
        Ssiz_t labelLength = label.Length();
        if (labelLength > 63) labelLength = 63;
        for (Ssiz_t charIter = 0; charIter < labelLength; ++charIter)
          labelAdd[charIter] = label(charIter);
        labelAdd[labelLength] = 0;
      }

      *catIndexAdds.at(catIter) = categories.at(catIter)->getIndex();
    }

    tree->Fill();
  }

  return tree;
}

RooDataSet* TreeToRooDataSet(TTree& tree, const RooArgSet& observables,
      const char* name, const char* title, const char* cuts,
      const char* indexName, RooDataSet* origDataSet)
{
  // get tree name and title
  TString dsName(name);
  TString dsTitle(title);
  if (dsName.Length() < 1) dsName = tree.GetName();
  if (dsTitle.Length() < 1) dsTitle = tree.GetTitle();

  if (cuts == 0 && indexName == 0 && origDataSet == 0)
    // default: import tree with RooDataSet constructor
    return new RooDataSet(dsName, dsTitle, observables, RooFit::Import(tree));

  // check number of entries in tree and original data set
  if (origDataSet != 0
      && tree.GetEntries() < (Long64_t)origDataSet->numEntries()) {
    cout << "P2VV - ERROR: TreeToRooDataSet(): less entries in tree than in original data set"
        << endl;
    return 0;
  }

  // get number of data set entries
  Long64_t numEntr = origDataSet == 0 ?
      tree.GetEntries() : (Long64_t)origDataSet->numEntries();
  if (indexName != 0 && numEntr > (Long64_t)1.e15) {
    cout << "P2VV - ERROR: TreeToRooDataSet(): number of entries with index variable limited to 10^15"
        << endl;
    return 0;
  }

  // get observables in data sets
  const RooArgSet* origObsSet = origDataSet == 0 ?
      new RooArgSet() : origDataSet->get();
  RooArgSet obsSet(observables);

  // find index variable
  RooRealVar* index(0);
  RooRealVar* origIndex(0);
  if (indexName != 0) {
    index = dynamic_cast<RooRealVar*>(obsSet.find(indexName));
    if (index == 0 && obsSet.find(indexName) != 0) {
      cout << "P2VV - ERROR: TreeToRooDataSet(): index variable is not a RooRealVar"
          << endl;
      return 0;
    }
    origIndex = dynamic_cast<RooRealVar*>(origObsSet->find(indexName));
    if (origIndex == 0 && origObsSet->find(indexName) != 0) {
      cout << "P2VV - ERROR: TreeToRooDataSet(): original index variable is not a RooRealVar"
          << endl;
      return 0;
    }
  }

  // get/set branch status
  std::vector<Bool_t> brStatusList;
  std::auto_ptr<TIterator> brIt(tree.GetListOfBranches()->MakeIterator());
  TBranch* br(0);
  while ((br = dynamic_cast<TBranch*>(brIt->Next())) != 0) {
    brStatusList.push_back(tree.GetBranchStatus(br->GetName()));
    br->SetStatus(kFALSE);
  }

  // initialize observables
  TString selStr(cuts);
  std::map<TString,Double_t*> doubleMap;
  std::map<TString,Float_t*> floatMap;
  std::map<TString,Int_t*> intMap;
  RooAbsArg* arg = 0;
  RooLinkedListIter obsSetIter = obsSet.iterator();
  while ((arg = static_cast<RooAbsArg*>(obsSetIter.Next())) != 0) {
    // check type of observables
    RooRealVar*  real = dynamic_cast<RooRealVar*>(arg);
    RooCategory* cat  = dynamic_cast<RooCategory*>(arg);
    if (real == 0 && cat == 0) {
      cout << "P2VV - WARNING: TreeToRooDataSet(): variable \""
          << arg->GetName() << "\" is not RooRealVar nor RooCategory" << endl;
      obsSet.remove(*arg);
      continue;
    }

    // check if observable is in original data set
    RooAbsArg* origArg = origObsSet->find(arg->GetName());
    if (origArg != 0) {
      if (real != 0) {
        // check if original observable is RooRealVar with correct range
        RooRealVar* origReal = dynamic_cast<RooRealVar*>(origArg);
        if (origReal == 0) {
          cout << "P2VV - WARNING: TreeToRooDataSet(): variable \""
              << real->GetName() << "\" is not a RooRealVar in original data"
              << endl;
          obsSet.remove(*arg);
        } else if (origReal->getMin() != real->getMin()
            || origReal->getMax() != real->getMax()) {
          cout << "P2VV - WARNING: TreeToRooDataSet(): variable \""
              << real->GetName()
              << "\" has not the same range in original data" << endl;
          obsSet.remove(*arg);
        }
      } else {
        // check if original observable is RooCategory with correct states
        RooCategory* origCat = dynamic_cast<RooCategory*>(origArg);
        if (origCat == 0) {
          cout << "P2VV - WARNING: TreeToRooDataSet(): variable \""
              << cat->GetName() << "\" is not a RooCategory in original data"
              << endl;
          obsSet.remove(*arg);
        } else {
          Bool_t checkStates = kTRUE;
          std::auto_ptr<TIterator> catIt(cat->typeIterator());
          RooCatType* type(0);
          while ((type = dynamic_cast<RooCatType*>(catIt->Next())) != 0) {
            if (!origCat->isValidIndex(type->getVal())) checkStates = kFALSE;
          }
          if (!checkStates) {
            cout << "P2VV - WARNING: TreeToRooDataSet(): variable \""
                << cat->GetName()
                << "\" has not the same range in original data" << endl;
            obsSet.remove(*arg);
          }
        }
      }
      continue;
    }

    // don't try to find branch if this is the index variable
    if (index && arg->GetName() == index->GetName()) continue;

    // set branch status
    UInt_t brFound = 0;
    tree.SetBranchStatus(arg->GetName(), kTRUE, &brFound);
    if (brFound != 1) {
      cout << "P2VV - WARNING: TreeToRooDataSet(): branch \""
          << arg->GetName() << "\" not found in tree" << endl;
      obsSet.remove(*arg);
      continue;
    }

    // set branch address and build selection string
    if (real != 0) {
      // set branch address of floating point branch
      TObjArray* lfList = tree.GetBranch(real->GetName())->GetListOfLeaves();
      TString brType = lfList->GetEntries() == 1 ?
          ((TLeaf*)lfList->At(0))->GetTypeName() : "";
      Int_t setAdd(0);
      if (brType == "Float_t") {
        // Float_t branch
        cout << "P2VV - INFO: TreeToRooDataSet(): values from Float_t branch \""
          << real->GetName() << "\" will be converted to double precision"
          << endl;
        Float_t* brAdd = new Float_t(0.);
        setAdd = tree.SetBranchAddress(real->GetName(), brAdd);
        if (setAdd == 0) floatMap[TString(real->GetName())] = brAdd;
        else delete brAdd;
      } else if (brType == "Double_t") {
        // Double_t branch
        Double_t* brAdd = new Double_t(0.);
        setAdd = tree.SetBranchAddress(real->GetName(), brAdd);
        if (setAdd == 0) doubleMap[TString(real->GetName())] = brAdd;
        else delete brAdd;
      }

      // check if branch address was set
      if (setAdd != 0 || (brType != "Float_t" && brType != "Double_t")) {
        cout << "P2VV - WARNING: TreeToRooDataSet(): branch \""
          << real->GetName() << "\" is not of type Double_t or Float_t"
          << endl;
        obsSet.remove(*arg);
        continue;
      }

      // build RealVar selection string
      selStr += TString("&&") + real->GetName() + ">=";
      selStr += real->getMin();
      selStr += TString("&&") + real->GetName() + "<=";
      selStr += real->getMax();
    } else if (cat != 0) {
      // set branch address to Int_t pointer
      Int_t* brAdd = new Int_t(0);
      Int_t setAdd = tree.SetBranchAddress(cat->GetName(), brAdd);
      if (setAdd == 0) {
        intMap[TString(cat->GetName())] = brAdd;
      } else {
        cout << "P2VV - WARNING: TreeToRooDataSet(): branch \""
          << cat->GetName() << "\" is not of type Int_t" << endl;
        delete brAdd;
        obsSet.remove(*arg);
        continue;
      }

      // build category selection string
      TString catSelStr("");
      std::auto_ptr<TIterator> catIt(cat->typeIterator());
      RooCatType* type(0);
      while ((type = dynamic_cast<RooCatType*>(catIt->Next())) != 0) {
        catSelStr += TString("||") + cat->GetName() + "==";
        catSelStr += type->getVal();
      }
      selStr += TString("&&(") + catSelStr(2, catSelStr.Length() - 2) + ")";
    }
  }

  // enable branches for selection
  std::vector<TBranch*> disabledBrList;
  brIt->Reset();
  while ((br = dynamic_cast<TBranch*>(brIt->Next())) != 0) {
    if (!tree.GetBranchStatus(br->GetName())
        && selStr.Contains(br->GetName())) {
      disabledBrList.push_back(br);
      br->SetStatus(kTRUE);
    }
  }

  // get selected entries
  selStr = selStr.BeginsWith("&&") ? selStr(2, selStr.Length() - 2) : selStr;
  tree.Draw(">>elist", selStr.Data(), "entrylist");
  TEntryList *entryList = static_cast<TEntryList*>(gDirectory->Get("elist"));
  cout << "P2VV - INFO: TreeToRooDataSet(): " << entryList->GetN()
      << "/" << tree.GetEntries() << " entries selected in tree";
  if (origDataSet != 0)
    cout << ", " << numEntr << " entries in data set" << endl;
  else
    cout << endl;
  if (origDataSet != 0 && ((origIndex != 0 && numEntr > entryList->GetN())
      || (origIndex == 0 && numEntr != entryList->GetN()))) {
    cout << "P2VV - ERROR: TreeToRooDataSet(): number of entries in original data set ("
        << numEntr
        << ") is not equal to number of selected entries in the tree ("
        << entryList->GetN() << ")" << endl;
    return 0;
  }

  // disable branches that were only enabled for selection
  for (std::vector<TBranch*>::iterator it = disabledBrList.begin();
      it != disabledBrList.end(); ++it)
    (*it)->SetStatus(kFALSE);
  disabledBrList.clear();

  // create data set
  RooDataSet* dataSet = new RooDataSet(dsName, dsTitle, obsSet);
  for (Long64_t it = 0; it < numEntr; ++it) {
    // get entries in tree and original data set
    if (origDataSet != 0) {
      origDataSet->get((Int_t)it);
      Long64_t entry(0);
      if (origIndex != 0) entry = (Long64_t)origIndex->getVal();
      else entry = it;
      if (!entryList->Contains(entry)) {
        cout << "P2VV - ERROR: TreeToRooDataSet(): entry " << entry
            << " in tree is not selected" << endl;
        delete dataSet;
        return 0;
      }
      tree.GetEntry(entry);
    } else {
      if (!entryList->Contains(it)) continue;
      tree.GetEntry(it);
    }

    // set values of observables in data set
    obsSetIter.Reset();
    while ((arg = static_cast<RooAbsArg*>(obsSetIter.Next())) != 0) {
      RooRealVar* real = dynamic_cast<RooRealVar*>(arg);
      RooAbsArg* origArg = origObsSet->find(arg->GetName());
      if (origArg != 0) {
        // get value from original data set
        if (real != 0) {
          real->setVal(dynamic_cast<RooRealVar*>(origArg)->getVal());
        } else {
          RooCategory* cat = dynamic_cast<RooCategory*>(arg);
          cat->setIndex(dynamic_cast<RooCategory*>(origArg)->getIndex());
        }
      } else if (index == 0 || arg->GetName() != index->GetName()) {
        // get value from tree
        if (real != 0) {
          std::map<TString,Float_t*>::iterator
              fIt(floatMap.find(arg->GetName()));
          if (fIt != floatMap.end()) real->setVal(*fIt->second);
          else real->setVal(*doubleMap[arg->GetName()]);
        } else {
          RooCategory* cat = dynamic_cast<RooCategory*>(arg);
          cat->setIndex(*intMap[arg->GetName()]);
        }
      } else if (index != 0) {
        index->setVal((Double_t)it);
      }
    }

    // add row to data set
    dataSet->add(obsSet);
  }

  // delete branch addresses
  tree.ResetBranchAddresses();
  for (std::map<TString,Double_t*>::iterator it = doubleMap.begin();
      it != doubleMap.end(); ++it)
    delete it->second;
  for (std::map<TString,Float_t*>::iterator it = floatMap.begin();
      it != floatMap.end(); ++it)
    delete it->second;
  for (std::map<TString,Int_t*>::iterator it = intMap.begin();
      it != intMap.end(); ++it)
    delete it->second;

  // delete dummy observables set
  if (origDataSet == 0) delete origObsSet;

  // reset branch status
  brIt->Reset();
  std::vector<Bool_t>::iterator brStatIt(brStatusList.begin());
  while ((br = dynamic_cast<TBranch*>(brIt->Next())) != 0) {
    br->SetStatus(*brStatIt);
    ++brStatIt;
  }

  // return data set
  return dataSet;
}
