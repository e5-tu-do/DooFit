#include "SPlotFit2.h"

// from STL
#include <string>

// from ROOT
#include "TIterator.h"

// from RooFit
#include "RooAbsArg.h"
#include "RooCategory.h"
#include "RooCmdArg.h"
#include "RooDataHist.h"
#include "RooDataSet.h"
#include "RooLinkedListIter.h"
#include "RooRealVar.h"
#include "RooLinkedListIter.h"
#include "RooArgList.h"

// from RooFit PDFs
#include "RooAbsPdf.h"
#include "RooAddPdf.h"
#include "RooExtendPdf.h"
#include "RooHistPdf.h"
#include "RooKeysPdf.h"
#include "RooProdPdf.h"

// from RooStats
#include "RooStats/SPlot.h"

// from DooCore
#include <doocore/io/MsgStream.h>

// from Project

using std::cout;
using std::endl;
using std::map;
using std::pair;

using namespace RooFit;

using namespace doocore::io;

namespace doofit {
namespace fitter {
namespace splot {
  
SPlotFit2::SPlotFit2() :
  pdf_(NULL),
  pdf_owned_(false),
  num_cpu_(4),
  input_data_(),
  pdf_disc_full_(NULL),
  disc_vars_(),
  cont_vars_(),
  disc_pdfs_(),
  cont_pdfs_(),
  disc_pdfs_extend_(),
  sweighted_data_(),
  sweighted_hist_(),
  use_minos_(true)
{
  
}
  
SPlotFit2::SPlotFit2(RooAbsPdf& pdf, RooDataSet& data, RooArgSet yields) :
  pdf_(&pdf),
  pdf_owned_(false),
  yields_(yields),
  num_cpu_(4),
  input_data_(&data),
  pdf_disc_full_(NULL),
  disc_vars_(),
  cont_vars_(),
  disc_pdfs_(),
  cont_pdfs_(),
  disc_pdfs_extend_(),
  sweighted_data_(),
  sweighted_hist_(),
  use_minos_(true)
{
  pdf_->Print("v");
  yields_.Print();
  input_data_->Print();
}

SPlotFit2::SPlotFit2(std::vector<RooAbsPdf*> pdfs, RooDataSet& data) :
  pdf_(NULL),
  pdf_owned_(true),
  yields_(),
  num_cpu_(4),
  input_data_(&data),
  pdf_disc_full_(NULL),
  disc_vars_(),
  cont_vars_(),
  disc_pdfs_(),
  cont_pdfs_(),
  disc_pdfs_extend_(),
  sweighted_data_(),
  sweighted_hist_(),
  use_minos_(true)
{
  RooArgList pdfs_list;
  for (std::vector<RooAbsPdf*>::const_iterator it = pdfs.begin();
       it != pdfs.end(); ++it) {
    pdfs_list.add(**it);
    RooRealVar* var = new RooRealVar(TString()+(*it)->GetName()+"_yield",
                                     TString()+(*it)->GetName()+"_yield",
                                     input_data_->sumEntries()/pdfs.size(), 0.0,
                                     input_data_->sumEntries()*10);
    yields_.addOwned(*var);
  }
  pdf_ = new RooAddPdf("pdf_splotfit2", "pdf_splotfit2", pdfs_list, yields_);
}

  
SPlotFit2::~SPlotFit2(){
  if (pdf_owned_ && pdf_ != NULL) delete pdf_;
}

void SPlotFit2::Fit(RooLinkedList* ext_fit_args) {
  //=========================================================================
  // merge our and external fitting arguments
  RooLinkedList fitting_args;
  fitting_args.Add((TObject*)(new RooCmdArg(NumCPU(num_cpu_))));
  fitting_args.Add((TObject*)(new RooCmdArg(Extended())));
  fitting_args.Add((TObject*)(new RooCmdArg(Minos(use_minos_))));
  fitting_args.Add((TObject*)(new RooCmdArg(Strategy(2))));
  fitting_args.Add((TObject*)(new RooCmdArg(Save(true))));
  fitting_args.Add((TObject*)(new RooCmdArg(Verbose(false))));
  fitting_args.Add((TObject*)(new RooCmdArg(Timer(true))));
  fitting_args.Add((TObject*)(new RooCmdArg(Minimizer("Minuit2","minimize"))));
  if (ext_fit_args != NULL) {
    RooLinkedListIter it = ext_fit_args->iterator();
    TObject* arg = NULL;
    
    while ((arg = it.Next())) {
      arg->Print();
      fitting_args.Add(arg);
    }
  }

  //=========================================================================
  // fit discriminating pdf
  pdf_->fitTo(*input_data_, fitting_args);
  
  //=========================================================================
  // create sPlot
  // get all parameters
  RooArgSet* par_disc_set = pdf_->getParameters(*input_data_);
  
  // remove yields from parameter set
  par_disc_set->remove(yields_);
  
  // Helper pointers
  RooRealVar* var_iter1 = NULL;
  
  // iterate over left over parameters of full disc pdf and set constant
  RooLinkedListIter* par_disc_set_iterator = (RooLinkedListIter*)par_disc_set->createIterator();
  while (var_iter1=(RooRealVar*)par_disc_set_iterator->Next()) {
    var_iter1->setConstant();
  }
  delete par_disc_set_iterator;
  
  // create datasets
  RooStats::SPlot *sData = new RooStats::SPlot("sData","SPlot",*input_data_,pdf_,yields_);

  //=========================================================================
  // create sweighted datasets
  
  // iterate over yields
  RooLinkedListIter* yield_iterator = (RooLinkedListIter*)yields_.createIterator();
  while (var_iter1=(RooRealVar*)yield_iterator->Next()) {
    std::string comp_name = var_iter1->GetName();
    
    sinfo << "SPlotFit2: Adding sweighted dataset with name " << comp_name << endmsg;
    sweighted_data_[comp_name] = new RooDataSet(input_data_->GetName(),input_data_->GetTitle(),input_data_,*input_data_->get(),0,TString("")+var_iter1->GetName()+"_sw");
  }
  delete yield_iterator;
}
  
void SPlotFit2::Run(RooLinkedList* ext_fit_args){

  // merge our and external fitting arguments
  RooLinkedList fitting_args;
  fitting_args.Add((TObject*)(new RooCmdArg(NumCPU(num_cpu_))));
  fitting_args.Add((TObject*)(new RooCmdArg(Extended())));
  fitting_args.Add((TObject*)(new RooCmdArg(Minos(use_minos_))));
  fitting_args.Add((TObject*)(new RooCmdArg(Strategy(2))));
  fitting_args.Add((TObject*)(new RooCmdArg(Save(true))));
  fitting_args.Add((TObject*)(new RooCmdArg(Verbose(false))));
  fitting_args.Add((TObject*)(new RooCmdArg(Timer(true))));
  if (ext_fit_args != NULL) {
    RooLinkedListIter it = ext_fit_args->iterator();
    TObject* arg = NULL;
    
    while ((arg = it.Next())) {
      arg->Print();
      fitting_args.Add(arg);
    }
  }
  
  // Make estimtes on yields (for starting values that are not complete nonsense)
  double yield_start  = input_data_->sumEntries()/disc_pdfs_.size();
  double yield_max    = 1200*yield_start;
  
  //=========================================================================
  // create discriminating pdfs and fit
  
  // prepare iterator
  map<std::string,RooArgSet>::const_iterator disc_pdfs_it;
  
  // iterate over disc_pdfs_ map and create ProdPdfs, yields, and extended PDFs
  RooArgList pdfs_extend;
  
  for (disc_pdfs_it = disc_pdfs_.begin(); disc_pdfs_it != disc_pdfs_.end(); ++disc_pdfs_it){
    std::string   comp_name = (*disc_pdfs_it).first;
    RooArgSet     pdfs_set  = (*disc_pdfs_it).second;
    RooRealVar*   yield = new RooRealVar(TString(comp_name)+"Yield",TString("N_{")+comp_name+"}",yield_start,0,yield_max);
    RooProdPdf*   pdf_prod = new RooProdPdf(TString(comp_name)+"ProdPdf",TString("Product Disc Pdf of ")+comp_name+" Component",pdfs_set);
    RooExtendPdf* pdf_extend = new RooExtendPdf(TString(comp_name)+"ExtPdf",TString("Extended Disc Pdf of ")+comp_name+" Component",*pdf_prod,*yield);
    
    disc_pdfs_extend_[comp_name] = std::pair<RooRealVar*,RooAbsPdf*>(yield,pdf_extend);
    //pdfs_set.Print();
    //pdf_extend->Print("t");
    pdfs_extend.add(*pdf_extend);
  }
  
  // create full discriminating pdf by RooAddPdf of component pdfs
  pdf_disc_full_ = new RooAddPdf("pdfDiscFull","Full discriminating Pdf",pdfs_extend);
    
	//Introduced by Tobi for the possibility of giving the fit sane satrting values for hadronic background event yields.
	//Should not interfer with anything
	pdf_disc_full_->getParameters(input_data_)->writeToFile("SPlotParams.new"); 
	pdf_disc_full_->getParameters(input_data_)->readFromFile("SPlotParams.txt");
    
  // fit discriminating pdf
  pdf_disc_full_->fitTo(*input_data_, fitting_args);
  
	//Introduced by Tobi ... ^^
	pdf_disc_full_->getParameters(input_data_)->writeToFile("SPlotParams_result.txt"); 
  
  //=========================================================================
  // create sweighted datasets
  
  // set all parameters of discriminating pdf constant except for yields
  
  // get all parameters
  RooArgSet* par_disc_set = pdf_disc_full_->getParameters(*input_data_);
  
  // get yields
  RooArgSet disc_pdfs_yields;
  map<std::string,std::pair<RooRealVar*,RooAbsPdf*> >::const_iterator disc_pdfs_extend_it;
  for (disc_pdfs_extend_it = disc_pdfs_extend_.begin(); disc_pdfs_extend_it != disc_pdfs_extend_.end(); ++disc_pdfs_extend_it){
    disc_pdfs_yields.add(*((*disc_pdfs_extend_it).second.first));
  }
  
  // remove yields from parameter set
  par_disc_set->remove(disc_pdfs_yields);
  
  // Helper pointers
  RooRealVar* var_iter1 = NULL;
  RooRealVar* var_iter2 = NULL;
  
  // iterate over left over parameters of full disc pdf and set constant
  RooLinkedListIter* par_disc_set_iterator = (RooLinkedListIter*)par_disc_set->createIterator();
  while (var_iter1=(RooRealVar*)par_disc_set_iterator->Next()) {
    var_iter1->setConstant();
  }
  
  // create datasets
  RooStats::SPlot *sData = new RooStats::SPlot("sData","SPlot",*input_data_,pdf_disc_full_,disc_pdfs_yields);
  
  //=========================================================================
  // create sweighted datasets
  for (disc_pdfs_extend_it = disc_pdfs_extend_.begin(); disc_pdfs_extend_it != disc_pdfs_extend_.end(); ++disc_pdfs_extend_it){
    std::string comp_name = (*disc_pdfs_extend_it).first;
    sweighted_data_[comp_name] = new RooDataSet(input_data_->GetName(),input_data_->GetTitle(),input_data_,*input_data_->get(),0,TString("")+(*disc_pdfs_extend_it).second.first->GetName()+"_sw");
  }
  
  // create sweighted datahists
  pdf_disc_full_->fitTo(*input_data_, fitting_args);
  
  for (disc_pdfs_extend_it = disc_pdfs_extend_.begin(); disc_pdfs_extend_it != disc_pdfs_extend_.end(); ++disc_pdfs_extend_it){
    std::string comp_name = (*disc_pdfs_extend_it).first;
    sweighted_hist_[comp_name] = new RooDataHist(TString("sDataHist")+comp_name,TString("sDataHist")+comp_name,cont_vars_,*(sweighted_data_[comp_name]));
  }
  
  //=========================================================================
  // Fit
  // prepare iterator
  map<std::string,RooArgSet>::const_iterator cont_pdfs_it;
  for (cont_pdfs_it = cont_pdfs_.begin(); cont_pdfs_it != cont_pdfs_.end(); ++cont_pdfs_it){
    std::string comp_name = (*cont_pdfs_it).first;
    RooArgSet   pdfs_set  = (*cont_pdfs_it).second;
    
    std::string prod_pdf_name = comp_name+"DiscProdPdf";
    RooProdPdf* pdf_fit = new RooProdPdf(prod_pdf_name.c_str(), TString("Product Disc Pdf of ")+comp_name+" Component",pdfs_set);
    cont_pdfs_prod_[comp_name] = pdf_fit;
    RooDataHist* fit_data = sweighted_hist_[comp_name];
    fit_data->Print();
    
    fitting_args.Add((TObject*)(new RooCmdArg(SumW2Error(true))));
    pdf_fit->fitTo(*fit_data, fitting_args);
  }
}

  std::pair<RooHistPdf*,RooDataHist*> SPlotFit2::GetHistPdf(const std::string& pdf_name, const RooArgSet& vars_set, const std::string& comp_name, const std::string& binningName){
  RooDataHist* data_hist = new RooDataHist(TString("sDataHist")+comp_name,TString("sDataHist")+comp_name,cont_vars_, TString(binningName));
  data_hist->add(*(sweighted_data_[comp_name]));
  RooHistPdf*  pdf_hist  = new RooHistPdf(pdf_name.c_str(),pdf_name.c_str(),vars_set,*data_hist);
  
  return std::pair<RooHistPdf*,RooDataHist*>(pdf_hist,data_hist);
}

RooDataHist* SPlotFit2::GetRooDataHist( const std::string& comp_name, const std::string& binningName ){
  RooDataHist* data_hist = new RooDataHist(TString("sDataHist")+comp_name,TString("sDataHist")+comp_name,cont_vars_, TString(binningName));
  data_hist->add(*(sweighted_data_[comp_name]));
  return data_hist;
}

RooDataHist* SPlotFit2::GetRooDataHist( const std::string& comp_name, RooRealVar * var, const std::string& binningName ){
  RooDataHist* data_hist = new RooDataHist(TString("sDataHist")+comp_name+var->GetName(),TString("sDataHist")+comp_name+var->GetName(),RooArgList(*var), TString(binningName));
  data_hist->add(*(sweighted_data_[comp_name]));
  return data_hist;
}

RooKeysPdf& SPlotFit2::GetKeysPdf(const std::string& pdf_name, RooRealVar& var, const std::string& comp_name){
  RooKeysPdf* pdf_keys = new RooKeysPdf(pdf_name.c_str(),pdf_name.c_str(),var,*sweighted_data_[comp_name]);
  return *pdf_keys;
}

RooDataSet* SPlotFit2::GetSwDataSet(const std::string& comp_name){
  return sweighted_data_[comp_name];
}

} //namespace splot
} //namespace fitter
} //namespace doofit
