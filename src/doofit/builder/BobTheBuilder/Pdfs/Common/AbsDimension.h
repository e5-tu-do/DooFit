#ifndef BUILDER_BOBTHEBUILDER_ABSDIMENSION_H
#define BUILDER_BOBTHEBUILDER_ABSDIMENSION_H

// from STL
#include <map>
#include <string>

// from boost
#include <boost/property_tree/ptree.hpp>
#include <boost/shared_ptr.hpp>

// forward declarations
class RooWorkspace;
class RooRealVar;

namespace doofit {
namespace builder { namespace BobTheBuilder {
// forward declarations for project
class AbsPdf;

  
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
  virtual bool AddToWorkspace( RooWorkspace& ws ) = 0;
  
  virtual boost::shared_ptr<AbsPdf> CreatePdf( const std::string& pdf_type ) = 0;
  
  // Getter
  const std::string&  dim_id()    const{ return dim_id_;   };
  const std::string&  name()      const{ return name_;   };
  const std::string&  desc()      const{ return desc_;   };
  
  
  // Setter, only for debugging, to be deleted
  void set_name( const std::string& name ){ name_ = name; };
  void set_desc( const std::string& desc ){ desc_ = desc; };
  
  
  
 protected:
  std::string dim_id_;
  std::string name_;
  std::string desc_;

  
  
 private:

};

}}
}
#endif // BUILDER_BOBTHEBUILDER_ABSDIMENSION_H
