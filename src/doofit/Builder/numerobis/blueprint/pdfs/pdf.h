#ifndef DOOFIT_BUILDER_NUMEROBIS_BLUEPRINT_PDFS_PDF_H
#define DOOFIT_BUILDER_NUMEROBIS_BLUEPRINT_PDFS_PDF_H

// from STL
#include <map>
#include <string>


// forward declarations - RooFit
class RooAbsArg;
class RooAbsPdf;
class RooWorkspace;

namespace doofit {
namespace builder {
namespace numerobis {
namespace blueprint {
namespace pdfs {

class Pdf {
 public:
   Pdf();
   Pdf(const std::string& id_rel, const std::string& id_abs);
   virtual ~Pdf();

   /** Getter for id_rel_ */
   std::string id_rel() const { return id_rel_; }
   /** Setter for id_rel_ */
   void set_id_rel(const std::string& id_rel) { id_rel_ = id_rel; } 

   /** Getter for id_abs_ */
   std::string id_abs() const { return id_abs_; }
   /** Setter for id_abs_ */
   void set_id_abs(const std::string& id_abs) { id_abs_ = id_abs; } 

   /** @brief Adds RooFit representation to RooWorkspace
    *
    *  This function will initialize the appropriate RooFit object and import 
    *  this to the supplied workspace. A map of the dependants as RooAbsArg* 
    *  must be supplied with the relative dependants' ids as keys.
    *
    *  @param ws the workspace to add the object to
    *  @param dependants the dependants to be used for initialization
    *  @return the RooAbsPdf on the workspace
    */
   RooAbsPdf* AddToWorkspace(RooWorkspace* ws, const std::map<std::string,RooAbsArg*>& dependants);

   /**
    *  @brief Get list of dependants
    *
    *  This function will return the list of dependants as 
    *  std::map<std::string,std::string> with relative IDs as key and absolute 
    *  IDs as value.
    *
    *  @return the dependants of this element
    */
   const std::map<std::string, std::string>& dependants() const { return dependants_; }

   /** Getter for initialized_ */
   bool initialized() const { return initialized_; }
   /** Setter for initialized_ */
   void set_initialized(bool initialized) { initialized_ = initialized; } 

   /** Getter for onworkspace_ */
   bool onworkspace() const { return onworkspace_; }
   /** Setter for onworkspace_ */
   void set_onworkspace(bool onworkspace) { onworkspace_ = onworkspace; } 

   /** Getter for ready_ */
   bool ready() const { return ready_; }
   /** Setter for ready_ */
   void set_ready(bool ready) { ready_ = ready; }

  protected:
   /** @brief Create instance of this element as RooFit object
    *  
    *  Initialize the appropriate RooFit object and return it as RooAbsPdf 
    *  pointer.
    *
    *  @param dependants the dependants to be used for initialization
    *  @return the temporary initialized object to be put onto a workspace
    */
   virtual RooAbsPdf* CreateTempRooObj(const std::map<std::string, RooAbsArg*>& dependants) = 0;

   /** @brief Create instance of this element as RooFit object
    *  
    *  Initialize the appropriate RooFit object and return it as RooAbsArg 
    *  pointer.
    *
    *  @param dependants the dependants to be used for initialization
    *  @return the temporary initialized object to be put onto a workspace
    */
   virtual RooAbsPdf* GetRooObjFromWorkspace(RooWorkspace* ws) = 0;

   /**
    *  @brief List of dependants
    */
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

#endif // DOOFIT_BUILDER_NUMEROBIS_BLUEPRINT_PDFS_PDF_H
