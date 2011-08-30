#include "Pdf2Ws/Pdf2WsStd/Pdf2WsStdCommonFuncs.h"

// STL
#include <iostream>
#include <typeinfo>

// Boost

// ROOT

// RooFit
#include "RooAbsReal.h"
#include "RooRealVar.h"
#include "RooWorkspace.h"
#include "RooAbsArg.h"

// from Project
#include "Utils/MsgStream.h"

using namespace std;
using namespace ROOT;
using namespace RooFit;

RooAbsReal* Pdf2WsStd::CommonFuncs::getVar(RooWorkspace* ws,
                                           const TString& var_name, const TString& var_title, 
                                           Double_t init_value, Double_t min_value, Double_t max_value, 
                                           const TString& unit){
  // Pointer to be returned
  RooAbsReal* var_temp;
  
  // Check if object with this name exists on workspace, else create one.
  if (ws->obj(var_name) != NULL){
    // if it does, check, that it is castable to RooAbsReal, else exit program
    var_temp = dynamic_cast<RooAbsReal*>(ws->obj(var_name));
    
    if (var_temp == NULL) {
      serr << "Pdf2WsStd: You tried to use the Workspace object with name " 
      << var_name << " as a RooAbsReal parameter in a pdf!" << endmsg;
      throw;
    }
  }
  else{
    import(ws, RooRealVar(var_name, var_title, init_value, min_value, max_value, unit));
    var_temp = (RooAbsReal*)ws->obj(var_name);
  }
  
  return var_temp;
}  

void Pdf2WsStd::CommonFuncs::import(RooWorkspace* ws, const RooAbsArg& arg) {
  ws->import(arg, Silence());
}
