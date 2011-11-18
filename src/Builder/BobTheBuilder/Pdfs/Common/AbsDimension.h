#ifndef BUILDER_BOBTHEBUILDER_ABSDIMENSION_H
#define BUILDER_BOBTHEBUILDER_ABSDIMENSION_H

// from STL
#include <string>

// from boost
#include <boost/property_tree/ptree.hpp>

// forward declarations
class RooWorkspace;
class RooRealVar;


namespace Builder { namespace BobTheBuilder {

/** 
 *  @class AbsDimension
 *  @brief Abstract class for dimensions.
 *  
 */
class AbsDimension{
 public:
  AbsDimension();
  virtual ~AbsDimension();
  
  virtual void Initialize( const boost::property_tree::ptree::value_type& pt_head ) = 0;
  
  
  // Getter
  const std::string&  name()      const{ return name_;   };
  const std::string&  desc()      const{ return desc_;   };
  const std::string&  types()     const{ return types_; };
  
  
  // Setter, only for debugging, to be deleted
  void set_name( const std::string& name ){ name_ = name; };
  void set_desc( const std::string& desc ){ desc_ = desc; };
  void set_types( const std::string& types ){ types_ = types; };
  
 protected:
  std::string name_;
  std::string desc_;
  std::string types_;

 private:

};

}}

#endif // BUILDER_BOBTHEBUILDER_ABSDIMENSION_H
