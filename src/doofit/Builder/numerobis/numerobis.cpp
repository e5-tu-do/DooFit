#include "doofit/Builder/numerobis/numerobis.h"

// from project
#include "doofit/Config/CommonConfig.h"
#include "doofit/Builder/numerobis/numerobis_config.h"
#include "doofit/Builder/numerobis/blueprint/blueprint.h"

//=== OLD SPB stuff ===========//
// from STL
#include <string>
#include <map>
#include <vector>

// from boost
#include <boost/bimap.hpp>
#include <boost/foreach.hpp>

// from ROOT

// from RooFit
#include "RooConstVar.h"
#include "RooRealVar.h"
#include "RooWorkspace.h"

// from RooFit - Category
#include "RooCategory.h"
#include "RooSuperCategory.h"

// from RooFit - Blinded
#include "RooUnblindUniform.h"

// from RooFit Pdfs - Comb
#include "RooAddPdf.h"
#include "RooEffProd.h"
#include "RooExtendPdf.h"
#include "RooProdPdf.h"
#include "RooSimultaneous.h"

// from RooFit Pdfs - Basic
#include "RooGaussian.h"
#include "RooExponential.h"

// from RooFit Pdfs - Basic w. Resol
#include "RooDecay.h"
#include "RooBCPGenDecay.h"

// from RooFit Resols - Comb
#include "RooAddModel.h"

// from RooFit Resols - Basic
#include "RooGaussModel.h"
#include "RooTruthModel.h"

//=== END OF OLD SPB stuff ====//


namespace doofit {
namespace builder {
namespace numerobis {

using namespace std;
using namespace RooFit;

using doofit::Config::CommonConfig;

Numerobis::Numerobis(const CommonConfig& cfg_com, const NumerobisConfig& cfg_num) 
  : cfg_com_(cfg_com),
    cfg_num_(cfg_num),
    workspace_(NULL),
    import_arg_(Silence()),
    cf_fixed_(true)
{
  
}

Numerobis::~Numerobis() {
  
}

void Numerobis::Configure() {
  
}

void Numerobis::AddToWorkspace(RooWorkspace& ws) {
  workspace_ = &ws;
  
  // Dimensions 
  RooRealVar* varMass     = CreateDimReal("varMass", "m", 5200., 5370., "MeV/c^{2}", "Mass");
  RooRealVar* varProptime = CreateDimReal("varProptime", "t", 0.3, 15., "ps", "Proptime");
  RooRealVar* varProptimeErr = CreateDimReal("varProptimeErr", "#sigma_{t}", 0., 3., "ps", "ProptimeErr");

  // Dimensions - Discrete
  RooCategory* varTag = CreateDimDisc("varTag","d");
  varTag->defineType("B0",1);
  varTag->defineType("B0bar",-1);


  // Categories
  RooCategory   isBiased("isBiased","Has Biased Trigger");
        isBiased.defineType("B", 1);
        isBiased.defineType("Nb",0);
  workspace_->import(isBiased,import_arg_);

  RooCategory   catTagging("tagcat","tagcat");
  catTagging.defineType("tc1",1);
  catTagging.defineType("tc2",2);
  catTagging.defineType("tc3",3);
  catTagging.defineType("tc4",4);
  catTagging.defineType("tc5",5);
  workspace_->import(catTagging,import_arg_);

  // Blinded vars
  {
    RooRealVar parProptimeSf("parProptimeSf","S_{J/#psi K_{S}}",-0.7,-10.0,10.0); 
    RooRealVar parProptimeCf("parProptimeCf","C_{J/#psi K_{S}}",0.0,-10.0,10.0);

    RooUnblindUniform parProptimeSf_ub("parProptimeSf_ub","S_{J#!/#!#psi K_{S}} (blinded)","SJpsiKS2011",2.,parProptimeSf);
    workspace_->import(parProptimeSf_ub, import_arg_);

    if(!cf_fixed_){
      RooUnblindUniform parProptimeCf_ub("parProptimeCf_ub","C_{J#!/#!#psi K_{S}} (blinded)","CJpsiKS2011",2.,parProptimeCf);
      workspace_->import(parProptimeCf_ub, import_arg_);        
    } else {
      RooConstVar parProptimeCf_ub("parProptimeCf_ub","C_{J/#psi K_{S}} (fixed)",0.);
      workspace_->import(parProptimeCf_ub, import_arg_);
    }
  }


  string par_pre, pdf_pre, res_pre, acc_pre;
  par_pre = "par";
  pdf_pre = "pdf";
  res_pre = "res";
  acc_pre = "acc";

  string sim_id, comp_id, dim_id;

  sim_id = "DBT";
  /*=======================================================================*/
  // SIGNAL
  comp_id="Sig";
  // Mass
  {
    dim_id = "Mass";
    string par_prefix = par_pre+sim_id+comp_id+dim_id;
    string pdf_prefix = pdf_pre+sim_id+comp_id+dim_id;
    RooRealVar parMean((par_prefix+"Mean").c_str(), "m_{B}", 5250., 5350.,"MeV/c^{2}");
    RooRealVar parSigma1((par_prefix+"SigmaOne").c_str(), "#sigma_{1,m}", 5., 10.,"MeV/c^{2}");
    RooRealVar parSigma2((par_prefix+"SigmaTwo").c_str(), "#sigma_{2,m}", 10., 150.,"MeV/c^{2}");
    RooRealVar parFrac((par_prefix+"Frac").c_str(), "f", 0., 1.);
    RooGaussian pdfG1((pdf_prefix+"One").c_str(),(pdf_prefix+"One").c_str(),*varMass,parMean,parSigma1);
    RooGaussian pdfG2((pdf_prefix+"Two").c_str(),(pdf_prefix+"Two").c_str(),*varMass,parMean,parSigma2);
    RooAddPdf pdf(pdf_prefix.c_str(),pdf_prefix.c_str(),RooArgSet(pdfG1,pdfG2),RooArgSet(parFrac));
    workspace_->import(pdf, import_arg_);
    workspace_->extendSet(("SetPdf"+sim_id+comp_id).c_str(),pdf_prefix.c_str());
  }
  // Proptime
  {
    dim_id = "Proptime";
    string pdf_prefix = pdf_pre+sim_id+comp_id+dim_id;
    string res_prefix = res_pre+sim_id+comp_id+dim_id;
    string acc_prefix = acc_pre+sim_id+comp_id+dim_id;
    // Resolution
    {
      string par_prefix = par_pre+"r"+sim_id+comp_id+dim_id;

      RooRealVar      parrBias(   (par_prefix+"Bias"   ).c_str()  , "Resolution Bias",  0.0);
      RooRealVar      parrSigma1( (par_prefix+"Sigma1" ).c_str()  , "#sigma_{t,1}^{R}", 0.04238);//, 0.01  ,0.04 );
      RooRealVar      parrSigma2( (par_prefix+"Sigma2" ).c_str()  , "#sigma_{t,2}^{R}", 0.0923);//, 0.04  ,0.1  );
      RooRealVar      parrSigma3( (par_prefix+"Sigma3" ).c_str()  , "#sigma_{t,3}^{R}", 0.489 );//, 0.1   ,1.   );
      RooRealVar      parrGMFrac1((par_prefix+"GMFrac1").c_str()  , "f_{1}^{R}",        0.733);//, 0.    ,1.   );
      RooRealVar      parrGMFrac2((par_prefix+"GMFrac2"  ).c_str(), "f_{2}^{R}",        0.257);//, 0.    ,1.   );  

      RooGaussModel   resGM1((res_prefix+"GM1").c_str(),(res_prefix+"GM1").c_str(),*varProptime,parrBias,parrSigma1);
      RooGaussModel   resGM2((res_prefix+"GM2").c_str(),(res_prefix+"GM2").c_str(),*varProptime,parrBias,parrSigma2);
      RooGaussModel   resGM3((res_prefix+"GM3").c_str(),(res_prefix+"GM3").c_str(),*varProptime,parrBias,parrSigma3);
      RooAddModel     res(res_prefix.c_str(),res_prefix.c_str(),RooArgList(resGM1,resGM2,resGM3),RooArgList(parrGMFrac1,parrGMFrac2));

      workspace_->import(res, import_arg_);
    }
    // Acceptance
    { 
      string par_prefix = par_pre+"a"+sim_id+comp_id+dim_id;

      RooRealVar    paraA(   (par_prefix+"A"   ).c_str()   ,"A_{S,a}^{B}"  ,4.549);
      RooRealVar    paraC(   (par_prefix+"C"   ).c_str()   ,"C_{S,a}^{B}"  ,10.91);
      RooRealVar    paraN(   (par_prefix+"N"   ).c_str()   ,"N_{S,a}^{B}"  ,3.);
      RooRealVar    paraB(   (par_prefix+"B"   ).c_str()   ,"B_{S,a}^{B}"  ,1e-8);
      RooRealVar    paraT(   (par_prefix+"T"   ).c_str()   ,"T_{S,a}^{B}"  ,0.0);
        RooRealVar              paraBeta((par_prefix+"Beta").c_str()   ,"#beta^_{S,a}" ,-0.0037);
        RooRealVar              paraTU(  (par_prefix+"TU"  ).c_str()   ,"#t^{U}_{S,a}" ,0.0);

        RooFormulaVar tempacc(acc_prefix.c_str(),"(@0>@4)*(@3*pow((@1*@0),@2*(@0>@4))/(1+pow((@1*@0),@2*(@0>@4))))+@5", RooArgList(*varProptime,paraA,paraC,paraN,paraT,paraB) );
        workspace_->import(tempacc, import_arg_);
    }
    // Decay pdf
    {
      string par_prefix = par_pre+sim_id+comp_id+dim_id;
      RooResolutionModel* res_model  = (RooResolutionModel*)workspace_->obj(res_prefix.c_str());
      RooFormulaVar*      acceptance = (RooFormulaVar*)workspace_->obj(acc_prefix.c_str());

      RooAbsReal* Sf = (RooAbsReal*)workspace_->obj("parProptimeSf_ub");
      RooAbsReal* Cf = (RooAbsReal*)workspace_->obj("parProptimeCf_ub");

      RooRealVar parTau((par_prefix+"Tau").c_str(), "#tau", 1.525,0.5,2.,"ps");
      RooRealVar parDeltaM((par_prefix+"DeltaM").c_str(), "#Delta m_d",0.507);   // PDG2010 (0.507 +- 0.005)/ps

      //RooRealVar parSigProptimeOmegaScale(  par_prefix+"OmegaScale",  "p_{1}",1.);
      //RooRealVar parSigProptimeOmegaMean(   par_prefix+"OmegaMean",   "#bar{#eta}_{c}",0.35);
      RooRealVar parOmegaOffset((par_prefix+"OmegaOffset").c_str(),"p_{0}",0.38);

      RooRealVar parDeltaOmega((par_prefix+"DeltaOmega").c_str()  , "#delta#omega",0.);
      RooRealVar parDeltaRecoEff((par_prefix+"DeltaRecoEff").c_str(),"#delta#varepsilon_{tag}",0.);

      //RooFormulaVar parSigProptimeOmegaFuncUT("parSigProptimeOmegaFunc{U;T}","@0*(@1-@2)+@3",RooArgList(parSigProptimeOmegaScale,*varOmega,parSigProptimeOmegaMean,parSigProptimeOmegaOffset));
      RooBCPGenDecay pdfCP((pdf_prefix+"CP").c_str(), (pdf_prefix+"CP").c_str(), *varProptime, *varTag, parTau, 
                                              parDeltaM, parOmegaOffset, *Cf, *Sf,
                                              parDeltaOmega, parDeltaRecoEff, *res_model, RooBCPGenDecay::SingleSided);

      //RooDecay   pdfCP((pdf_prefix+"CP").c_str(),(pdf_prefix+"CP").c_str(),*varProptime,parTau,*res_model,RooDecay::SingleSided);

      RooEffProd pdfEff(pdf_prefix.c_str(),pdf_prefix.c_str(), pdfCP,*acceptance);
      workspace_->import(pdfEff, import_arg_);
      workspace_->extendSet(("SetPdf"+sim_id+comp_id).c_str(),pdf_prefix.c_str());
    }  
  }
  // Lbg
  comp_id="Lbg";
  // Mass
  {
    dim_id = "Mass";
    string par_prefix = par_pre+sim_id+comp_id+dim_id;
    string pdf_prefix = pdf_pre+sim_id+comp_id+dim_id;

    RooRealVar            parExpo((par_prefix+"Expo").c_str(),"#alpha_{m}",-0.001,-0.01,0.0,"(MeV/c^{2})^{-1}");
    RooExponential  pdf(pdf_prefix.c_str(),pdf_prefix.c_str(),*varMass,parExpo);
    workspace_->import(pdf, import_arg_);
    workspace_->extendSet(("SetPdf"+sim_id+comp_id).c_str(),pdf_prefix.c_str());
  }
  // Proptime
  {
    dim_id = "Proptime";
    string pdf_prefix = pdf_pre+sim_id+comp_id+dim_id;
    string res_prefix = res_pre+sim_id+comp_id+dim_id;
    string acc_prefix = acc_pre+sim_id+comp_id+dim_id;
    // Resolution
    {

    }
    // Acceptance
    { 
      string par_prefix = par_pre+"a"+sim_id+comp_id+dim_id;

      RooRealVar    paraA(   (par_prefix+"A"   ).c_str()   ,"A_{S,a}^{B}"  ,4.46);
      RooRealVar    paraC(   (par_prefix+"C"   ).c_str()   ,"C_{S,a}^{B}"  ,11.36);
      RooRealVar    paraN(   (par_prefix+"N"   ).c_str()   ,"N_{S,a}^{B}"  ,1.);
      RooRealVar    paraB(   (par_prefix+"B"   ).c_str()   ,"B_{S,a}^{B}"  ,1e-8);
      RooRealVar    paraT(   (par_prefix+"T"   ).c_str()   ,"T_{S,a}^{B}"  ,0.0);
        RooRealVar              paraBeta((par_prefix+"Beta").c_str()   ,"#beta^_{S,a}" ,-0.0037);
        RooRealVar              paraTU(  (par_prefix+"TU"  ).c_str()   ,"#t^{U}_{S,a}" ,0.0);

        RooFormulaVar tempacc(acc_prefix.c_str(),"(@0>@4)*(@3*pow((@1*@0),@2*(@0>@4))/(1+pow((@1*@0),@2*(@0>@4))))+@5", RooArgList(*varProptime,paraA,paraC,paraN,paraT,paraB) );
        workspace_->import(tempacc, import_arg_);
    }
    // Decay pdf
    {
      string par_prefix = par_pre+sim_id+comp_id+dim_id;
      RooResolutionModel* res_model  = (RooResolutionModel*)workspace_->obj("resDBTSigProptime");
      RooFormulaVar*      acceptance = (RooFormulaVar*)workspace_->obj(acc_prefix.c_str());

      RooRealVar parTau1((par_prefix+"TauOne").c_str(), "#tau_{1}",  0.3,0.001, 0.4, "ps");
      RooRealVar parTau2((par_prefix+"TauTwo").c_str(), "#tau_{2}",  1.3,  0.4, 10.0, "ps");    
      RooRealVar parFrac((par_prefix+"Frac").c_str(), "f_{t}", 0.8,0.5,1.);  

      RooDecay   pdfDec1((pdf_prefix+"Dec1").c_str(),(pdf_prefix+"Dec1").c_str(),*varProptime,parTau1,*res_model,RooDecay::SingleSided);
      RooDecay   pdfDec2((pdf_prefix+"Dec2").c_str(),(pdf_prefix+"Dec2").c_str(),*varProptime,parTau2,*res_model,RooDecay::SingleSided);
      RooAddPdf  pdfDec( (pdf_prefix+"Dec" ).c_str(),(pdf_prefix+"Dec" ).c_str(),pdfDec1,pdfDec2,parFrac); 


      RooEffProd pdfEff(pdf_prefix.c_str(),pdf_prefix.c_str(), pdfDec,*acceptance);
      workspace_->import(pdfEff, import_arg_);
      workspace_->extendSet(("SetPdf"+sim_id+comp_id).c_str(),pdf_prefix.c_str());
    }  
  }
  // Pbg
  comp_id="Pbg";
  // Mass
  {
    dim_id = "Mass";
    string par_prefix = par_pre+sim_id+comp_id+dim_id;
    string pdf_prefix = pdf_pre+sim_id+comp_id+dim_id;

    RooRealVar            parExpo((par_prefix+"Expo").c_str(),"#alpha_{m}",-0.001,-0.01,0.1,"(MeV/c^{2})^{-1}");
    RooExponential  pdf(pdf_prefix.c_str(),pdf_prefix.c_str(),*varMass,parExpo);
    workspace_->import(pdf, import_arg_);
    workspace_->extendSet(("SetPdf"+sim_id+comp_id).c_str(),pdf_prefix.c_str());
  }
  // Proptime
  {
    dim_id = "Proptime";
    string pdf_prefix = pdf_pre+sim_id+comp_id+dim_id;
    string res_prefix = res_pre+sim_id+comp_id+dim_id;
    string acc_prefix = acc_pre+sim_id+comp_id+dim_id;
    // Resolution
    {

    }
    // Acceptance
    { 
        // Same acceptance as Lbg
      RooFormulaVar* tempacc = (RooFormulaVar*)workspace_->obj("accDBTLbgProptime");
      tempacc = (RooFormulaVar*)tempacc->Clone(acc_prefix.c_str());
        workspace_->import(*tempacc, import_arg_);
    }
    // Decay pdf
    {
      string par_prefix = par_pre+sim_id+comp_id+dim_id;
      RooResolutionModel* res_model  = (RooResolutionModel*)workspace_->obj("resDBTSigProptime");
      RooFormulaVar*      acceptance = (RooFormulaVar*)workspace_->obj(acc_prefix.c_str());


      RooEffProd pdfEff(pdf_prefix.c_str(),pdf_prefix.c_str(), *res_model,*acceptance);
      workspace_->import(pdfEff, import_arg_);
      workspace_->extendSet(("SetPdf"+sim_id+comp_id).c_str(),pdf_prefix.c_str());
    }  
  }

  vector<string> comp_vec;
  comp_vec.push_back("Sig");
  comp_vec.push_back("Lbg");
  //comp_vec.push_back("Pbg");

  BOOST_FOREACH(std::string comp_name, comp_vec){
    RooRealVar parYield((par_pre+sim_id+comp_name+"Yield").c_str(),(par_pre+sim_id+comp_name+"Yield").c_str(),500,0,5e8);
    const RooArgSet* setPdf = workspace_->set(("SetPdf"+sim_id+comp_name).c_str());
    RooProdPdf  pdfProd(("pdfProd"+sim_id+comp_name).c_str(),(sim_id+comp_name+"PDF").c_str(), *setPdf);
    RooExtendPdf pdf(("pdf"+sim_id+comp_name).c_str(),("Extended "+sim_id+comp_name+" PDF").c_str(),pdfProd,parYield);    
    workspace_->import(pdf, import_arg_);
    workspace_->extendSet(("SetPdf"+sim_id).c_str(),("pdf"+sim_id+comp_name).c_str());
  } 

  RooAddPdf   pdfFullDBT(("pdf"+sim_id+"Full").c_str(),"Full PDF",RooArgList(*(workspace_->set(("SetPdf"+sim_id).c_str()))));
  workspace_->import(pdfFullDBT, import_arg_);


  RooSimultaneous pdfFull("pdfFull","pdfFull",*(workspace_->cat("isBiased")));
  pdfFull.addPdf(*(workspace_->pdf("pdfDBTFull")),"B");


  workspace_->import(pdfFull, import_arg_);
} 

  
RooRealVar* Numerobis::CreateDimReal(const string& name, const string& desc, double val_min, double val_max, const string& unit, const string& dim_id) {
  RooRealVar temp_var(name.c_str(),desc.c_str(),val_min,val_max,unit.c_str());
  workspace_->import(temp_var,import_arg_);
  return workspace_->var(name.c_str());
}

RooCategory* Numerobis::CreateDimDisc(const string& name, const string& desc ) {
  RooCategory temp_cat(name.c_str(),desc.c_str());
  workspace_->import(temp_cat,import_arg_);
  return workspace_->cat(name.c_str());
}

void Numerobis::PrintLogo() {
  cout << ". . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . ." << endl;
  cout << ". . . . . . . . . . . . . . . . . . : : . . . . . . . . . . . . . . . . . . . ." << endl;
  cout << ". . . . . . . . . . . . : . j G : . L D D E i # j : . . . . . . . . . . . . . ." << endl;
  cout << ". . . . . . . . . . . .   K G . : G D E i i ; # W E . . . . . . . . . . . . . ." << endl;
  cout << ". . . . . . . . . . . j : L . : L D j ; j # # i ; K . . . . . . . . . . . . . ." << endl;
  cout << ". . . . . . . . . . . G . E : L G L ; i j W ; ; i i : : . . . . . . . . . . . ." << endl;
  cout << ". . . . . . . . . . : D . G . G G i i t G # t , . t L ; : . . . . . . . . . . ." << endl;
  cout << ". . . . . . . . . j . G : L ; G E i i i ; i   # ; ; ; i , L : : . . . . . . . ." << endl;
  cout << ". . . . . . . . . G ; L . f L G j i ; t t ; j ; ; t ; i i i ; L . . . . . . . ." << endl;
  cout << ". . . . . . . . i G D , f f D G G E ; ; i f i ; ; i i i i i i i . : . . . . . ." << endl;
  cout << ". . . . . . . : L : D i : i G D E L t i i i ; i i i i i i i i i G . . . . . . ." << endl;
  cout << ". . . . . . . . L , f G ; i i D # f ; i i i i K t i i i i i i i i . . . . . . ." << endl;
  cout << ". . . . . . .   G G . t i ; i i D K ; i i L # i E i i i i i i i i . . . . . . ." << endl;
  cout << ". . . . . . . D : : G # ; G ; ; i # ; ; i ; W t ; t i i i i i i i . . . . . . ." << endl;
  cout << ". . . . . . . D G : G W t i K L i G i i i ; W W t K ; ; i i i G W D . . . . . ." << endl;
  cout << ". . . . . . . : i L . : ; j L i t t i ; i i E W i ; # i i i i , ; W f . . . . ." << endl;
  cout << ". . . . . . : ; , : D D W ; ; ; i i G ; i ; G D i ; ; W i K G f f E j . . . . ." << endl;
  cout << ". . . . . : t : i . . . : : i , K i i t i f E D ; i ; i . i i i i t : . . . . ." << endl;
  cout << ". . . . . : , . i . t , . . : j L W ; i t i i i i ; ; t D i i i t i ; . . . . ." << endl;
  cout << ". . . . . : t t i : , . . . f ,     i W ; E D t K L . : j E i G , i ; . . . . ." << endl;
  cout << ". . . . . . . . , t j . . :           # j W # # # #   ; ; j i i i i L . . . . ." << endl;
  cout << ". . . . . . . . .   , . j ;             ; D # # # # D i ; ; i K , ; . . . . . ." << endl;
  cout << ". . . . . . . . . . : .   . :   , W i i ;   # t , : i i i ; i i i j . . . . . ." << endl;
  cout << ". . . . . . . . . . . . E   i # W ; i ; i i     . t i i i ; t t ; : . . . . . ." << endl;
  cout << ". . . . . . . . . . . j .   , .   j ; i i ; ; i ; i i i i t t i # . . . . . . ." << endl;
  cout << ". . . . . . . . . . . E     . L   , ; i i ; i i i i i ; i W ; i j . . . . . . ." << endl;
  cout << ". . . . . . . : . :   #             D ; i ; ; i i i i i # t i ; . . . . . . . ." << endl;
  cout << ". . . . . . . . . W # # #             G t j i ; i K t i ; i L : . . . . . . . ." << endl;
  cout << ". . . . . . : .     j # # W               f i     E i i ; t . . . . . . . . . ." << endl;
  cout << ". . . . . : . t       # # # # D                   W E ; D . . . . . . . . . . ." << endl;
  cout << ". . . . . : G           # # # # # # K G ;   .     W . . . . . . . . . . . . . ." << endl;
  cout << ". . . . . .             j # # # # # # t # # f   j   . . . . . . . . . . . . . ." << endl;
  cout << ". . . . . .                 # # # # j W # # j # t . . . . . . . . . . . . . . ." << endl;
  cout << ". . . . . .                   G # # j # # # j # . . . . . . . . . . . . . . . ." << endl;
  cout << ". . . . . .                       ; t # # # f #   . . . . . . . . . . . . . . ." << endl;
  cout << ". . . . . . :           , ; ; i , j E # # # G   , . . . . . . . . . . . . . . ." << endl;
  cout << ". . . . . . L                   ; j # # # E ;   f . . . . . . . . . . . . . . ." << endl;
  cout << ". . . . . . D                   f t # # # G     K . . . . . . . . . . . . . . ." << endl;
  cout << ". . . . . : L             . i   G j # # # j     W . . . . . . . . . . . . . . ." << endl;
  cout << ". . . . . . f       i t   .     t K # # # t :   K . . . . . . . . . . . . . . ." << endl;
  cout << ". . . . . . L           t     . t G K # # j t   , . . . . . . . . . . . . . . :" << endl;
  cout << ". . . . . . .   i i             t t j j j j L E . . . . . . . . . . . . . . . ." << endl;
  cout << ". . . . . . . . t                     . t L D . . . . . . . . . . . . . . . . ." << endl;
  cout << ". . . . . . . . G                   . :     L . . . . . . . . . . . . . . . . ." << endl;
  cout << ". . . . . . . : # K             t           t . . . . . . . . . . . . . . . . ." << endl;
  cout << ". . . . . :   : K # # # # # E D D # # # # K : . . . . . . . . . . . . . . . . ." << endl;
  cout << ". . . . .     ,     t K W L E ; f f :     i D L , . . . . . . . . . . . . . . ." << endl;
  cout << ". . . . . : ; E         : G j ; ; t t i i ; f i i G L j . . . . . . . . . . . ." << endl;
  cout << ". . . . . . G i ; t ; ; ; ; i i ; L , f t t t ; G t i G G f . . . . . . . . . ." << endl;
  cout << ". . . . . . . . : : . , G E G G f ; D t G G # i j W G E L ; . . . . . . . . . ." << endl;
  cout << ". . . . . . . . . . . . . . : : . . : . : . . . . . . . : . . . . . . . . . . ." << endl;
  cout << ". . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . ." << endl;
  cout << ". . . . . . . . . . . .N.U.M.E.R.O.B.I.S. . . . . . . . . . . . . . . . . . . ." << endl;
  cout << ". . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . ." << endl;
  
  
  
}

} // namespace numerobis 
} // namespace builder 
} // namespace doofit