#include "Riostream.h"

#include "DecRateCoeff_Bd.h"
#include "RooAbsReal.h"
#include "RooAbsCategory.h"
#include "RooCategory.h"
#include <math.h>
#include "TMath.h"
#include "RooRealConstant.h"
#include "RooConstVar.h"

DecRateCoeff_Bd::DecRateCoeff_Bd() {};

DecRateCoeff_Bd::DecRateCoeff_Bd(const std::string& name,
                           const std::string& title,
                           CoeffType   _coeff_type_,
                           RooAbsCategory& _finalstate_,
                           RooAbsReal& _cp_coeff_,
                           RooAbsReal& _cp_coeff_bar_,
                           RooAbsCategory& _tag_os_,
                           RooAbsReal& _eta_os_,
                           RooAbsReal& _p0_os_,
                           RooAbsReal& _p1_os_,
                           RooAbsReal& _delta_p0_os_,
                           RooAbsReal& _delta_p1_os_,
                           RooAbsReal& _avg_eta_os_,
                           RooAbsReal& _tageff_os_,
                           RooAbsReal& _tageff_asym_os_,
                           RooAbsCategory& _tag_ss_,
                           RooAbsReal& _eta_ss_,
                           RooAbsReal& _p0_ss_,
                           RooAbsReal& _p1_ss_,
                           RooAbsReal& _delta_p0_ss_,
                           RooAbsReal& _delta_p1_ss_,
                           RooAbsReal& _avg_eta_ss_,
                           RooAbsReal& _tageff_ss_,
                           RooAbsReal& _tageff_asym_ss_,
                           RooAbsReal& _production_asym_,
                           RooAbsReal& _detection_asym_) :
  RooAbsReal(name.c_str(),title.c_str()),
  cp_coeff_("cp_coeff_","cp_coeff_",this,_cp_coeff_),
  cp_coeff_bar_("cp_coeff_bar_","cp_coeff_bar_",this,_cp_coeff_bar_),
  coeff_type_(_coeff_type_),
  finalstate_("finalstate_","finalstate_",this,_finalstate_),
  tag_os_("tag_os_","tag_os_",this,_tag_os_),
  eta_os_("eta_os_","eta_os_",this,_eta_os_),
  avg_eta_os_("avg_eta_os_","avg_eta_os_",this,_avg_eta_os_),
  p0_os_("p0_os_","p0_os_",this,_p0_os_),
  p1_os_("p1_os_","p1_os_",this,_p1_os_),
  delta_p0_os_("delta_p0_os_","delta_p0_os_",this,_delta_p0_os_),
  delta_p1_os_("delta_p1_os_","delta_p1_os_",this,_delta_p1_os_),
  tageff_os_("tageff_os_","tageff_os_",this,_tageff_os_),
  tageff_asym_os_("tageff_asym_os_","tageff_asym_os_",this,_tageff_asym_os_),
  tag_ss_("tag_ss_","tag_ss_",this,_tag_ss_),
  eta_ss_("eta_ss_","eta_ss_",this,_eta_ss_),
  avg_eta_ss_("avg_eta_ss_","avg_eta_ss_",this,_avg_eta_ss_),
  p0_ss_("p0_ss_","p0_ss_",this,_p0_ss_),
  p1_ss_("p1_ss_","p1_ss_",this,_p1_ss_),
  delta_p0_ss_("delta_p0_ss_","delta_p0_ss_",this,_delta_p0_ss_),
  delta_p1_ss_("delta_p1_ss_","delta_p1_ss_",this,_delta_p1_ss_),
  tageff_ss_("tageff_ss_","tageff_ss_",this,_tageff_ss_),
  tageff_asym_ss_("tageff_asym_ss_","tageff_asym_ss_",this,_tageff_asym_ss_),
  production_asym_("production_asym_","production_asym_",this,_production_asym_),
  detection_asym_("detection_asym_","detection_asym_",this,_detection_asym_),
  onetagger_(false)
{
}

DecRateCoeff_Bd::DecRateCoeff_Bd(const std::string& name,
                            const std::string& title,
                            CoeffType   _coeff_type_,
                            RooAbsCategory& _finalstate_,
                            RooAbsReal& _cp_coeff_,
                            RooAbsReal& _cp_coeff_bar_,
                            RooAbsCategory& _tag_os_,
                            RooAbsReal& _eta_os_,
                            RooAbsReal& _p0_os_,
                            RooAbsReal& _p1_os_,
                            RooAbsReal& _delta_p0_os_,
                            RooAbsReal& _delta_p1_os_,
                            RooAbsReal& _avg_eta_os_,
                            RooAbsReal& _tageff_os_,
                            RooAbsReal& _tageff_asym_os_,
                            RooAbsReal& _production_asym_,
                            RooAbsReal& _detection_asym_) :
  RooAbsReal(name.c_str(),title.c_str()),
  cp_coeff_("cp_coeff_","cp_coeff_",this,_cp_coeff_),
  cp_coeff_bar_("cp_coeff_bar_","cp_coeff_bar_",this,_cp_coeff_bar_),
  coeff_type_(_coeff_type_),
  finalstate_("finalstate_","finalstate_",this,_finalstate_),
  tag_os_("tag_os_","tag_os_",this,_tag_os_),
  eta_os_("eta_os_","eta_os_",this,_eta_os_),
  avg_eta_os_("avg_eta_os_","avg_eta_os_",this,_avg_eta_os_),
  p0_os_("p0_os_","p0_os_",this,_p0_os_),
  p1_os_("p1_os_","p1_os_",this,_p1_os_),
  delta_p0_os_("delta_p0_os_","delta_p0_os_",this,_delta_p0_os_),
  delta_p1_os_("delta_p1_os_","delta_p1_os_",this,_delta_p1_os_),
  tageff_os_("tageff_os_","tageff_os_",this,_tageff_os_),
  tageff_asym_os_("tageff_asym_os_","tageff_asym_os_",this,_tageff_asym_os_),
  tag_ss_("tag_ss_","tag_ss_",this,_tag_os_),
  eta_ss_("eta_ss_","eta_ss_",this,_eta_os_),
  avg_eta_ss_("avg_eta_ss_","avg_eta_ss_",this,_avg_eta_os_),
  p0_ss_("p0_ss_","p0_ss_",this,_p0_os_),
  p1_ss_("p1_ss_","p1_ss_",this,_p1_os_),
  delta_p0_ss_("delta_p0_ss_","delta_p0_ss_",this,_delta_p0_os_),
  delta_p1_ss_("delta_p1_ss_","delta_p1_ss_",this,_delta_p1_os_),
  tageff_ss_("tageff_ss_","tageff_ss_",this,_tageff_os_),
  tageff_asym_ss_("tageff_asym_ss_","tageff_asym_ss_",this,_tageff_asym_os_),
  production_asym_("production_asym_","production_asym_",this,_production_asym_),
  detection_asym_("detection_asym_","detection_asym_",this,_detection_asym_),
  onetagger_(true)
{
}


DecRateCoeff_Bd::DecRateCoeff_Bd(const DecRateCoeff_Bd& other, const char* name) :
  RooAbsReal(other,name),
  cp_coeff_("cp_coeff_",this,other.cp_coeff_),
  cp_coeff_bar_("cp_coeff_bar_",this,other.cp_coeff_bar_),
  coeff_type_(other.coeff_type_),
  finalstate_("finalstate_",this,other.finalstate_),
  tag_os_("tag_os_",this,other.tag_os_),
  eta_os_("eta_os_",this,other.eta_os_),
  avg_eta_os_("avg_eta_os_",this,other.avg_eta_os_),
  p0_os_("p0_os_",this,other.p0_os_),
  p1_os_("p1_os_",this,other.p1_os_),
  delta_p0_os_("delta_p0_os_",this,other.delta_p0_os_),
  delta_p1_os_("delta_p1_os_",this,other.delta_p1_os_),
  tageff_os_("tageff_os_",this,other.tageff_os_),
  tageff_asym_os_("tageff_asym_os_",this,other.tageff_asym_os_),
  tag_ss_("tag_ss_",this,other.tag_ss_),
  eta_ss_("eta_ss_",this,other.eta_ss_),
  avg_eta_ss_("avg_eta_ss_",this,other.avg_eta_ss_),
  p0_ss_("p0_ss_",this,other.p0_ss_),
  p1_ss_("p1_ss_",this,other.p1_ss_),
  delta_p0_ss_("delta_p0_ss_",this,other.delta_p0_ss_),
  delta_p1_ss_("delta_p1_ss_",this,other.delta_p1_ss_),
  tageff_ss_("tageff_ss_",this,other.tageff_ss_),
  tageff_asym_ss_("tageff_asym_ss_",this,other.tageff_asym_ss_),
  production_asym_("production_asym_",this,other.production_asym_),
  detection_asym_("detection_asym_",this,other.detection_asym_),
  onetagger_(other.onetagger_)
{
}


Double_t DecRateCoeff_Bd::evaluate() const
{
  return evaluate(cp_coeff_, cp_coeff_bar_, coeff_type_, finalstate_, tag_os_, eta_os_, avg_eta_os_, p0_os_, p1_os_, delta_p0_os_, delta_p1_os_, tageff_os_, tageff_asym_os_, tag_ss_, eta_ss_, avg_eta_ss_, p0_ss_, p1_ss_, delta_p0_ss_, delta_p1_ss_, tageff_ss_, tageff_asym_ss_, production_asym_, detection_asym_);
}


Int_t DecRateCoeff_Bd::getAnalyticalIntegral(RooArgSet& allVars, RooArgSet& analVars, const char* /*rangeName*/) const
{
  // debug
  // std::printf("CHECK: In %s line %u (%s): #Vars = %d : allVars = ", __func__, __LINE__, __FILE__, allVars.getSize());
  // allVars.Print();

  if (matchArgs(allVars, analVars, tag_os_, tag_ss_, finalstate_)) return 2 ;
  if (matchArgs(allVars, analVars, tag_os_, finalstate_)) return 1 ;
  if (matchArgs(allVars, analVars, tag_ss_, finalstate_)) return -1 ;
  return 0 ;
}

Int_t DecRateCoeff_Bd::getAnalyticalIntegralWN(RooArgSet& allVars, RooArgSet& analVars, const RooArgSet* normSet, const char* /*rangeName*/) const
{
  // debug
  // std::printf("CHECK: In %s line %u (%s): #Vars = %d : allVars = \n", __func__, __LINE__, __FILE__, allVars.getSize());
  // allVars.Print();
  if (normSet) normSet->Print();

  if (matchArgs(allVars, analVars, tag_os_, tag_ss_)) return 2 ;
  if (matchArgs(allVars, analVars, tag_os_)) return 1 ;
  if (matchArgs(allVars, analVars, tag_ss_)) return -1 ;
  return 0 ;
}

Double_t DecRateCoeff_Bd::analyticalIntegral(Int_t code, const char* rangeName) const
{
  // debug
  // std::printf("CHECK: In %s line %u (%s): Range: %s : Code: %d \n", __func__, __LINE__, __FILE__, rangeName, code);

  // first return analytical integral for defined range
  if (rangeName){
    double integral = 0;
    if (isTagInRange(tag_ss_, +1, rangeName)){
      if (isTagInRange(tag_os_, +1, rangeName)){
        if (isFinalstateInRange(finalstate_, +1, rangeName)){
          integral += evaluate(cp_coeff_, cp_coeff_bar_, coeff_type_, +1, +1, eta_os_, avg_eta_os_, p0_os_, p1_os_, delta_p0_os_, delta_p1_os_, tageff_os_, tageff_asym_os_, +1, eta_ss_, avg_eta_ss_, p0_ss_, p1_ss_, delta_p0_ss_, delta_p1_ss_, tageff_ss_, tageff_asym_ss_, production_asym_, detection_asym_);
        }
        if (isFinalstateInRange(finalstate_, -1, rangeName)){
          integral += evaluate(cp_coeff_, cp_coeff_bar_, coeff_type_, -1, +1, eta_os_, avg_eta_os_, p0_os_, p1_os_, delta_p0_os_, delta_p1_os_, tageff_os_, tageff_asym_os_, +1, eta_ss_, avg_eta_ss_, p0_ss_, p1_ss_, delta_p0_ss_, delta_p1_ss_, tageff_ss_, tageff_asym_ss_, production_asym_, detection_asym_);
        }
        // debug
        // std::cout << "Coeff: " << coeff_type_ << " Range: B0B0 : " << integral << std::endl;
      }
      if (isTagInRange(tag_os_, -1, rangeName)){
        if (isFinalstateInRange(finalstate_, +1, rangeName)){
          integral += evaluate(cp_coeff_, cp_coeff_bar_, coeff_type_, +1, -1, eta_os_, avg_eta_os_, p0_os_, p1_os_, delta_p0_os_, delta_p1_os_, tageff_os_, tageff_asym_os_, +1, eta_ss_, avg_eta_ss_, p0_ss_, p1_ss_, delta_p0_ss_, delta_p1_ss_, tageff_ss_, tageff_asym_ss_, production_asym_, detection_asym_);
        }
        if (isFinalstateInRange(finalstate_, -1, rangeName)){
          integral += evaluate(cp_coeff_, cp_coeff_bar_, coeff_type_, -1, -1, eta_os_, avg_eta_os_, p0_os_, p1_os_, delta_p0_os_, delta_p1_os_, tageff_os_, tageff_asym_os_, +1, eta_ss_, avg_eta_ss_, p0_ss_, p1_ss_, delta_p0_ss_, delta_p1_ss_, tageff_ss_, tageff_asym_ss_, production_asym_, detection_asym_);
        }
        // debug
        // std::cout << "Coeff: " << coeff_type_ << " Range: B0barB0 : " << integral << std::endl;
      }
      if (isTagInRange(tag_os_, 0, rangeName)){
        if (isFinalstateInRange(finalstate_, +1, rangeName)){
          integral += evaluate(cp_coeff_, cp_coeff_bar_, coeff_type_, +1, 0, eta_os_, avg_eta_os_, p0_os_, p1_os_, delta_p0_os_, delta_p1_os_, tageff_os_, tageff_asym_os_, +1, eta_ss_, avg_eta_ss_, p0_ss_, p1_ss_, delta_p0_ss_, delta_p1_ss_, tageff_ss_, tageff_asym_ss_, production_asym_, detection_asym_);
        }
        if (isFinalstateInRange(finalstate_, -1, rangeName)){
          integral += evaluate(cp_coeff_, cp_coeff_bar_, coeff_type_, -1, 0, eta_os_, avg_eta_os_, p0_os_, p1_os_, delta_p0_os_, delta_p1_os_, tageff_os_, tageff_asym_os_, +1, eta_ss_, avg_eta_ss_, p0_ss_, p1_ss_, delta_p0_ss_, delta_p1_ss_, tageff_ss_, tageff_asym_ss_, production_asym_, detection_asym_);
        }
        // debug
        // std::cout << "Coeff: " << coeff_type_ << " Range: B0barB0 : " << integral << std::endl;
      }
    }
    if (isTagInRange(tag_ss_, -1, rangeName)){
      if (isTagInRange(tag_os_, +1, rangeName)){
        if (isFinalstateInRange(finalstate_, +1, rangeName)){
          integral += evaluate(cp_coeff_, cp_coeff_bar_, coeff_type_, +1, +1, eta_os_, avg_eta_os_, p0_os_, p1_os_, delta_p0_os_, delta_p1_os_, tageff_os_, tageff_asym_os_, -1, eta_ss_, avg_eta_ss_, p0_ss_, p1_ss_, delta_p0_ss_, delta_p1_ss_, tageff_ss_, tageff_asym_ss_, production_asym_, detection_asym_);
        }
        if (isFinalstateInRange(finalstate_, -1, rangeName)){
          integral += evaluate(cp_coeff_, cp_coeff_bar_, coeff_type_, -1, +1, eta_os_, avg_eta_os_, p0_os_, p1_os_, delta_p0_os_, delta_p1_os_, tageff_os_, tageff_asym_os_, -1, eta_ss_, avg_eta_ss_, p0_ss_, p1_ss_, delta_p0_ss_, delta_p1_ss_, tageff_ss_, tageff_asym_ss_, production_asym_, detection_asym_);
        }
        // debug
        // std::cout << "Coeff: " << coeff_type_ << " Range: B0B0bar : " << integral << std::endl;
      }
      if (isTagInRange(tag_os_, -1, rangeName)){
        if (isFinalstateInRange(finalstate_, +1, rangeName)){
          integral += evaluate(cp_coeff_, cp_coeff_bar_, coeff_type_, +1, -1, eta_os_, avg_eta_os_, p0_os_, p1_os_, delta_p0_os_, delta_p1_os_, tageff_os_, tageff_asym_os_, -1, eta_ss_, avg_eta_ss_, p0_ss_, p1_ss_, delta_p0_ss_, delta_p1_ss_, tageff_ss_, tageff_asym_ss_, production_asym_, detection_asym_);
        }
        if (isFinalstateInRange(finalstate_, -1, rangeName)){
          integral += evaluate(cp_coeff_, cp_coeff_bar_, coeff_type_, -1, -1, eta_os_, avg_eta_os_, p0_os_, p1_os_, delta_p0_os_, delta_p1_os_, tageff_os_, tageff_asym_os_, -1, eta_ss_, avg_eta_ss_, p0_ss_, p1_ss_, delta_p0_ss_, delta_p1_ss_, tageff_ss_, tageff_asym_ss_, production_asym_, detection_asym_);
        }
        // debug
        // std::cout << "Coeff: " << coeff_type_ << " Range: B0barB0bar : " << integral << std::endl;
      }
      if (isTagInRange(tag_os_, 0, rangeName)){
        if (isFinalstateInRange(finalstate_, +1, rangeName)){
          integral += evaluate(cp_coeff_, cp_coeff_bar_, coeff_type_, +1, 0, eta_os_, avg_eta_os_, p0_os_, p1_os_, delta_p0_os_, delta_p1_os_, tageff_os_, tageff_asym_os_, -1, eta_ss_, avg_eta_ss_, p0_ss_, p1_ss_, delta_p0_ss_, delta_p1_ss_, tageff_ss_, tageff_asym_ss_, production_asym_, detection_asym_);
        }
        if (isFinalstateInRange(finalstate_, -1, rangeName)){
          integral += evaluate(cp_coeff_, cp_coeff_bar_, coeff_type_, -1, 0, eta_os_, avg_eta_os_, p0_os_, p1_os_, delta_p0_os_, delta_p1_os_, tageff_os_, tageff_asym_os_, -1, eta_ss_, avg_eta_ss_, p0_ss_, p1_ss_, delta_p0_ss_, delta_p1_ss_, tageff_ss_, tageff_asym_ss_, production_asym_, detection_asym_);
        }
        // debug
        // std::cout << "Coeff: " << coeff_type_ << " Range: B0barB0bar : " << integral << std::endl;
      }
    }
    if (isTagInRange(tag_ss_, 0, rangeName)){
      if (isTagInRange(tag_os_, +1, rangeName)){
        if (isFinalstateInRange( finalstate_, +1, rangeName)){
          integral += evaluate(cp_coeff_, cp_coeff_bar_, coeff_type_, +1, +1, eta_os_, avg_eta_os_, p0_os_, p1_os_, delta_p0_os_, delta_p1_os_, tageff_os_, tageff_asym_os_, 0, eta_ss_, avg_eta_ss_, p0_ss_, p1_ss_, delta_p0_ss_, delta_p1_ss_, tageff_ss_, tageff_asym_ss_, production_asym_, detection_asym_);
        }
        if (isFinalstateInRange( finalstate_, -1, rangeName)){
          integral += evaluate(cp_coeff_, cp_coeff_bar_, coeff_type_, -1, +1, eta_os_, avg_eta_os_, p0_os_, p1_os_, delta_p0_os_, delta_p1_os_, tageff_os_, tageff_asym_os_, 0, eta_ss_, avg_eta_ss_, p0_ss_, p1_ss_, delta_p0_ss_, delta_p1_ss_, tageff_ss_, tageff_asym_ss_, production_asym_, detection_asym_);
        }
        // debug
        // std::cout << "Coeff: " << coeff_type_ << " Range: B0B0bar : " << integral << std::endl;
      }
      if (isTagInRange(tag_os_, -1, rangeName)){
        if (isFinalstateInRange(finalstate_, +1, rangeName)){
          integral += evaluate(cp_coeff_, cp_coeff_bar_, coeff_type_, +1, -1, eta_os_, avg_eta_os_, p0_os_, p1_os_, delta_p0_os_, delta_p1_os_, tageff_os_, tageff_asym_os_, 0, eta_ss_, avg_eta_ss_, p0_ss_, p1_ss_, delta_p0_ss_, delta_p1_ss_, tageff_ss_, tageff_asym_ss_, production_asym_, detection_asym_);
        }
        if (isFinalstateInRange(finalstate_, -1, rangeName)){
          integral += evaluate(cp_coeff_, cp_coeff_bar_, coeff_type_, -1, -1, eta_os_, avg_eta_os_, p0_os_, p1_os_, delta_p0_os_, delta_p1_os_, tageff_os_, tageff_asym_os_, 0, eta_ss_, avg_eta_ss_, p0_ss_, p1_ss_, delta_p0_ss_, delta_p1_ss_, tageff_ss_, tageff_asym_ss_, production_asym_, detection_asym_);
        }
        // debug
        // std::cout << "Coeff: " << coeff_type_ << " Range: B0barB0bar : " << integral << std::endl;
      }
      if (isTagInRange(tag_os_, 0, rangeName)){
        if (isFinalstateInRange(finalstate_, +1, rangeName)){
          integral += evaluate(cp_coeff_, cp_coeff_bar_, coeff_type_, +1, 0, eta_os_, avg_eta_os_, p0_os_, p1_os_, delta_p0_os_, delta_p1_os_, tageff_os_, tageff_asym_os_, 0, eta_ss_, avg_eta_ss_, p0_ss_, p1_ss_, delta_p0_ss_, delta_p1_ss_, tageff_ss_, tageff_asym_ss_, production_asym_, detection_asym_);
        }
        if (isFinalstateInRange(finalstate_, -1, rangeName)){
          integral += evaluate(cp_coeff_, cp_coeff_bar_, coeff_type_, -1, 0, eta_os_, avg_eta_os_, p0_os_, p1_os_, delta_p0_os_, delta_p1_os_, tageff_os_, tageff_asym_os_, 0, eta_ss_, avg_eta_ss_, p0_ss_, p1_ss_, delta_p0_ss_, delta_p1_ss_, tageff_ss_, tageff_asym_ss_, production_asym_, detection_asym_);
        }
        // debug
        // std::cout << "Coeff: " << coeff_type_ << " Range: B0barB0bar : " << integral << std::endl;
      }
    }
    return integral;
  }
  else{
    if (code == +1){
      double integral = 0.;
      if (hasTagState(tag_os_, +1)){
        if (hasFinalstateState(finalstate_, +1)){
          integral += evaluate(cp_coeff_, cp_coeff_bar_, coeff_type_, +1, +1, eta_os_, avg_eta_os_, p0_os_, p1_os_, delta_p0_os_, delta_p1_os_, tageff_os_, tageff_asym_os_, 0, eta_ss_, avg_eta_ss_, p0_ss_, p1_ss_, delta_p0_ss_, delta_p1_ss_, tageff_ss_, tageff_asym_ss_, production_asym_, detection_asym_);
        }
        if (hasFinalstateState(finalstate_, -1)){
          integral += evaluate(cp_coeff_, cp_coeff_bar_, coeff_type_, -1, +1, eta_os_, avg_eta_os_, p0_os_, p1_os_, delta_p0_os_, delta_p1_os_, tageff_os_, tageff_asym_os_, 0, eta_ss_, avg_eta_ss_, p0_ss_, p1_ss_, delta_p0_ss_, delta_p1_ss_, tageff_ss_, tageff_asym_ss_, production_asym_, detection_asym_);
        }
      }
      if (hasTagState(tag_os_, -1)){
        if (hasFinalstateState(finalstate_, +1)){
          integral += evaluate(cp_coeff_, cp_coeff_bar_, coeff_type_, +1, -1, eta_os_, avg_eta_os_, p0_os_, p1_os_, delta_p0_os_, delta_p1_os_, tageff_os_, tageff_asym_os_, 0, eta_ss_, avg_eta_ss_, p0_ss_, p1_ss_, delta_p0_ss_, delta_p1_ss_, tageff_ss_, tageff_asym_ss_, production_asym_, detection_asym_);
        }
        if (hasFinalstateState(finalstate_, -1)){
          integral += evaluate(cp_coeff_, cp_coeff_bar_, coeff_type_, -1, -1, eta_os_, avg_eta_os_, p0_os_, p1_os_, delta_p0_os_, delta_p1_os_, tageff_os_, tageff_asym_os_, 0, eta_ss_, avg_eta_ss_, p0_ss_, p1_ss_, delta_p0_ss_, delta_p1_ss_, tageff_ss_, tageff_asym_ss_, production_asym_, detection_asym_);
        }
      }
      if (hasTagState(tag_os_, 0)){
        if (hasFinalstateState(finalstate_, +1)){
          integral += evaluate(cp_coeff_, cp_coeff_bar_, coeff_type_, +1, 0, eta_os_, avg_eta_os_, p0_os_, p1_os_, delta_p0_os_, delta_p1_os_, tageff_os_, tageff_asym_os_, 0, eta_ss_, avg_eta_ss_, p0_ss_, p1_ss_, delta_p0_ss_, delta_p1_ss_, tageff_ss_, tageff_asym_ss_, production_asym_, detection_asym_);
        }
        if (hasFinalstateState(finalstate_, -1)){
          integral += evaluate(cp_coeff_, cp_coeff_bar_, coeff_type_, -1, 0, eta_os_, avg_eta_os_, p0_os_, p1_os_, delta_p0_os_, delta_p1_os_, tageff_os_, tageff_asym_os_, 0, eta_ss_, avg_eta_ss_, p0_ss_, p1_ss_, delta_p0_ss_, delta_p1_ss_, tageff_ss_, tageff_asym_ss_, production_asym_, detection_asym_);
        }
      }
      // debug
      // std::cout << "Coeff: " << coeff_type_ << " : OS Integral : " << integral << std::endl;
      return integral;
    }
    if (code == -1){
      double integral = 0.;
      if (hasTagState(tag_ss_, +1)){
        if (hasFinalstateState(finalstate_, +1)){
          integral += evaluate(cp_coeff_, cp_coeff_bar_, coeff_type_, +1, 0, eta_os_, avg_eta_os_, p0_os_, p1_os_, delta_p0_os_, delta_p1_os_, tageff_os_, tageff_asym_os_, +1, eta_ss_, avg_eta_ss_, p0_ss_, p1_ss_, delta_p0_ss_, delta_p1_ss_, tageff_ss_, tageff_asym_ss_, production_asym_, detection_asym_);
        }
        if (hasFinalstateState(finalstate_, -1)){
          integral += evaluate(cp_coeff_, cp_coeff_bar_, coeff_type_, -1, 0, eta_os_, avg_eta_os_, p0_os_, p1_os_, delta_p0_os_, delta_p1_os_, tageff_os_, tageff_asym_os_, +1, eta_ss_, avg_eta_ss_, p0_ss_, p1_ss_, delta_p0_ss_, delta_p1_ss_, tageff_ss_, tageff_asym_ss_, production_asym_, detection_asym_);
        }
      }
      if (hasTagState(tag_ss_, -1)){
        if (hasFinalstateState(finalstate_, +1)){
          integral += evaluate(cp_coeff_, cp_coeff_bar_, coeff_type_, +1, 0, eta_os_, avg_eta_os_, p0_os_, p1_os_, delta_p0_os_, delta_p1_os_, tageff_os_, tageff_asym_os_, -1, eta_ss_, avg_eta_ss_, p0_ss_, p1_ss_, delta_p0_ss_, delta_p1_ss_, tageff_ss_, tageff_asym_ss_, production_asym_, detection_asym_);
        }
        if (hasFinalstateState(finalstate_, -1)){
          integral += evaluate(cp_coeff_, cp_coeff_bar_, coeff_type_, -1, 0, eta_os_, avg_eta_os_, p0_os_, p1_os_, delta_p0_os_, delta_p1_os_, tageff_os_, tageff_asym_os_, -1, eta_ss_, avg_eta_ss_, p0_ss_, p1_ss_, delta_p0_ss_, delta_p1_ss_, tageff_ss_, tageff_asym_ss_, production_asym_, detection_asym_);
        }
      }
      if (hasTagState(tag_ss_, 0)){
        if (hasFinalstateState(finalstate_, +1)){
          integral += evaluate(cp_coeff_, cp_coeff_bar_, coeff_type_, +1, 0, eta_os_, avg_eta_os_, p0_os_, p1_os_, delta_p0_os_, delta_p1_os_, tageff_os_, tageff_asym_os_, 0, eta_ss_, avg_eta_ss_, p0_ss_, p1_ss_, delta_p0_ss_, delta_p1_ss_, tageff_ss_, tageff_asym_ss_, production_asym_, detection_asym_);
        }
        if (hasFinalstateState(finalstate_, -1)){
          integral += evaluate(cp_coeff_, cp_coeff_bar_, coeff_type_, -1, 0, eta_os_, avg_eta_os_, p0_os_, p1_os_, delta_p0_os_, delta_p1_os_, tageff_os_, tageff_asym_os_, 0, eta_ss_, avg_eta_ss_, p0_ss_, p1_ss_, delta_p0_ss_, delta_p1_ss_, tageff_ss_, tageff_asym_ss_, production_asym_, detection_asym_);
        }
      }
      // debug
      // std::cout << "Coeff: " << coeff_type_ << " : SS Integral : " << integral << std::endl;
      return integral;
    }
    if (code == 2){
      double integral = 0.;
      if (hasTagState(tag_os_, +1) && hasTagState(tag_ss_, +1) && hasFinalstateState(finalstate_, +1)) integral += evaluate(cp_coeff_, cp_coeff_bar_, coeff_type_, +1, +1, eta_os_, avg_eta_os_, p0_os_, p1_os_, delta_p0_os_, delta_p1_os_, tageff_os_, tageff_asym_os_, +1, eta_ss_, avg_eta_ss_, p0_ss_, p1_ss_, delta_p0_ss_, delta_p1_ss_, tageff_ss_, tageff_asym_ss_, production_asym_, detection_asym_);
      if (hasTagState(tag_os_, +1) && hasTagState(tag_ss_, +1) && hasFinalstateState(finalstate_, -1)) integral += evaluate(cp_coeff_, cp_coeff_bar_, coeff_type_, -1, +1, eta_os_, avg_eta_os_, p0_os_, p1_os_, delta_p0_os_, delta_p1_os_, tageff_os_, tageff_asym_os_, +1, eta_ss_, avg_eta_ss_, p0_ss_, p1_ss_, delta_p0_ss_, delta_p1_ss_, tageff_ss_, tageff_asym_ss_, production_asym_, detection_asym_);

      if (hasTagState(tag_os_, +1) && hasTagState(tag_ss_, -1) && hasFinalstateState(finalstate_, +1)) integral += evaluate(cp_coeff_, cp_coeff_bar_, coeff_type_, +1, +1, eta_os_, avg_eta_os_, p0_os_, p1_os_, delta_p0_os_, delta_p1_os_, tageff_os_, tageff_asym_os_, -1, eta_ss_, avg_eta_ss_, p0_ss_, p1_ss_, delta_p0_ss_, delta_p1_ss_, tageff_ss_, tageff_asym_ss_, production_asym_, detection_asym_);
      if (hasTagState(tag_os_, +1) && hasTagState(tag_ss_, -1) && hasFinalstateState(finalstate_, -1)) integral += evaluate(cp_coeff_, cp_coeff_bar_, coeff_type_, -1, +1, eta_os_, avg_eta_os_, p0_os_, p1_os_, delta_p0_os_, delta_p1_os_, tageff_os_, tageff_asym_os_, -1, eta_ss_, avg_eta_ss_, p0_ss_, p1_ss_, delta_p0_ss_, delta_p1_ss_, tageff_ss_, tageff_asym_ss_, production_asym_, detection_asym_);

      if (hasTagState(tag_os_, +1) && hasTagState(tag_ss_, 0) && hasFinalstateState(finalstate_, +1))  integral += evaluate(cp_coeff_, cp_coeff_bar_, coeff_type_, +1, +1, eta_os_, avg_eta_os_, p0_os_, p1_os_, delta_p0_os_, delta_p1_os_, tageff_os_, tageff_asym_os_, 0, eta_ss_, avg_eta_ss_, p0_ss_, p1_ss_, delta_p0_ss_, delta_p1_ss_, tageff_ss_, tageff_asym_ss_, production_asym_, detection_asym_);
      if (hasTagState(tag_os_, +1) && hasTagState(tag_ss_, 0) && hasFinalstateState(finalstate_, -1))  integral += evaluate(cp_coeff_, cp_coeff_bar_, coeff_type_, -1, +1, eta_os_, avg_eta_os_, p0_os_, p1_os_, delta_p0_os_, delta_p1_os_, tageff_os_, tageff_asym_os_, 0, eta_ss_, avg_eta_ss_, p0_ss_, p1_ss_, delta_p0_ss_, delta_p1_ss_, tageff_ss_, tageff_asym_ss_, production_asym_, detection_asym_);

      if (hasTagState(tag_os_, -1) && hasTagState(tag_ss_, +1) && hasFinalstateState(finalstate_, +1)) integral += evaluate(cp_coeff_, cp_coeff_bar_, coeff_type_, +1, -1, eta_os_, avg_eta_os_, p0_os_, p1_os_, delta_p0_os_, delta_p1_os_, tageff_os_, tageff_asym_os_, +1, eta_ss_, avg_eta_ss_, p0_ss_, p1_ss_, delta_p0_ss_, delta_p1_ss_, tageff_ss_, tageff_asym_ss_, production_asym_, detection_asym_);
      if (hasTagState(tag_os_, -1) && hasTagState(tag_ss_, +1) && hasFinalstateState(finalstate_, -1)) integral += evaluate(cp_coeff_, cp_coeff_bar_, coeff_type_, -1, -1, eta_os_, avg_eta_os_, p0_os_, p1_os_, delta_p0_os_, delta_p1_os_, tageff_os_, tageff_asym_os_, +1, eta_ss_, avg_eta_ss_, p0_ss_, p1_ss_, delta_p0_ss_, delta_p1_ss_, tageff_ss_, tageff_asym_ss_, production_asym_, detection_asym_);

      if (hasTagState(tag_os_, -1) && hasTagState(tag_ss_, -1) && hasFinalstateState(finalstate_, +1)) integral += evaluate(cp_coeff_, cp_coeff_bar_, coeff_type_, +1, -1, eta_os_, avg_eta_os_, p0_os_, p1_os_, delta_p0_os_, delta_p1_os_, tageff_os_, tageff_asym_os_, -1, eta_ss_, avg_eta_ss_, p0_ss_, p1_ss_, delta_p0_ss_, delta_p1_ss_, tageff_ss_, tageff_asym_ss_, production_asym_, detection_asym_);
      if (hasTagState(tag_os_, -1) && hasTagState(tag_ss_, -1) && hasFinalstateState(finalstate_, -1)) integral += evaluate(cp_coeff_, cp_coeff_bar_, coeff_type_, -1, -1, eta_os_, avg_eta_os_, p0_os_, p1_os_, delta_p0_os_, delta_p1_os_, tageff_os_, tageff_asym_os_, -1, eta_ss_, avg_eta_ss_, p0_ss_, p1_ss_, delta_p0_ss_, delta_p1_ss_, tageff_ss_, tageff_asym_ss_, production_asym_, detection_asym_);

      if (hasTagState(tag_os_, -1) && hasTagState(tag_ss_, 0) && hasFinalstateState(finalstate_, +1))  integral += evaluate(cp_coeff_, cp_coeff_bar_, coeff_type_, +1, -1, eta_os_, avg_eta_os_, p0_os_, p1_os_, delta_p0_os_, delta_p1_os_, tageff_os_, tageff_asym_os_, 0, eta_ss_, avg_eta_ss_, p0_ss_, p1_ss_, delta_p0_ss_, delta_p1_ss_, tageff_ss_, tageff_asym_ss_, production_asym_, detection_asym_);
      if (hasTagState(tag_os_, -1) && hasTagState(tag_ss_, 0) && hasFinalstateState(finalstate_, -1))  integral += evaluate(cp_coeff_, cp_coeff_bar_, coeff_type_, -1, -1, eta_os_, avg_eta_os_, p0_os_, p1_os_, delta_p0_os_, delta_p1_os_, tageff_os_, tageff_asym_os_, 0, eta_ss_, avg_eta_ss_, p0_ss_, p1_ss_, delta_p0_ss_, delta_p1_ss_, tageff_ss_, tageff_asym_ss_, production_asym_, detection_asym_);

      if (hasTagState(tag_os_, 0) && hasTagState(tag_ss_, +1) && hasFinalstateState(finalstate_, +1)) integral += evaluate(cp_coeff_, cp_coeff_bar_, coeff_type_, +1, 0, eta_os_, avg_eta_os_, p0_os_, p1_os_, delta_p0_os_, delta_p1_os_, tageff_os_, tageff_asym_os_, +1, eta_ss_, avg_eta_ss_, p0_ss_, p1_ss_, delta_p0_ss_, delta_p1_ss_, tageff_ss_, tageff_asym_ss_, production_asym_, detection_asym_);
      if (hasTagState(tag_os_, 0) && hasTagState(tag_ss_, +1) && hasFinalstateState(finalstate_, -1)) integral += evaluate(cp_coeff_, cp_coeff_bar_, coeff_type_, -1, 0, eta_os_, avg_eta_os_, p0_os_, p1_os_, delta_p0_os_, delta_p1_os_, tageff_os_, tageff_asym_os_, +1, eta_ss_, avg_eta_ss_, p0_ss_, p1_ss_, delta_p0_ss_, delta_p1_ss_, tageff_ss_, tageff_asym_ss_, production_asym_, detection_asym_);

      if (hasTagState(tag_os_, 0) && hasTagState(tag_ss_, -1) && hasFinalstateState(finalstate_, +1)) integral += evaluate(cp_coeff_, cp_coeff_bar_, coeff_type_, +1, 0, eta_os_, avg_eta_os_, p0_os_, p1_os_, delta_p0_os_, delta_p1_os_, tageff_os_, tageff_asym_os_, -1, eta_ss_, avg_eta_ss_, p0_ss_, p1_ss_, delta_p0_ss_, delta_p1_ss_, tageff_ss_, tageff_asym_ss_, production_asym_, detection_asym_);
      if (hasTagState(tag_os_, 0) && hasTagState(tag_ss_, -1) && hasFinalstateState(finalstate_, -1)) integral += evaluate(cp_coeff_, cp_coeff_bar_, coeff_type_, -1, 0, eta_os_, avg_eta_os_, p0_os_, p1_os_, delta_p0_os_, delta_p1_os_, tageff_os_, tageff_asym_os_, -1, eta_ss_, avg_eta_ss_, p0_ss_, p1_ss_, delta_p0_ss_, delta_p1_ss_, tageff_ss_, tageff_asym_ss_, production_asym_, detection_asym_);

      if (hasTagState(tag_os_, 0) && hasTagState(tag_ss_, 0) && hasFinalstateState(finalstate_, +1))  integral += evaluate(cp_coeff_, cp_coeff_bar_, coeff_type_, +1, 0, eta_os_, avg_eta_os_, p0_os_, p1_os_, delta_p0_os_, delta_p1_os_, tageff_os_, tageff_asym_os_, 0, eta_ss_, avg_eta_ss_, p0_ss_, p1_ss_, delta_p0_ss_, delta_p1_ss_, tageff_ss_, tageff_asym_ss_, production_asym_, detection_asym_);
      if (hasTagState(tag_os_, 0) && hasTagState(tag_ss_, 0) && hasFinalstateState(finalstate_, -1))  integral += evaluate(cp_coeff_, cp_coeff_bar_, coeff_type_, -1, 0, eta_os_, avg_eta_os_, p0_os_, p1_os_, delta_p0_os_, delta_p1_os_, tageff_os_, tageff_asym_os_, 0, eta_ss_, avg_eta_ss_, p0_ss_, p1_ss_, delta_p0_ss_, delta_p1_ss_, tageff_ss_, tageff_asym_ss_, production_asym_, detection_asym_);
      // debug
      // std::cout << "Coeff: " << coeff_type_ << " : OS + SS Integral : " << integral << std::endl;
      return integral;
    }
  }
  return 0;
}


std::pair<double, double> DecRateCoeff_Bd::calibrate(double eta, double avg_eta, double p0, double p1, double delta_p0, double delta_p1) const
{
  double eta_cal = 0;
  double eta_cal_b = 0;
  double eta_cal_bbar = 0;

  // calculate calibrated average eta
  eta_cal = p0 + p1 * ( eta - avg_eta );

  // if eta is larger or equal 0.5 return 0.5
  if (eta >= 0.5){
    eta_cal      = 0.5;
    eta_cal_b    = 0.5;
    eta_cal_bbar = 0.5;
  }
  else{
    // calibrate eta
    eta_cal_b    = p0 + 0.5 * delta_p0 + ( p1 + 0.5 * delta_p1 ) * ( eta - avg_eta );
    eta_cal_bbar = p0 - 0.5 * delta_p0 + ( p1 - 0.5 * delta_p1 ) * ( eta - avg_eta );
  }
  // if calibrated average eta is larger or equal 0.5 return 0.5
  if (eta_cal >= 0.5){
    eta_cal_b    = 0.5;
    eta_cal_bbar = 0.5;
  }
  // if calibrated eta is smaller than 0 return 0
  if (eta_cal_b < 0.0)    eta_cal_b = 0;
  if (eta_cal_bbar < 0.0) eta_cal_bbar = 0;

  // the next few lines set every eta value (avg and high/low from asymmetries) to zero
  // if only one of them is below zero. This seems to introduce a fit bias on our CP
  // observables. (CC)
  // if (eta_cal < 0.0 || eta_cal_b < 0.0 || eta_cal_bbar < 0.0){
  //   eta_cal_b    = 0.0;
  //   eta_cal_bbar = 0.0;
  // }
  return std::make_pair(eta_cal_b, eta_cal_bbar);
}


Double_t DecRateCoeff_Bd::evaluate(double cp_coeff,
                                double cp_coeff_bar,
                                CoeffType coeff_type,
                                int    finalstate,
                                int    tag_os,
                                double eta_os,
                                double avg_eta_os,
                                double p0_os,
                                double p1_os,
                                double delta_p0_os,
                                double delta_p1_os,
                                double tageff_os,
                                double tageff_asym_os,
                                int    tag_ss,
                                double eta_ss,
                                double avg_eta_ss,
                                double p0_ss,
                                double p1_ss,
                                double delta_p0_ss,
                                double delta_p1_ss,
                                double tageff_ss,
                                double tageff_asym_ss,
                                double production_asym,
                                double detection_asym) const
{
  // calibrate single tagger
  std::pair<double, double> calibrated_mistag_os = calibrate(eta_os, avg_eta_os, p0_os, p1_os, delta_p0_os, delta_p1_os);
  std::pair<double, double> calibrated_mistag_ss = calibrate(eta_ss, avg_eta_ss, p0_ss, p1_ss, delta_p0_ss, delta_p1_ss);

  double eta_os_b    = calibrated_mistag_os.first;
  double eta_os_bbar = calibrated_mistag_os.second;
  double eta_ss_b    = calibrated_mistag_ss.first;
  double eta_ss_bbar = calibrated_mistag_ss.second;

  if(onetagger_) {
    if(coeff_type == kCos){
      if(finalstate == 1){
        if(tag_os == 0) return cp_coeff * (1 + detection_asym) * (tageff_asym_os - 2 * production_asym * (1 - tageff_os));
        else return cp_coeff * (1 + detection_asym) * (tag_os * tageff_os - 0.5 * tageff_asym_os
                                                       - production_asym * (tageff_os - 0.5 * tag_os * tageff_asym_os
                                                                                      - tag_os * tageff_os * (eta_os_b - eta_os_bbar)
                                                                                      + 0.5 * tag_os * tageff_asym_os * (eta_os_b + eta_os_bbar))
                                                       - tageff_os * tag_os * (eta_os_b + eta_os_bbar)
                                                       + 0.5 * tageff_asym_os * tag_os * (eta_os_b - eta_os_bbar) );
      }
      else if(finalstate == -1){
        if(tag_os == 0) return cp_coeff_bar * (1 - detection_asym) * (tageff_asym_os - 2 * production_asym * (1 - tageff_os));
        else return -1.0 * cp_coeff_bar * (1 - detection_asym) * (tag_os * tageff_os - 0.5 * tageff_asym_os
                                                                 - production_asym * (tageff_os - 0.5 * tag_os * tageff_asym_os
                                                                                                - tag_os * tageff_os * (eta_os_b - eta_os_bbar)
                                                                                                + 0.5 * tag_os * tageff_asym_os * (eta_os_b + eta_os_bbar))
                                                                 - tageff_os * tag_os * (eta_os_b + eta_os_bbar)
                                                                 + 0.5 * tageff_asym_os * tag_os * (eta_os_b - eta_os_bbar) );
      }
      else{
        std::cout << "ERROR\t" << "DecRateCoeff_Bd::evaluate(...): No valid finalstate!" << std::endl;
        abort();
      }
    }
    if(coeff_type == kSin){
      if(finalstate == 1){
        if(tag_os == 0) return cp_coeff * (1 + detection_asym) * (tageff_asym_os - 2 * production_asym * (1 - tageff_os));
        else return -1.0 * cp_coeff * (1 + detection_asym) * (tag_os * tageff_os - 0.5 * tageff_asym_os
                                                             - production_asym * (tageff_os - 0.5 * tag_os * tageff_asym_os
                                                                                            - tag_os * tageff_os * (eta_os_b - eta_os_bbar)
                                                                                            + 0.5 * tag_os * tageff_asym_os * (eta_os_b + eta_os_bbar))
                                                             - tageff_os * tag_os * (eta_os_b + eta_os_bbar)
                                                             + 0.5 * tageff_asym_os * tag_os * (eta_os_b - eta_os_bbar) );
      }
      else if(finalstate == -1){
        if(tag_os == 0) return cp_coeff_bar * (1 - detection_asym) * (tageff_asym_os - 2 * production_asym * (1 - tageff_os));
        else return cp_coeff_bar * (1 - detection_asym) * (tag_os * tageff_os - 0.5 * tageff_asym_os
                                                          - production_asym * (tageff_os - 0.5 * tag_os * tageff_asym_os
                                                                                         - tag_os * tageff_os * (eta_os_b - eta_os_bbar)
                                                                                         + 0.5 * tag_os * tageff_asym_os * (eta_os_b + eta_os_bbar))
                                                          - tageff_os * tag_os * (eta_os_b + eta_os_bbar)
                                                          + 0.5 * tageff_asym_os * tag_os * (eta_os_b - eta_os_bbar) );
      }
      else{
        std::cout << "ERROR\t" << "DecRateCoeff_Bd::evaluate(...): No valid finalstate!" << std::endl;
        abort();
      }
    }
    if(coeff_type == kSinh){
      if(finalstate == 1){
        if(tag_os == 0) return cp_coeff * (1 + detection_asym) * (2 * (1 - tageff_os) - tageff_asym_os * production_asym);
        else return cp_coeff * (1 + detection_asym) * (tageff_os - 0.5 * tag_os * tageff_asym_os
                                                                 - tag_os * production_asym * (tageff_os - 0.5 * tag_os * tageff_asym_os
                                                                                               - tageff_os * (eta_os_b + eta_os_bbar)
                                                                                               + 0.5 * tageff_asym_os * (eta_os_b - eta_os_bbar))
                                                                 - tageff_os * tag_os * (eta_os_b - eta_os_bbar)
                                                                 + 0.5 * tageff_asym_os * tag_os * (eta_os_b + eta_os_bbar) );
      }
      else if(finalstate == -1){
        if(tag_os == 0) return cp_coeff_bar * (1 - detection_asym) * (2 * (1 - tageff_os) - tageff_asym_os * production_asym);
        else return cp_coeff_bar * (1 - detection_asym) * (tageff_os - 0.5 * tag_os * tageff_asym_os
                                                                     - tag_os * production_asym * (tageff_os - 0.5 * tag_os * tageff_asym_os
                                                                                                  - tageff_os * (eta_os_b + eta_os_bbar)
                                                                                                  + 0.5 * tageff_asym_os * (eta_os_b - eta_os_bbar))
                                                                     - tageff_os * tag_os * (eta_os_b - eta_os_bbar)
                                                                     + 0.5 * tageff_asym_os * tag_os * (eta_os_b + eta_os_bbar) );
      }
      else{
        std::cout << "ERROR\t" << "DecRateCoeff_Bd::evaluate(...): No valid finalstate!" << std::endl;
        abort();
      }
    }
    if(coeff_type == kCosh){
      if(finalstate == 1){
        if(tag_os == 0) return cp_coeff * (1 + detection_asym) * (2 * (1 - tageff_os) - tageff_asym_os * production_asym);
        else return cp_coeff * (1 + detection_asym) * (tageff_os - 0.5 * tag_os * tageff_asym_os
                                                                 - tag_os * production_asym * (tageff_os - 0.5 * tag_os * tageff_asym_os
                                                                                               - tageff_os * (eta_os_b + eta_os_bbar)
                                                                                               + 0.5 * tageff_asym_os * (eta_os_b - eta_os_bbar))
                                                                 - tageff_os * tag_os * (eta_os_b - eta_os_bbar)
                                                                 + 0.5 * tageff_asym_os * tag_os * (eta_os_b + eta_os_bbar) );
      }
      else if(finalstate == -1){
        if(tag_os == 0) return cp_coeff_bar * (1 - detection_asym) * (2 * (1 - tageff_os) - tageff_asym_os * production_asym);
        else return cp_coeff_bar * (1 - detection_asym) * (tageff_os - 0.5 * tag_os * tageff_asym_os
                                                                     - tag_os * production_asym * (tageff_os - 0.5 * tag_os * tageff_asym_os
                                                                                                  - tageff_os * (eta_os_b + eta_os_bbar)
                                                                                                  + 0.5 * tageff_asym_os * (eta_os_b - eta_os_bbar))
                                                                     - tageff_os * tag_os * (eta_os_b - eta_os_bbar)
                                                                     + 0.5 * tageff_asym_os * tag_os * (eta_os_b + eta_os_bbar) );
      }
      else{
        std::cout << "ERROR\t" << "DecRateCoeff_Bd::evaluate(...): No valid finalstate!" << std::endl;
        abort();
      }
    }
    else{
      std::cout << "ERROR\t" << "DecRateCoeff_Bd::evaluate(...): No valid finalstate!" << std::endl;
      abort();
    }
  }

  // combine taggers and calculate intermediate steps
  double sum = 0;
  double difference = 0;

  double tageff_os_b = tageff_os - 0.5 * tageff_asym_os;
  double tageff_os_bbar = tageff_os + 0.5 * tageff_asym_os;

  double tageff_ss_b = tageff_ss - 0.5 * tageff_asym_ss;
  double tageff_ss_bbar = tageff_ss + 0.5 * tageff_asym_ss;

  if( tag_os == 0 && tag_ss == 0) {
    sum = (1 - tageff_os_b - tageff_ss_b + tageff_os_b * tageff_ss_b) + (1 - tageff_os_bbar - tageff_ss_bbar + tageff_os_bbar * tageff_ss_bbar);
    difference = (1 - tageff_os_b - tageff_ss_b + tageff_os_b * tageff_ss_b) - (1 - tageff_os_bbar - tageff_ss_bbar + tageff_os_bbar * tageff_ss_bbar);
  }
  else if(tag_os == 0 && tag_ss != 0){
    sum = 0.5 * tageff_ss_b * (1 - tageff_os_b + tag_ss * (1 - tageff_os_b - 2 * eta_ss_b + 2 * eta_ss_b * tageff_os_b))
          + 0.5 * tageff_ss_bbar * (1 - tageff_os_bbar - tag_ss * (1 - tageff_os_bbar - 2 * eta_ss_bbar + 2 * eta_ss_bbar * tageff_os_bbar));
    difference = 0.5 * tageff_ss_b * (1 - tageff_os_b + tag_ss * (1 - tageff_os_b - 2 * eta_ss_b + 2 * eta_ss_b * tageff_os_b))
                 - 0.5 * tageff_ss_bbar * (1 - tageff_os_bbar - tag_ss * (1 - tageff_os_bbar - 2 * eta_ss_bbar + 2 * eta_ss_bbar * tageff_os_bbar));
  }
  else if(tag_ss == 0 && tag_os != 0){
    sum = 0.5 * tageff_os_b * (1 - tageff_ss_b + tag_os * (1 - tageff_ss_b - 2 * eta_os_b + 2 * eta_os_b * tageff_ss_b))
          + 0.5 * tageff_os_bbar * (1 - tageff_ss_bbar - tag_os * (1 - tageff_ss_bbar - 2 * eta_os_bbar + 2 * eta_os_bbar * tageff_ss_bbar));
    difference = 0.5 * tageff_os_b * (1 - tageff_ss_b + tag_os * (1 - tageff_ss_b - 2 * eta_os_b + 2 * eta_os_b * tageff_ss_b))
                 - 0.5 * tageff_os_bbar * (1 - tageff_ss_bbar - tag_os * (1 - tageff_ss_bbar - 2 * eta_os_bbar + 2 * eta_os_bbar * tageff_ss_bbar));
  }
  else if(tag_ss != 0 && tag_os != 0){
    sum = 0.25 * tageff_os_b * tageff_ss_b * (1 + tag_ss * (1 - 2 * eta_ss_b) + tag_os * (1 - 2 * eta_os_b)
                                                + tag_os * tag_ss * (1 - 2 * eta_os_b - 2 * eta_ss_b + 4 * eta_os_b * eta_ss_b))
          + 0.25 * tageff_os_bbar * tageff_ss_bbar * (1 - tag_ss * (1 - 2 * eta_ss_bbar) - tag_os * (1 - 2 * eta_os_bbar)
                                                        + tag_os * tag_ss * (1 - 2 * eta_os_bbar - 2 * eta_ss_bbar + 4 * eta_os_bbar * eta_ss_bbar));
    difference = 0.25 * tageff_os_b * tageff_ss_b * (1 + tag_ss * (1 - 2 * eta_ss_b) + tag_os * (1 - 2 * eta_os_b)
                                                       + tag_os * tag_ss * (1 - 2 * eta_os_b - 2 * eta_ss_b + 4 * eta_os_b * eta_ss_b))
                - 0.25 * tageff_os_bbar * tageff_ss_bbar * (1 - tag_ss * (1 - 2 * eta_ss_bbar) - tag_os * (1 - 2 * eta_os_bbar)
                                                              + tag_os * tag_ss * (1 - 2 * eta_os_bbar - 2 * eta_ss_bbar + 4 * eta_os_bbar * eta_ss_bbar));
  }
  else{
    std::cout << "ERROR\t" << "DecRateCoeff_Bd::evaluate(...): No valid tag combination!" << std::endl;
      abort();
  }

  // calculate and return coefficients
  if (coeff_type == kSin){
    if(finalstate==1){
      return -1.0 * (1 + detection_asym) * cp_coeff * ( difference - production_asym * sum );
    }
    else if(finalstate==-1){
      return +1.0 * (1 - detection_asym) * cp_coeff_bar * ( difference - production_asym * sum );
    }
    else{
      std::cout << "ERROR\t" << "DecRateCoeff_Bd::evaluate(...): No valid finalstate!" << std::endl;
      abort();
    }
  }
  else if (coeff_type == kCos){
    if(finalstate==1){
      return +1.0 * (1 + detection_asym) * cp_coeff * ( difference - production_asym * sum );
    }
    else if(finalstate==-1){
      return -1.0 * (1 - detection_asym) * cp_coeff_bar * ( difference - production_asym * sum );
    }
    else{
      std::cout << "ERROR\t" << "DecRateCoeff_Bd::evaluate(...): No valid finalstate!" << std::endl;
    abort();
    }
  }
  else if (coeff_type == kSinh){
    if(finalstate==1){
      return +1.0 * (1 + detection_asym) * cp_coeff * ( sum - production_asym * difference );
    }
    else if(finalstate==-1){
      return +1.0 * (1 - detection_asym) * cp_coeff_bar * ( sum - production_asym * difference );
    }
    else{
      std::cout << "ERROR\t" << "DecRateCoeff_Bd::evaluate(...): No valid finalstate!" << std::endl;
      abort();
    }
  }
  else if (coeff_type == kCosh){
    if(finalstate==1){
      return +1.0 * (1 + detection_asym) * cp_coeff * ( sum - production_asym * difference );
    }
    else if(finalstate==-1){
      return +1.0 * (1 - detection_asym) * cp_coeff_bar * ( sum - production_asym * difference );
    }
    else{
      std::cout << "ERROR\t" << "DecRateCoeff_Bd::evaluate(...): No valid finalstate!" << std::endl;
      abort();
    }
  }
  else{
    std::cout << "ERROR\t" << "DecRateCoeff_Bd::evaluate(...): No valid coefficient type!" << std::endl;
    abort();
  }
}


bool DecRateCoeff_Bd::isTagInRange(const RooCategoryProxy& tag, int tag_state, const char* rangeName) const
{
  if (tag.arg().lookupType(tag_state) == nullptr){
    return false;
  }
  else{
    return dynamic_cast<const RooCategory&>(tag.arg()).isStateInRange(rangeName, tag.arg().lookupType(tag_state)->GetName());
  }
}


bool DecRateCoeff_Bd::hasTagState(const RooCategoryProxy& tag, int tag_state) const
{
  return dynamic_cast<const RooCategory&>(tag.arg()).isValidIndex(tag_state);
}

bool DecRateCoeff_Bd::isFinalstateInRange(const RooCategoryProxy& finalstate, int finalstate_state, const char* rangeName) const
{
  if (finalstate.arg().lookupType(finalstate_state) == nullptr){
    return false;
  }
  else{
    return dynamic_cast<const RooCategory&>(finalstate.arg()).isStateInRange(rangeName, finalstate.arg().lookupType(finalstate_state)->GetName());
  }
}

bool DecRateCoeff_Bd::hasFinalstateState(const RooCategoryProxy& finalstate, int finalstate_state) const
{
  return dynamic_cast<const RooCategory&>(finalstate.arg()).isValidIndex(finalstate_state);
}

int  DecRateCoeff_Bd::getIndex(const RooCategoryProxy& tag) const
{
  return dynamic_cast<const RooCategory&>(tag.arg()).getIndex();
}

