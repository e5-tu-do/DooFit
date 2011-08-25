// STL
#include <iostream>
#include <typeinfo>
// Boost

// ROOT

// RooFit
#include "RooAbsReal.h"
#include "RooRealVar.h"
#include "RooWorkspace.h"

// RooFit PDFs
#include "RooGaussian.h"

// RooFit PDFs from Project 

// from Project
#include "Pdf2Ws/Pdf2WsStd/Pdf2WsStdMass.h"


using namespace std;

void Pdf2WsStd::Mass::Gaussian(RooWorkspace* ws){
  
}

RooAbsReal* Pdf2WsStd::CommonFuncs::getVar(RooWorkspace* ws,
                                           const TString& var_name, const TString& var_title, 
                                           Double_t init_value, Double_t min_value, Double_t max_value, 
                                           const TString& unit){
  // Pointer to be returned
  RooAbsReal* var_temp;
  
  // Check if object with this name exists on workspace, else create one.
  if (ws->obj(var_name) != NULL){
    // if it does, check, that it is castable to RooAbsReal, else exit program
    try {
      var_temp = dynamic_cast<RooAbsReal*>(ws->obj(var_name));
    } catch (const std::bad_cast& e) {
      cerr << "Pdf2WsStd: You tried to use the Workspace object with name " 
      << var_name << " as a RooAbsReal parameter in a pdf!" << endl;
      cerr << "  bad_cast caught: " << e.what() << endl;
      cerr << "Exiting..." << endl;
      exit(1);
    }
  }
  else{
    ws->import(RooRealVar(var_name, var_title, init_value, min_value, max_value, unit));
    var_temp = (RooAbsReal*)ws->obj(var_name);
  }
  
  return var_temp;
}  

