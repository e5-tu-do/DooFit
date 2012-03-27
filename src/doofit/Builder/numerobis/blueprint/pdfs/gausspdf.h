#ifndef DOOFIT_BUILDER_NUMEROBIS_BLUEPRINT_PDFS_GAUSSPDF_H
#define DOOFIT_BUILDER_NUMEROBIS_BLUEPRINT_PDFS_GAUSSPDF_H

// from STL
#include <string>

//from project
#include "genpdf.h"

// forward declaration
class RooGaussian;

namespace doofit {
namespace builder {
namespace numerobis {
namespace blueprint {
namespace pdfs {

class GaussPdf : public GenPdf {
 public:
  /**
   *  @brief Default constructor for GaussPdf
   */
  GaussPdf();
  
  /**
   *  @brief Constructor for GaussPdf with arguments
   *
   *  @param id_rel relative id of the object
   *  @param id_abs absolute id of the object
   *  @param id_abs_dimension absolute id of the dimension
   *  @param id_abs_mean absolute id of the mean
   *  @param id_abs_sigma absolute id of the sigma
   
   */
  GaussPdf(const std::string& id_rel, const std::string& id_abs, const std::string& id_abs_dimension, const std::string& id_abs_mean, const std::string& id_abs_sigma);
  
  virtual ~GaussPdf();
 protected:
  /** @brief Create instance of this element as RooFit object
   *  
   *  Initialize the appropriate RooFit object and return it as RooAbsPdf 
   *  pointer.
   *
   *  @param dependants the dependants to be used for initialization
   *  @return the temporary initialized object to be put onto a workspace
   */
  virtual RooAbsPdf* CreateTempRooObj(const std::map<std::string, RooAbsArg*>& dependants);
  
  /** @brief Create instance of this element as RooFit object
   *  
   *  Initialize the appropriate RooFit object and return it as RooAbsArg 
   *  pointer.
   *
   *  @param dependants the dependants to be used for initialization
   *  @return the temporary initialized object to be put onto a workspace
   */
  virtual RooAbsPdf* GetRooObjFromWorkspace(RooWorkspace* ws);
  
 private:
  /**
   *  @brief Pointer to RooFit object on workspace
   */
  RooGaussian* roo_obj_;
};

} // namespace pdfs
} // namespace blueprint 
} // namespace numerobis 
} // namespace builder 
} // namespace doofit

#endif // DOOFIT_BUILDER_NUMEROBIS_BLUEPRINT_PDFS_GAUSSPDF_H