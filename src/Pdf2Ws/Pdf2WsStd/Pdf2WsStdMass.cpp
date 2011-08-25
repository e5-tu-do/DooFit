/** @namespace PdfWsStd::Mass
 *
 *  @brief PdfWsStd::Mass namespace for mass pdf adders.
 *
 */

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


/**
 *  @brief Function to add a single Gaussian mass pdf to the workspace.
 *
 *  This function adds a RooGaussian pdf with name @a pdf_name to the workspace.
 *  @param ws The target workspace.
 *  @param pdf_name The name of the pdf
 *  @param pdf_desc The description of the pdf. Normally a latex like formula.
 *  @param var_mass_name Name of the mass variable.
 *  @param par_mean_name Name of the Gaussian mean parameter, typically the mass to be fit.
 *  @param par_sigma_name Name of the standard deviation parameter of the Gaussian. 
 */
void Pdf2WsStd::Mass::Gaussian(RooWorkspace* ws, 
                               const TString& pdf_name, const TString& pdf_desc,
                               const TString& var_mass_name, 
                               const TString& par_mean_name, const TString& par_sigma_name){
  // Check if pdf with name already exists on workspace. If so, stop and exit!
  if (ws->obj(pdf_name) != NULL){
    cerr << "Pdf2WsStd::Mass::Gaussian found an object with name " << pdf_name
    << " on the workspace. Thus, creation of pdf with same name impossible! Exiting!" << endl;
    exit(1);
  }
  
  
  RooAbsReal* var_mass  = CommonFuncs::getVar(ws, var_mass_name,  "m",5300,5150,5500,"MeV/c^{2}");
  RooAbsReal* par_mean  = CommonFuncs::getVar(ws, par_mean_name,  "m_{B}",5300,5150,5500,"MeV/c^{2}");
  RooAbsReal* par_sigma = CommonFuncs::getVar(ws, par_sigma_name, "#sigma_{m}",15,5,20,"MeV/c^{2}");
  
  ws->import(RooGaussian(pdf_name, pdf_desc, *var_mass, *par_mean, *par_sigma));
}


