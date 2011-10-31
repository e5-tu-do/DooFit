/** \class BuilderS2b
 *  \brief Standard Builder. Adds Pdf object to a workspace.
 *
 *  This Builder class is meant as standard implementation. The configuration in
 *  the BuilderS2bConfig class is used to create the correct Pdfs on a workspace.
 *  \see BuilderS2bConfig
 */

#ifndef BUILDERS2B_h
#define BUILDERS2B_h

// forward declarations
class CommonConfig; 
class BuilderS2bConfig;

class BuilderS2b{
 public:
  BuilderS2b(const CommonConfig& cfg_com, const BuilderS2bConfig& cfg_bld);
  ~BuilderS2b();
  
  void build();
  
 protected:
  
 private:
  void build_vars();
  void build_special_pars();
  
  void build_mass();
  void build_omega();
  void build_proptime_resol();
  void build_proptime();
  
  
};


#endif // BUILDERS2B_h
