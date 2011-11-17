#ifndef BUILDER_STANDARD_PDFFULL_H
#define BUILDER_STANDARD_PDFFULL_H


// from STL
#include <string>

// from boost
#include <boost/property_tree/ptree.hpp>

namespace Builder{ namespace Standard{

class PdfFull{
 public:
  PdfFull();
  ~PdfFull();

  void Initialize( const boost::property_tree::ptree::value_type& pt_head );

 protected:

 private:
  std::string name_;
  std::string desc_; 
  
};
  
  
}}

#endif // BUILDER_STANDARD_PDFFULL_H
