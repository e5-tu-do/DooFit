/** @namespace Pdf2WsStd::Mass
 *
 *  @brief Pdf2WsStd::Mass namespace for mass pdf adders.
 *
 */
#ifndef PDF2WSSTDMASS_h
#define PDF2WSSTDMASS_h


// from ROOT


// forward declarations
class RooWorkspace;
class TString;

namespace Pdf2WsStd{ namespace Mass{
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
  void Gaussian(RooWorkspace* ws, 
                const TString& pdf_name, const TString& pdf_desc,
                const TString& var_mass, 
                const TString& par_mean, const TString& par_sigma);
   
}}



#endif // PDF2WSSTDMASS_h
