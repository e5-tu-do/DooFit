#include "ToyFactory/ToyFactoryStd/ToyFactoryStd.h"

// ROOT
#include "TClass.h"

// from RooFit
#include "RooDataSet.h"
#include "RooAbsPdf.h"
#include "RooRandom.h"

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

bool ToyFactoryStd::PdfIsDecomposable(const RooAbsPdf& pdf) {
  if (strcmp(pdf.IsA()->GetName(),"RooSimultaneous") == 0 
      || strcmp(pdf.IsA()->GetName(),"RooAddPdf") == 0
      || strcmp(pdf.IsA()->GetName(),"RooProdPdf") == 0
      || strcmp(pdf.IsA()->GetName(),"RooExtendPdf") == 0) {
    return true;
  } else {
    return false;
  }
}

RooDataSet* ToyFactoryStd::GenerateForPdf(RooAbsPdf& pdf, const RooArgSet& argset_generation_observables, int expected_yield) {
  if (PdfIsDecomposable(pdf)) {
    // pdf needs to be decomposed and generated piece-wise
    serr << "PDF is decomposable, but decomposition is not yet implemented. Giving up." << endmsg;
    throw;
  } else {
    // pdf can be generated straightly
    sinfo << "Generating for PDF " << pdf.GetName() << " (" << pdf.IsA()->GetName() << "). Expected yield: " << expected_yield << " events." << endmsg;
    
    RooDataSet* data = pdf.generate(*(pdf.getObservables(argset_generation_observables)), expected_yield, Extended());
    
    sinfo << "Generated " << data->numEntries() << " events for PDF " << pdf.GetName() << endmsg;
    
    return data;
  }
}
