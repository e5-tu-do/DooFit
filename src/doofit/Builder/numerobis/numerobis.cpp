#include "doofit/Builder/numerobis/numerobis.h"

// from project
#include "doofit/Config/CommonConfig.h"
#include "doofit/Builder/numerobis/numerobis_config.h"

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
  : workspace_(NULL),
    import_arg_(Silence()),
    cf_fixed_(true)
{
  PrintNumerobis();
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

void Numerobis::PrintNumerobis() {
  cout << "................................................................................" << endl;
  cout << "................................................................................" << endl;
  cout << "................................................................................" << endl;
  cout << ".................................,fEKDKDEL......................................" << endl;
  cout << "...............................DE,..jGGGGDGD...;................................" << endl;
  cout << ".............................KL....jGGGGDGEEE;t##j.............................." << endl;
  cout << "..........................tDGG...,GGGDDEiifiii#W#W#............................." << endl;
  cout << ".........................DiGGj...EGGGDEifji##it####............................." << endl;
  cout << "........................;.DGG...iGGGD#itiiDWWDiiii#:............................" << endl;
  cout << "......................GG..GGj..iGGGGfitiftWWWiiifit;............................" << endl;
  cout << ".....................iGG..GG...EGGDGiiiii#WWfiiLiiif............................" << endl;
  cout << ".....................EGG.:DD..;GGGGDiiij;WWWiii#fDiG............................" << endl;
  cout << "....................jLGE..Wj..GGGGEiiiiiiWWLi#iG  ijt..........................." << endl;
  cout << "....................j:GG..E:..GGGDjiiiiii##jG Gj.:iiij#........................." << endl;
  cout << "...................f..LG..E..:GGDEiiiiiii##f  .if#iitLD#j;......................" << endl;
  cout << "..................:G..GG..E..LGGDjiiiiiiiiii :DiiiiiiiiiiiifG..................." << endl;
  cout << "..................DGLiDG..E..GDDDiiiiiiiiiiif fiiiiiiiiiiiiiiD.................." << endl;
  cout << "..................GGGL:D;.D..GDDE#DiiiijiiiiiiiiiijitiiiiiiiiiE................." << endl;
  cout << "..................GGG:..G.t.:GDDWWtiiiiiitiiittjiiiiiiiiiiiiiiij;..............." << endl;
  cout << ".................GfGG:..E.:.iGDDWWKtjiiiiitfiiiKiiiiitiiiiiiiiiiE..............." << endl;
  cout << ".................D.jGD...;..jDDDWWK#fiiiiiiiiiiLiiiiiiiiiiiiiiiii..............." << endl;
  cout << "................f:..;GGj....jDDEWWfDiiiiiiiiiiiDfiiiiiiiiiiiiiiiiG.............." << endl;
  cout << "................#D..LDDDDDDKWEDEWDDjiiiiiiiiiii##iiiiiiitiiiiiiiiD.............." << endl;
  cout << "................EGi:LEDGLiiiifEEWWDiiiiiiiiiiiij#iiiiiiiiiiiiiiiit.............." << endl;
  cout << "...............GGDGGi..fiiiiiiiEK#Diiiiiiiftj#Litfiiiiiiiiiiiiiiii;............." << endl;
  cout << "...............ELGGG,..DiiiiiiiiiW#tiiiiiiiiKjiii#iiiiiiiiiiiiiiiit............." << endl;
  cout << "...............j:jGGGiifiiiiiiiiiWDDiiiiiiiiEKiiiDiiiiiiiiiiiiiiiit............." << endl;
  cout << "...............D:.:,GDEiiiL#LiiiiiKLiiiiiiii#WWi;iDiiiiiiiiiiiiiii.............." << endl;
  cout << "..............:GD::;DDEjiiiLDWifji##iiiiiiiiWWW#LD#iiiiiiiiiiiiiti.............." << endl;
  cout << "..............EGGGGE,LLKifiijiifiifLiiiiiiiiWWWLiiiWtiiiiiiiiiDii#it............" << endl;
  cout << "..............jjGGGD..:KiiiiLiiLiij#iiiiiiiiWEWfiiiLWiiiiiiii#ji##ji............" << endl;
  cout << ".............:i.:iGD:..tiitiiLjDiiiDiiiiiiitDEEfiiiiDDiiiiii#iiDiiDDiE.........." << endl;
  cout << ".............G,.:.:jGEDDWfiiiiifDiii#iiiiiiDDDDLiiiiiDWDiiiDiLiLjiiLiG.........." << endl;
  cout << ".............D:;i,:jGDDDGEDiiiii#iiiDtiiiiiDDDDDiiiiiii#WDLiDiDiLiifiE.........." << endl;
  cout << ".............Ei;tt,fGGGGEjiKtiii#iiiiDiiiiitEDDEifiiiiii:f#iiijtiiLiD..........." << endl;
  cout << ".............;:::t.t;.....t:;..EKKiiiiLiiiiiLEDEiiiiiii#.GijiDiiiiDf............" << endl;
  cout << "............G;.;.:.tt.,.;.t,i.DjjEKiiiiDDijiifWiiiiiii#t.ftiijiiititGG.........." << endl;
  cout << "............;i,f;..i;.i.i.j;i.fifjDKiiiDtitiiiiiiiiii#t.fiDiDiiiiiii#i#........." << endl;
  cout << "............t:::Lii,:;t:i.jii:     LE#Lii#iiKiiiiDiEG...iLiLiii#iLfi#i#........." << endl;
  cout << ".............j..,i,t:;j:i.LjiGi     #jG#G##ti#DL#jG....#fi#iiiLiLtiifit........." << endl;
  cout << "..............j,::.Di:j:i.Ljj i     ,Gjjjj##Diif###...#iiDiitDiiiiiDiE.........." << endl;
  cout << ".................i.;j.i:t.G;i         iDjjf#########.#iiiijLiiiii#jiE..........." << endl;
  cout << "..................,::.:;i.LD           f#jj#########GLiiiijiiiii#iii............" << endl;
  cout << "...................D:..itt.t            if###########iiiiiiiiiLLDiij............" << endl;
  cout << "..........................t  .      fGLf;;  #####G..#iiiiiiifLiDiii............." << endl;
  cout << "..........................G  ;   j#Diiiiif. ###. G..LiiiiiifLiiitiL............." << endl;
  cout << ".........................Gi  j  ##iiiiiiiif.:.   ...tiiiiiiiiiiiii#............." << endl;
  cout << "........................DG   G####iiiiiiiiii    ,fDiiiiiiiiLiijiiG.............." << endl;
  cout << "........................D    G,t;ijiiiiiiitD###fiiiitiiiiiiEiiiiif.............." << endl;
  cout << ".......................#.    L    #iiiiiiiiiiiiiiiiifiiiiiiLijiii;.............." << endl;
  cout << "......................;L          .Diiiiiiiiiiiiiiiiiiiiiiiifiiii..............." << endl;
  cout << ".......................i      ft   #iiiiiiiiiiiiiiiiiiiiii#jiiiiL..............." << endl;
  cout << "......................jj        ;ft,iiiiiiiiiiiiiiiiiiiiij#tiiii#..............." << endl;
  cout << ".....................j##            fjiiiiiiiiiiiiiiiiii#jiiiii#;..............." << endl;
  cout << ".....................###D            Gtiiiiiiiiiiiiit###iiiDii#i................" << endl;
  cout << "..................DLt####G            #iEiifiiiiiiD##iiiiiiijt.................." << endl;
  cout << ".................G   ######            ;Eiiiiij##j;iiiiiiiiit..................." << endl;
  cout << "................L    #######:            iG#Gt:   tiiiiiiii#...................." << endl;
  cout << "...............G     D#######G                    f#iiiiiiG....................." << endl;
  cout << "..............G       #########G                  DtGjijG#......................" << endl;
  cout << ".............i:       .##########;   .            #;.;G;,......................." << endl;
  cout << ".............G         j###########GfffD:         #............................." << endl;
  cout << "............L           D#############j#####L    ,f............................." << endl;
  cout << "............G            #############j#####jf   #.............................." << endl;
  cout << "............j             G##########Lj#####jj###..............................." << endl;
  cout << "............,              ;#########jj#####jf###..............................." << endl;
  cout << "............:                G######Gjj#####jL###..............................." << endl;
  cout << "............:                 j#####jjD#####jD###..............................." << endl;
  cout << "............;                    D##jj######j####..............................." << endl;
  cout << "............i                      Djj######j###f..............................." << endl;
  cout << "............f                      fjD#####DjG; t..............................." << endl;
  cout << "............G          ::,,:.  .,t#jj######Lt   f..............................." << endl;
  cout << "............G            ....     Gjj######jf f j..............................." << endl;
  cout << "............G                    ijjD######jL   i..............................." << endl;
  cout << "............G;              ,t.  Ljj#######jG,  ;..............................." << endl;
  cout << "............Lf       .:,;,.      Djj#######jG;  ,:.............................." << endl;
  cout << "............iG                 ;.fjj#######jf.  ,;.............................." << endl;
  cout << ".............D             ,,i  GjjL######Dj; : ;i.............................." << endl;
  cout << ".............D:        ,i..,  G. jj#######Gj: t tt.............................." << endl;
  cout << "............tff    ,    t;  tL  tjj#######Lj. , Lt.............................." << endl;
  cout << ".............tD:  .:     .G     Djf#######jj    #..............................." << endl;
  cout << "............D           Dt      LtL#######jj    #..............................." << endl;
  cout << ".............;Dt    tDG.        tttjjjjjjDjj   #;..............................." << endl;
  cout << "...............fDDDGt          ;ttjjjjjjjjjj  ,f................................" << endl;
  cout << "...............jG              ;fffjjjjjjjjj.#f................................." << endl;
  cout << "................D.                      :tGL;..................................." << endl;
  cout << "................jt                          i..................................." << endl;
  cout << ".................t                     ;,   t..................................." << endl;
  cout << ".................G                 t        L..................................." << endl;
  cout << "................f##,             f          D..................................." << endl;
  cout << "................#####G,        j, .,   ,fD###..................................." << endl;
  cout << "............G   t###########GDt;LD##########L..................................." << endl;
  cout << "............j .   ##########LG#j#######f,   .;.................................." << endl;
  cout << "............f ;     j######i##t#####L,      .:.................................." << endl;
  cout << "............ji  ,,,,,           .tjfjjfLE#DLiiiiij#i............................" << endl;
  cout << "...........tiGG            ,fGjiiiiiiDiLiiiLjiiiiDtt#t.........................." << endl;
  cout << "...........fiif##f:..;tL#DfiiiiiiDLiiDjijiiiLLtiiiDDLiL#........................" << endl;
  cout << "...........ftiiiiiiiiiiiiiiiiiiiiiijiiKittf#ffiitfjiii#tLiLG...................." << endl;
  cout << "............tE#GLfDLDiiiiiiiiijiiijtDifiDDiiiiiiiKiiiiDifE;LG..................." << endl;
  cout << ".....................iELttii;iiDfiiiiDiijLiiEiiii#iiiiKiD###;..................." << endl;
  cout << "..................................EEG.i;.....;DG....,..........................." << endl;
  cout << "................................................................................" << endl;
  cout << "................................................................................" << endl;
  cout << "................................................................................" << endl;
  cout << "................................................................................" << endl;
  cout << "................................................................................" << endl;
  cout << "................................................................................" << endl;
  cout << "................................................................................" << endl;
  cout << "................................................................................" << endl;
  
}

} // namespace numerobis 
} // namespace builder 
} // namespace doofit