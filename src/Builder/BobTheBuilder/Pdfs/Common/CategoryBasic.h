#ifndef BUILDER_BOBTHEBUILDER_CATEGORYBASIC_H
#define BUILDER_BOBTHEBUILDER_CATEGORYBASIC_H

// from STL
#include <map>
#include <string>
#include <vector>

// from boost
#include <boost/property_tree/ptree.hpp>

// forward declarations
class RooWorkspace;


namespace Builder { namespace BobTheBuilder {

class CategoryBasic{
 public:
  CategoryBasic();
  ~CategoryBasic();
  
  void Initialize( const boost::property_tree::ptree::value_type& pt_head );
  bool AddToWorkspace( RooWorkspace* ws);
  
  const std::map< std::string, int >& map_types(){ return map_types_; };
  
  /** 
   *  @brief Returns a vector of category types
   *
   */
  std::vector< std::string > GetTypesVector();
  
 protected:


 private:
  std::string name_;
  std::string desc_;
  std::string types_;
  
  std::map < std::string, int > map_types_;
  
  void CreateTypes( const std::string &types );
};


}}
  
#endif // BUILDER_BOBTHEBUILDER_CATEGORYBASIC_H
