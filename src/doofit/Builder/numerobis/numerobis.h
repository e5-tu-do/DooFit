/** @class Numerobis Builder
 *  @brief The Numerobis builder is a not so generic pdf builder.
 *
 *  We'll think about it.
 *  
 *  © Copyright 2012  - Julian Wishahi. All Rights Reserved.
 *
 *  @author Julian Wishahi 
 *  @author $Author$
 *  @date 2012-02-08
 *  @date $Date$
 *  @version $Rev$
 */
#ifndef DOOFIT_BUILDER_NUMEROBIS_NUMEROBIS_H
#define DOOFIT_BUILDER_NUMEROBIS_NUMEROBIS_H

// from STL
#include <map>
#include <string>

// from RooFit
#include "RooCmdArg.h"

// forward declarations
class RooCategory;
class RooRealVar;
class RooWorkspace;

namespace doofit {
// forward declarations in doofit namespace
namespace Config {
// forward declarations in doofit::config namespace
class CommonConfig;
} // namespace config 


namespace builder {
namespace numerobis {

// forward declarations in doofit::builder::numerobis namespace
class NumerobisConfig;

class Numerobis {
 public:
  Numerobis(const Config::CommonConfig& cfg_com, const NumerobisConfig& cfg_num);
  virtual ~Numerobis();
  
  void Configure();
  
  void AddToWorkspace(RooWorkspace& ws);
  
 private:
  RooRealVar* CreateDimReal(const std::string& name, const std::string& desc, double val_min, double val_max, const std::string& unit, const std::string& dim_id);
  RooCategory* CreateDimDisc(const std::string& name, const std::string& desc);
  //RooRealVar* CreateParameterStd(const std::string& name, const std::string& desc, double val_min, double val_max, const std::string unit = "", const std::string& prefix="", const std::string& postfix="");
  
  void PrintNumerobis();
  
  RooWorkspace* workspace_;
  bool is_simultaneous_;
  RooCmdArg import_arg_;
  bool cf_fixed_;
};
  
} // namespace numerobis 
} // namespace builder 
} // namespace doofit 

#endif // DOOFIT_BUILDER_NUMEROBIS_NUMEROBIS_H