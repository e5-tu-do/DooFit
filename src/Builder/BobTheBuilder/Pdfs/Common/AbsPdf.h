#ifndef BUILDER_BOBTHEBUILDER_ABSPDF_H
#define BUILDER_BOBTHEBUILDER_ABSPDF_H

// from boost
#include <boost/property_tree/ptree.hpp>

// forward declarations
class RooAbsPdf;
class RooWorkspace;

namespace Builder { namespace BobTheBuilder {

class AbsPdf{
 public:
  AbsPdf();
  virtual ~AbsPdf();
  
  
  virtual void Initialize( const boost::property_tree::ptree::value_type& tree_pdf_head ) = 0;
  virtual void AddToWorkspace( RooWorkspace* ws ) = 0;
  
  
  
 protected:
  std::string name_;
  std::string desc_;
 
 private:
  
};

}}

#endif // BUILDER_BOBTHEBUILDER_ABSPDF_H
