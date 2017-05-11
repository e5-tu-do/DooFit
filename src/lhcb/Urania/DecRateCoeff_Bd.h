/*!
 * \brief     cosh/sinh/cos/sin coefficients in decay rate equations
 * \author    Alex Birnkraut, Christophe Cauet, Frank Meier, Julian Wishahi
 * \date      2016-04-22
 *
 * This class calculates the coefficients which go in front of the
 * cosh/sinh/cos/sin terms in the decay rate equations (RooBDecay). By forming
 * a suitable sum over the true initial state flavour inside the class
 * implementation, it is possible to treat production, detection and tagging
 * efficiency asymmetries from first principles.
 * The class was developed based on the idea of Manuel Schiller.
 */
#ifndef DECRATECOEFF_BD
#define DECRATECOEFF_BD

#include "RooAbsReal.h"
#include "RooRealProxy.h"
#include "RooAbsCategory.h"
#include "RooCategoryProxy.h"

class DecRateCoeff_Bd : public RooAbsReal {
public:
  enum CoeffType {
    kSin  = 3,
    kCos  = 2,
    kSinh = 1,
    kCosh = 0
  };

  DecRateCoeff_Bd();

  DecRateCoeff_Bd(const std::string& name,
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
               RooAbsReal& _detection_asym_);

  DecRateCoeff_Bd(const std::string& name,
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
                RooAbsReal& _detection_asym_);

  DecRateCoeff_Bd(const DecRateCoeff_Bd& other, const char* name=0) ;
  virtual TObject* clone(const char* newname) const { return new DecRateCoeff_Bd(*this,newname); }
  inline virtual ~DecRateCoeff_Bd() { }

  Int_t getAnalyticalIntegral(RooArgSet& allVars, RooArgSet& analVars, const char* rangeName=0) const ;
  Int_t getAnalyticalIntegralWN(RooArgSet& allVars, RooArgSet& analVars, const RooArgSet* normSet, const char* rangeName=0) const ;
  Double_t analyticalIntegral(Int_t code, const char* rangeName=0) const ;

protected:

  RooRealProxy cp_coeff_ ;
  RooRealProxy cp_coeff_bar_ ;
  CoeffType    coeff_type_ ;
  RooCategoryProxy finalstate_;

  RooCategoryProxy tag_os_ ;
  RooRealProxy eta_os_ ;
  RooRealProxy avg_eta_os_ ;
  RooRealProxy p0_os_ ;
  RooRealProxy p1_os_ ;
  RooRealProxy delta_p0_os_ ;
  RooRealProxy delta_p1_os_ ;
  RooRealProxy tageff_os_;
  RooRealProxy tageff_asym_os_;

  RooCategoryProxy tag_ss_ ;
  RooRealProxy eta_ss_ ;
  RooRealProxy avg_eta_ss_ ;
  RooRealProxy p0_ss_ ;
  RooRealProxy p1_ss_ ;
  RooRealProxy delta_p0_ss_ ;
  RooRealProxy delta_p1_ss_ ;
  RooRealProxy tageff_ss_;
  RooRealProxy tageff_asym_ss_;

  RooRealProxy production_asym_ ;
  RooRealProxy detection_asym_ ;

  Bool_t onetagger_;

  Double_t evaluate() const ;

private:

  std::pair<double, double> calibrate(double eta, double avg_eta, double p0, double p1, double delta_p0, double delta_p1) const ;

  Double_t evaluate(double cp_coeff,
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
                    double detection_asym) const ;

  bool isTagInRange(const RooCategoryProxy& tag, int tag_state, const char* rangeName) const ;
  bool hasTagState(const RooCategoryProxy& tag, int tag_state) const;

  bool isFinalstateInRange(const RooCategoryProxy& finalstate, int finalstate_state, const char* rangeName) const ;
  bool hasFinalstateState(const RooCategoryProxy& finalstate, int finalstate_state) const ;

  int  getIndex(const RooCategoryProxy& tag) const;

  ClassDef(DecRateCoeff_Bd,1) // CP coefficient for RooBDecay PDF
};

#endif
