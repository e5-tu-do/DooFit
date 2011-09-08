#include "ToyFactory/ToyFactoryStd/ToyFactoryStd.h"

// STL
#include <cstring>
#include <vector>

// ROOT
#include "TClass.h"

// from RooFit
#include "RooDataSet.h"
#include "RooAbsPdf.h"
#include "RooAddPdf.h"
#include "RooProdPdf.h"
#include "RooRandom.h"
#include "RooArgList.h"
#include "RooArgSet.h"
#include "RooCmdArg.h"

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
  
  RooDataSet* data = GenerateForPdf(*(config_toyfactory_.generation_pdf()), *(config_toyfactory_.argset_generation_observables()), config_toyfactory_.expected_yield());
  
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
        data->append(*(GenerateForPdf(*sub_pdf, argset_generation_observables, sub_yield)));
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
  
  // @todo: check for PDF arguments to be uncorrelated
  
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
      data->merge(GenerateForPdf(*sub_pdf, argset_generation_observables, expected_yield, false));
    } else {
      data = (GenerateForPdf(*sub_pdf, argset_generation_observables, expected_yield, false));
    }
  }
  
  sinfo.set_indent(sinfo.indent()-2);
  return data;
}
