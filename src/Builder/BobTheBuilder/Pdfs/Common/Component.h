#ifndef BUILDER_BOBTHEBUILDER_COMPONENT_H
#define BUILDER_BOBTHEBUILDER_COMPONENT_H

// from STL
#include <map>
#include <string>

// from boost
#include <boost/property_tree/ptree.hpp>

namespace Builder { namespace BobTheBuilder {

// forward declarations
class AbsDimension;

class Component{
 public:
  Component();
  ~Component();
  
  void Initialize( const boost::property_tree::ptree::value_type& tree_component_head, 
                   const std::map< std::string,boost::shared_ptr<AbsDimension> >& map_dimensions, 
                   const std::string& subpdffull_id = "" );
  
 protected:
  
 private:
  std::string comp_id_;
  std::string comp_id_full_;
  std::string name_;
  std::string desc_;
  bool extended_;
  
  std::map< std::string, boost::shared_ptr<AbsDimension> > dimensions_;
  
  void CreateDimPdfs();
};

}}

#endif // BUILDER_BOBTHEBUILDER_COMPONENT_H
