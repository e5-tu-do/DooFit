#include "doofit/builder/EasyPdf/EasyPdf.h"

// from STL
#include <map>
#include <string>

// from ROOT
#include "TFile.h"

// from RooFit
#include "RooRealVar.h"
#include "RooGaussian.h"
#include "RooExponential.h"
#include "RooArgList.h"
#include "RooProdPdf.h"
#include "RooWorkspace.h"
#include "RooExtendPdf.h"
#include "RooAddPdf.h"
#include "RooFormulaVar.h"
#include "RooCategory.h"
#include "RooGaussModel.h"
#include "RooAddModel.h"
#include "RooDecay.h"
#include "RooResolutionModel.h"
#include "RooEffProd.h"
#include "RooBDecay.h"
#include "RooSimultaneous.h"
#include "RooCBShape.h"
#include "RooSuperCategory.h"
#include "RooKeysPdf.h"
#include "RooHistPdf.h"
#include "RooDataHist.h"
#include "RooUnblindUniform.h"
#include "RooLognormal.h" 
#include "RooConstVar.h"

// from DooCore
#include "doocore/io/MsgStream.h"
#include "doocore/config/EasyConfig.h"

// from project
#include "doofit/config/CommaSeparatedList.h"
#include "P2VV/RooAbsGaussModelEfficiency.h"
#include "P2VV/RooGaussEfficiencyModel.h"
#include "P2VV/RooEffResAddModel.h"

using namespace ROOT;
using namespace RooFit;
using namespace doocore::io;

doofit::builder::EasyPdf::EasyPdf(RooWorkspace* ws)
: ws_(ws)
{}

doofit::builder::EasyPdf::~EasyPdf() {
  PurgeAllObjects();
}

void doofit::builder::EasyPdf::PurgeAllObjects() {
  if (ws_ == NULL) {
    // if no workspace is to be used, delete all elements
    for (std::map<std::string,RooAbsPdf*>::iterator it = pdfs_.begin();
         it != pdfs_.end(); ++it) {
      //      sdebug << "deleting " << it->second->GetName() << endmsg;
      delete it->second;
    }
    for (std::map<std::string,RooAbsReal*>::iterator it = external_reals_.begin();
         it != external_reals_.end(); ++it) {
      delete it->second;
    }
    for (std::map<std::string,RooAbsHiddenReal*>::iterator it = hidden_reals_.begin();
         it != hidden_reals_.end(); ++it) {
      delete it->second;
    }
    for (std::map<std::string,RooRealVar*>::iterator it = vars_.begin();
         it != vars_.end(); ++it) {
      delete it->second;
    }
    for (std::map<std::string,RooSuperCategory*>::iterator it = supercats_.begin();
         it != supercats_.end(); ++it) {
      delete it->second;
    }
    for (std::map<std::string,RooCategory*>::iterator it = cats_.begin();
         it != cats_.end(); ++it) {
      delete it->second;
    }
    for (std::map<std::string,RooFormulaVar*>::iterator it = formulas_.begin();
         it != formulas_.end(); ++it) {
      delete it->second;
    }
    for (std::vector<RooDataHist*>::iterator it = hists_.begin();
         it != hists_.end(); ++it) {
      delete *it;
    }
  }
  
  pdfs_.clear();
  external_reals_.clear();
  hidden_reals_.clear();
  vars_.clear();
  supercats_.clear();
  cats_.clear();
  formulas_.clear();
  hists_.clear();
  variable_sets_.clear();
}

RooAbsReal& doofit::builder::EasyPdf::Real(const std::string &name) {
  if (vars_.count(name) == 1) {
    return *vars_[name];
  } else if (formulas_.count(name) == 1) {
    return *formulas_[name];
  } else if (hidden_reals_.count(name) == 1) {
    return *hidden_reals_[name];
  } else if (external_reals_.count(name) == 1) {
    return *external_reals_[name];
  } else {
    return Var(name);
  }
}

bool doofit::builder::EasyPdf::RealExists(const std::string &name) {
  if (vars_.count(name) == 1) {
    return true;
  } else if (formulas_.count(name) == 1) {
    return true;
  } else if (hidden_reals_.count(name) == 1) {
    return true;
  } else if (external_reals_.count(name) == 1) {
    return true;
  } else {
    return false;
  }
}

bool doofit::builder::EasyPdf::PdfExists(const std::string &name) {
  if (pdfs_.count(name) == 1) {
    return true;
  } else {
    return false;
  }
}

RooRealVar& doofit::builder::EasyPdf::Var(const std::string &name) {
  if (vars_.count(name) == 1) {
    return *vars_[name];
  } else {
    RooRealVar* temp_var = new RooRealVar(name.c_str(), name.c_str(), 0.0);
    if (ws_ == NULL) {
      vars_[name] = temp_var;
    } else {
      ws_->import(*temp_var, Silence());
      delete temp_var;
      vars_[name] = temp_var = ws_->var(name.c_str());
    }
    return *temp_var;
  }
}

RooCategory& doofit::builder::EasyPdf::Cat(const std::string &name) {
  if (cats_.count(name) == 1) {
    return *cats_[name];
  } else {
    RooCategory* temp_cat = new RooCategory(name.c_str(), name.c_str());
    if (ws_ == NULL) {
      cats_[name] = temp_cat;
    } else {
      ws_->import(*temp_cat, Silence());
      delete temp_cat;
      cats_[name] = temp_cat = ws_->cat(name.c_str());
    }
    return *temp_cat;
  }
}

RooSuperCategory& doofit::builder::EasyPdf::SuperCat(const std::string& name, const RooArgSet& input_categories) {
  if (supercats_.count(name) == 1) {
    throw ObjectExistsException();
  } else {
    RooSuperCategory* temp_cat = new RooSuperCategory(name.c_str(), name.c_str(), input_categories);
    if (ws_ == NULL) {
      supercats_[name] = temp_cat;
    } else {
      ws_->import(*temp_cat, Silence());
      delete temp_cat;
      supercats_[name] = temp_cat = dynamic_cast<RooSuperCategory*>(ws_->obj(name.c_str()));
    }
    return *temp_cat;
  }
}

RooFormulaVar& doofit::builder::EasyPdf::Formula(const std::string& name, const std::string& formula,
                                                 const RooArgList& dependents) {
  if (formulas_.count(name) == 1) {
    throw ObjectExistsException();
  } else {
    RooFormulaVar* temp_formula = new RooFormulaVar(name.c_str(), name.c_str(),
                                                    formula.c_str(), dependents);
    if (ws_ == NULL) {
      formulas_[name] = temp_formula;
    } else {
      ws_->import(*temp_formula, Silence());
      delete temp_formula;
      formulas_[name] = temp_formula = dynamic_cast<RooFormulaVar*>(ws_->function(name.c_str()));
    }
    return *temp_formula;
  }
}

RooUnblindUniform& doofit::builder::EasyPdf::UnblindUniform(const std::string& name,
                                                            const std::string& blind_string,
                                                            double scale,
                                                            RooAbsReal& blind_value) {
  if (hidden_reals_.count(name) == 1) {
    throw ObjectExistsException();
  } else {
    RooUnblindUniform* temp_unblind = new RooUnblindUniform(name.c_str(), name.c_str(),
                                                            blind_string.c_str(),
                                                            scale, blind_value);
    if (ws_ == NULL) {
      hidden_reals_[name] = temp_unblind;
    } else {
      ws_->import(*temp_unblind, Silence());
      delete temp_unblind;
      hidden_reals_[name] = temp_unblind = dynamic_cast<RooUnblindUniform*>(ws_->obj(name.c_str()));
    }
    return *temp_unblind;
  }
}

RooFormulaVar& doofit::builder::EasyPdf::Formula(const std::string& name) {
  if (formulas_.count(name) == 1) {
    return *formulas_[name];
  } else {
    throw ObjectNotExistingException();
  }
}

RooAbsHiddenReal& doofit::builder::EasyPdf::HiddenReal(const std::string& name) {
  if (hidden_reals_.count(name) == 1) {
    return *hidden_reals_[name];
  } else {
    throw ObjectNotExistingException();
  }
}

RooSuperCategory& doofit::builder::EasyPdf::SuperCat(const std::string &name) {
  if (supercats_.count(name) == 1) {
    return *supercats_[name];
  } else {
    throw ObjectNotExistingException();
  }
}

void doofit::builder::EasyPdf::SetTitles(const std::string& config_file) {
  using namespace doocore::config;
  EasyConfig title_config(config_file);
  std::string section = "easypdf_titles.";

  for (std::map<std::string,RooRealVar*>::iterator it = vars_.begin(), end = vars_.end();
       it != end; ++it) {
    std::string title = title_config.getString(section+it->second->GetName());
    if (title != "") it->second->SetTitle(title.c_str());
  }
  for (std::map<std::string,RooCategory*>::iterator it = cats_.begin(), end = cats_.end();
       it != end; ++it) {
    std::string title = title_config.getString(section+it->second->GetName());
    if (title != "") it->second->SetTitle(title.c_str());
  }
  for (std::map<std::string,RooSuperCategory*>::iterator it = supercats_.begin(), end = supercats_.end();
       it != end; ++it) {
    std::string title = title_config.getString(section+it->second->GetName());
    if (title != "") it->second->SetTitle(title.c_str());
  }
  for (std::map<std::string,RooAbsPdf*>::iterator it = pdfs_.begin(), end = pdfs_.end();
       it != end; ++it) {
    std::string title = title_config.getString(section+it->second->GetName());
    if (title != "") it->second->SetTitle(title.c_str());
  }
}

void doofit::builder::EasyPdf::SetUnits(const std::string& config_file) {
  using namespace doocore::config;
  EasyConfig title_config(config_file);
  std::string section = "easypdf_units.";
  
  for (std::map<std::string,RooRealVar*>::iterator it = vars_.begin(), end = vars_.end();
       it != end; ++it) {
    std::string title = title_config.getString(section+it->second->GetName());
    if (title != "") it->second->setUnit(title.c_str());
  }
}

RooArgSet doofit::builder::EasyPdf::Vars(const std::string &names, const std::string define_set_name) {
  config::CommaSeparatedList<std::string> variables;
  variables.Parse(names);
  
  RooArgSet argset;
  for (int i=0; i<variables.size(); ++i) {
    if (vars_.count(variables[i]) == 1) {
      argset.add(Var(variables[i]));
    } else if (cats_.count(variables[i]) == 1) {
      argset.add(Cat(variables[i]));
    } else if (formulas_.count(variables[i]) == 1) {
      argset.add(Formula(variables[i]));
    } else {
      argset.add(Var(variables[i]));
    }
  }
  
  if (define_set_name.length() > 0) {
    variable_sets_[define_set_name] = names;
    
    if (ws_ != NULL) {
      ws_->defineSet(define_set_name.c_str(), argset);
    }
  }
  
  return argset;
}

RooArgSet doofit::builder::EasyPdf::Set(const std::string &set_name) {
  std::map<std::string,std::string>::const_iterator it = variable_sets_.find(set_name);
  
  if (it != variable_sets_.end()) {
    return Vars(it->second);
  } else {
    throw ObjectNotExistingException();
  }
}

RooArgList doofit::builder::EasyPdf::VarList(const std::string &name) {
  config::CommaSeparatedList<std::string> variables;
  variables.Parse(name);
  
  RooArgList arglist;
  for (int i=0; i<variables.size(); ++i) {
    if (vars_.count(variables[i]) == 1) {
      arglist.add(Var(variables[i]));
    } else if (cats_.count(variables[i]) == 1) {
      arglist.add(Cat(variables[i]));
    } else if (formulas_.count(variables[i]) == 1) {
      arglist.add(Formula(variables[i]));
    } else {
      arglist.add(Var(variables[i]));
    }
  }
  return arglist;
}

RooArgSet doofit::builder::EasyPdf::AllVars() {
  RooArgSet argset;
  
  for (std::map<std::string,RooRealVar*>::const_iterator it = vars_.begin();
       it != vars_.end(); ++it) {
    argset.add(*(it->second));
  }
  for (std::map<std::string,RooCategory*>::const_iterator it = cats_.begin();
       it != cats_.end(); ++it) {
    argset.add(*(it->second));
  }
  for (std::map<std::string,RooFormulaVar*>::const_iterator it = formulas_.begin();
       it != formulas_.end(); ++it) {
    argset.add(*(it->second));
  }
  return argset;
}


RooGaussian& doofit::builder::EasyPdf::Gaussian(const std::string &name, RooAbsReal& x, RooAbsReal& mean, RooAbsReal& sigma) {
  return AddPdfToStore<RooGaussian>(new RooGaussian(name.c_str(), name.c_str(), x, mean, sigma));
}

RooCBShape& doofit::builder::EasyPdf::CBShape(const std::string& name, RooAbsReal& x, RooAbsReal& mean, RooAbsReal& sigma, RooAbsReal& alpha, RooAbsReal& n) {
  return AddPdfToStore<RooCBShape>(new RooCBShape(name.c_str(), name.c_str(), x, mean, sigma, alpha, n));
}

RooExponential& doofit::builder::EasyPdf::Exponential(const std::string &name, RooAbsReal& x, RooAbsReal& e) {
  return AddPdfToStore<RooExponential>(new RooExponential(name.c_str(), name.c_str(), x, e));
}

RooDecay& doofit::builder::EasyPdf::Decay(const std::string& name, RooRealVar& t, RooAbsReal& tau, const RooResolutionModel& model) {
  return AddPdfToStore<RooDecay>(new RooDecay(name.c_str(), name.c_str(), t, tau, model, RooDecay::SingleSided));
}

RooLognormal& doofit::builder::EasyPdf::Lognormal(const std::string& name, RooAbsReal& x, RooAbsReal& m, RooAbsReal& k) {
  return AddPdfToStore<RooLognormal>(new RooLognormal(name.c_str(), name.c_str(), x, m, k));
}

RooAddPdf& doofit::builder::EasyPdf::DoubleLognormal(const std::string& name, RooAbsReal& x, RooAbsReal& m1, RooAbsReal& k1, RooAbsReal& m2, RooAbsReal& k2, RooAbsReal& fraction) {
  //return AddPdfToStore<RooLognormal>(new RooLognormal(name.c_str(), name.c_str(), x, m, k));
  
  return Add(name, RooArgList(Lognormal("p1_"+name,x,m1,k1),
                              Lognormal("p2_"+name,x,m2,k2)),
             RooArgList(fraction));
}

RooSimultaneous& doofit::builder::EasyPdf::Simultaneous(const std::string& name, RooAbsCategoryLValue& category) {
  return AddPdfToStore<RooSimultaneous>(new RooSimultaneous(name.c_str(), name.c_str(), category));
}

RooSimultaneous& doofit::builder::EasyPdf::Simultaneous(const std::string& name, std::map<std::string,RooAbsPdf*> pdfs, RooAbsCategoryLValue& category) {
  return AddPdfToStore<RooSimultaneous>(new RooSimultaneous(name.c_str(), name.c_str(), pdfs, category));
}

RooProdPdf& doofit::builder::EasyPdf::Product(const std::string& name, const RooArgList& pdfs) {
  return AddPdfToStore<RooProdPdf>(new RooProdPdf(name.c_str(), name.c_str(), pdfs));
}

RooProdPdf& doofit::builder::EasyPdf::Product(const std::string& name, const RooArgList& pdfs, const RooCmdArg& arg1, const RooCmdArg& arg2, const RooCmdArg& arg3, const RooCmdArg& arg4, const RooCmdArg& arg5, const RooCmdArg& arg6, const RooCmdArg& arg7, const RooCmdArg& arg8) {
  return AddPdfToStore<RooProdPdf>(new RooProdPdf(name.c_str(), name.c_str(), pdfs, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8));
}


RooExtendPdf& doofit::builder::EasyPdf::Extend(const std::string& name, const RooAbsPdf& pdf, const RooAbsReal& yield) {
  return AddPdfToStore<RooExtendPdf>(new RooExtendPdf(name.c_str(), name.c_str(), pdf, yield));
}

RooAddPdf& doofit::builder::EasyPdf::Add(const std::string& name, const RooArgList& pdfs) {
  return AddPdfToStore<RooAddPdf>(new RooAddPdf(name.c_str(), name.c_str(), pdfs));
}

RooAddPdf& doofit::builder::EasyPdf::Add(const std::string& name, const RooArgList& pdfs, const RooArgList& coefs) {
  return AddPdfToStore<RooAddPdf>(new RooAddPdf(name.c_str(), name.c_str(), pdfs, coefs));
}

RooEffProd& doofit::builder::EasyPdf::EffProd(const std::string& name, RooAbsPdf& pdf, RooAbsReal& efficiency) {
  return AddPdfToStore<RooEffProd>(new RooEffProd(name.c_str(), name.c_str(), pdf, efficiency));
}

RooGaussModel& doofit::builder::EasyPdf::GaussModel(const std::string& name, RooRealVar& x, RooAbsReal& mean, RooAbsReal& sigma) {
  return AddPdfToStore<RooGaussModel>(new RooGaussModel(name.c_str(), name.c_str(), x, mean, sigma));
}

RooAddModel& doofit::builder::EasyPdf::DoubleGaussModel(const std::string& name, RooRealVar& x, RooAbsReal& mean, RooAbsReal& sigma1, RooAbsReal& sigma2, RooAbsReal& fraction) {
  return AddModel(name, RooArgList(GaussModel("p1_"+name,x,mean,sigma1),
                                   GaussModel("p2_"+name,x,mean,sigma2)),
                  RooArgList(fraction));
}

RooAddModel& doofit::builder::EasyPdf::TripleGaussModel(const std::string& name, RooRealVar& x, RooAbsReal& mean, RooAbsReal& sigma1, RooAbsReal& sigma2, RooAbsReal& sigma3, RooAbsReal& fraction1, RooAbsReal& fraction2) {
  return AddModel(name, RooArgList(GaussModel("p1_"+name,x,mean,sigma1),
                                   GaussModel("p2_"+name,x,mean,sigma2),
                                   GaussModel("p3_"+name,x,mean,sigma3)),
                  RooArgList(fraction1, fraction2));
}

RooAddModel& doofit::builder::EasyPdf::QuadGaussModel(const std::string& name, RooRealVar& x, RooAbsReal& mean, RooAbsReal& sigma1, RooAbsReal& sigma2, RooAbsReal& sigma3, RooAbsReal& sigma4, RooAbsReal& fraction1, RooAbsReal& fraction2, RooAbsReal& fraction3) {
  return AddModel(name, RooArgList(GaussModel("p1_"+name,x,mean,sigma1),
                                   GaussModel("p2_"+name,x,mean,sigma2),
                                   GaussModel("p3_"+name,x,mean,sigma3),
                                   GaussModel("p4_"+name,x,mean,sigma4)),
                  RooArgList(fraction1, fraction2, fraction3));
}

RooAddModel& doofit::builder::EasyPdf::DoubleGaussModelScaled(const std::string& name, RooRealVar& x, RooAbsReal& mean, RooAbsReal& sigma, RooAbsReal& scale, RooAbsReal& fraction, std::string sigma2_name) {
  if (sigma2_name.length() == 0) {
    sigma2_name = name+"_sigma2";
  }
  
  return AddModel(name,
                  RooArgList(GaussModel("p1_"+name,x,mean,sigma),
                             GaussModel("p2_"+name,x,mean,
                                        Formula(sigma2_name,
                                                "@0*@1",
                                                RooArgList(sigma,scale)))),
                  RooArgList(fraction));
}

RooAddModel& doofit::builder::EasyPdf::TripleGaussModelScaled(const std::string& name, RooRealVar& x, RooAbsReal& mean, RooAbsReal& sigma, RooAbsReal& scale2, RooAbsReal& scale3, RooAbsReal& fraction1, RooAbsReal& frac_rec2) {
  std::string sigma2_name = name+"_sigma2";
  std::string sigma3_name = name+"_sigma3";
  std::string fraction2_name = name+"_fraction2";
  
  GaussModel("p2_"+name,x,mean,
             Formula(sigma2_name,"@0*@1",RooArgList(sigma,scale2)));
  GaussModel("p3_"+name,x,mean,
             Formula(sigma3_name,"@0*@1@2",RooArgList(sigma,scale2,scale3)));
  Formula(fraction2_name, "(1-@0)*@1", RooArgList(fraction1,frac_rec2));
  
  return AddModel(name,
                  RooArgList(GaussModel("p1_"+name,x,mean,sigma),
                             Model("p2_"+name),
                             Model("p3_"+name)),
                  RooArgList(fraction1,Formula(fraction2_name)));
}

RooAddModel& doofit::builder::EasyPdf::QuadGaussModelScaled(const std::string& name, RooRealVar& x, RooAbsReal& mean, RooAbsReal& sigma, RooAbsReal& scale2, RooAbsReal& scale3, RooAbsReal& scale4, RooAbsReal& fraction1, RooAbsReal& frac_rec2, RooAbsReal& frac_rec3) {
  std::string sigma2_name = name+"_sigma2";
  std::string sigma3_name = name+"_sigma3";
  std::string sigma4_name = name+"_sigma4";
  std::string fraction2_name = name+"_fraction2";
  std::string fraction3_name = name+"_fraction3";
  
  GaussModel("p2_"+name,x,mean,
             Formula(sigma2_name,"@0*@1",RooArgList(sigma,scale2)));
  GaussModel("p3_"+name,x,mean,
             Formula(sigma3_name,"@0*@1@2",RooArgList(sigma,scale2,scale3)));
  GaussModel("p4_"+name,x,mean,
             Formula(sigma4_name,"@0*@1@2*@3",
                     RooArgList(sigma,scale2,scale3,scale4)));
  Formula(fraction2_name, "(1-@0)*@1", RooArgList(fraction1,frac_rec2));
  Formula(fraction3_name, "(1-@0)*(1-@1)*@2",
          RooArgList(fraction1,frac_rec2,frac_rec3));
  
  return AddModel(name,
                  RooArgList(GaussModel("p1_"+name,x,mean,sigma),
                             Model("p2_"+name),
                             Model("p3_"+name),
                             Model("p4_"+name)),
                  RooArgList(fraction1,Formula(fraction2_name),
                             Formula(fraction3_name)));
}

RooAddModel& doofit::builder::EasyPdf::QuinGaussModelScaled(const std::string& name, RooRealVar& x, RooAbsReal& mean, RooAbsReal& sigma, RooAbsReal& scale2, RooAbsReal& scale3, RooAbsReal& scale4, RooAbsReal& scale5, RooAbsReal& fraction1, RooAbsReal& frac_rec2, RooAbsReal& frac_rec3, RooAbsReal& frac_rec4) {
  std::string sigma2_name = name+"_sigma2";
  std::string sigma3_name = name+"_sigma3";
  std::string sigma4_name = name+"_sigma4";
  std::string sigma5_name = name+"_sigma5";
  std::string fraction2_name = name+"_fraction2";
  std::string fraction3_name = name+"_fraction3";
  std::string fraction4_name = name+"_fraction4";
  
  GaussModel("p2_"+name,x,mean,
             Formula(sigma2_name,"@0*@1",RooArgList(sigma,scale2)));
  GaussModel("p3_"+name,x,mean,
             Formula(sigma3_name,"@0*@1@2",RooArgList(sigma,scale2,scale3)));
  GaussModel("p4_"+name,x,mean,
             Formula(sigma4_name,"@0*@1@2*@3",
                     RooArgList(sigma,scale2,scale3,scale4)));
  GaussModel("p5_"+name,x,mean,
             Formula(sigma5_name,"@0*@1@2*@3*@4",
                     RooArgList(sigma,scale2,scale3,scale4,scale5)));
  Formula(fraction2_name, "(1-@0)*@1", RooArgList(fraction1,frac_rec2));
  Formula(fraction3_name, "(1-@0)*(1-@1)*@2",
          RooArgList(fraction1,frac_rec2,frac_rec3));
  Formula(fraction4_name, "(1-@0)*(1-@1)*(1-@2)*@3",
          RooArgList(fraction1,frac_rec2,frac_rec3,frac_rec4));
  
  return AddModel(name,
                  RooArgList(GaussModel("p1_"+name,x,mean,sigma),
                             Model("p2_"+name),
                             Model("p3_"+name),
                             Model("p4_"+name),
                             Model("p5_"+name)),
                  RooArgList(fraction1,Formula(fraction2_name),
                             Formula(fraction3_name), Formula(fraction4_name)));
}

RooGaussModel& doofit::builder::EasyPdf::GaussModelPerEvent(const std::string& name, RooRealVar& x, RooAbsReal& mean, RooAbsReal& scale_error, RooAbsReal& scale_mean, RooAbsReal& error) {
  return AddPdfToStore<RooGaussModel>(new RooGaussModel(name.c_str(), name.c_str(), x, mean, scale_error, scale_mean, error));
}

RooAddModel& doofit::builder::EasyPdf::DoubleGaussModelPerEvent(const std::string& name, RooRealVar& x, RooAbsReal& mean, RooAbsReal& scale_error1, RooAbsReal& scale_error2, RooAbsReal& scale_mean, RooAbsReal& error, RooAbsReal& fraction) {
  return AddModel(name,
                  RooArgList(GaussModelPerEvent("p1_"+name,x,mean,scale_error1,
                                                scale_mean, error),
                             GaussModelPerEvent("p2_"+name,x,mean,scale_error2,
                                                scale_mean, error)),
                  RooArgList(fraction));
}

RooAddModel& doofit::builder::EasyPdf::TripleGaussModelPerEvent(const std::string& name, RooRealVar& x, RooAbsReal& mean, RooAbsReal& scale_error1, RooAbsReal& scale_error2, RooAbsReal& scale_error3, RooAbsReal& scale_mean, RooAbsReal& error, RooAbsReal& fraction1, RooAbsReal& frac_rec2) {
  std::string fraction2_name = name+"_fraction2";
  
  Formula(fraction2_name, "(1-@0)*@1", RooArgList(fraction1,frac_rec2));
  
  return AddModel(name,
                  RooArgList(GaussModelPerEvent("p1_"+name,x,mean,scale_error1,
                                                scale_mean, error),
                             GaussModelPerEvent("p2_"+name,x,mean,scale_error2,
                                                scale_mean, error),
                             GaussModelPerEvent("p3_"+name,x,mean,scale_error3,
                                                scale_mean, error)),
                  RooArgList(fraction1, Formula(fraction2_name)));
}

RooAddModel& doofit::builder::EasyPdf::AddModel(const std::string& name, const RooArgList& pdfs, const RooArgList& coefs) {
  return AddPdfToStore<RooAddModel>(new RooAddModel(name.c_str(), name.c_str(), pdfs, coefs));
}

RooGaussEfficiencyModel& doofit::builder::EasyPdf::GaussEfficiencyModel(const std::string& name, RooRealVar& x, RooAbsGaussModelEfficiency &eff, RooAbsReal& mean, RooAbsReal& sigma) {
  return AddPdfToStore<RooGaussEfficiencyModel>(new RooGaussEfficiencyModel(name.c_str(), name.c_str(), x, eff, mean, sigma));
}

RooEffResAddModel& doofit::builder::EasyPdf::DoubleGaussEfficiencyModel(const std::string& name, RooRealVar& x, RooAbsGaussModelEfficiency &eff, RooAbsReal& mean, RooAbsReal& sigma1, RooAbsReal& sigma2, RooAbsReal& fraction) {
  return EffResAddModel(name,
                        RooArgList(GaussEfficiencyModel("p1_"+name,x,eff,mean,sigma1),
                                   GaussEfficiencyModel("p2_"+name,x,eff,mean,sigma2)),
                  RooArgList(fraction));
}

RooGaussEfficiencyModel& doofit::builder::EasyPdf::GaussEfficiencyModelPerEvent(const std::string& name, RooRealVar& x, RooAbsGaussModelEfficiency &eff, RooAbsReal& mean, RooAbsReal& error, RooAbsReal &scale_error) {
  return AddPdfToStore<RooGaussEfficiencyModel>(new RooGaussEfficiencyModel(name.c_str(), name.c_str(), x, eff, mean, error, RooConst(1.0), scale_error));
}

RooEffResAddModel& doofit::builder::EasyPdf::DoubleGaussEfficiencyModelPerEvent(const std::string& name, RooRealVar& x, RooAbsGaussModelEfficiency &eff, RooAbsReal& mean, RooAbsReal& error, RooAbsReal& scale_error1, RooAbsReal& scale_error2, RooAbsReal& fraction) {
  return EffResAddModel(name,
                        RooArgList(GaussEfficiencyModelPerEvent("p1_"+name,
                                                                x,eff,mean,
                                                                error,
                                                                scale_error1),
                                   GaussEfficiencyModelPerEvent("p2_"+name,
                                                                x,eff,mean,
                                                                error,
                                                                scale_error2)),
                        RooArgList(fraction));
}

RooEffResAddModel& doofit::builder::EasyPdf::EffResAddModel(const std::string& name, const RooArgList& pdfs, const RooArgList& coefs) {
  return AddPdfToStore<RooEffResAddModel>(new RooEffResAddModel(name.c_str(), name.c_str(), pdfs, coefs));
}

RooAddPdf& doofit::builder::EasyPdf::DoubleGaussianScaled(const std::string& name, RooAbsReal& x, RooAbsReal& mean, RooAbsReal& sigma, RooAbsReal& scale, RooAbsReal& fraction, std::string sigma2_name) {
  if (sigma2_name.length() == 0) {
    sigma2_name = name+"_sigma2";
  }
  
  return Add(name, RooArgList(Gaussian("p1_"+name,x,mean,sigma),
                              Gaussian("p2_"+name,x,mean,Formula(sigma2_name, "@0*@1", RooArgList(sigma,scale)))),
             RooArgList(fraction));
}

RooAddPdf& doofit::builder::EasyPdf::TripleGaussianScaled(const std::string& name, RooAbsReal& x, RooAbsReal& mean, RooAbsReal& sigma, RooAbsReal& scale2, RooAbsReal& scale3, RooAbsReal& fraction1, RooAbsReal& frac_rec2) {
  std::string sigma2_name = name+"_sigma2";
  std::string sigma3_name = name+"_sigma3";
  std::string fraction2_name = name+"_fraction2";
  
  Gaussian("p2_"+name,x,mean,
           Formula(sigma2_name, "@0*@1", RooArgList(sigma,scale2)));
  Gaussian("p3_"+name,x,mean,
           Formula(sigma3_name,"@0*@1@2",RooArgList(sigma,scale2,scale3)));
  Formula(fraction2_name, "(1-@0)*@1", RooArgList(fraction1,frac_rec2));
  
  return Add(name,
             RooArgList(Gaussian("p1_"+name,x,mean,sigma),
                        Pdf("p2_"+name),
                        Pdf("p3_"+name)),
             RooArgList(fraction1,Formula(fraction2_name)));
}

RooFormulaVar& doofit::builder::EasyPdf::AtanAcceptance(const std::string& name, RooRealVar& x, RooAbsReal& slope_a, RooAbsReal& slope_b) {
  return Formula(name, "(atan(@0*exp(@1*@0+@2)))/1.570796327", RooArgList(x, slope_a, slope_b));
}

RooFormulaVar& doofit::builder::EasyPdf::AtanAcceptanceIncludingBeta(const std::string& name, RooRealVar& x, RooAbsReal& slope_a, RooAbsReal& slope_b, RooAbsReal& beta) {
  return Formula(name, "(atan(@0*exp(@1*@0+@2)))*(1-@3*@0)/1.570796327", RooArgList(x, slope_a, slope_b, beta));
}

RooAddPdf& doofit::builder::EasyPdf::DoubleDecay(const std::string& name, RooRealVar& t, RooAbsReal& tau1, RooAbsReal& tau2, RooAbsReal& fraction, const RooResolutionModel& model) {
  return Add(name, RooArgList(Decay("p1_"+name, t, tau1, model),
                              Decay("p2_"+name, t, tau2, model)),
             RooArgList(fraction));
}

RooBDecay& doofit::builder::EasyPdf::BDecay(const std::string& name, RooRealVar& t, RooAbsReal& tau, RooAbsReal& dgamma, RooAbsReal& coef_cosh, RooAbsReal& coef_sinh, RooAbsReal& coef_cos, RooAbsReal& coef_sin, RooAbsReal& dm, const RooResolutionModel& model) {
  return AddPdfToStore<RooBDecay>(new RooBDecay(name.c_str(), name.c_str(), t, tau, dgamma,
                                                coef_cosh, coef_sinh, coef_cos, coef_sin, dm, model,
                                                RooBDecay::SingleSided));
}

RooKeysPdf& doofit::builder::EasyPdf::KeysPdf(const std::string& name, const std::string& file_name, const std::string& ws_name, const std::string& pdf_name_on_ws) {
  TFile file(file_name.c_str());
  RooWorkspace* ws = dynamic_cast<RooWorkspace*>(file.Get(ws_name.c_str()));
  RooKeysPdf* temp_pdf = dynamic_cast<RooKeysPdf*>(ws->pdf(pdf_name_on_ws.c_str()));
  
  return AddPdfToStore<RooKeysPdf>(dynamic_cast<RooKeysPdf*>(temp_pdf->cloneTree(name.c_str())));
}

RooHistPdf& doofit::builder::EasyPdf::HistPdf(const std::string& name, const RooArgSet& vars, const std::string& file_name, const std::string& hist_name) {
  TFile file(file_name.c_str());
  RooDataHist* hist       = dynamic_cast<RooDataHist*>(file.Get(hist_name.c_str()));
  std::string name_hist_clone = hist_name + "_clone";

  if (hist == NULL) {
    serr << "Cannot load RooDataHist " << hist_name << " from file " << file_name << endmsg;
    throw ObjectNotExistingException();
  }
  
  RooDataHist* hist_clone = dynamic_cast<RooDataHist*>(hist->Clone(name_hist_clone.c_str()));
  hists_.push_back(hist_clone);
  
  return AddPdfToStore<RooHistPdf>(new RooHistPdf(name.c_str(), name.c_str(), vars, *hist_clone));
}

RooAbsPdf& doofit::builder::EasyPdf::Pdf(const std::string &name) {
  if (pdfs_.count(name) == 1) {
    return *pdfs_[name];
  } else {
    serr << "Pdf " << name << " not existing!" << endmsg;
    throw ObjectNotExistingException();
  }
}

RooResolutionModel& doofit::builder::EasyPdf::Model(const std::string &name) {
  if (pdfs_.count(name) == 1) {
    RooResolutionModel* model = dynamic_cast<RooResolutionModel*>(pdfs_[name]);
    if (model != NULL) {
      return *model;
    } else {
      throw ObjectNotExistingException();
    }
  } else {
    serr << "EasyPdf::Model(const std::string&): Requested model " << name << " not existing." << endmsg;
    throw ObjectNotExistingException();
  }
}
