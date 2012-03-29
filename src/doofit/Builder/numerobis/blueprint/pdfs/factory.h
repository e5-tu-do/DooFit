#ifndef DOOFIT_BUILDER_NUMEROBIS_BLUEPRINT_PDFS_FACTORY_H
#define DOOFIT_BUILDER_NUMEROBIS_BLUEPRINT_PDFS_FACTORY_H

// from STL
#include <string>

// from boost
#include <boost/ptr_container/ptr_map.hpp>

// from project
#include "doofit/Builder/numerobis/blueprint/pdfs/registrar.h"
#include "doofit/Builder/numerobis/blueprint/pdfs/gausspdf.h"

namespace doofit {
namespace builder {
namespace numerobis {
namespace blueprint {
namespace pdfs {

// forward declarations
class Registrar;

/** @class Factory
 *  @brief The PDFs factory
 *
 *  In this first iteration this factory will merely function as a wrapper for
 *  PDF's constructors.
**/  
class Factory {
 public:
  /**
   *  @brief Constructor for this Factory with the Registrar to use
   *
   *  @param registrar the Registrar for registering the Elements
  **/
  Factory(Registrar& registrar);
  
  /**
   *  @brief Destructor
  **/
  virtual ~Factory ();
  
  /**
   *  @brief Assemble function for GaussPdf
   *
   *  @param id_rel relative id of the object
   *  @param id_abs absolute id of the object
   *  @param id_abs_dimension absolute ID of the dimension to use
   *  @param id_abs_mean absolute ID of the mean to use
   *  @param id_abs_sigma absolute ID of the sigma to use
  **/
  void AssembleGaussPdf(const std::string& id_rel, const std::string& id_abs, const std::string& id_abs_dimension, const std::string& id_abs_mean, const std::string& id_abs_sigma) {
    registrar_.Declare(new GaussPdf(id_rel, id_abs, id_abs_dimension, 
                                    id_abs_mean, id_abs_sigma));
  }
  
 protected:
  
 private:
  /**
   *  @brief Brief description
  **/
  Registrar& registrar_;
};

} // namespace pdfs 
} // namespace blueprint 
} // namespace numerobis 
} // namespace builder 
} // namespace doofit 

#endif // DOOFIT_BUILDER_NUMEROBIS_BLUEPRINT_PDFS_FACTORY_H
