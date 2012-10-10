#include "doofit/Toy/ToyFactoryStd/ToyFactoryStd.h"

// STL
#include <cstring>
#include <vector>

// POSIX/UNIX
#include <unistd.h>

// boost
#include "boost/tuple/tuple.hpp"
#include "boost/filesystem.hpp"
#include "boost/math/special_functions/round.hpp"

// ROOT
#include "TClass.h"
#include "TStopwatch.h"
#include "TFile.h"

// from RooFit
#include "RooDataSet.h"
#include "RooAbsPdf.h"
#include "RooAddPdf.h"
#include "RooProdPdf.h"
#include "RooRandom.h"
#include "RooArgList.h"
#include "RooArgSet.h"
#include "RooCmdArg.h"
#include "RooTable.h"
#include "Roo1DTable.h"
#include "RooRealVar.h"
#include "RooCategory.h"
#include "RooEffProd.h"
#include "RooSimultaneous.h"
#include "RooSuperCategory.h"
#include "RooLinkedListIter.h"

// from Project
#include "doofit/Config/CommonConfig.h"
#include "doofit/Toy/ToyFactoryStd/ToyFactoryStdConfig.h"
#include "doocore/io/MsgStream.h"

using namespace ROOT;
using namespace RooFit;
using namespace doocore::lutils; using namespace doocore::io;

namespace doofit {
namespace Toy {
  ToyFactoryStd::ToyFactoryStd(const Config::CommonConfig& cfg_com, const ToyFactoryStdConfig& cfg_tfac) :
  config_common_(cfg_com),
  config_toyfactory_(cfg_tfac)
  {
	RooRandom::randomGenerator()->SetSeed(cfg_tfac.random_seed());
  }
  
  ToyFactoryStd::~ToyFactoryStd(){
    
  }
  
  RooDataSet* ToyFactoryStd::Generate() {
    sinfo.Ruler();
    TStopwatch sw;
    sw.Start();
    
    ReadParametersFromFile();
    DrawConstrainedParameters();
        
    // try to generate with PDF (if set)
    RooDataSet* data = NULL;
    try {
      data = GenerateForPdf(*(config_toyfactory_.generation_pdf()), *(config_toyfactory_.argset_generation_observables()), config_toyfactory_.expected_yield(), !config_toyfactory_.dataset_size_fixed());
    } catch (const PdfNotSetException& e) {
      if (config_toyfactory_.discrete_probabilities().size() == 0) {
        // could not generate continous sample via PDF, nor is a discrete sample requested
        serr << "Not generating any data as neither PDF if set nor discrete variables are requested." << endmsg;
        throw NotGeneratingDataException();
      }
    }
      
    const std::vector<Config::DiscreteProbabilityDistribution>& discrete_probabilities = config_toyfactory_.discrete_probabilities();
    
    if (discrete_probabilities.size() > 0) {
      // determine yield and already generated argset for discrete dataset
      double discrete_yield = 0;
      const RooArgSet* argset_already_generated = NULL;
      if (data != NULL) {
        // continous PDF was generated, take args and yield from there
        discrete_yield = data->numEntries();
        argset_already_generated = data->get();
      } else {
        // no PDF set, get a Poisson distributed yield (if necessary) and empty 
        // argset for discrete variables.
        if (config_toyfactory_.dataset_size_fixed()) {
          discrete_yield = config_toyfactory_.expected_yield();
        } else {
          discrete_yield = RooRandom::randomGenerator()->Poisson(config_toyfactory_.expected_yield());
        }
        
        argset_already_generated = new RooArgSet();
      }
      
      RooDataSet* data_discrete = GenerateDiscreteSample(discrete_probabilities, *(config_toyfactory_.argset_generation_observables()), *argset_already_generated, discrete_yield);
      
      // merge if necessary
      if (data != NULL) {
        MergeDatasets(data, data_discrete);
      } else {
        data = data_discrete;
      }
      
      if (argset_already_generated != NULL) delete argset_already_generated;
    }
    
    sinfo << "Generation of this sample took " << sw << endmsg;
    
    if (config_toyfactory_.dataset_filename_name().first().length() > 0) {
      sinfo << "Writing dataset as " << config_toyfactory_.dataset_filename_name().second() << " into file " << config_toyfactory_.dataset_filename_name().first() << endmsg;
      TFile dataset_file(config_toyfactory_.dataset_filename_name().first().c_str(),"recreate");
      
      data->Write(config_toyfactory_.dataset_filename_name().second().c_str());
      dataset_file.Close();
    }
    
    if (config_toyfactory_.parameter_save_file().size() > 0) {
      sinfo << "Saving parameters to " << config_toyfactory_.parameter_save_file() << endmsg;
      RooArgSet* argset_parameters = config_toyfactory_.generation_pdf()->getParameters(config_toyfactory_.argset_generation_observables());
      argset_parameters->writeToFile(config_toyfactory_.parameter_save_file().c_str());
      delete argset_parameters;
    }
    
    //ReadParametersFromFile();
    
    sinfo.Ruler();
    
    return data;
  }
  
  bool ToyFactoryStd::PdfIsDecomposable(const RooAbsPdf& pdf) const {
    if (PdfIsSimultaneous(pdf) 
        || PdfIsAdded(pdf)
        || PdfIsProduct(pdf)
        || PdfIsExtended(pdf)) {
      return true;
    } else {
      return false;
    }
  }
  
  void ToyFactoryStd::ReadParametersFromFile() {
    if (config_toyfactory_.parameter_read_file().size() > 0) {
      if (!boost::filesystem::exists(config_toyfactory_.parameter_read_file())) {
        serr << "Cannot read parameters from file " << config_toyfactory_.parameter_read_file() << endmsg;
        throw NotGeneratingDataException();
      }
      sinfo << "Reading parameters from " << config_toyfactory_.parameter_read_file() << endmsg;
      try {
        RooArgSet* argset_parameters = config_toyfactory_.generation_pdf()->getParameters(config_toyfactory_.argset_generation_observables());
        argset_parameters->readFromFile(config_toyfactory_.parameter_read_file().c_str());
        delete argset_parameters;
      } catch (const PdfNotSetException& e) {
        if (config_toyfactory_.discrete_probabilities().size() == 0) {
          // could not generate continous sample via PDF, nor is a discrete sample requested
          serr << "Not setting any parameters as neither PDF if set nor discrete variables are requested." << endmsg;
          throw NotGeneratingDataException();
        }
      } catch (const ArgSetNotSetException& e) {
        serr << "Cannot set parameters as observables argument set is not set." << endmsg;
        throw e;
      }  
    }
  }
  
  void ToyFactoryStd::DrawConstrainedParameters() {
    const RooArgSet* argset_constraining_pdfs = config_toyfactory_.argset_constraining_pdfs();
    if (argset_constraining_pdfs) {
      sinfo << "Drawing values of constrained parameters." << endmsg;
      sinfo.set_indent(sinfo.indent()+2);
      
      TIterator* arg_it = argset_constraining_pdfs->createIterator();
      RooAbsArg* arg = NULL;
      RooArgSet* parameters = config_toyfactory_.generation_pdf()->getParameters(config_toyfactory_.argset_generation_observables());

      while ((arg = (RooAbsArg*)arg_it->Next())) {
        RooAbsPdf* constr_pdf    = dynamic_cast<RooAbsPdf*>(arg);
        RooArgSet* constr_params = constr_pdf->getObservables(parameters);
        
        sinfo << "Drawing for " << *constr_params << " with PDF " << constr_pdf->GetName() << endmsg;
        
        if (config_toyfactory_.parameter_read_file().size() > 0) {
          if (!boost::filesystem::exists(config_toyfactory_.parameter_read_file())) {
            serr << "Cannot read parameters from file " << config_toyfactory_.parameter_read_file() << endmsg;
            throw NotGeneratingDataException();
          }
          RooArgSet* argset_parameters = constr_pdf->getParameters(constr_params);
          argset_parameters->readFromFile(config_toyfactory_.parameter_read_file().c_str());
          delete argset_parameters;
        }
        RooDataSet * constr_data = constr_pdf->generate(*constr_params, 1);
    
        TIterator* par_it = constr_data->get(0)->createIterator();
        RooAbsArg* par = NULL;
        while ((par = (RooAbsArg*)par_it->Next())) {
          RooRealVar* par_real = dynamic_cast<RooRealVar*>(par);
          dynamic_cast<RooRealVar*>(parameters->find(par_real->GetName()))->setVal(par_real->getVal());
        }
        delete par_it;
        
        delete constr_params;
        delete constr_data;
      }
      
      delete parameters;
      delete arg_it;
      sinfo.set_indent(sinfo.indent()-2);
    }
  }
  
  void ToyFactoryStd::MergeDatasets(RooDataSet* master_dataset, RooDataSet* slave_dataset, const RooArgSet* ignore_argset, bool delete_slave) const {
    // sanity check
    const RooArgSet& master_argset = *master_dataset->get();
    const RooArgSet& slave_argset  = *slave_dataset->get();
    
    if (master_dataset->numEntries() != slave_dataset->numEntries()) {
      serr << "Attempting two merge two datasets without equal size. Unable to cope with that. Giving up." << endmsg;
      throw DatasetsNotDisjointException();
    }
    
    if (master_dataset->get()->overlaps(*slave_dataset->get())) {
      // there is an overlap in both datasets, if the overlap is in columns 
      // which are also in ignore_argset this is no problem; need to check for 
      // that.
      // First assume datasets are not mergable.
      bool not_mergable = true;
      
      if (ignore_argset != NULL) {
        // there is an ignore argset, might be mergable
        not_mergable = false;
        // Happy fun time using TIterator, yay!
        TIterator* iter   = slave_argset.createIterator();
        RooAbsArg* column = NULL;
        while ((column = (RooAbsArg*)iter->Next())) {
          // If any overlaping column is not in ignore_argset, datasets are not
          // mergable.
          if (master_argset.contains(*column) && !(ignore_argset->contains(*column))) {
            not_mergable = true;
          }
        }
        delete iter;
      }
      
      if (not_mergable) {
        serr << "Attempting two merge two non-disjoint datasets. Unable to cope with that. Giving up." << endmsg;
        serr << "Cannot merge " << master_argset << " with " << slave_argset;
        if (ignore_argset != NULL) {
          serr << " (ignoring " << ignore_argset << ")";
        }
        serr << endmsg;
        throw DatasetsNotDisjointException();
      }
    }
    
    master_dataset->merge(slave_dataset);
    if (delete_slave) delete slave_dataset;
  }
  
  void ToyFactoryStd::AppendDatasets(RooDataSet* master_dataset, RooDataSet* slave_dataset) const {
    // sanity check
    const RooArgSet& master_argset = *master_dataset->get();
    const RooArgSet& slave_argset  = *slave_dataset->get();
    
    if (master_argset.getSize() != slave_argset.getSize()) {
      serr << "Attempting two append two datasets with mixed column number. Unable to cope with that. Giving up." << endmsg;
      serr << "Cannot append " << slave_argset << " to " << master_argset << endmsg;
      throw DatasetsNotAppendableException();
    }
    
    // Happy fun time using TIterator, yay!
    TIterator* iter   = slave_argset.createIterator();
    RooAbsArg* column = NULL;
    while ((column = (RooAbsArg*)iter->Next())) {
      if (!master_argset.contains(*column)) {
        serr << "Attempting two append two datasets with non-identical columns. Unable to cope with that. Giving up." << endmsg;
        serr << "Cannot append " << slave_argset << " to " << master_argset << endmsg;
        throw DatasetsNotAppendableException();
      }
    }
    delete iter;
    
    // if we reached this, both datasets are compatible
    master_dataset->append(*slave_dataset);
    delete slave_dataset;
  }
  
  RooDataSet* ToyFactoryStd::MergeDatasetVector(const std::vector<RooDataSet*>& datasets) const {
    if (datasets.size() == 0) {
      serr << "Cannot merge datasets in empty vector." << endmsg;
      throw;
    } 
    RooDataSet* new_dataset = new RooDataSet(*datasets[0]);
    
    if (datasets.size() > 1) {
      for (std::vector<RooDataSet*>::const_iterator it=datasets.begin()+1; it!=datasets.end(); ++it) {
        MergeDatasets(new_dataset, *it, NULL, false);
      }
    }
    return new_dataset;
  }
  
  std::vector<Config::CommaSeparatedPair> ToyFactoryStd::GetPdfProtoSections(const std::string& pdf_name) const {
    const std::vector<Config::CommaSeparatedPair>& proto_sections = config_toyfactory_.proto_sections();
    std::vector<Config::CommaSeparatedPair> matched_sections;
    
    for (std::vector<Config::CommaSeparatedPair>::const_iterator it=proto_sections.begin(); it != proto_sections.end(); ++it) {
      if (pdf_name.compare((*it).first()) == 0) {
        matched_sections.push_back(*it);
      }
    }
    return matched_sections;
  }
  
  RooDataSet* ToyFactoryStd::GenerateForPdf(RooAbsPdf& pdf, const RooArgSet& argset_generation_observables, double expected_yield, bool extended, std::vector<RooDataSet*> proto_data) const {
    RooDataSet* data = NULL;
    bool have_to_delete_proto_data = false;
    
    const std::vector<Config::CommaSeparatedPair>& matched_proto_sections = GetPdfProtoSections(pdf.GetName());
    if (matched_proto_sections.size() > 0) {
      // @todo If PDF ist extended AND no yield is set, we need to get yield from
      //       PDF itself for proto.
      
      int proto_size;
      if (proto_data.size() > 0) {
        proto_size = proto_data[0]->numEntries();
      } else {
        double yield = expected_yield;
        if (expected_yield==0) {
          yield = pdf.expectedEvents(argset_generation_observables);
        }
        proto_size = yield+10*TMath::Sqrt(yield);
      }
      
      // Store only proto data specific for this PDF (remember, there might be 
      // proto data already coming along from higher PDFs).
      RooDataSet* proto_data_this_pdf = NULL;
      
      for (std::vector<Config::CommaSeparatedPair>::const_iterator it=matched_proto_sections.begin(); it != matched_proto_sections.end(); ++it) {
        RooDataSet* temp_data = GenerateProtoSample(pdf, *it, argset_generation_observables, config_toyfactory_.workspace(), proto_size);
        
        // merge proto sets if necessary
        if (proto_data_this_pdf == NULL) {
          proto_data_this_pdf = temp_data;
        } else {
          MergeDatasets(proto_data_this_pdf, temp_data);
        }
        sinfo.set_indent(sinfo.indent()-2);
      }
      
      // merge this proto data at the end of already existing proto data
      proto_data.push_back(proto_data_this_pdf);
      have_to_delete_proto_data = true;
    }
    
    if (PdfIsDecomposable(pdf)) {
      // pdf needs to be decomposed and generated piece-wise 
      if (PdfIsExtended(pdf)) {
        RooRealVar& yield = *((RooRealVar*)pdf.findServer(1));
        RooAbsPdf& sub_pdf = *((RooAbsPdf*)pdf.findServer(0));
                
        sinfo << "RooExtendPdf " << pdf.GetName() << "(" << sub_pdf.GetName() << "," << yield.GetName() << "=" << yield.getVal() << ") will be decomposed." << endmsg;
        
        sinfo.set_indent(sinfo.indent()+2);
        
        data = GenerateForPdf(sub_pdf, argset_generation_observables, expected_yield>0 ? expected_yield : yield.getVal(), extended, proto_data);
        sinfo.set_indent(sinfo.indent()-2);
      } else if (PdfIsAdded(pdf)) {
        data = GenerateForAddedPdf(pdf, argset_generation_observables, expected_yield, extended, proto_data);
      } else if (PdfIsProduct(pdf)) {
        data = GenerateForProductPdf(pdf, argset_generation_observables, expected_yield, extended, proto_data);
      } else if (PdfIsSimultaneous(pdf)) {
        data = GenerateForSimultaneousPdf(pdf, argset_generation_observables, expected_yield, extended, proto_data);
      } else {
        serr << "PDF is decomposable, but decomposition is not yet implemented. Giving up." << endmsg;
        throw;
      }
    } else {
      // pdf can be generated straightly
      sinfo << "Generating for PDF " << pdf.GetName() << " (" << pdf.IsA()->GetName() << "). Expected yield: " << expected_yield << " events.";
      RooDataSet * proto_set = NULL;
      if (proto_data.size() > 0) {
        proto_set = MergeDatasetVector(proto_data);
        sinfo << " Proto dataset is available with " << proto_set->numEntries() << " entries in following dimensions: " << *proto_set->get();
      }
      sinfo << endmsg;
      
      RooCmdArg extend_arg = RooCmdArg::none();
      if (extended) {
        extend_arg = Extended();
      }
      RooCmdArg proto_arg = RooCmdArg::none();
      if (proto_set != NULL) {
        proto_arg = ProtoData(*proto_set);
      }
      
      // correct rounding of number of events to generate
      int yield_to_generate = boost::math::iround(expected_yield);
      
      RooArgSet* obs_argset = pdf.getObservables(argset_generation_observables);
      // if necessary, remove observables already generated as proto set
      if (proto_set != NULL) {
        obs_argset->remove(*proto_set->get(),true,true);
      }
            
#if ROOT_VERSION_CODE >= ROOT_VERSION(5,32,0)
      data = pdf.generate(*obs_argset, yield_to_generate, extend_arg, proto_arg, AutoBinned(false));
#else
      data = pdf.generate(*obs_argset, yield_to_generate, extend_arg, proto_arg);
#endif
      delete obs_argset;
      if (proto_set != NULL) {
        delete proto_set;
      }
    }
    sinfo << "Generated " << data->numEntries() << " events for PDF " << pdf.GetName() << endmsg;
    
    if (have_to_delete_proto_data) {
      delete proto_data.back();
      proto_data.pop_back(); 
    }    
    return data;
  }
  
  RooDataSet* ToyFactoryStd::GenerateForAddedPdf(RooAbsPdf& pdf, const RooArgSet& argset_generation_observables, double expected_yield, bool extended, std::vector<RooDataSet*> proto_data) const {
    // if no external yield set, get expectation from PDF
    bool expected_yield_set = (expected_yield>0);
    if (expected_yield==0) {
      expected_yield = pdf.expectedEvents(argset_generation_observables);
    }
    sinfo << "RooAddPdf " << pdf.GetName();
    
    RooAddPdf& add_pdf = dynamic_cast<RooAddPdf&>(pdf);
    const RooArgList& coefs = add_pdf.coefList();
    // whether PDF is extended or relying on coefficients
    bool add_pdf_extended = (coefs.getSize() == 0);
    
    TIterator* it;
    if (add_pdf_extended) {
      sinfo << " (extended) ";
      
      // in extended case it's best to iterate over the PDF's servers.
      it = pdf.serverIterator();
    } else {
      sinfo << " (not extended) ";
          
      // in non-extended case it's best to iterate over the PDF's components.
      it = add_pdf.pdfList().createIterator();
    }
    
    sinfo << "will be decomposed. Expecting " << expected_yield << " events." << endmsg;
    if (add_pdf_extended && expected_yield_set) {
      swarn << "Although PDF is extended, an external yield has been set. PDF itself would expect " << pdf.expectedEvents(argset_generation_observables) << " events." << endmsg;
      swarn << "Will generate " << expected_yield << " events instead (external set yield via config file/command line or higher PDFs)." << endmsg;
    }
    sinfo.set_indent(sinfo.indent()+2);
    
    RooAbsPdf* sub_pdf = NULL;
    RooDataSet* data = NULL;
    
    int coef_i = 0;
    double sum_coef = 0.0;
    
    RooDataSet* proto_dataset = NULL;
    int proto_dataset_pos = 0;
    if (proto_data.size() > 0) {
      proto_dataset = MergeDatasetVector(proto_data);
    }
    
    double yield_lost_due_rounding = 0.0;
    
    while ((sub_pdf = (RooAbsPdf*)it->Next())) {
      if (sub_pdf->IsA()->InheritsFrom("RooAbsPdf")) {
        double sub_yield, coef;
        if (add_pdf_extended) {
          // yields have to be scaled according to sub PDF's expectation and 
          // requested yield for the RooAddPdf. This is equivalent to 
          // coefficient in non-extended PDF.
          coef = sub_pdf->expectedEvents(argset_generation_observables)/pdf.expectedEvents(argset_generation_observables);
        } else {
          // in this case, the RooAddPdf is extended inside the RooAddPdf, although
          // the daughter PDFs are not. We need to take special care here.
          if (pdf.mustBeExtended()) {
            coef = dynamic_cast<RooAbsReal&>(coefs[coef_i++]).getVal();
          } else {
            if (coef_i < coefs.getSize()) {
              coef = dynamic_cast<RooAbsReal&>(coefs[coef_i++]).getVal();
            } else {
              // last coefficient is (in non-recursive way) always 
              // 1-(sum coeffs)
              coef = 1.0 - sum_coef;
            }
            sum_coef += coef;
          }
        }
        std::cout.precision(15);
        
        if (!add_pdf_extended && pdf.mustBeExtended()) {
          sub_yield = coef;
        } else {
          sub_yield = coef*expected_yield;
        }
        if (extended) {
          sub_yield = RooRandom::randomGenerator()->Poisson(sub_yield);
        } 
        
        // check for need to pass proto set
        RooDataSet* sub_proto_dataset = NULL;
        std::vector<RooDataSet*> sub_proto_data;
        if (proto_dataset != NULL) {
          sub_proto_dataset = new RooDataSet("sub_proto_dataset","sub_proto_dataset", *proto_dataset->get());
          for (int i=proto_dataset_pos; i<(proto_dataset_pos+sub_yield); ++i) {
            sub_proto_dataset->addFast(*proto_dataset->get(i));
          }
          proto_dataset_pos += sub_yield;
          sub_proto_data.push_back(sub_proto_dataset);
        }
        
        yield_lost_due_rounding += sub_yield - boost::math::iround(sub_yield);
        int add_roundup_yield = boost::math::iround(yield_lost_due_rounding);
        if (TMath::Abs(add_roundup_yield) >= 1 && TMath::Abs(yield_lost_due_rounding) != 0.5) {
          sub_yield += add_roundup_yield;
          yield_lost_due_rounding = 0.0;
        }
        
        if (data) {
          RooDataSet* data_temp = GenerateForPdf(*sub_pdf, argset_generation_observables, sub_yield, false, sub_proto_data);
          AppendDatasets(data, data_temp);
        } else {
          data = (GenerateForPdf(*sub_pdf, argset_generation_observables, sub_yield, false, sub_proto_data));
        }
        
        if (sub_proto_dataset != NULL) {
          delete sub_proto_dataset;
        }
      }
    }
    delete it;
    if (proto_dataset != NULL) {
      delete proto_dataset;
    }
    
    sinfo.set_indent(sinfo.indent()-2);
    return data;
  }
  
  RooDataSet* ToyFactoryStd::GenerateForProductPdf(RooAbsPdf& pdf, const RooArgSet& argset_generation_observables, double expected_yield, bool extended, std::vector<RooDataSet*> proto_data) const {
    sinfo << "RooProdPdf " << pdf.GetName() << " will be decomposed." << endmsg;
    sinfo.set_indent(sinfo.indent()+2);
        
    RooProdPdf& prod_pdf = dynamic_cast<RooProdPdf&>(pdf);
    const RooArgList& sub_pdf_list = prod_pdf.pdfList();
    TIterator* it = sub_pdf_list.createIterator();
    
    RooAbsPdf* sub_pdf = NULL;
    RooDataSet* data = NULL;
    
    if (extended) {
      expected_yield = RooRandom::randomGenerator()->Poisson(expected_yield);
    }
    
    while ((sub_pdf = (RooAbsPdf*)it->Next())) {
      if (data) {
        RooDataSet* data_temp = GenerateForPdf(*sub_pdf, argset_generation_observables, expected_yield, false, proto_data);
        
        if (proto_data.size() > 0) {
          MergeDatasets(data, data_temp, proto_data[0]->get());
        } else {
          MergeDatasets(data, data_temp);
        }
      } else {
        data = GenerateForPdf(*sub_pdf, argset_generation_observables, expected_yield, false, proto_data);
      }
    }
    delete it;
    
    sinfo.set_indent(sinfo.indent()-2);
    return data;
  }
  
  RooDataSet* ToyFactoryStd::GenerateForSimultaneousPdf(RooAbsPdf& pdf, const RooArgSet& argset_generation_observables, double expected_yield, bool extended, std::vector<RooDataSet*> proto_data) const {
    sinfo << "RooSimultaneous " << pdf.GetName() << " will be decomposed." << endmsg;
    if (expected_yield>0) {
      swarn << "A yield to generate is set for the simultaneous PDF. This will lead to all simultaneous sub PDFs" << endmsg;
      swarn << "being generated in this size (yield=" << expected_yield << "). This is generally not what is intended for simultaneous PDFs." << endmsg;
      swarn << "It is advised to use extended PDFs in each sample and not set a yield to generate via the expected_yield option." << endmsg;
    }
    sinfo.set_indent(sinfo.indent()+2);
    
    RooSimultaneous& sim_pdf = dynamic_cast<RooSimultaneous&>(pdf);
    const RooAbsCategoryLValue& sim_cat_grr = (sim_pdf.indexCat());
    RooAbsCategoryLValue& sim_cat = *dynamic_cast<RooAbsCategoryLValue*>(sim_pdf.findServer(sim_cat_grr.GetName()));
    
    RooDataSet* data = NULL;
    
    RooCatType* sim_cat_type = NULL;
    TIterator* sim_cat_type_iter = sim_cat.typeIterator();
    while((sim_cat_type=(RooCatType*)sim_cat_type_iter->Next())) {
      std::string sim_cat_name = sim_cat_type->GetName();
      sim_cat.setLabel(sim_cat_name.c_str());
      RooAbsPdf& sub_pdf = *(sim_pdf.getPdf(sim_cat_name.c_str()));
      
      sinfo << "Generating for simultaneous sub PDF " << sub_pdf.GetName() << " for category " << sim_cat.getLabel() << endmsg;
      RooDataSet* data_temp = GenerateForPdf(sub_pdf, argset_generation_observables, expected_yield, extended, proto_data);
      data_temp->addColumn(sim_cat);
      
      // add categories if super category
      RooSuperCategory* sim_super_cat = dynamic_cast<RooSuperCategory*>(&sim_cat);
      if (sim_super_cat != NULL) {
        RooLinkedListIter* it  = (RooLinkedListIter*)sim_super_cat->inputCatList().createIterator();
        RooAbsArg*         arg = NULL;
        
        while ((arg=(RooAbsArg*)it->Next())) {
          data_temp->addColumn(*arg);
        }
        delete it;
      }
      
      if (data) {
        AppendDatasets(data, data_temp);
      } else {
        data = data_temp;
      }
    }
    delete sim_cat_type_iter;
    sinfo.set_indent(sinfo.indent()-2);
    return data;
  }
  
  RooDataSet* ToyFactoryStd::GenerateDiscreteSample(const std::vector<Config::DiscreteProbabilityDistribution>& discrete_probabilities, const RooArgSet& argset_generation_observables, const RooArgSet& argset_already_generated, int yield) const {
    // Vector containing necessary information for discrete variable generation.
    // Tuple contents:
    //  1. The variable as RooAbsArg* itself
    //  2. the C style array containing cumulative probabilities (efficiency is 
    //     relevant here)
    //  3. the corresponding values of the variable as C style array
    //  4. the number how many times the variable was generated as C style array
    //  5. number of entries in C style arrays
    std::vector<boost::tuple<RooAbsArg*,double*,double*,int*,int> > disc_vars;
    
    // prepare everything:
    //  - check variables for validity
    //  - generate arrays with values and cumulative probabilities
    //  - add to relevant arg sets
    RooArgSet disc_argset;
    RooArgSet disc_cat_argset;
    for (std::vector<Config::DiscreteProbabilityDistribution>::const_iterator it = discrete_probabilities.begin(); it != discrete_probabilities.end(); ++it) {
      RooAbsArg* disc_var = config_toyfactory_.argset_generation_observables()->find((*it).var_name().c_str());
      
      if (disc_var == NULL) {
        serr << "ERROR: Discrete probability for " << (*it).var_name() 
        << " is requested for generation but is not in the generation argument set. Cannot generate." << endmsg;
        throw NotGeneratingDiscreteData();
      } else {
        if (argset_already_generated.contains(*disc_var)) {
          serr << "ERROR: Discrete probability for " << (*it).var_name() 
          << " is requested for generation but was already generated by PDF "
          << config_toyfactory_.generation_pdf()->GetName() 
          << " (" << config_toyfactory_.generation_pdf()->IsA()->GetName() 
          << ")." << endmsg;
          throw NotGeneratingDiscreteData();
        } else {
          const std::vector<std::pair<double,double> >& prob_map = (*it).probabilities();
          // create some simple arrays for faster generation (looping over arrays 
          // faster than looping over complex vectors and stuff).
          // I tried creating a special case for constant variables 
          // (num_values == 1), but it's not worth (the overhead through looping 
          // is irrelevant).
          int num_values = prob_map.size();
          double* cum_probs     = new double[num_values];
          double* values        = new double[num_values];
          int*    num_generated = new int[num_values];
                    
          int j = 0;
          for (std::vector<std::pair<double,double> >::const_iterator it_prob = prob_map.begin(); it_prob != prob_map.end(); ++it_prob) {
            cum_probs[j]     = (*it_prob).second;
            values[j]        = (*it_prob).first;
            num_generated[j] = 0;
            ++j;
          }
          
          RooRealVar* disc_realvar = dynamic_cast<RooRealVar*>(disc_var);
          RooCategory* disc_catvar = dynamic_cast<RooCategory*>(disc_var);
          
          if (!disc_realvar && !disc_catvar) {
            serr << "ERROR: Variable " << disc_var->GetName() << " (" << disc_var->IsA()->GetName() << ") is neither RooRealVar nor RooCategory." << endmsg;
            throw;
          }
          
          disc_vars.push_back(boost::tuple<RooAbsArg*,double*,double*,int*,int>(disc_var,cum_probs,values,num_generated,num_values));
          disc_argset.add(*disc_var);
          if (disc_catvar) disc_cat_argset.add(*disc_catvar);
        }
      }
    }
    
    // prepare dataset
    RooDataSet* data_discrete = new RooDataSet("data_discrete", "data_discrete", disc_argset);
    double r;
    int j;
    
    sinfo << "Generating discrete dataset for ";
    for (std::vector<boost::tuple<RooAbsArg*,double*,double*,int*,int> >::const_iterator it = disc_vars.begin(); it != disc_vars.end(); ++it) {
      if (it != disc_vars.begin()) sinfo << ", ";
      sinfo << (*it).get<0>()->GetName();
    }
    sinfo << endmsg;
    
    // the actual loop generating the dataset
    for (int i=0; i<yield; ++i) {
      // loop over variables (thanks to previous preparations, the only call 
      // consuming relevant CPU time is the call to RooDataSet::addFast(...))
      for (std::vector<boost::tuple<RooAbsArg*,double*,double*,int*,int> >::const_iterator it = disc_vars.begin(); it != disc_vars.end(); ++it) {   
        // get all stuff from the tuple
        RooAbsArg* disc_var   = (*it).get<0>();
        double* cum_probs     = (*it).get<1>();
        double* values        = (*it).get<2>();
        int*    num_generated = (*it).get<3>();
        int     num_values    = (*it).get<4>();
        RooRealVar* disc_realvar = dynamic_cast<RooRealVar*>(disc_var);
        RooCategory* disc_catvar = dynamic_cast<RooCategory*>(disc_var);
        
        r = RooRandom::uniform();
        // stupid walkthrough through cumulative probabilities.
        // it's definitely not the smartest way, but again it does not matter.
        // a smart binary search does not gain anything even in perfect test 
        // cases.
        for (j=0; j<num_values; ++j) {  
          if (r < cum_probs[j]) {              
            if (disc_realvar) disc_realvar->setVal(values[j]);
            if (disc_catvar) disc_catvar->setIndex(values[j]);
            num_generated[j]++;
            break;
          }
        }
      }
      data_discrete->addFast(disc_argset);
    }
    
    // table print and cleanup
    for (std::vector<boost::tuple<RooAbsArg*,double*,double*,int*,int> >::const_iterator it = disc_vars.begin(); it != disc_vars.end(); ++it) {       
      sinfo << "Generation table for " << (*it).get<0>()->GetName() << ": ";
      for (int i=0; i<(*it).get<4>(); ++i) {
        if (i > 0) sinfo << ", ";
        sinfo << "N(" << (*it).get<2>()[i] << ") = " << (*it).get<3>()[i];
      }
      sinfo << endmsg;
      
      delete[] (*it).get<1>();
      delete[] (*it).get<2>();
      delete[] (*it).get<3>();
    }
    
    return data_discrete;
  }
  
  RooDataSet* ToyFactoryStd::GenerateProtoSample(const RooAbsPdf& pdf, const Config::CommaSeparatedPair& proto_section, const RooArgSet& argset_generation_observables, RooWorkspace* workspace, int yield) const {
    
    assert(yield>0);
    sinfo << "Generating proto data for PDF " << pdf.GetName() << " using config section " << proto_section.second() << endmsg;
    sinfo.set_indent(sinfo.indent()+2);
    ToyFactoryStdConfig cfg_tfac_proto(proto_section.second());
    cfg_tfac_proto.InitializeOptions(config_common_);
    
    if (workspace != NULL) cfg_tfac_proto.set_workspace(workspace);
    cfg_tfac_proto.set_argset_generation_observables(&argset_generation_observables);
    cfg_tfac_proto.set_expected_yield(yield);
    cfg_tfac_proto.set_dataset_size_fixed(true);
    cfg_tfac_proto.set_random_seed(config_toyfactory_.random_seed());

    ToyFactoryStd tfac_proto(config_common_, cfg_tfac_proto);
    
    return tfac_proto.Generate();
  }
} // namespace Toy
} // namespace doofit

