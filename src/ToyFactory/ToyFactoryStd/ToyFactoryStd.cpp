#include "ToyFactory/ToyFactoryStd/ToyFactoryStd.h"

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
#include "ToyFactory/ToyFactoryStd/ToyFactoryStdConfig.h"
#include "Utils/MsgStream.h"

using namespace ROOT;
using namespace RooFit;

ToyFactoryStd::ToyFactoryStd(const CommonConfig& cfg_com, const ToyFactoryStdConfig& cfg_tfac) :
config_common_(cfg_com),
config_toyfactory_(cfg_tfac)
{
  RooRandom::randomGenerator()->SetSeed(cfg_tfac.random_seed());
}

ToyFactoryStd::~ToyFactoryStd(){
  
}

RooDataSet* ToyFactoryStd::Generate() {
  if (config_toyfactory_.generation_pdf() == NULL) {
    serr << "Generation PDF not set. Cannot generate toy sample." << endmsg;
    throw;
  }
  if (config_toyfactory_.argset_generation_observables() == NULL) {
    serr << "Generation observables argset not set. Cannot generate toy sample." << endmsg;
    throw;
  }
  
  sinfo << endmsg;
  sinfo.Ruler();
  TStopwatch sw;
  sw.Start();
  
  RooDataSet* data = GenerateForPdf(*(config_toyfactory_.generation_pdf()), *(config_toyfactory_.argset_generation_observables()), config_toyfactory_.expected_yield());
  
  
  const std::vector<Config::DiscreteProbabilityDistribution>& discrete_probabilities = config_toyfactory_.discrete_probabilities();
  
  RooDataSet* data_discrete = GenerateDiscreteSample(config_toyfactory_.discrete_probabilities(), *(config_toyfactory_.argset_generation_observables()), *(data->get()), data->numEntries());
  
    
  data->merge(data_discrete);
  delete data_discrete;

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

RooDataSet* ToyFactoryStd::GenerateForPdf(RooAbsPdf& pdf, const RooArgSet& argset_generation_observables, double expected_yield, bool extended) {
  RooDataSet* data = NULL;
  
  if (PdfIsDecomposable(pdf)) {
    // pdf needs to be decomposed and generated piece-wise 
    if (PdfIsExtended(pdf)) {
      RooRealVar& yield = *((RooRealVar*)pdf.findServer(1));
      RooAbsPdf& sub_pdf = *((RooAbsPdf*)pdf.findServer(0));
      
      sinfo << "RooExtendPdf " << pdf.GetName() << "(" << sub_pdf.GetName() << "," << yield.GetName() << "=" << yield.getVal() << ") will be decomposed." << endmsg;
      
      sinfo.set_indent(sinfo.indent()+2);
      data = GenerateForPdf(sub_pdf, argset_generation_observables, expected_yield>0 ? expected_yield : yield.getVal());
      sinfo.set_indent(sinfo.indent()-2);
    } else if (PdfIsAdded(pdf)) {
      data = GenerateForAddedPdf(pdf, argset_generation_observables, expected_yield);
    } else if (PdfIsProduct(pdf)) {
      data = GenerateForProductPdf(pdf, argset_generation_observables, expected_yield);
    } else {
      serr << "PDF is decomposable, but decomposition is not yet implemented. Giving up." << endmsg;
      throw;
    }
  } else {
    // pdf can be generated straightly
    sinfo << "Generating for PDF " << pdf.GetName() << " (" << pdf.IsA()->GetName() << "). Expected yield: " << expected_yield << " events." << endmsg;
    
    RooCmdArg extend_arg = RooCmdArg::none();
    if (extended) {
      extend_arg = Extended();
    }
    
    data = pdf.generate(*(pdf.getObservables(argset_generation_observables)), expected_yield, extend_arg);
  }
  sinfo << "Generated " << data->numEntries() << " events for PDF " << pdf.GetName() << endmsg;
  return data;
}

RooDataSet* ToyFactoryStd::GenerateForAddedPdf(RooAbsPdf& pdf, const RooArgSet& argset_generation_observables, double expected_yield, bool extended) {
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
      sub_yield = coef*expected_yield;
      
      if (data) {
        RooDataSet* data_temp = GenerateForPdf(*sub_pdf, argset_generation_observables, sub_yield);
        data->append(*data_temp);
        delete data_temp;
      } else {
        data = (GenerateForPdf(*sub_pdf, argset_generation_observables, sub_yield));
      }
    }
  }
  
  sinfo.set_indent(sinfo.indent()-2);
  return data;
}

RooDataSet* ToyFactoryStd::GenerateForProductPdf(RooAbsPdf& pdf, const RooArgSet& argset_generation_observables, double expected_yield, bool extended) {
  sinfo << "RooProdPdf " << pdf.GetName() << " will be decomposed." << endmsg;
  sinfo.set_indent(sinfo.indent()+2);
  
  RooProdPdf& prod_pdf = dynamic_cast<RooProdPdf&>(pdf);
  
  TIterator* it = prod_pdf.getComponents()->createIterator();
  // the first component is the RooAddPdf itself, get rid of it
  it->Next();
  
  RooAbsPdf* sub_pdf = NULL;
  RooDataSet* data = NULL;
  
  if (extended) {
    expected_yield = RooRandom::randomGenerator()->Poisson(expected_yield);
  }
  
  while ((sub_pdf = (RooAbsPdf*)it->Next())) {
    if (data) {
      RooDataSet* data_temp = GenerateForPdf(*sub_pdf, argset_generation_observables, expected_yield, false);
      data->merge(data_temp);
      delete data_temp;
    } else {
      data = (GenerateForPdf(*sub_pdf, argset_generation_observables, expected_yield, false));
    }
  }
  
  sinfo.set_indent(sinfo.indent()-2);
  return data;
}

RooDataSet* ToyFactoryStd::GenerateDiscreteSample(const std::vector<Config::DiscreteProbabilityDistribution>& discrete_probabilities, const RooArgSet& argset_generation_observables, const RooArgSet& argset_already_generated, int yield) {
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
      throw;
    } else {
      if (argset_already_generated.contains(*disc_var)) {
        serr << "ERROR: Discrete probability for " << (*it).var_name() 
        << " is requested for generation but was already generated by PDF "
        << config_toyfactory_.generation_pdf()->GetName() 
        << " (" << config_toyfactory_.generation_pdf()->IsA()->GetName() 
        << ")." << endmsg;
        throw;
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
