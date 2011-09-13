// STL
#include <iostream>

// Boost

// ROOT
#include "TString.h"

// RooFit
#include "RooAbsReal.h"
#include "RooRealVar.h"
#include "RooWorkspace.h"

// RooFit PDFs
#include "RooGaussian.h"

// RooFit PDFs from Project 

// from Project
#include "Pdf2Ws/Pdf2WsStd/Pdf2WsStdCommonFuncs.h"
#include "Pdf2Ws/Pdf2WsStd/Pdf2WsStdMass.h"


using namespace std;
using namespace Pdf2WsStd;


RooGaussian* Pdf2WsStd::Mass::Gaussian(RooWorkspace* ws, 
                               const TString& pdf_name, const TString& pdf_desc,
                               const TString& var_mass_name, 
                               const TString& par_mean_name, const TString& par_sigma_name){
  // Check if object with name already exists on workspace. If so, stop and exit!
  if (ws->obj(pdf_name) != NULL){
    cerr << "Pdf2WsStd::Mass::Gaussian found an object with name " << pdf_name
    << " on the workspace. Thus, creation of pdf with same name impossible! Exiting!" << endl;
    throw;
  }
  
  
  RooAbsReal* var_mass  = CommonFuncs::getVar(ws, var_mass_name,  "m",5300,5150,5500,"MeV/c^{2}");
  RooAbsReal* par_mean  = CommonFuncs::getVar(ws, par_mean_name,  "m_{B}",5300,5150,5500,"MeV/c^{2}");
  RooAbsReal* par_sigma = CommonFuncs::getVar(ws, par_sigma_name, "#sigma_{m}",15,5,20,"MeV/c^{2}");
  
  CommonFuncs::import(ws,RooGaussian(pdf_name, pdf_desc, *var_mass, *par_mean, *par_sigma));
  
  return (RooGaussian*)ws->pdf(pdf_name);
}
