#ifndef BUILDER_BOBTHEBUILDER_COMPONENT_H
#define BUILDER_BOBTHEBUILDER_COMPONENT_H

// from STL
#include <map>
#include <string>

// from boost
#include <boost/property_tree/ptree.hpp>

// forward declarations
class RooWorkspace;

namespace Builder { namespace BobTheBuilder {

// forward declarations
class AbsDimension;
class AbsPdf;

class Component{
 public:
  Component();
  ~Component();
  
  void Initialize( const boost::property_tree::ptree::value_type& tree_component_head, 
                   const std::map< std::string,boost::shared_ptr<AbsDimension> >& map_dimensions, 
                   const std::string& subpdffull_id = "" );
  
  void AddToWorkspace( RooWorkspace & );
  
 protected:
  
 private:
  std::string comp_id_;
  std::string comp_id_full_;
  std::string name_;
  std::string desc_;
  bool        extended_;
  std::string subpdffull_id_;
  
  
  std::map< std::string, boost::shared_ptr<AbsPdf> > map_dimpdfs_;
  std::map< std::string, boost::shared_ptr<AbsDimension> > map_dimensions_;
  
  void CreateDimPdfs(const boost::property_tree::ptree& tree_compdimensions);
};

}}

#endif // BUILDER_BOBTHEBUILDER_COMPONENT_H
