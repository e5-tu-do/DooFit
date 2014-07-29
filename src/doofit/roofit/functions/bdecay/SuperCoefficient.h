#ifndef DOOFIT_ROOFIT_FUNCTIONS_BDECAY_SUPERCOEFFICIENT
#define DOOFIT_ROOFIT_FUNCTIONS_BDECAY_SUPERCOEFFICIENT

#include "RooAbsReal.h"
#include "RooRealProxy.h"
#include "RooAbsCategory.h"
#include "RooCategoryProxy.h"

namespace doofit {
namespace roofit {
namespace functions {
namespace bdecay {

class SuperCoefficient : public RooAbsReal {
public:
  enum CoeffType {
    kSin  = 3,
    kCos  = 2,
    kSinh = 1,
    kCosh = 0
  };

  SuperCoefficient(const std::string& name,
	      RooAbsReal& _cp_coeff_,
        CoeffType   _coeff_type_,
	      RooAbsCategory& _tag_os_,
        RooAbsReal& _eta_os_,
	      RooAbsReal& _avg_eta_os_,
	      RooAbsReal& _p0_os_,
	      RooAbsReal& _p1_os_,
	      RooAbsReal& _delta_p0_os_,
	      RooAbsReal& _delta_p1_os_,
        RooAbsCategory& _tag_ss_,
	      RooAbsReal& _eta_ss_,
	      RooAbsReal& _avg_eta_ss_,
	      RooAbsReal& _p0_ss_,
	      RooAbsReal& _p1_ss_,
	      RooAbsReal& _delta_p0_ss_,
	      RooAbsReal& _delta_p1_ss_,
	      RooAbsReal& _production_asym_);
  SuperCoefficient(const SuperCoefficient& other, const char* name=0) ;
  virtual TObject* clone(const char* newname) const { return new SuperCoefficient(*this,newname); }
  inline virtual ~SuperCoefficient() { }

  Int_t getAnalyticalIntegral(RooArgSet& allVars, RooArgSet& analVars, const char* rangeName=0) const ;
  Double_t analyticalIntegral(Int_t code, const char* rangeName=0) const ;

protected:

  RooRealProxy cp_coeff_ ;
  CoeffType    coeff_type_ ;
  
  RooCategoryProxy tag_os_ ;
  RooRealProxy eta_os_ ;
  RooRealProxy avg_eta_os_ ;
  RooRealProxy p0_os_ ;
  RooRealProxy p1_os_ ;
  RooRealProxy delta_p0_os_ ;
  RooRealProxy delta_p1_os_ ;

  RooCategoryProxy tag_ss_ ;
  RooRealProxy eta_ss_ ;
  RooRealProxy avg_eta_ss_ ;
  RooRealProxy p0_ss_ ;
  RooRealProxy p1_ss_ ;
  RooRealProxy delta_p0_ss_ ;
  RooRealProxy delta_p1_ss_ ;

  RooRealProxy production_asym_ ;

  Double_t evaluate() const ;

private:

  std::pair<double, double> calibrate(double eta, double avg_eta, double p0, double p1, double delta_p0, double delta_p1) const ;
  std::pair<double, double> combine_mistags(std::pair<double, double> calibrated_mistag_os, std::pair<double, double> calibrated_mistag_ss, int tag_os, int tag_ss) const ;
  int                       combine_tags(std::pair<double, double> calibrated_mistag_os, std::pair<double, double> calibrated_mistag_ss, int tag_os, int tag_ss) const ;

  Double_t evaluate(double cp_coeff,
                    CoeffType coeff_type,
                    int    tag_os,
                    double eta_os,
                    double avg_eta_os,
                    double p0_os,
                    double p1_os,
                    double delta_p0_os,
                    double delta_p1_os,
                    int    tag_ss,
                    double eta_ss,
                    double avg_eta_ss,
                    double p0_ss,
                    double p1_ss,
                    double delta_p0_ss,
                    double delta_p1_ss,
                    double production_asym) const ;

  bool isTagInRange(const RooCategoryProxy& tag, int tag_state, const char* rangeName) const ;

  ClassDef(SuperCoefficient,1) // CP coefficient for RooBDecay PDF
};

} // namespace bdecay
} // namespace functions
} // namespace roofit
} // namespace doofit

#endif
