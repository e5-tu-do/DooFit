#ifndef BUILDER_STANDARD_ABSDIMENSIONREAL_H
#define BUILDER_STANDARD_ABSDIMENSIONREAL_H


// from project
#include "Builder/Standard/Pdfs/Common/AbsDimension.h"

// forward declarations

namespace Builder { namespace Standard {

class AbsDimensionReal : public AbsDimension{
 public:
  AbsDimensionReal();
  
  virtual ~AbsDimensionReal();
  
  void Initialize(  const boost::property_tree::ptree::value_type &pt_head );
  
  
  // Getter
  const double        val_min()   const{ return val_min_; };
  const double        val_max()   const{ return val_max_; };
  const std::string&  unit()      const{ return unit_; };
  
  
  // Setter, only for debugging, to be deleted
  void set_val_min( const double val_min ){ val_min_ = val_min; };
  void set_val_max( const double val_max ){ val_max_ = val_max; }; 
  void set_unit( const std::string& unit ){ unit_ = unit; };
  

  RooRealVar* const add_to_workspace( RooWorkspace* const ws ); 

 protected:
  double      val_min_;
  double      val_max_;
  std::string unit_;
  
 private:

};

}}

#endif // BUILDER_STANDARD_ABSDIMENSIONREAL_H