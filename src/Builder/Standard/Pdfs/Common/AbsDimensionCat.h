#ifndef BUILDER_STANDARD_ABSDIMENSIONCAT_H
#define BUILDER_STANDARD_ABSDIMENSIONCAT_H

// from STL
#include <map>
#include <string>

// from boost


// from project
#include "Builder/Standard/Pdfs/Common/AbsDimension.h"

namespace Builder { namespace Standard {

class AbsDimensionCat : public AbsDimension{
 public:
  AbsDimensionCat();
  virtual ~AbsDimensionCat();
  
  void Initialize(  const boost::property_tree::ptree::value_type &pt_head );
  
 protected:
  std::string types_;
  std::map < std::string, int > map_types_;
  
  
 private:
  void CreateTypes( const std::string &type_string );
};
  
} }



#endif // BUILDER_STANDARD_ABSDIMENSIONCAT_H
