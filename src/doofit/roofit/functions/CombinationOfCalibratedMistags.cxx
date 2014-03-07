/***************************************************************************** 
 * Project: RooFit                                                           * 
 *                                                                           * 
 * This code was autogenerated by RooClassFactory                            * 
 *****************************************************************************/ 

// Your description goes here... 

#include "Riostream.h" 

#include "CombinationOfCalibratedMistags.h" 
#include "RooAbsReal.h" 
#include "RooAbsCategory.h" 
#include <math.h> 
#include "TMath.h" 

ClassImp(doofit::roofit::functions::CombinationOfCalibratedMistags)

namespace doofit {
namespace roofit {
namespace functions {
  
CombinationOfCalibratedMistags::CombinationOfCalibratedMistags(const char *name, const char *title,
                                                               RooAbsCategory& _cat_tag_OS,
                                                               RooAbsReal& _par_tag_omega_OS,
                                                               RooAbsCategory& _cat_tag_SS,
                                                               RooAbsReal& _par_tag_omega_SS) :
  RooAbsReal(name,title),
  cat_tag_OS("cat_tag_OS","cat_tag_OS",this,_cat_tag_OS),
  par_tag_omega_OS("par_tag_omega_OS","par_tag_omega_OS",this,_par_tag_omega_OS),
  cat_tag_SS("cat_tag_SS","cat_tag_SS",this,_cat_tag_SS),
  par_tag_omega_SS("par_tag_omega_SS","par_tag_omega_SS",this,_par_tag_omega_SS)
  {
  }

CombinationOfCalibratedMistags::CombinationOfCalibratedMistags(const CombinationOfCalibratedMistags& other, const char* name) :
  RooAbsReal(other,name),
  cat_tag_OS("cat_tag_OS",this,other.cat_tag_OS),
  par_tag_omega_OS("par_tag_omega_OS",this,other.par_tag_omega_OS),
  cat_tag_SS("cat_tag_SS",this,other.cat_tag_SS),
  par_tag_omega_SS("par_tag_omega_SS",this,other.par_tag_omega_SS)
 {
 } 

Int_t CombinationOfCalibratedMistags::getAnalyticalIntegral(RooArgSet& allVars,
                                                     RooArgSet& analVars, const char* rangeName) const
{
#ifdef FUNCTIONS_COUNT_CALLS
    std::printf("CombinationOfCalibratedMistags::getAnalyticalIntegral(): In %s line %u (%s): allVars = ",
                __func__, __LINE__, __FILE__);
    //  analVars.Print();
    allVars.Print();
#endif
    
    //if (matchArgs(allVars, analVars, cat_tag)) return 1;
    
    return 0;
}
  
  Int_t CombinationOfCalibratedMistags::getAnalyticalIntegralWN(RooArgSet& allVars, RooArgSet& analVars, const RooArgSet* normSet, const char* rangeName) const
{
#ifdef FUNCTIONS_COUNT_CALLS
    std::printf("CombinationOfCalibratedMistags::getAnalyticalIntegralWN(): In %s line %u (%s): allVars = ",
                __func__, __LINE__, __FILE__);
    //analVars.Print();
    allVars.Print();
    if (normSet) normSet->Print();
    if (rangeName) std::cout << "rangeName: " << rangeName << std::endl;
#endif
    
    //if (matchArgs(allVars, analVars, cat_tag)) return 1;
    
    return 0;
}
} // namespace functions
} // namespace roofit
} // namespace doofit