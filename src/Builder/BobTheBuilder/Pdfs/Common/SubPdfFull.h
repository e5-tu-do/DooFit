#ifndef BUILDER_BOBTHEBUILDER_SUBPDFFULL_H
#define BUILDER_BOBTHEBUILDER_SUBPDFFULL_H

// from STL
#include <map>
#include <string>

// from boost
#include <boost/property_tree/ptree.hpp>

// forward declarations
class Component;

class SubPdfFull{
 public:
  SubPdfFull();
  ~SubPdfFull();
  
  void Initialize( const boost::property_tree::ptree &pt );
  
 protected:
 private:
  std::string name_;
  std::map <std::string, Component*> components_;
};



#endif // BUILDER_BOBTHEBUILDER_SUBPDFFULL_H
