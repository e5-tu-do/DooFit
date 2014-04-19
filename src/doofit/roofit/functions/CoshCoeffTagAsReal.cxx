/***************************************************************************** 
 * Project: RooFit                                                           * 
 *                                                                           * 
 * This code was autogenerated by RooClassFactory                            * 
 *****************************************************************************/ 

// Your description goes here... 

#include "Riostream.h" 

#include "CoshCoeffTagAsReal.h" 
#include "RooAbsReal.h" 
#include <math.h> 
#include "TMath.h" 

ClassImp(doofit::roofit::functions::CoshCoeffTagAsReal)

namespace doofit {
namespace roofit {
namespace functions {
  
CoshCoeffTagAsReal::CoshCoeffTagAsReal(const char *name, const char *title,
                                     RooAbsReal& _par_tag_omega_Bd,
                                     RooAbsReal& _par_tag_omega_Bdb,
                                     RooAbsReal& _par_prod_asym,
                                     RooAbsReal& _par_tag) :
  RooAbsReal(name,title),
  par_tag_omega_Bd("par_tag_omega_Bd","par_tag_omega_Bd",this,_par_tag_omega_Bd),
  par_tag_omega_Bdb("par_tag_omega_Bdb","par_tag_omega_Bdb",this,_par_tag_omega_Bdb),
  par_prod_asym("par_prod_asym","par_prod_asym",this,_par_prod_asym),
  par_tag("par_tag","par_tag",this,_par_tag)
  {
  }
  
CoshCoeffTagAsReal::CoshCoeffTagAsReal(const CoshCoeffTagAsReal& other, const char* name) :
  RooAbsReal(other,name),
  par_tag_omega_Bd("par_tag_omega_Bd",this,other.par_tag_omega_Bd),
  par_tag_omega_Bdb("par_tag_omega_Bdb",this,other.par_tag_omega_Bdb),
  par_prod_asym("par_prod_asym",this,other.par_prod_asym),
  par_tag("par_tag",this,other.par_tag)
  {
  }

Int_t CoshCoeffTagAsReal::getAnalyticalIntegral(RooArgSet& allVars,
                                                     RooArgSet& analVars, const char* rangeName) const
{
#ifdef FUNCTIONS_COUNT_CALLS
    std::printf("CoshCoeffTagAsReal::getAnalyticalIntegral(): In %s line %u (%s): allVars = ",
                __func__, __LINE__, __FILE__);
    //  analVars.Print();
    allVars.Print();
#endif
    
    //if (matchArgs(allVars, analVars)) return 1;
    
    return 0;
}
  
  Int_t CoshCoeffTagAsReal::getAnalyticalIntegralWN(RooArgSet& allVars, RooArgSet& analVars, const RooArgSet* normSet, const char* rangeName) const
{
#ifdef FUNCTIONS_COUNT_CALLS
    std::printf("CoshCoeffTagAsReal::getAnalyticalIntegralWN(): In %s line %u (%s): allVars = ",
                __func__, __LINE__, __FILE__);
    //analVars.Print();
    allVars.Print();
    if (normSet) normSet->Print();
    if (rangeName) std::cout << "rangeName: " << rangeName << std::endl;
#endif
    
    //if (matchArgs(allVars, analVars)) return 1;
    
    return 0;
}
} // namespace functions
} // namespace roofit
} // namespace doofit