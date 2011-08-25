/** \class BuilderStd
 *  \brief Standard Builder. Adds Pdf object to a workspace.
 *
 *  This Builder class is meant as standard implementation. The configuration in
 *  the BuilderStdConfig class is used to create the correct Pdfs on a workspace.
 *  \see BuilderStdConfig
 */

#ifndef BUILDERSTD_h
#define BUILDERSTD_h

// forward declarations
class CommonConfig; 
class BuilderStdConfig;

class BuilderStd{
 public:
  BuilderStd(const CommonConfig&, const BuilderStdConfig&);
  ~BuilderStd();
  
 protected:
  
 private:
};


#endif // BUILDERSTD_h
