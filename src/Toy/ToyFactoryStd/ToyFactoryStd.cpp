#include "Toy/ToyFactoryStd/ToyFactoryStd.h"

// STL
#include <cstring>
#include <vector>

// boost
#include "boost/tuple/tuple.hpp"

// ROOT
#include "TClass.h"
#include "TStopwatch.h"

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

// from Project
#include "Config/CommonConfig.h"
#include "Toy/ToyFactoryStd/ToyFactoryStdConfig.h"
#include "Utils/MsgStream.h"

using namespace ROOT;
using namespace RooFit;

namespace Toy {
  ToyFactoryStd::ToyFactoryStd(const CommonConfig& cfg_com, const ToyFactoryStdConfig& cfg_tfac) :
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
    } catch (const ArgSetNotSetException& e) {
      serr << "Cannot generate any data as observables argument set is not set." << endmsg;
      throw e;
    }
      
    const std::vector<Config::DiscreteProbabilityDistribution>& discrete_probabilities = config_toyfactory_.discrete_probabilities();
    
    if (discrete_probabilities.size() > 0) {
      // determine yield and already generated argset for discrete dataset
      double discrete_yield = 0;
      const RooArgSet* argset_already_generated;
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
    }
    
    sinfo << "Generation of this sample took " << sw << endmsg;
    sinfo.Ruler();
    return data;
  }
  
  bool ToyFactoryStd::PdfIsDecomposable(const RooAbsPdf& pdf) const {
    if (strcmp(pdf.IsA()->GetName(),"RooSimultaneous") == 0 
        || PdfIsAdded(pdf)
        || PdfIsProduct(pdf)
        || PdfIsExtended(pdf)) {
      return true;
    } else {
      return false;
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
        proto_size = expected_yield+10*TMath::Sqrt(expected_yield);
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
        sinfo << " Proto dataset is available with " << proto_set->numEntries() << " entries.";
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
      
      RooArgSet* obs_argset = pdf.getObservables(argset_generation_observables);
      data = pdf.generate(*obs_argset, expected_yield, extend_arg, proto_arg);
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
      it = add_pdf.getComponents()->createIterator();
      // the first component is the RooAddPdf itself, get rid of it
      it->Next();
    }
    
    sinfo << "will be decomposed." << endmsg;
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
    
    while ((sub_pdf = (RooAbsPdf*)it->Next())) {
      if (sub_pdf->IsA()->InheritsFrom("RooAbsPdf")) {
        double sub_yield, coef;
        if (add_pdf_extended) {
          // yields have to be scaled according to sub PDF's expectation and 
          // requested yield for the RooAddPdf. This is equivalent to 
          // coefficient in non-extended PDF.
          coef = sub_pdf->expectedEvents(argset_generation_observables)/pdf.expectedEvents(argset_generation_observables);
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
        if (extended) {
          sub_yield = RooRandom::randomGenerator()->Poisson(coef*expected_yield);
        } else {
          sub_yield = coef*expected_yield;
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
    
    RooArgSet* comp_argset = prod_pdf.getComponents();
    TIterator* it = comp_argset->createIterator();
    // the first component is the RooProdPdf itself, get rid of it
    it->Next();
    
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
        data = (GenerateForPdf(*sub_pdf, argset_generation_observables, expected_yield, false, proto_data));
      }
    }
    delete it;
    delete comp_argset;
    
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
    vector<boost::tuple<RooAbsArg*,double*,double*,int*,int> > disc_vars;
    
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
    for (vector<boost::tuple<RooAbsArg*,double*,double*,int*,int> >::const_iterator it = disc_vars.begin(); it != disc_vars.end(); ++it) {
      if (it != disc_vars.begin()) sinfo << ", ";
      sinfo << (*it).get<0>()->GetName();
    }
    sinfo << endmsg;
    
    // the actual loop generating the dataset
    for (int i=0; i<yield; ++i) {
      // loop over variables (thanks to previous preparations, the only call 
      // consuming relevant CPU time is the call to RooDataSet::addFast(...))
      for (vector<boost::tuple<RooAbsArg*,double*,double*,int*,int> >::const_iterator it = disc_vars.begin(); it != disc_vars.end(); ++it) {   
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
    for (vector<boost::tuple<RooAbsArg*,double*,double*,int*,int> >::const_iterator it = disc_vars.begin(); it != disc_vars.end(); ++it) {       
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
    sinfo << "Generating proto data for PDF " << pdf.GetName() << " using config section " << proto_section.second() << endmsg;
    sinfo.set_indent(sinfo.indent()+2);
    ToyFactoryStdConfig cfg_tfac_proto(proto_section.second());
    cfg_tfac_proto.InitializeOptions(config_common_);
    
    if (workspace != NULL) cfg_tfac_proto.set_workspace(workspace);
    cfg_tfac_proto.set_argset_generation_observables(&argset_generation_observables);
    cfg_tfac_proto.set_expected_yield(yield);
    cfg_tfac_proto.set_dataset_size_fixed(true);
    
    ToyFactoryStd tfac_proto(config_common_, cfg_tfac_proto);
    
    return tfac_proto.Generate();
  }
}
