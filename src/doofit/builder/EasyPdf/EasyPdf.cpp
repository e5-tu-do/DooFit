#include "doofit/builder/EasyPdf/EasyPdf.h"

// from STL
#include <map>
#include <string>

// from RooFit
#include "RooRealVar.h"
#include "RooGaussian.h"
#include "RooExponential.h"
#include "RooArgList.h"
#include "RooProdPdf.h"
#include "RooWorkspace.h"
#include "RooExtendPdf.h"
#include "RooAddPdf.h"

using namespace ROOT;
using namespace RooFit;

doofit::builder::EasyPdf::EasyPdf(RooWorkspace* ws)
: ws_(ws)
{}

doofit::builder::EasyPdf::~EasyPdf() {
  if (ws_ == NULL) {
    // if no workspace is to be used, delete all elements
    for (std::map<std::string,RooAbsPdf*>::iterator it = pdfs_.begin();
         it != pdfs_.end(); ++it) {
      delete it->second;
    }
    for (std::map<std::string,RooRealVar*>::iterator it = vars_.begin();
         it != vars_.end(); ++it) {
      delete it->second;
    }
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

RooGaussian& doofit::builder::EasyPdf::Gaussian(const std::string &name, RooRealVar& x, RooRealVar& mean, RooRealVar& sigma) {
  return AddPdfToStore<RooGaussian>(new RooGaussian(name.c_str(), name.c_str(), x, mean, sigma));
}

RooExponential& doofit::builder::EasyPdf::Exponential(const std::string &name, RooRealVar& x, RooRealVar& e) {
  return AddPdfToStore<RooExponential>(new RooExponential(name.c_str(), name.c_str(), x, e));
}

RooProdPdf& doofit::builder::EasyPdf::Product(const std::string& name, const RooArgList& pdfs) {
  return AddPdfToStore<RooProdPdf>(new RooProdPdf(name.c_str(), name.c_str(), pdfs));
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

RooAbsPdf& doofit::builder::EasyPdf::Pdf(const std::string &name) {
  if (pdfs_.count(name) == 1) {
    return *pdfs_[name];
  } else {
    throw PdfNotExistingException();
  }
}
