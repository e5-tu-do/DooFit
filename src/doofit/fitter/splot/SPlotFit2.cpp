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
#include "RooArgList.h"
#include "RooFitResult.h"

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
#include "doofit/fitter/easyfit/EasyFit.h"

using std::cout;
using std::endl;
using std::map;
using std::pair;

using namespace RooFit;

using namespace doocore::io;

namespace doofit {
namespace fitter {
namespace splot {
  
SPlotFit2::SPlotFit2(doofit::fitter::easyfit::EasyFit& easyfit, RooDataSet& data, RooArgSet yields) :
  pdf_(NULL),
  pdf_owned_(false),
  yields_(yields),
  parameters_(NULL),
  num_cpu_(1),
  input_data_(&data),
  disc_vars_(),
  cont_vars_(),
  disc_pdfs_(),
  cont_pdfs_(),
  disc_pdfs_extend_(),
  sweighted_data_map_(),
  sweighted_hist_map_(),
  use_minos_(true),
  easyfitter_(&easyfit)
{
  
}

  
SPlotFit2::SPlotFit2() :
  pdf_(NULL),
  pdf_owned_(false),
  yields_(NULL),
  parameters_(NULL),
  num_cpu_(4),
  input_data_(),
  disc_vars_(),
  cont_vars_(),
  disc_pdfs_(),
  cont_pdfs_(),
  disc_pdfs_extend_(),
  sweighted_data_map_(),
  sweighted_hist_map_(),
  use_minos_(true),
  easyfitter_(NULL),
  starting_values_("")
{
  swarn << "SPlotFit2::SPlotFit2(): Using deprecated constructor without EasyFit. This functionality will go away in a future version. Please switch to EasyFit ASAP!" << endmsg;
}
  
SPlotFit2::SPlotFit2(RooAbsPdf& pdf, RooDataSet& data, RooArgSet yields) :
  pdf_(&pdf),
  pdf_owned_(false),
  yields_(yields),
  parameters_(NULL),
  num_cpu_(4),
  input_data_(&data),
  disc_vars_(),
  cont_vars_(),
  disc_pdfs_(),
  cont_pdfs_(),
  disc_pdfs_extend_(),
  sweighted_data_map_(),
  sweighted_hist_map_(),
  use_minos_(true),
  easyfitter_(NULL),
  starting_values_("")
{
  swarn << "SPlotFit2::SPlotFit2(RooAbsPdf&, RooDataSet&, RooArgSet): Using deprecated constructor without EasyFit. This functionality will go away in a future version. Please switch to EasyFit ASAP!" << endmsg;
}

SPlotFit2::SPlotFit2(std::vector<RooAbsPdf*> pdfs, RooDataSet& data, std::vector<RooRealVar*> yields) :
  pdf_(NULL),
  pdf_owned_(true),
  yields_(),
  parameters_(NULL),
  num_cpu_(4),
  input_data_(&data),
  disc_vars_(),
  cont_vars_(),
  disc_pdfs_(),
  cont_pdfs_(),
  disc_pdfs_extend_(),
  sweighted_data_map_(),
  sweighted_hist_map_(),
  use_minos_(true),
  easyfitter_(NULL),
  starting_values_("")
{
  swarn << "SPlotFit2::SPlotFit2(std::vector<RooAbsPdf*>, RooDataSet&, std::vector<RooRealVar*>): Using deprecated constructor without EasyFit. This functionality will go away in a future version. Please switch to EasyFit ASAP!" << endmsg;

  RooArgList pdfs_list;
  
  if (yields.size() != pdfs.size()) {
    if (yields.size() > 0) {
      serr << "SPlotFit2::SPlotFit2(std::vector<RooAbsPdf*>, RooDataSet&, std::vector<RooRealVar*>): yields are set, but number of yields does not match number of PDFs. Will ignore yields." << endmsg;
    }
    
    for (std::vector<RooAbsPdf*>::const_iterator it = pdfs.begin();
         it != pdfs.end(); ++it) {
      pdfs_list.add(**it);
      RooRealVar* var = new RooRealVar(TString()+(*it)->GetName()+"_yield",
                                       TString()+(*it)->GetName()+"_yield",
                                       input_data_->sumEntries()/pdfs.size(), 0.0,
                                       input_data_->sumEntries()*10);
      yields_.addOwned(*var);
    }
  } else {
    for (std::vector<RooAbsPdf*>::const_iterator it = pdfs.begin();
         it != pdfs.end(); ++it) {
      pdfs_list.add(**it);
    }
    for (std::vector<RooRealVar*>::const_iterator it = yields.begin();
         it != yields.end(); ++it) {
      yields_.add(**it);
    }
  }
  pdf_ = new RooAddPdf("pdf_splotfit2", "pdf_splotfit2", pdfs_list, yields_);
//  pdf_->Print("v");
}

SPlotFit2::SPlotFit2(std::vector<RooAbsPdf*> pdfs) :
  pdf_(NULL),
  pdf_owned_(true),
  yields_(),
  parameters_(NULL),
  num_cpu_(4),
  input_data_(NULL),
  disc_vars_(),
  cont_vars_(),
  disc_pdfs_(),
  cont_pdfs_(),
  disc_pdfs_extend_(),
  sweighted_data_map_(),
  sweighted_hist_map_(),
  use_minos_(true),
  easyfitter_(NULL)
{
  swarn << "SPlotFit2::SPlotFit2(std::vector<RooAbsPdf*>): Using deprecated constructor without EasyFit. This functionality will go away in a future version. Please switch to EasyFit ASAP!" << endmsg;
  
  RooArgList pdfs_list;
  for (std::vector<RooAbsPdf*>::const_iterator it = pdfs.begin();
       it != pdfs.end(); ++it) {
    pdfs_list.add(**it);
    RooRealVar* var = new RooRealVar(TString()+(*it)->GetName()+"_yield",
                                     TString()+(*it)->GetName()+"_yield",
                                     1e7, 0.0, 1e10);
    yields_.addOwned(*var);
  }
  pdf_ = new RooAddPdf("pdf_splotfit2", "pdf_splotfit2", pdfs_list, yields_);
}

  
SPlotFit2::~SPlotFit2(){
  if (pdf_owned_ && pdf_ != NULL) delete pdf_;
  if (parameters_ != NULL) delete parameters_;
}

void SPlotFit2::Fit(RooLinkedList* ext_fit_args) {
  //=========================================================================
  // merge our and external fitting arguments
  
  if (easyfitter_ != NULL) {
    sinfo << "SPlotFit2::Fit(): Using EasyFit for fitting!" << endmsg;
    
    easyfitter_->SetSave(true);
    
    easyfitter_->Fit();
    
    const RooFitResult* fit_result = easyfitter_->GetFitResult();
    parameters_ = easyfitter_->ParameterArgSet();

    if (fit_result != NULL) {
      fit_result->Print("v");
    } else {
      serr << "Error in SPlotFit2::Fit(RooLinkedList*): Fit result not available." << endmsg;
    }
  } else {
    swarn << "SPlotFit2::Fit(): Not using EasyFit for fitting! This will be deprecated in the near future." << endmsg;
    swarn << "SPlotFit2::Fit(): Seriously, switch to EasyFit ASAP! It's much good and beautiful." << endmsg;
    
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
        //arg->Print();
        fitting_args.Add(arg);
      }
    }
    
    //=========================================================================
    pdf_->getParameters(*input_data_)->readFromFile(starting_values_);
    // fit discriminating pdf
    RooFitResult* fit_result = pdf_->fitTo(*input_data_, fitting_args);
    fit_result->Print("v");
    delete fit_result;

    parameters_ = pdf().getParameters(*input_data_);
  }
  
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
  sweighted_data_ = sData->GetSDataSet();

  // iterate over yields
  RooLinkedListIter* yield_iterator = (RooLinkedListIter*)yields_.createIterator();
  while (var_iter1=(RooRealVar*)yield_iterator->Next()) {
    std::string comp_name = var_iter1->GetName();
    
    sinfo << "SPlotFit2: Adding sweighted dataset with name " << comp_name << endmsg;
    sweighted_data_map_[comp_name] = new RooDataSet(input_data_->GetName(),input_data_->GetTitle(),input_data_,*input_data_->get(),0,TString("")+var_iter1->GetName()+"_sw");
  }
  delete yield_iterator;
}

std::pair<RooHistPdf*,RooDataHist*> SPlotFit2::GetHistPdf(const std::string& pdf_name, const RooArgSet& vars_set, const std::string& comp_name, const std::string& binningName){
  RooDataHist* data_hist = new RooDataHist(TString("sDataHist")+comp_name,TString("sDataHist")+comp_name,cont_vars_, TString(binningName));
  data_hist->add(*(sweighted_data_map_[comp_name]));
  RooHistPdf*  pdf_hist  = new RooHistPdf(pdf_name.c_str(),pdf_name.c_str(),vars_set,*data_hist);
  
  return std::pair<RooHistPdf*,RooDataHist*>(pdf_hist,data_hist);
}

RooDataHist* SPlotFit2::GetRooDataHist( const std::string& comp_name, const std::string& binningName ){
  RooDataHist* data_hist = new RooDataHist(TString("sDataHist")+comp_name,TString("sDataHist")+comp_name,cont_vars_, TString(binningName));
  data_hist->add(*(sweighted_data_map_[comp_name]));
  return data_hist;
}

RooDataHist* SPlotFit2::GetRooDataHist( const std::string& comp_name, RooRealVar * var, const std::string& binningName ){
  RooDataHist* data_hist = new RooDataHist(TString("sDataHist")+comp_name+var->GetName(),TString("sDataHist")+comp_name+var->GetName(),RooArgList(*var), TString(binningName));
  data_hist->add(*(sweighted_data_map_[comp_name]));
  return data_hist;
}

RooKeysPdf& SPlotFit2::GetKeysPdf(const std::string& pdf_name, RooRealVar& var, const std::string& comp_name){
  RooKeysPdf* pdf_keys = new RooKeysPdf(pdf_name.c_str(),pdf_name.c_str(),var,*sweighted_data_map_[comp_name]);
  return *pdf_keys;
}

RooDataSet* SPlotFit2::GetSwDataSet(const std::string& comp_name){
  if (comp_name==""){
    return sweighted_data_;
  }
  else{
    return sweighted_data_map_[comp_name];
  }
}

void SPlotFit2::WriteParametersFile(std::string filename) {
  parameters_->writeToFile(filename.c_str());
}

} //namespace splot
} //namespace fitter
} //namespace doofit
