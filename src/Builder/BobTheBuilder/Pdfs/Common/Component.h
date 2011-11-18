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
  
  void Initialize( const boost::property_tree::ptree::value_type &pt_head, std::map< std::string,boost::shared_ptr<AbsDimension> >* map_dimensions );
  
 protected:
  
 private:
  bool initialized_;
  bool extended_;
  
  std::string name_;
  std::map< std::string,boost::shared_ptr<AbsDimension> >* dimensions_;
};

}}

#endif // BUILDER_BOBTHEBUILDER_COMPONENT_H
