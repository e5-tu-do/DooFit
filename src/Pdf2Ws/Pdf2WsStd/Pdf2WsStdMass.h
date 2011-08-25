#ifndef PDF2WSSTDMASS_h
#define PDF2WSSTDMASS_h


// from ROOT
#include "TString.h"

// forward declarations
class RooAbsReal;
class RooRealvar;
class RooWorkspace;

namespace Pdf2WsStd{ 
namespace Mass{
 void Gaussian(RooWorkspace* ws);
   
}

namespace CommonFuncs{
  RooAbsReal* getVar(RooWorkspace* ws,
                     const TString& var_name, const TString& var_title, 
                     Double_t init_value, Double_t min_value, Double_t max_value, 
                     const TString& unit);
}



}



#endif // PDF2WSSTDMASS_h
