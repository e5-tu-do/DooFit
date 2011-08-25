/** \namespace Pdf2WsStd
 *  \brief DooFit Pdf2WsStd namespace.
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
 *  \see Pdf2WsStd::CommonFuncs
 *  \see Pdf2WsStd::Mass
 *  \see Pdf2WsStd::Proptime
 *  \see Pdf2WsStd::ProptimeErr
 *  
 */
/** \namespace Pdf2WsStd::CommonFuncs
 *  \brief DooFit Pdf2WsStd::CommonFuncs namespace.
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
  RooAbsReal* getVar(RooWorkspace* ws,
                     const TString& var_name, const TString& var_title, 
                     Double_t init_value, Double_t min_value, Double_t max_value, 
                     const TString& unit);
} }

#endif // PDF2WSSTDCOMMONFUNCS_h
