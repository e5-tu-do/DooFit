// STL
#include <vector>
#include <sstream>
#include <utility>

// ROOT
#include "TCanvas.h"
#include "TFile.h"
#include "TStopwatch.h"
#include "TDirectory.h"
#include "TROOT.h"
#include "TObjectTable.h"
#include "TStopwatch.h"
#include "TTree.h"

// RooFit
#include "RooWorkspace.h"
#include "RooDataSet.h"
#include "RooArgSet.h"
#include "RooRealVar.h"
#include "RooCategory.h"
#include "RooPlot.h"
#include "RooGaussian.h"
#include "RooExtendPdf.h"
#include "RooAddPdf.h" 
#include "RooProdPdf.h"
#include "Roo1DTable.h"
#include "RooFitResult.h"

using namespace ROOT;
using namespace RooFit;

RooWorkspace* BuildPDF() {
  RooWorkspace* ws = new RooWorkspace("ws");
  ws->Print();
  
  ws->import(RooRealVar("mass", "mass", 5100, 5600, "MeV/c^{2}"));
  ws->import(RooRealVar("time", "time", 5100, 5600, "MeV/c^{2}"));
  
  ws->import(RooRealVar("mean1", "mean1", 5200, 5150, 5250, "MeV/c^{2}"));
  ws->import(RooRealVar("mean2", "mean2", 5300, 5250, 5350, "MeV/c^{2}"));
  ws->import(RooRealVar("mean3", "mean3", 5400, 5350, 5450, "MeV/c^{2}"));
  
  ws->import(RooRealVar("sigma1", "sigma1", 15, 5, 20));
  ws->import(RooRealVar("sigma2", "sigma2", 15, 5, 20));
  ws->import(RooRealVar("sigma3", "sigma3", 15, 5, 20));
  
  ws->import(RooRealVar("mean_time1", "mean_time1", 5200, 5150, 5250, "MeV/c^{2}"));
  ws->import(RooRealVar("mean_time2", "mean_time2", 5300, 5250, 5350, "MeV/c^{2}"));
  ws->import(RooRealVar("mean_time3", "mean_time3", 5400, 5350, 5450, "MeV/c^{2}"));
  
  ws->import(RooRealVar("sigmat1", "sigmat1", 15, 5, 20));
  ws->import(RooRealVar("sigmat2", "sigmat2", 15, 5, 20));
  ws->import(RooRealVar("sigmat3", "sigmat3", 15, 5, 20));
  
  ws->import(RooGaussian("test1", "Gaussian test pdf #1",*ws->var("mass"),*ws->var("mean1"),*ws->var("sigma1")));
  ws->import(RooGaussian("test2", "Gaussian test pdf #2",*ws->var("mass"),*ws->var("mean2"),*ws->var("sigma2")));
  ws->import(RooGaussian("test3", "Gaussian test pdf #3",*ws->var("mass"),*ws->var("mean3"),*ws->var("sigma3")));
  
  ws->import(RooGaussian("time1", "Gaussian test pdf #1 (time)",*ws->var("time"),*ws->var("mean_time1"),*ws->var("sigmat1")));
  ws->import(RooGaussian("time2", "Gaussian test pdf #2 (time)",*ws->var("time"),*ws->var("mean_time2"),*ws->var("sigmat2")));
  ws->import(RooGaussian("time3", "Gaussian test pdf #3 (time)",*ws->var("time"),*ws->var("mean_time3"),*ws->var("sigmat3")));

  RooProdPdf pdf_prod1("pdf_prod1", "pdf_prod1", RooArgList(*ws->pdf("test1"), *ws->pdf("time1")));
  RooProdPdf pdf_prod2("pdf_prod2", "pdf_prod2", RooArgList(*ws->pdf("test2"), *ws->pdf("time2")));
  RooProdPdf pdf_prod3("pdf_prod3", "pdf_prod3", RooArgList(*ws->pdf("test3"), *ws->pdf("time3")));
  
  RooRealVar yield1("yield1", "pdf yield", 10000, 0, 1000000);
  RooExtendPdf pdf_extend1("pdf_extend1", "extended pdf #1", pdf_prod1, yield1);
  
  RooRealVar yield2("yield2", "pdf yield", 50000, 0, 1000000);
  RooExtendPdf pdf_extend2("pdf_extend2", "extended pdf #2", pdf_prod2, yield2);
  
  RooRealVar yield3("yield3", "pdf yield", 5000, 0, 1000000);
  RooExtendPdf pdf_extend3("pdf_extend3", "extended pdf #3", pdf_prod3, yield3);
  
  RooRealVar coeff1("coeff1", "coeff1", 0.1, 0.0, 1.0);
  RooRealVar coeff2("coeff2", "coeff2", 0.1, 0.0, 1.0);
  RooAddPdf pdf_add("pdf_add", "added pdf", RooArgSet(pdf_extend1, pdf_extend2, pdf_extend3));
  
  ws->import(pdf_add);
  
  RooArgSet argset_obs("argset_obs");
  argset_obs.add(*ws->var("mass"));
  argset_obs.add(*ws->var("time"));

  ws->defineSet("argset_obs",argset_obs);  
  
  ws->Print("t");
  
  TFile wsfile("ws.root", "recreate");
  ws->Write("ws");
  wsfile.Close();
  
  return ws;
}

void StoreResult() {
  RooWorkspace* ws = BuildPDF();
  RooDataSet* data = ws->pdf("pdf_add")->generate(*ws->set("argset_obs"), 1000, Extended(true));
  data->Print();
  RooFitResult* fit_result = ws->pdf("pdf_add")->fitTo(*data, NumCPU(2), Extended(true), Save(true), Strategy(2), Minos(false), Hesse(false), Verbose(false),Timer(true));
  
  TFile f("testbug.root","update");
  TTree* tree_results = NULL;
  tree_results = (TTree*)f.Get("results");
  if (tree_results == NULL) {
    tree_results = new TTree("results", "Tree for toy study fit results");
    tree_results->Branch("fit_results", "RooFitResult", &fit_result, 64000, 0);
  } else {      
    tree_results->SetBranchAddress("fit_results", &fit_result);
  }
  
  tree_results->Fill();
  tree_results->Write("",TObject::kOverwrite);
  f.Close();
}

void ReadFiles() {
  TFile file("testbug.root", "read");
  TTree* tree = (TTree*)file.Get("results");
  
  TBranch* result_branch = tree->GetBranch("fit_results");
  RooFitResult* fit_result_read = NULL;
  result_branch->SetAddress(&fit_result_read);
  std::vector<RooFitResult*> fit_results_(tree->GetEntries());
  
  TStopwatch sw;
  for (int i=0; i<tree->GetEntries(); ++i) {
    result_branch->GetEntry(i);
    
    // save a copy
    sw.Reset();
    sw.Start();
    fit_results_.push_back(new RooFitResult(*fit_result_read));
    std::cout << i << std::endl;
    delete fit_result_read;
    fit_result_read = NULL;
    sw.Stop();
    sw.Print();
  }
  
  delete result_branch;
  delete tree;
}

int main(int argc, char *argv[]) {
  StoreResult();
  if (argc > 1) ReadFiles();
}