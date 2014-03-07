/***************************************************************************** 
 * Project: RooFit                                                           * 
 *                                                                           * 
 * This code was autogenerated by RooClassFactory                            * 
 *****************************************************************************/ 

// Your description goes here... 

#include "Riostream.h" 

#include "SinCoeffWithTaggingCalib.h" 
#include "RooAbsReal.h" 
#include "RooAbsCategory.h" 
#include <math.h> 
#include "TMath.h" 

ClassImp(doofit::roofit::functions::SinCoeffWithTaggingCalib)

namespace doofit {
namespace roofit {
namespace functions {

long long SinCoeffWithTaggingCalib::num_calls_evaluate_ = 0;
long long SinCoeffWithTaggingCalib::num_calls_integral_ = 0;


SinCoeffWithTaggingCalib::SinCoeffWithTaggingCalib() :
  type_coeff_(kCType)
  {
  }

SinCoeffWithTaggingCalib::SinCoeffWithTaggingCalib(std::string name,
                                   RooAbsReal& _par_S,
                                   RooAbsReal& _par_tag_p0,
                                   RooAbsReal& _par_tag_p1,
                                   RooAbsReal& _par_tag_meaneta,
                                   RooAbsReal& _par_tag_eta,
                                   RooAbsCategory& _cat_tag,
                                   CoeffType type_coeff) :
  RooAbsReal(name.c_str(), name.c_str()),
  par_S_("par_S_","par_S_",this,_par_S),
  cat_tag_("cat_tag_","cat_tag_",this,_cat_tag),
  type_coeff_(type_coeff),
  par_tag_p0_("par_tag_p0_","par_tag_p0_",this,_par_tag_p0),
  par_tag_p1_("par_tag_p1_","par_tag_p1_",this,_par_tag_p1),
  par_tag_meaneta_("par_tag_meaneta_","par_tag_meaneta_",this,_par_tag_meaneta),
  par_tag_eta_("par_tag_eta_","par_tag_eta_",this,_par_tag_eta)
  {
  }

SinCoeffWithTaggingCalib::SinCoeffWithTaggingCalib(const SinCoeffWithTaggingCalib& other, const char* name) :
  RooAbsReal(other,name),
  par_S_("par_S_",this,other.par_S_),
  cat_tag_("cat_tag_",this,other.cat_tag_),
  type_coeff_(other.type_coeff_),
  par_tag_p1_("par_tag_p1_",this,other.par_tag_p1_),
  par_tag_p0_("par_tag_p0_",this,other.par_tag_p0_),
  par_tag_meaneta_("par_tag_meaneta_",this,other.par_tag_meaneta_),
  par_tag_eta_("par_tag_eta_",this,other.par_tag_eta_)
  {
  }

Int_t SinCoeffWithTaggingCalib::getAnalyticalIntegral(RooArgSet& allVars,
                                        RooArgSet& analVars, const char* rangeName) const
{
  #ifdef FUNCTIONS_COUNT_CALLS
  std::printf("SinCoeffWithTaggingCalib::getAnalyticalIntegral(): In %s line %u (%s): allVars = ",
              __func__, __LINE__, __FILE__);
  //analVars.Print();
  allVars.Print();
  if (rangeName) std::cout << "rangeName: " << rangeName << std::endl;
  #endif
  
  matchArgs(allVars, analVars, cat_tag_);
  matchArgs(allVars, analVars, par_tag_eta_);
  
  if (analVars.contains(cat_tag_.arg())) {
    return 1;
  }
  
  return 0;
}


Int_t SinCoeffWithTaggingCalib::getAnalyticalIntegralWN(RooArgSet& allVars, RooArgSet& analVars, const RooArgSet* normSet, const char* rangeName) const
{
  #ifdef FUNCTIONS_COUNT_CALLS
  std::printf("SinCoeffWithTaggingCalib::getAnalyticalIntegralWN(): In %s line %u (%s): allVars = ",
              __func__, __LINE__, __FILE__);
  //analVars.Print();
  allVars.Print();
  if (normSet) normSet->Print();
  if (rangeName) std::cout << "rangeName: " << rangeName << std::endl;
  #endif
  
  //if (matchArgs(allVars, analVars, cat_tag_)) return 1;
  
  return 0;
}
} // namespace functions
} // namespace roofit
} // namespace doofit
