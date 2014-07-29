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
	      RooAbsReal& _production_asym_,
        bool _calibrate_os_,
        bool _calibrate_ss_);
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

  bool calibrate_os_ ;
  bool calibrate_ss_ ;

  Double_t evaluate() const ;

private:

  mutable double eta_cal_os_ ;
  mutable double eta_cal_os_b_ ;
  mutable double eta_cal_os_bbar_ ;

  mutable double eta_cal_ss_ ;
  mutable double eta_cal_ss_b_ ;
  mutable double eta_cal_ss_bbar_ ;

  mutable double eta_combined_b_ ;
  mutable double eta_combined_bbar_ ;
  mutable int    tag_combined_ ;

  void calibrate_os() const ;
  void calibrate_ss() const ;
  void combine_mistags() const ;
  void combine_tags() const ;

  ClassDef(SuperCoefficient,1) // CP coefficient for RooBDecay PDF
};

} // namespace bdecay
} // namespace functions
} // namespace roofit
} // namespace doofit

#endif
