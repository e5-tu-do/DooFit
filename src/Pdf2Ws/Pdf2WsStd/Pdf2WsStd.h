/** \class Pdf2WsStd
 *  \brief Standard Pdf2Ws. Adds Pdf object to a workspace.
 *
 *  This Pdf2Ws class is meant as standard implementation. The configuration in
 *  the Pdf2WsStdConfig class is used to create the correct Pdfs on a workspace.
 *  \see Pdf2WsStdConfig
 */

#ifndef PDF2WSSTD_h
#define PDF2WSSTD_h

// forward declarations
class CommonConfig; 
class Pdf2WsStdConfig;

class Pdf2WsStd{
 public:
  Pdf2WsStd(const CommonConfig&, const Pdf2WsStdConfig&);
  ~Pdf2WsStd();
  
 protected:
  
 private:
};


#endif // PDF2WSSTD_h
