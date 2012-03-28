#ifndef DOOFIT_BUILDER_NUMEROBIS_BLUEPRINT_PDFS_PDF_H
#define DOOFIT_BUILDER_NUMEROBIS_BLUEPRINT_PDFS_PDF_H

// from STL
#include <map>
#include <string>

// from project
#include "doofit/utils//MsgStream.h"

// forward declarations - RooFit
class RooAbsArg;
class RooAbsPdf;
class RooWorkspace;

namespace doofit {
namespace builder {
namespace numerobis {
namespace blueprint {
namespace pdfs {
  
/** @namespace doofit::builder::numerobis::blueprint::pdfs
 *
 *  @brief PDF namespace in Numerobis/Blueprint
 *
 *  All elements which represent PDFs are grouped into this workspace as well 
 *  as the approprate Registrar and Factory acting as interface to outside 
 *  code.
 */

/**
 *  @class doofit::builder::numerobis::blueprint::pdfs::Pdf
 *  @brief The elements mother class for all PDFs
 *
 *  This abstract class is base class for all PDFs used in 
 *  doofit::builder::numerobis::blueprint. It carries relative and absolute IDs
 *  as well as different states of readyness. Also the 
 *  Pdf::AddToWorkspace(RooWorkspace*, const std::map<std::string,RooAbsArg*>&) 
 *  function is supplied to add the RooFit object to a RooWorkspace.
 */
class Pdf {
 public:
   Pdf(const std::string& id_rel, const std::string& id_abs);
   virtual ~Pdf();

  /** @name Getter/Setters for IDs
   */
  ///@{
  /** 
   * @brief Getter for id_rel_
   **/
  std::string id_rel() const { return id_rel_; }
  /** 
   * @brief Setter for id_rel_
   **/
  void set_id_rel(const std::string& id_rel) { id_rel_ = id_rel; } 
  
  /** 
   * @brief Getter for id_abs_
   **/
  std::string id_abs() const { return id_abs_; }
  /** 
   * @brief Setter for id_abs_
   **/
  void set_id_abs(const std::string& id_abs) { id_abs_ = id_abs; } 
  ///@}

   /** @brief Adds RooFit representation to RooWorkspace
    *
    *  This function will initialize the appropriate RooFit object and import 
    *  this to the supplied workspace. A map of the dependants as RooAbsArg* 
    *  must be supplied with the relative dependants' ids as keys.
    *
    *  @param ws the workspace to add the object to
    *  @param dependants the dependants to be used for initialization
    *  @return the RooAbsPdf on the workspace
   **/
   RooAbsPdf* AddToWorkspace(RooWorkspace* ws, const std::map<std::string,RooAbsArg*>& dependants);

   /**
    *  @brief Get list of dependants
    *
    *  This function will return the list of dependants as 
    *  std::map<std::string,std::string> with relative IDs as key and absolute 
    *  IDs as value.
    *
    *  @return the dependants of this element
   **/
   const std::map<std::string, std::string>& dependants() const { return dependants_; }

  /** @name Getter/Setters for readyness state
   */
  ///@{
  /** 
   * @brief Getter for initialized_
   **/
  bool initialized() const { return initialized_; }
  /** 
   * @brief Setter for initialized_
   **/
  void set_initialized(bool initialized) { initialized_ = initialized; } 
  
  /** 
   * @brief Getter for onworkspace_
   **/
  bool onworkspace() const { return onworkspace_; }
  /** 
   * @brief Setter for onworkspace_
   **/
  void set_onworkspace(bool onworkspace) { onworkspace_ = onworkspace; } 
  
  /** 
   * @brief Getter for ready_
   **/
  bool ready() const { return ready_; }
  /** 
   * @brief Setter for ready_
   **/
  void set_ready(bool ready) { ready_ = ready; }
  ///@}

  protected:
   /** @brief Create instance of this element as RooFit object
    *  
    *  Initialize the appropriate RooFit object and return it as RooAbsPdf 
    *  pointer.
    *
    *  @param dependants the dependants to be used for initialization
    *  @return the temporary initialized object to be put onto a workspace
   **/
   virtual RooAbsPdf* CreateTempRooObj(const std::map<std::string, RooAbsArg*>& dependants) = 0;

   /** @brief Create instance of this element as RooFit object
    *  
    *  Initialize the appropriate RooFit object and return it as RooAbsArg 
    *  pointer.
    *
    *  @param dependants the dependants to be used for initialization
    *  @return the temporary initialized object to be put onto a workspace
   **/
   virtual RooAbsPdf* GetRooObjFromWorkspace(RooWorkspace* ws) = 0;

   /**
    *  @brief List of dependants
   **/
   std::map<std::string,std::string> dependants_;

 
 private:
  std::string id_rel_;
  std::string id_abs_;
  
  bool initialized_;
  bool ready_;
  bool onworkspace_;
};

} // namespace pdfs
} // namespace blueprint 
} // namespace numerobis 
} // namespace builder 
} // namespace doofit

namespace doofit {
namespace utils {
/**
 *  @brief Function to output Numerobis PDFs directly and nicely into 
 *         MsgStreams
 *
 *  This function just prints all information in a PDF nicely.
**/
inline MsgStream& operator<<(MsgStream& lhs, const doofit::builder::numerobis::blueprint::pdfs::Pdf& pdf) {
  lhs.stream() << "PDF: " << pdf.id_abs();
  
  if (pdf.initialized()) {
    lhs.stream() << " initialized";
  }
  if (pdf.ready()) {
    lhs.stream() << " ready";
  }
  if (pdf.onworkspace()) {
    lhs.stream() << " on workspace";
  }
  
  return lhs;
}
} // namespace utils
} // namespace doofit

#endif // DOOFIT_BUILDER_NUMEROBIS_BLUEPRINT_PDFS_PDF_H
