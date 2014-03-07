/***************************************************************************** 
 * Project: RooFit                                                           * 
 *                                                                           * 
 * This code was autogenerated by RooClassFactory                            * 
 *****************************************************************************/ 

// Your description goes here... 

#include "Riostream.h" 

#include "CoshCoeff.h" 
#include "RooAbsReal.h" 
#include "RooAbsCategory.h" 
#include <math.h> 
#include "TMath.h" 

ClassImp(doofit::roofit::functions::CoshCoeff)

namespace doofit {
namespace roofit {
namespace functions {
  
CoshCoeff::CoshCoeff(const char *name, const char *title,
                                     RooAbsReal& _par_tag_omega_Bd,
                                     RooAbsReal& _par_tag_omega_Bdb,
                                     RooAbsReal& _par_prod_asym,
                                     RooAbsCategory& _cat_tag) :
  RooAbsReal(name,title),
  par_tag_omega_Bd("par_tag_omega_Bd","par_tag_omega_Bd",this,_par_tag_omega_Bd),
  par_tag_omega_Bdb("par_tag_omega_Bdb","par_tag_omega_Bdb",this,_par_tag_omega_Bdb),
  par_prod_asym("par_prod_asym","par_prod_asym",this,_par_prod_asym),
  cat_tag("cat_tag","cat_tag",this,_cat_tag)
  {
  }
  
CoshCoeff::CoshCoeff(const CoshCoeff& other, const char* name) :
  RooAbsReal(other,name),
  par_tag_omega_Bd("par_tag_omega_Bd",this,other.par_tag_omega_Bd),
  par_tag_omega_Bdb("par_tag_omega_Bdb",this,other.par_tag_omega_Bdb),
  par_prod_asym("par_prod_asym",this,other.par_prod_asym),
  cat_tag("cat_tag",this,other.cat_tag)
  {
  }

Int_t CoshCoeff::getAnalyticalIntegral(RooArgSet& allVars,
                                                     RooArgSet& analVars, const char* rangeName) const
{
#ifdef FUNCTIONS_COUNT_CALLS
    std::printf("CoshCoeff::getAnalyticalIntegral(): In %s line %u (%s): allVars = ",
                __func__, __LINE__, __FILE__);
    //  analVars.Print();
    allVars.Print();
#endif
    
    //if (matchArgs(allVars, analVars, cat_tag)) return 1;
    
    return 0;
}
  
  Int_t CoshCoeff::getAnalyticalIntegralWN(RooArgSet& allVars, RooArgSet& analVars, const RooArgSet* normSet, const char* rangeName) const
{
#ifdef FUNCTIONS_COUNT_CALLS
    std::printf("CoshCoeff::getAnalyticalIntegralWN(): In %s line %u (%s): allVars = ",
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