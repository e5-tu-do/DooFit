/***************************************************************************** 
 * Project: RooFit                                                           * 
 *                                                                           * 
 * This code was autogenerated by RooClassFactory                            * 
 *****************************************************************************/ 

// Your description goes here... 

#include "Riostream.h" 

#include "FunctionSum.h" 
#include "RooAbsReal.h" 
#include "RooAbsCategory.h" 
#include <math.h> 
#include "TMath.h" 

ClassImp(doofit::roofit::functions::FunctionSum) 

namespace doofit {
namespace roofit {
namespace functions {

FunctionSum::FunctionSum() :
  num_addends(2)
  {
  }
  
FunctionSum::FunctionSum(std::string name,
                                 RooAbsReal& _par_addend1,
                                 RooAbsReal& _par_addend2) :
  RooAbsReal(name.c_str(),name.c_str()),
  par_addend1("par_addend1","par_addend1",this,_par_addend1),
  par_addend2("par_addend2","par_addend2",this,_par_addend2),
  num_calls_(0),
  num_addends(2)
  {
  } 

FunctionSum::FunctionSum(std::string name,
                                 RooAbsReal& _par_addend1,
                                 RooAbsReal& _par_addend2,
                                 RooAbsReal& _par_addend3) :
  RooAbsReal(name.c_str(),name.c_str()),
  par_addend1("par_addend1","par_addend1",this,_par_addend1),
  par_addend2("par_addend2","par_addend2",this,_par_addend2),
  par_addend3("par_addend3","par_addend3",this,_par_addend3),
  num_calls_(0),
  num_addends(3)
  {
  }


FunctionSum::FunctionSum(const FunctionSum& other, const char* name) :
  RooAbsReal(other,name),
  par_addend1("par_addend1",this,other.par_addend1),
  par_addend2("par_addend2",this,other.par_addend2),
  par_addend3("par_addend3",this,other.par_addend3),
  num_calls_(other.num_calls_),
  num_addends(other.num_addends)
  { 
  } 

Int_t FunctionSum::getAnalyticalIntegral(RooArgSet& allVars,RooArgSet& analVars, const char* rangeName) const
{
  std::printf("FunctionSum::getAnalyticalIntegral(): In %s line %u (%s): allVars = ",
              __func__, __LINE__, __FILE__);
  analVars.Print();
  allVars.Print();
  
  return 0;
}
} // namespace functions
} // namespace roofit
} // namespace doofit
