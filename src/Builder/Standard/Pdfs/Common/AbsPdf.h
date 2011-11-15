#ifndef BUILDER_STANDARD_ABSPDF_H
#define BUILDER_STANDARD_ABSPDF_H

// from boost
#include <boost/property_tree/ptree.hpp>

// forward declarations
class RooAbsPdf;
class RooWorkspace;

namespace Builder { namespace Standard {

class AbsPdf{
 public:
  AbsPdf();
  virtual ~AbsPdf();
  
  
  virtual void Initialize( const boost::property_tree::ptree::value_type& pt_head ) = 0;
  virtual RooAbsPdf* add_to_workspace() = 0;
  
 protected:
  std::string name_;
  std::string desc_;
 
 private:
  
};

}}

#endif // BUILDER_STANDARD_ABSPDF_H