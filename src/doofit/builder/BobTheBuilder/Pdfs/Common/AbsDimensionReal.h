#ifndef BUILDER_BOBTHEBUILDER_ABSDIMENSIONREAL_H
#define BUILDER_BOBTHEBUILDER_ABSDIMENSIONREAL_H


// from project
#include "doofit/builder/BobTheBuilder/Pdfs/Common/AbsDimension.h"

// forward declarations
namespace doofit {
namespace builder { namespace BobTheBuilder {

class AbsDimensionReal : public AbsDimension{
 public:
  AbsDimensionReal();
  
  virtual ~AbsDimensionReal();
  
  void Initialize(  const boost::property_tree::ptree::value_type &pt_head );
  bool AddToWorkspace( RooWorkspace& ws );
  
  virtual boost::shared_ptr<AbsPdf> CreatePdf( const std::string& pdf_name) = 0;
  
  // Getter
  const double        val_min()   const{ return val_min_; };
  const double        val_max()   const{ return val_max_; };
  const std::string&  unit()      const{ return unit_; };
  
  
  // Setter, only for debugging, to be deleted
  void set_val_min( const double val_min ){ val_min_ = val_min; };
  void set_val_max( const double val_max ){ val_max_ = val_max; }; 
  void set_unit( const std::string& unit ){ unit_ = unit; };
  

   

 protected:
  double      val_min_;
  double      val_max_;
  std::string unit_;
  
 private:

};

}}
}
#endif // BUILDER_BOBTHEBUILDER_ABSDIMENSIONREAL_H