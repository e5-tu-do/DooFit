#ifndef DOOFIT_FITTER_SPLOT_SPLOTFIT_h
#define DOOFIT_FITTER_SPLOT_SPLOTFIT_h

// from STL
#include <map>
#include <vector>
#include <utility>
#include <string>

// from ROOT
#include "TString.h"

// from RooFit
#include "RooArgList.h"
#include "RooArgSet.h"

// from DooFit
#include "doofit/fitter/easyfit/EasyFit.h"

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
  /**
   *  @brief Constructor based on a given PDF and dataset
   *
   *  Based on a given PDF which needs to be extended, yields and a dataset, the 
   *  SPlotFit2 is initialized to use this PDF as discriminating PDF and all 
   *  components to generate individual sweights.
   *
   *  @param pdf the PDF to use for discrimination
   *  @param data the dataset to fit on
   *  @param yields yields for extended components to generate sweights for
   */
  SPlotFit2(RooAbsPdf& pdf, RooDataSet& data, RooArgSet yields);
  
  /**
   *  @brief Constructor based on a set of PDFs and dataset
   *
   *  Based on a given vector of PDFs and a dataset, the SPlotFit2 is 
   *  initialized. It will generate a RooAddPdf based on the supplied PDFs and 
   *  use this added PDF to discriminate and to generate individual sweights.
   *
   *  @param pdfs the PDFs to use for discrimination
   *  @param data the dataset to fit on
   *  @param yields (optional) yields for the PDFs
   */
  SPlotFit2(std::vector<RooAbsPdf*> pdfs, RooDataSet& data, std::vector<RooRealVar*> yields=std::vector<RooRealVar*>());
  
  /**
   *  @brief Constructor based on a set of PDFs
   *
   *  Based on a given vector of PDFs, the SPlotFit2 is initialized. It will 
   *  generate a RooAddPdf based on the supplied PDFs and
   *  use this added PDF to discriminate and to generate individual sweights.
   *
   *  @param pdfs the PDFs to use for discrimination
   */
  SPlotFit2(std::vector<RooAbsPdf*> pdfs);
  
  SPlotFit2();
  ~SPlotFit2();
  
  /**
   *  @brief Fit on the PDF and extract sweights
   *
   *  Fit the pdf to the data and extract sweighted datasets afterwards.
   *
   *  @param ext_fit_args (optional) external arguments to use while fitting
   */
  void Fit(RooLinkedList* ext_fit_args=NULL);
  
  /**
   *  @brief Get full discriminating PDF
   *
   *  @return the discriminating PDF
   */
  RooAbsPdf& pdf() { return *pdf_; }
  
  /**
   *  @brief Write parameters to file
   *
   *  Write parameter information (ranges etc.) to an ASCII file
   *
   *  @param filename file name to write to
   */
  void WriteParametersFile(std::string filename);
  
//  void Run(RooLinkedList* ext_fit_args=NULL);
  std::pair<RooHistPdf*,RooDataHist*> GetHistPdf(const std::string& pdf_name, const RooArgSet& vars_set, const std::string& comp_name, const std::string& binningName = "");
  
  RooDataHist* GetRooDataHist( const std::string& com_name, const std::string& binningName );

	RooDataHist* GetRooDataHist( const std::string& com_name, RooRealVar * var, const std::string& binningName );
  
  RooKeysPdf& GetKeysPdf(const std::string& pdf_name, RooRealVar& var, const std::string& comp_name);

  RooDataSet* GetSwDataSet(const std::string& comp_name = "");
  
  /**
   *  @brief Get all sweighted datasets
   *
   *  @return map of all sweighted datasets
   */
  std::map<std::string,RooDataSet*> GetSwDataSets() { return sweighted_data_map_; }
  
  /// Setters and Getters
  void set_num_cpu(unsigned int num_cpu){ num_cpu_ = num_cpu; }
  void set_input_data(RooDataSet* input_data){ input_data_ = input_data; }
  void set_use_minos(bool val){use_minos_ = val;}
  
  void add_disc_var(const RooAbsArg& disc_var){ disc_vars_.add(disc_var); }
  void add_cont_var(const RooAbsArg& cont_var){ cont_vars_.add(cont_var); }
  
  void set_disc_pdfs(const std::map<std::string,RooArgSet>& disc_pdfs){ disc_pdfs_ = disc_pdfs; }
  void set_cont_pdfs(const std::map<std::string,RooArgSet>& cont_pdfs){ cont_pdfs_ = cont_pdfs; }
  
  void set_startingvalues(TString startingvalues){ starting_values_ = startingvalues; }
  
  RooAbsPdf* get_pdf_disc_full(){return pdf_;}
  RooAbsPdf* get_pdf_cont(const std::string& comp_name){ return cont_pdfs_prod_[comp_name];}
  const RooArgList& cont_vars() const {return cont_vars_;}
  
  bool use_minos()  const {return use_minos_;}
  
  /**
   *  @brief Set EasyFit to use for fitting
   *
   *  If this EasyFit is set, it will be preferred for fitting.
   *
   *  @param efit EasyFit to use
   */
  void set_easyfitter(doofit::fitter::easyfit::EasyFit& efit) {
    easyfitter_ = &efit;
  }
  
 private:
  /**
   *  @brief Full discriminating PDF
   */
  RooAbsPdf* pdf_;
  
  /**
   *  @brief Flag whether pdf_ is owned or not
   */
  bool pdf_owned_;
  
  /**
   *  @brief Argset of yields for discrimination
   */
  RooArgList yields_;
  
  /**
   *  @brief Parameter set
   */
  RooArgSet* parameters_;
  
  unsigned int num_cpu_;
  RooDataSet* input_data_;    //< input data
  RooDataSet* sweighted_data_; //< sweighted dataset
     
  RooArgList disc_vars_; //< discriminating variables
  RooArgList cont_vars_; //< control variables
  
  std::map<std::string,RooArgSet> disc_pdfs_; //< maps component name (Sig,Bkg,...) to RooArgSet of component's PDFs of discriminating variables
  std::map<std::string,RooArgSet> cont_pdfs_; //< maps component name (Sig,Bkg,...) to RooArgSet of PDFs to be fitted to control variable's SPlot
  
  std::map<std::string,std::pair<RooRealVar*,RooAbsPdf*> > disc_pdfs_extend_; //< Extended version of ProdPdfs in disc_pdfs_
  std::map<std::string,RooAbsPdf*> cont_pdfs_prod_;                           //< ProdPdfs of cont_pdfs_
  
  std::map<std::string,RooDataSet*>   sweighted_data_map_; //< maps component name to sweighted dataset
  std::map<std::string,RooDataHist*>  sweighted_hist_map_; //< maps component name to sweighted datahist
  
  bool use_minos_;
  
  TString starting_values_;
  
  doofit::fitter::easyfit::EasyFit* easyfitter_;
};

} //namespace splot
} //namespace fitter
} //namespace doofit

#endif //DOOFIT_FITTER_SPLOT_SPLOTFIT_h
