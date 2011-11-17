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
  
  const std::map< std::string, int >& map_types(){ return map_types_; };
  
 protected:


 private:
  std::string name_;
  std::string desc_;
  std::string types_;
  
  std::map < std::string, int > map_types_;
  
  void CreateTypes( const std::string &types );
};


}}
  
#endif // BUILDER_STANDARD_CATEGORYBASIC_H
