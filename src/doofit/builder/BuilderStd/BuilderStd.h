/** @class doofit::BuilderStd
 *  @brief Standard Builder. Adds Pdf object to a workspace.
 *
 *  This Builder class is meant as standard implementation. The configuration in
 *  the BuilderStdConfig class is used to create the correct Pdfs on a workspace.
 *  @see BuilderStdConfig
 */

#ifndef BUILDERSTD_h
#define BUILDERSTD_h

namespace doofit {
// forward declarations
class CommonConfig; 
class BuilderStdConfig;

class BuilderStd{
 public:
  BuilderStd(const CommonConfig& cfg_com, const BuilderStdConfig& cfg_bld);
  ~BuilderStd();
  
 protected:
  
 private:
};
}

#endif // BUILDERSTD_h
