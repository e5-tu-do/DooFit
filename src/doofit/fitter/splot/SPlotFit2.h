#ifndef DOOFIT_FITTER_SPLOT_SPLOTFIT_h
#define DOOFIT_FITTER_SPLOT_SPLOTFIT_h

// from STL
#include <map>
#include <utility>

// from ROOT
#include "TString.h"

// from RooFit
#include "RooArgList.h"
#include "RooArgSet.h"

// forward declarations
class RooAbsArg;
class RooAbsPdf;
class RooDataHist;
class RooDataSet;
class RooHistPdf;
class RooKeysPdf;
class RooRealVar;

/** @class doofit::fitter::splot::SPlotFit2
 *  @brief This class allows easy and user-friendly usage of SPlots.
 *
 *  The SPlotFit2 class is intended for fast and easy usage of the SPlot
 *  technique as provided by RooFit. To use it you only need to provde the
 *  dataset, the discriminating and control variables, and the pdfs to be used
 *  in the discriminating observables.
 */
namespace doofit {
namespace fitter {
namespace splot {

class SPlotFit2{
 public:
  SPlotFit2();
  ~SPlotFit2();
  
  void Run(RooLinkedList* ext_fit_args=NULL);
  std::pair<RooHistPdf*,RooDataHist*> GetHistPdf(const TString& pdf_name, const RooArgSet& vars_set, const TString& comp_name, const TString& binningName = "");
  
  RooDataHist* GetRooDataHist( const TString& com_name, const TString& binningName );

	RooDataHist* GetRooDataHist( const TString& com_name, RooRealVar * var, const TString& binningName );
  
  RooKeysPdf& GetKeysPdf(const TString& pdf_name, RooRealVar& var, const TString& comp_name);
  
  RooDataSet* GetSwDataSet(const TString& comp_name);
  
  /// Setters and Getters
  void set_num_cpu(unsigned int num_cpu){ num_cpu_ = num_cpu; }
  void set_input_data(RooDataSet* input_data){ input_data_ = input_data; }
  void set_use_minos(bool val){use_minos_ = val;}
  
  void add_disc_var(const RooAbsArg& disc_var){ disc_vars_.add(disc_var); }
  void add_cont_var(const RooAbsArg& cont_var){ cont_vars_.add(cont_var); }
  
  void set_disc_pdfs(const std::map<TString,RooArgSet>& disc_pdfs){ disc_pdfs_ = disc_pdfs; }
  void set_cont_pdfs(const std::map<TString,RooArgSet>& cont_pdfs){ cont_pdfs_ = cont_pdfs; }
  
  RooAbsPdf* get_pdf_disc_full(){return pdf_disc_full_;}
  RooAbsPdf* get_pdf_cont(const TString& comp_name){ return cont_pdfs_prod_[comp_name];}
  const RooArgList& cont_vars() const {return cont_vars_;}
  
  bool use_minos()  const {return use_minos_;}
  
 private:
  unsigned int num_cpu_;
   
  RooDataSet* input_data_;   //< input data
  
  RooAbsPdf* pdf_disc_full_; //< full discriminating pdf
   
  RooArgList disc_vars_; //< discriminating variables
  RooArgList cont_vars_; //< control variables
  
  std::map<TString,RooArgSet> disc_pdfs_; //< maps component name (Sig,Bkg,...) to RooArgSet of component's PDFs of discriminating variables
  std::map<TString,RooArgSet> cont_pdfs_; //< maps component name (Sig,Bkg,...) to RooArgSet of PDFs to be fitted to control variable's SPlot
  
  std::map<TString,std::pair<RooRealVar*,RooAbsPdf*> > disc_pdfs_extend_; //< Extended version of ProdPdfs in disc_pdfs_
  std::map<TString,RooAbsPdf*> cont_pdfs_prod_;                           //< ProdPdfs of cont_pdfs_
  
  std::map<TString,RooDataSet*>   sweighted_data_; //< maps component name to sweighted dataset
  std::map<TString,RooDataHist*>  sweighted_hist_; //< maps component name to sweighted datahist 
  
  bool use_minos_;
};

} //namespace splot
} //namespace fitter
} //namespace doofit

#endif //DOOFIT_FITTER_SPLOT_SPLOTFIT_h