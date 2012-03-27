#include "gausspdf.h"

// from STL
#include <string>
#include <map>

// from RooFit
#include "RooAbsReal.h"
#include "RooGaussian.h"

namespace doofit {
namespace builder {
namespace numerobis {
namespace blueprint {
namespace pdfs {

GaussPdf::GaussPdf() {
  set_initialized(false);
  set_ready(false);  
}

GaussPdf::GaussPdf(const std::string& id_rel, const std::string& id_abs, const std::string& id_abs_dimension, const std::string& id_abs_mean, const std::string& id_abs_sigma)
  : GenPdf(id_rel, id_abs)
{
  set_initialized(true);
  set_ready(false);
  
  dependants_["dimension"] = id_abs_dimension;
  dependants_["mean"] = id_abs_mean;
  dependants_["sigma"] = id_abs_sigma;
}
  
GaussPdf::~GaussPdf() {
  
}

RooAbsPdf* GaussPdf::CreateTempRooObj(const std::map<std::string, RooAbsArg*>& dependants) {
  std::map<std::string, RooAbsArg*>::const_iterator it;
  RooAbsReal* dimension = NULL;
  RooAbsReal* mean      = NULL;
  RooAbsReal* sigma     = NULL;
  
  it = dependants.find("dimension");
  if (it != dependants.end()) dimension = dynamic_cast<RooAbsReal*>(it->second);
  it = dependants.find("mean");
  if (it != dependants.end()) mean = dynamic_cast<RooAbsReal*>(it->second);
  it = dependants.find("sigma");
  if (it != dependants.end()) sigma = dynamic_cast<RooAbsReal*>(it->second);
  
  RooGaussian* temp_gaussian = NULL;
  temp_gaussian = new RooGaussian(id_abs().c_str(), id_abs().c_str(), *dimension, *mean, *sigma);
  return temp_gaussian;
}

RooAbsPdf* GaussPdf::GetRooObjFromWorkspace(RooWorkspace* ws) {
  //roo_obj_ = ws->pdf(id_abs().c_str());
  //return dynamic_cast<RooGaussian*>(roo_obj_);
  return NULL;
}

} // namespace pdfs
} // namespace blueprint 
} // namespace numerobis 
} // namespace builder 
} // namespace doofit
