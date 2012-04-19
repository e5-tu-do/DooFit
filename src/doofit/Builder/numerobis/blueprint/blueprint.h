#ifndef DOOFIT_BUILDER_NUMEROBIS_BLUEPRINT_BLUEPRINT_H
#define DOOFIT_BUILDER_NUMEROBIS_BLUEPRINT_BLUEPRINT_H

// from STL
#include <string>

// from boost
#include <boost/ptr_container/ptr_map.hpp>

// from project
#include "doofit/Builder/numerobis/blueprint/elements/registrar.h"
#include "doofit/Builder/numerobis/blueprint/elements/factory.h"
#include "doofit/Builder/numerobis/blueprint/pdfs/registrar.h"
#include "doofit/Builder/numerobis/blueprint/pdfs/factory.h"
#include "doofit/Builder/numerobis/blueprint/component.h"
#include "doofit/Builder/numerobis/blueprint/dimension.h"

// forward declarations
class RooWorkspace;

namespace doofit {
namespace builder {
namespace numerobis {
namespace blueprint {

class Blueprint {
 public:
  Blueprint();
  virtual ~Blueprint();
  
  /** 
   * @brief Getter for reg_elements_
   **/
  elements::Registrar& reg_elements() { return reg_elements_; }
  /** 
   * @brief Getter for fac_elements_
   **/
  elements::Factory& fac_elements() { return fac_elements_; }
  /** 
   * @brief Getter for reg_pdfs_
   **/
  pdfs::Registrar& reg_pdfs() { return reg_pdfs_; }
  /** 
   * @brief Getter for fac_pdfs_
   **/
  pdfs::Factory& fac_pdfs() { return fac_pdfs_; }

  /**
   *  @brief Assembly for dimension with one PDF
   *
   *  @param id_rel this Dimension's relative ID
   *  @param id_abs this Dimension's absolute ID
   *  @param id_abs_dimelement the DimReal element's ID to use
   *  @param id_abs_pdf corresponding Pdf for this dimension
   */
  void AssembleDimension(const std::string& id_rel, const std::string& id_abs, const std::string& id_abs_dimelement, const std::string& id_abs_pdf) {
    Dimension* dim = new Dimension(id_rel, id_abs, id_abs_dimelement, id_abs_pdf);
    std::string temp_key = dim->id_abs();
    dimensions_.insert(temp_key, dim);
  }
  
  /**
   *  @brief Assembly for component with one dimension
   *
   *  @param id_rel this Component's relative ID
   *  @param id_abs this Component's absolute ID
   *  @param id_abs_dimensions vector of absolute IDs of dimensions to use
   *  @param id_abs_yield absolute ID of the yield element (if omitted, 
   *                      will assume non extended component)
   */
  void AssembleComponent(const std::string& id_rel, const std::string& id_abs, 
                         const std::vector<std::string>& id_abs_dimensions,
                         const std::string& id_abs_yield="") {
    Component* com = new Component(id_rel, id_abs, id_abs_dimensions, id_abs_yield);
    std::string temp_key = com->id_abs();
    comps_.insert(temp_key, com);
  }

  /**
   *  @brief Register all dimensions
   *
   *  This function will try to register all dimensions (by trying to register
   *  all dependants in the process). If all dimensions could be registered 
   *  properly, it will return true.
   *
   *  @param workspace the workspace to register all elements and PDFs to
   *  @return true if all dependants and therefore all dimensions could be registered
   */
  bool RegisterDimensions(RooWorkspace* workspace);
  
  /**
   *  @brief Register all components
   *
   *  This function will try to register all components (by trying to register
   *  all dependants in the process). If all components could be registered 
   *  properly, it will return true.
   *
   *  @param workspace the workspace to register all elements and PDFs to
   *  @return true if all dependants and therefore all components could be registered
   */
  bool RegisterComponents(RooWorkspace* workspace);
  
 private:
  
  /**
   *  @brief Register one dimension
   *
   *  This function will try to register one dimensions (by trying to register
   *  all dependants in the process). If all dependants could be registered 
   *  properly, it will return true.
   *
   *  @param workspace the workspace to register all elements and PDFs to
   *  @param id_abs_dim the absolute ID of the dimension to register   
   *  @return true if all dependants and therefore all dimensions could be registered
   */
  bool RegisterDimension(RooWorkspace* workspace, const std::string& id_abs_dim);
  
  /**
   *  @brief Register one components
   *
   *  This function will try to register one components (by trying to register
   *  all dependants in the process). If all dependants could be registered 
   *  properly, it will return true.
   *
   *  @param workspace the workspace to register all elements and PDFs to
   *  @param id_abs_comp the absolute ID of the component to register   
   *  @return true if all dependants and therefore all components could be registered
   */
  RooAbsArg* RegisterComponent(RooWorkspace* workspace, const std::string& id_abs_comp);
  
  elements::Registrar reg_elements_;
  elements::Factory   fac_elements_;
  
  pdfs::Registrar     reg_pdfs_;
  pdfs::Factory       fac_pdfs_;
  
  boost::ptr_map<std::string, Component> comps_;
  boost::ptr_map<std::string, Dimension> dimensions_;
  
  // std::string pdf_pre_;
  // std::string var_pre_;
  // std::string par_pre_;
  
  // map names
  
  // map dimensionsreal
  // map dimensionscat
  // map categories
  // map supercategories
  // map acceptances
  // map pdfs
  // map resols
  // map pdfswresol
  
  // map samepar
  
  /* data**/
  
  
};

/**
 *  @brief Convert a vector of RooAbsArgs to a RooArgList
 *
 *  @param vec the vector to convert
 *  @return the RooArgList based on the vector
**/
RooArgList VectorToArgList(std::vector<RooAbsArg*> vec);




} // namespace blueprint 
} // namespace numerobis 
} // namespace builder 
} // namespace doofit 


#endif // DOOFIT_BUILDER_NUMEROBIS_BLUEPRINT_BLUEPRINT_H