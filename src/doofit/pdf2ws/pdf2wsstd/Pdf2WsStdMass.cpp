#include "doofit/pdf2ws/pdf2wsstd/Pdf2WsStdMass.h"

// from STL
#include <iostream>

// from Boost

// from ROOT
#include "TString.h"

// from RooFit
#include "RooAbsReal.h"
#include "RooRealVar.h"
#include "RooWorkspace.h"

// RooFit PDFs from RooFit
#include "RooGaussian.h"

// RooFit PDFs from Project 

// from Project
#include "doofit/pdf2ws/pdf2wsstd/Pdf2WsStdCommonFuncs.h"
#include "doocore/io/MsgStream.h"

namespace doofit {
using namespace std;
using namespace pdf2wsstd;
using namespace doocore::io;

RooGaussian* pdf2wsstd::mass::Gaussian(RooWorkspace* ws, 
                               const TString& pdf_name, const TString& pdf_desc,
                               const TString& var_mass_name, 
                               const TString& par_mean_name, const TString& par_sigma_name){
  // Check if object with name already exists on workspace. If so, stop and exit!
  if (ws->obj(pdf_name) != NULL){
    serr << "pdf2wsstd::mass::Gaussian found an object with name " << pdf_name
    << " on the workspace. Thus, creation of pdf with same name impossible!" << endmsg;
    throw;
  }
  
  
  RooAbsReal* var_mass  = commonfuncs::getVar(ws, var_mass_name,  "m",5300,5150,5500,"MeV/c^{2}");
  RooAbsReal* par_mean  = commonfuncs::getVar(ws, par_mean_name,  "m_{B}",5300,5150,5500,"MeV/c^{2}");
  RooAbsReal* par_sigma = commonfuncs::getVar(ws, par_sigma_name, "#sigma_{m}",15,5,20,"MeV/c^{2}");
  
  commonfuncs::import(ws,RooGaussian(pdf_name, pdf_desc, *var_mass, *par_mean, *par_sigma));
  
  return (RooGaussian*)ws->pdf(pdf_name);
}
}
