#ifndef BUILDER_BOBTHEBUILDER_ABSPDF_H
#define BUILDER_BOBTHEBUILDER_ABSPDF_H

// from STL
#include <algorithm>

// from boost
#include <boost/algorithm/string.hpp>
#include <boost/property_tree/ptree.hpp>

// forward declarations
class RooAbsPdf;
class RooWorkspace;

namespace Builder { namespace BobTheBuilder {

class AbsPdf{
 public:
  AbsPdf();
  virtual ~AbsPdf();
  
  
  virtual void Initialize( const boost::property_tree::ptree::value_type& tree_pdf_head, 
                           const std::string& dim_id, const std::string dim_name, 
                           const std::string& simcat_id = "", const std::string comp_id = "") = 0;
  virtual void AddToWorkspace( RooWorkspace* ws ) = 0;
  
  
  
 protected:
  std::string name_;
  std::string desc_;
 
  std::string dim_id_;
  std::string dim_name_;
  
  std::string simcat_id_;
  std::string comp_id_;
  
  std::pair< std::string, std::string > ParseParameter( std::string parameter_string );
 private:
  
};

  
  
}}

#endif // BUILDER_BOBTHEBUILDER_ABSPDF_H
