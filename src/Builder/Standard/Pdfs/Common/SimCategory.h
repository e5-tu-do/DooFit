#ifndef SIMCATEGORY_H
#define SIMCATEGORY_H

// from STL
#include <map>
#include <string>

// from boost
#include <boost/property_tree/ptree.hpp>

// forward declarations
class Component;

class SimCategory{
 public:
  SimCategory( const boost::property_tree::ptree &pt );
  ~SimCategory();
  
  void CreatePdf();
  
 protected:
 private:
  std::string name_;
  std::map <std::string, Component*> components_;
};



#endif // SIMCATEGORY_H
