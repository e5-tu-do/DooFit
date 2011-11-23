#ifndef BUILDER_BOBTHEBUILDER_ABSDIMENSIONCAT_H
#define BUILDER_BOBTHEBUILDER_ABSDIMENSIONCAT_H

// from STL
#include <map>
#include <string>

// from boost


// from project
#include "Builder/BobTheBuilder/Pdfs/Common/AbsDimension.h"

namespace Builder { namespace BobTheBuilder {

class AbsDimensionCat : public AbsDimension{
 public:
  AbsDimensionCat();
  virtual ~AbsDimensionCat();
  
  void Initialize(  const boost::property_tree::ptree::value_type &pt_head );
  bool AddToWorkspace( RooWorkspace* ws );
  
  virtual boost::shared_ptr<AbsPdf> CreatePdf( const std::string& pdf_name) = 0;
  
 protected:
  std::string types_;
  std::map < std::string, int > map_types_;
  
  
 private:
  void CreateTypes( const std::string &type_string );
};
  
} }



#endif // BUILDER_BOBTHEBUILDER_ABSDIMENSIONCAT_H
