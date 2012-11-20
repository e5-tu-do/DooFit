#ifndef BUILDER_BOBTHEBUILDER_ABSPDF_H
#define BUILDER_BOBTHEBUILDER_ABSPDF_H

// from STL
#include <algorithm>
#include <map>

// from boost
#include <boost/property_tree/ptree.hpp>
#include <boost/shared_ptr.hpp>

// forward declarations
class RooAbsPdf;
class RooWorkspace;

namespace doofit {
namespace builder { namespace BobTheBuilder {
// forward declaration in namespace
class Parameter;
  
class AbsPdf{
 public:
  AbsPdf();
  virtual ~AbsPdf();
  
  
  virtual void Initialize( const boost::property_tree::ptree::value_type& tree_pdf_head, 
                           const std::string& dim_id, const std::string dim_name, 
                           const std::string& subpdffull_id = "", const std::string comp_id = "") = 0; 
  
  virtual void AddToWorkspace( RooWorkspace* ws ) = 0;
  
  
  
 protected:
  std::string name_;
  std::string desc_;
 
  std::string dim_id_;
  std::string dim_name_;
  
  std::string subpdffull_id_;
  std::string comp_id_;
  
  boost::property_tree::ptree tree_pdf_;
  
  std::map< std::string, boost::shared_ptr<Parameter> > map_params_;
  
 private:
  
};

  
  
}}
}

#endif // BUILDER_BOBTHEBUILDER_ABSPDF_H
