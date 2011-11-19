#ifndef BUILDER_BOBTHEBUILDER_PDFFULL_H
#define BUILDER_BOBTHEBUILDER_PDFFULL_H


// from STL
#include <map>
#include <string>

// from boost
#include <boost/property_tree/ptree.hpp>
#include <boost/shared_ptr.hpp>

// from project
#include "Builder/BobTheBuilder/Pdfs/Common/SubPdfFull.h"

namespace Builder{ namespace BobTheBuilder{

class PdfFull{
 public:
  PdfFull();
  ~PdfFull();

  void Initialize( const boost::property_tree::ptree::value_type& pt_head  );

 protected:

 private:
  std::string name_;
  std::string desc_; 
  bool extended_;
  
  std::map< std::string, boost::shared_ptr< SubPdfFull > > map_suppdfs_;
  
};
  
  
}}

#endif // BUILDER_BOBTHEBUILDER_PDFFULL_H
