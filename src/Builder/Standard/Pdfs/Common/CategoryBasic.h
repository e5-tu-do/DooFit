#ifndef BUILDER_STANDARD_CATEGORYBASIC_H
#define BUILDER_STANDARD_CATEGORYBASIC_H

// from STL
#include <map>
#include <string>

// from boost
#include <boost/property_tree/ptree.hpp>


namespace Builder { namespace Standard {

class CategoryBasic{
 public:
  CategoryBasic();
  ~CategoryBasic();
  
  void Initialize( const boost::property_tree::ptree::value_type& pt_head );

 protected:


 private:
  std::string name_;
  std::string desc_;
   
  std::map < std::string, int > map_types_;
  
  void CreateTypes();
};


}}
  
#endif // BUILDER_STANDARD_CATEGORYBASIC_H
