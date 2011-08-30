/** @namespace Pdf2WsStd
 *  @brief DooFit Pdf2WsStd namespace.
 *
 *  The Pdf2WsStd namespace contains the basic helper functions to allow
 *  the easy creation of RooAbsPdf objects and their subsequent import to a
 *  RooWorkspace. As this namespace contains very basic functions it does not
 *  rely on the rest of the DooFit framework.
 *
 *  The namespace is subdivided into several other namespaces, e.g the 
 *  Pdf2WsStd::CommonFuncs namespace contains functions used by other Pdf2WsStd 
 *  functions. Each physical dimension of the data to be described by pdfs has 
 *  its own namespace with specific pdf functions.
 *  @see Pdf2WsStd::CommonFuncs
 *  @see Pdf2WsStd::Mass
 *  @see Pdf2WsStd::Proptime
 *  @see Pdf2WsStd::ProptimeErr
 *  
 */
/** @namespace Pdf2WsStd::CommonFuncs
 *  @brief DooFit Pdf2WsStd::CommonFuncs namespace with helper functions.
 *
 *  The Pdf2WsStd::CommonFuncs namespace contains functions used by other 
 *  Pdf2WsStd functions. Most functions are basic helper functions to reduce
 *  the amount of recurring code in the other pdf creator functions.
 */
#ifndef PDF2WSSTDCOMMONFUNCS_h
#define PDF2WSSTDCOMMONFUNCS_h

// ROOT
#include "TString.h"

// forward declarations
class RooAbsReal;
class RooWorkspace;


namespace Pdf2WsStd{ namespace CommonFuncs{
  /**
   *  @brief Get variable with name @a var_name from workspace @a ws.
   *
   *  This function checks if a RooAbsReal with the name @a var_name exists
   *  on the workspace @a ws. If this is the case it returns it. If an object of
   *  this name is found but is no RooAbsReal, the program exits. If no object
   *  of this name is found, the other parameters are used to create a
   *  RooRealVar which is subsequently imported to the workspace @a ws. A pointer
   *  to its copy on the workspace is then returned.
   *
   *  Please note that this function will gladly return any object on the 
   *  workspace that is a RooAbsReal. As nearly all objects on the workspace 
   *  will be RooAbsReal, the name getVar might be a bit misleading as it will 
   *  also return PDFs for example.
   *
   *  @param ws the workspace
   *  @param var_name the name of the variable
   *
   *  The following parameters are only relevant if variable @a var_name does not exist on @a ws:
   *  @param var_title the title 
   *  @param init_value initial value 
   *  @param min_value lower end of the value range 
   *  @param max_value upper end of the value range 
   *  @param unit unit of the variable    
   */
  RooAbsReal* getVar(RooWorkspace* ws,
                     const TString& var_name, const TString& var_title, 
                     Double_t init_value, Double_t min_value, Double_t max_value, 
                     const TString& unit);
} }

#endif // PDF2WSSTDCOMMONFUNCS_h
