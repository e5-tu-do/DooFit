/***************************************************************************** 
 * Project: RooFit                                                           * 
 *                                                                           * 
 * This code was autogenerated by RooClassFactory                            * 
 *****************************************************************************/ 

// Your description goes here... 

#include "Riostream.h" 

#include "SinCoeffCombo.h" 
#include "RooAbsReal.h" 
#include "RooAbsCategory.h" 
#include <math.h> 
#include "TMath.h" 

ClassImp(doofit::roofit::functions::SinCoeffCombo)

namespace doofit {
namespace roofit {
namespace functions {

long long SinCoeffCombo::num_calls_evaluate_ = 0;
long long SinCoeffCombo::num_calls_integral_ = 0;


SinCoeffCombo::SinCoeffCombo() :
  type_coeff(kCType)
  {
  }

SinCoeffCombo::SinCoeffCombo(std::string name,
                             RooAbsReal& _par_S,
                             RooAbsCategory& _cat_tag_OS,
                             RooAbsReal& _par_p0_OS,
                             RooAbsReal& _par_p1_OS,
                             RooAbsReal& _par_meaneta_OS,
                             RooAbsReal& _par_eta_OS,
                             RooAbsReal& _par_delta_p0_OS,
                             RooAbsReal& _par_delta_p1_OS,
                             RooAbsCategory& _cat_tag_SS,
                             RooAbsReal& _par_p0_SS,
                             RooAbsReal& _par_p1_SS,
                             RooAbsReal& _par_meaneta_SS,
                             RooAbsReal& _par_eta_SS,
                             RooAbsReal& _par_delta_p0_SS,
                             RooAbsReal& _par_delta_p1_SS,
                             RooAbsReal& _par_prod_asym,
                             CoeffType type_coeff) :
  RooAbsReal(name.c_str(), name.c_str()),
  par_S("par_S","par_S",this,_par_S),
  cat_tag_OS("cat_tag_OS","cat_tag_OS",this,_cat_tag_OS),
  par_p0_OS("par_p0_OS","par_p0_OS",this,_par_p0_OS),
  par_p1_OS("par_p1_OS","par_p1_OS",this,_par_p1_OS),
  par_meaneta_OS("par_meaneta_OS","par_meaneta_OS",this,_par_meaneta_OS),
  par_eta_OS("par_eta_OS","par_eta_OS",this,_par_eta_OS),
  par_delta_p0_OS("par_delta_p0_OS","par_delta_p0_OS",this,_par_delta_p0_OS),
  par_delta_p1_OS("par_delta_p1_OS","par_delta_p1_OS",this,_par_delta_p1_OS),
  cat_tag_SS("cat_tag_SS","cat_tag_SS",this,_cat_tag_SS),
  par_p0_SS("par_p0_SS","par_p0_SS",this,_par_p0_SS),
  par_p1_SS("par_p1_SS","par_p1_SS",this,_par_p1_SS),
  par_meaneta_SS("par_meaneta_SS","par_meaneta_SS",this,_par_meaneta_SS),
  par_eta_SS("par_eta_SS","par_eta_SS",this,_par_eta_SS),
  par_delta_p0_SS("par_delta_p0_SS","par_delta_p0_SS",this,_par_delta_p0_SS),
  par_delta_p1_SS("par_delta_p1_SS","par_delta_p1_SS",this,_par_delta_p1_SS),
  par_prod_asym("par_prod_asym","par_prod_asym_",this,_par_prod_asym),
  type_coeff(type_coeff)
  {
  }

SinCoeffCombo::SinCoeffCombo(const SinCoeffCombo& other, const char* name) :
  RooAbsReal(other,name),
  par_S("par_S",this,other.par_S),
  cat_tag_OS("cat_tag_OS",this,other.cat_tag_OS),
  par_p0_OS("par_p0_OS",this,other.par_p0_OS),
  par_p1_OS("par_p1_OS",this,other.par_p1_OS),
  par_meaneta_OS("par_meaneta_OS",this,other.par_meaneta_OS),
  par_eta_OS("par_eta_OS",this,other.par_eta_OS),
  par_delta_p0_OS("par_delta_p0_OS",this,other.par_delta_p0_OS),
  par_delta_p1_OS("par_delta_p1_OS",this,other.par_delta_p1_OS),
  cat_tag_SS("cat_tag_SS",this,other.cat_tag_SS),
  par_p0_SS("par_p0_SS",this,other.par_p0_SS),
  par_p1_SS("par_p1_SS",this,other.par_p1_SS),
  par_meaneta_SS("par_meaneta_SS",this,other.par_meaneta_SS),
  par_eta_SS("par_eta_SS",this,other.par_eta_SS),
  par_delta_p0_SS("par_delta_p0_SS",this,other.par_delta_p0_SS),
  par_delta_p1_SS("par_delta_p1_SS",this,other.par_delta_p1_SS),
  type_coeff(other.type_coeff),
  par_prod_asym("par_prod_asym",this,other.par_prod_asym)
  {
  }

Int_t SinCoeffCombo::getAnalyticalIntegral(RooArgSet& allVars,
                                        RooArgSet& analVars, const char* rangeName) const
{
  #ifdef FUNCTIONS_COUNT_CALLS
  std::printf("SinCoeffCombo::getAnalyticalIntegral(): In %s line %u (%s): allVars = ",
              __func__, __LINE__, __FILE__);
  //analVars.Print();
  allVars.Print();
  if (rangeName) std::cout << "rangeName: " << rangeName << std::endl;
  #endif
  
  matchArgs(allVars, analVars, cat_tag_OS);
    
  if (analVars.contains(cat_tag_OS.arg())) {
    return 1;
  }
  
  return 0;
}


Int_t SinCoeffCombo::getAnalyticalIntegralWN(RooArgSet& allVars, RooArgSet& analVars, const RooArgSet* normSet, const char* rangeName) const
{
  #ifdef FUNCTIONS_COUNT_CALLS
  std::printf("SinCoeffCombo::getAnalyticalIntegralWN(): In %s line %u (%s): allVars = ",
              __func__, __LINE__, __FILE__);
  //analVars.Print();
  allVars.Print();
  if (normSet) normSet->Print();
  if (rangeName) std::cout << "rangeName: " << rangeName << std::endl;
  #endif
  
  //if (matchArgs(allVars, analVars, cat_tag_OS)) return 1;
  
  return 0;
}
} // namespace functions
} // namespace roofit
} // namespace doofit

