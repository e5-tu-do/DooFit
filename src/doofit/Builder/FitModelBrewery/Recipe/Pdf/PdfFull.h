#ifndef BUILDER_FITMODELBREWERY_RECIPE_PDF_FULLPDF_H
#define BUILDER_FITMODELBREWERY_RECIPE_PDF_FULLPDF_H

// from STL
#include <map>
#include <string>

namespace DooFit {
namespace builder {
namespace fitmodelbrewery {

// forward declarations
class AbsPdf;
class AbsCategory;

class PdfFull
{
 public:
  PdfFull();
  virtual ~PdfFull();
  
  /** @brief Setter and getters for name */
  std::string name() const { return name_;}
  void set_name(const std::string& name) { name_ = name; }
  
  /** @brief Setter and getters for desc */
  std::string desc() const { return desc_;}
  void set_desc(const std::string& desc) { desc_ = desc; }
  
  /** @brief Setter and getters for is_simultaneous */
  bool is_simultaneous() const { return is_simultaneous_;}
  void set_is_simultaneous(bool is_simultaneous) { is_simultaneous_ = is_simultaneous; }
  
  /** @brief Setter and getters for is_extended */
  bool is_extended() const { return is_extended_;}
  void set_is_extended(bool is_extended) { is_extended_ = is_extended; }
  
 protected:
 private:
  std::string name_;
  std::string desc_;
  bool is_simultaneous_;
  bool is_extended_;
  
};

} // namespace fitmodelbrewery 
} // namespace builder
} // namespace DooFit 

#endif // BUILDER_FITMODELBREWERY_RECIPE_PDF_FULLPDF_H
