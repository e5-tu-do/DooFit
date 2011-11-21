#ifndef BUILDER_BOBTHEBUILDER_SUBPDFFULL_H
#define BUILDER_BOBTHEBUILDER_SUBPDFFULL_H

// from STL
#include <map>
#include <string>

// from boost
#include <boost/property_tree/ptree.hpp>
#include <boost/shared_ptr.hpp>


namespace Builder{ namespace BobTheBuilder{
// forward declarations for project classes
class AbsDimension;
class Component;

class SubPdfFull{
 public:
  SubPdfFull();
  ~SubPdfFull();
  
  void Initialize( const boost::property_tree::ptree& tree_subpdffull, 
                   const std::map< std::string, boost::shared_ptr<AbsDimension> >& map_dimensions, const std::string& simcat_type = "" );
  
 protected:
  
 private:
  std::string simcat_type_;
  std::string simcat_type_stripped_;
  std::string name_;
  std::string desc_;
  bool        extended_;
  std::map <std::string, Component*> components_;
  
  void InitializeComponents(const boost::property_tree::ptree& tree_components, 
                            const std::map< std::string, boost::shared_ptr<AbsDimension> >& map_dimensions);
};

}}

#endif // BUILDER_BOBTHEBUILDER_SUBPDFFULL_H
