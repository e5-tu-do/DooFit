#ifndef PDF2WSSTDMASS_h
#define PDF2WSSTDMASS_h


// from ROOT


// forward declarations
class RooWorkspace;
class TString;

namespace Pdf2WsStd{ namespace Mass{
 void Gaussian(RooWorkspace* ws, 
               const TString& pdf_name, const TString& pdf_desc,
               const TString& var_mass, 
               const TString& par_mean, const TString& par_sigma);
   
}}



#endif // PDF2WSSTDMASS_h
