#ifndef PDF2WSSTDCOMMONFUNC_h
#define PDF2WSSTDCOMMONFUNC_h

// ROOT
#include "TString.h"

// forward declarations
class RooAbsReal;
class RooWorkspace;

namespace Pdf2WsStd{ namespace CommonFuncs{
  RooAbsReal* getVar(RooWorkspace* ws,
                     const TString& var_name, const TString& var_title, 
                     Double_t init_value, Double_t min_value, Double_t max_value, 
                     const TString& unit);
} }

#endif // PDF2WSSTDCOMMONFUNC_h
