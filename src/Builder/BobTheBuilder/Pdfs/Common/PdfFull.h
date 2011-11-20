#ifndef BUILDER_BOBTHEBUILDER_PDFFULL_H
#define BUILDER_BOBTHEBUILDER_PDFFULL_H


// from STL
#include <map>
#include <string>

// from boost
#include <boost/property_tree/ptree.hpp>
#include <boost/shared_ptr.hpp>



namespace Builder{ namespace BobTheBuilder{

// forward declarations for project
class AbsDimension;
class CategoryBasic;
class CategorySuper;
class SubPdfFull;
  
  
class PdfFull{
 public:
  PdfFull();
  ~PdfFull();
  
  
  void Initialize( const boost::property_tree::ptree::value_type& pt_head, 
                   const std::map< std::string, boost::shared_ptr<AbsDimension> >& map_dimensions,
                   const std::map< std::string, boost::shared_ptr<CategoryBasic> >& map_cats_basic,
                   const std::map< std::string, boost::shared_ptr<CategorySuper> >& map_cats_super );
  
  
 protected:

 private:
  std::string name_;
  std::string desc_;
  bool extended_;
 
  std::string simcat_;
  bool simultaneous_;
  
  std::map< std::string, boost::shared_ptr< SubPdfFull > > map_subpdfs_;
  
};
  
  
}}

#endif // BUILDER_BOBTHEBUILDER_PDFFULL_H
