#ifndef BUILDER_BOBTHEBUILDER_CATEGORYSUPER_H
#define BUILDER_BOBTHEBUILDER_CATEGORYSUPER_H

// from STL
#include <vector>
#include <map>
#include <string>

// from boost
#include <boost/property_tree/ptree.hpp>
#include <boost/shared_ptr.hpp>

// forward declarations
class RooWorkspace;

namespace doofit {
namespace Builder { namespace BobTheBuilder {

// forward declarations in project
  class CategoryBasic;

class CategorySuper{
 public:
  CategorySuper();
  ~CategorySuper();
  
  void Initialize( const boost::property_tree::ptree::value_type& pt_head, const std::map< std::string, boost::shared_ptr< CategoryBasic > > &map_category_basic );
  bool AddToWorkspace( RooWorkspace& ws );
  
  
  /** 
   *  @brief Returns a vector of category types
   *
   */
  std::vector< std::string > GetTypesVector(){ return vector_types_;}
  
 protected:
  
 private:
  std::string name_;
  std::string desc_;
  std::string subcats_;
  std::vector< std::string > vector_subcats_;
  std::vector< std::string > vector_types_;
  
  void CreateTypes( const std::string &subcats, const std::map< std::string, boost::shared_ptr< CategoryBasic > > &map_category_basic );
  std::vector< std::string > CombineTypes( const std::vector< std::string > &subcat_one_types,  const std::vector< std::string > &subcat_two_types);
  
  
};
  
}}
}
#endif // BUILDER_BOBTHEBUILDER_CATEGORYSUPER_H
