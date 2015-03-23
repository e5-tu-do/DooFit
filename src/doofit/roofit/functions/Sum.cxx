#include "Riostream.h" 

#include "Sum.h" 
#include "RooAbsReal.h" 
#include "RooAbsCategory.h" 
#include <math.h> 
#include "TMath.h" 

ClassImp(doofit::roofit::functions::Sum) 

namespace doofit {
namespace roofit {
namespace functions {

Sum::Sum(std::string name,
                                 RooAbsReal& _par_addend1,
                                 RooAbsReal& _par_addend2) :
  RooAbsReal(name.c_str(),name.c_str()),
  par_addend1("par_addend1","par_addend1",this,_par_addend1),
  par_addend2("par_addend2","par_addend2",this,_par_addend2)//,
  // num_calls_(0)
  {
  } 

Sum::Sum(const Sum& other, const char* name) :
  RooAbsReal(other,name),
  par_addend1("par_addend1",this,other.par_addend1),
  par_addend2("par_addend2",this,other.par_addend2)//,
  // num_calls_(other.num_calls_)
  {
  } 

Int_t Sum::getAnalyticalIntegral(RooArgSet& allVars,RooArgSet& analVars, const char* rangeName) const
{
  // std::printf("Sum::getAnalyticalIntegral(): In %s line %u (%s): allVars = ",
  //             __func__, __LINE__, __FILE__);
  // analVars.Print();
  // allVars.Print();
  
  return 0;
}
} // namespace functions
} // namespace roofit
} // namespace doofit
