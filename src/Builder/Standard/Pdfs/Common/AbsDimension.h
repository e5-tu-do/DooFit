#ifndef BUILDER_STANDARD_ABSDIMENSION_H
#define BUILDER_STANDARD_ABSDIMENSION_H

// from STL
#include <string>

// from boost
#include <boost/property_tree/ptree.hpp>

// forward declarations
class RooWorkspace;
class RooRealVar;


namespace Builder { namespace Standard {

class AbsDimension{
 public:
  AbsDimension();
  virtual ~AbsDimension();
  
  virtual void Initialize( const boost::property_tree::ptree::value_type& pt_head ) = 0;
  
  
  // Getter
  const std::string&  name()      const{ return name_; };
  const std::string&  desc()      const{ return desc_; };
  
  
  // Setter, only for debugging, to be deleted
  void set_name( const std::string& name ){ name_ = name; };
  void set_desc( const std::string& desc ){ desc_ = desc; };
  
 protected:
  std::string name_;
  std::string desc_;

 private:

};

}}

#endif // BUILDER_STANDARD_ABSDIMENSION_H
