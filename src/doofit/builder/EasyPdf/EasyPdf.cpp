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
  if (pdfs_.count(name) == 1) {
    throw PdfExistsException();
  } else {
    RooGaussian* temp_pdf = new RooGaussian(name.c_str(), name.c_str(), x, mean, sigma);
    if (ws_ == NULL) {
      pdfs_[name] = temp_pdf;
    } else {
      ws_->import(*temp_pdf, Silence());
      delete temp_pdf;
      pdfs_[name] = temp_pdf = dynamic_cast<RooGaussian*>(ws_->pdf(name.c_str()));
    }
    return *temp_pdf;
  }
}

RooExponential& doofit::builder::EasyPdf::Exponential(const std::string &name, RooRealVar& x, RooRealVar& e) {
  if (pdfs_.count(name) == 1) {
    throw PdfExistsException();
  } else {
    RooExponential* temp_pdf = new RooExponential(name.c_str(), name.c_str(), x, e);
    if (ws_ == NULL) {
      pdfs_[name] = temp_pdf;
    } else {
      ws_->import(*temp_pdf, Silence());
      delete temp_pdf;
      pdfs_[name] = temp_pdf = dynamic_cast<RooExponential*>(ws_->pdf(name.c_str()));
    }
    return *temp_pdf;
  }
}

RooProdPdf& doofit::builder::EasyPdf::Product(const std::string& name, const RooArgList& pdfs) {
  if (pdfs_.count(name) == 1) {
    throw PdfExistsException();
  } else {
    RooProdPdf* temp_pdf = new RooProdPdf(name.c_str(), name.c_str(), pdfs);
    if (ws_ == NULL) {
      pdfs_[name] = temp_pdf;
    } else {
      ws_->import(*temp_pdf, Silence());
      delete temp_pdf;
      pdfs_[name] = temp_pdf = dynamic_cast<RooProdPdf*>(ws_->pdf(name.c_str()));
    }
    return *temp_pdf;
  }
}

RooExtendPdf& doofit::builder::EasyPdf::Extend(const std::string& name, const RooAbsPdf& pdf, const RooAbsReal& yield) {
  if (pdfs_.count(name) == 1) {
    throw PdfExistsException();
  } else {
    RooExtendPdf* temp_pdf = new RooExtendPdf(name.c_str(), name.c_str(), pdf, yield);
    if (ws_ == NULL) {
      pdfs_[name] = temp_pdf;
    } else {
      ws_->import(*temp_pdf, Silence());
      delete temp_pdf;
      pdfs_[name] = temp_pdf = dynamic_cast<RooExtendPdf*>(ws_->pdf(name.c_str()));
    }
    return *temp_pdf;
  }
}

RooAddPdf& doofit::builder::EasyPdf::Add(const std::string& name, const RooArgList& pdfs) {
  if (pdfs_.count(name) == 1) {
    throw PdfExistsException();
  } else {
    RooAddPdf* temp_pdf = new RooAddPdf(name.c_str(), name.c_str(), pdfs);
    if (ws_ == NULL) {
      pdfs_[name] = temp_pdf;
    } else {
      ws_->import(*temp_pdf, Silence());
      delete temp_pdf;
      pdfs_[name] = temp_pdf = dynamic_cast<RooAddPdf*>(ws_->pdf(name.c_str()));
    }
    return *temp_pdf;
  }
}

RooAddPdf& doofit::builder::EasyPdf::Add(const std::string& name, const RooArgList& pdfs, const RooArgList& coefs) {
  if (pdfs_.count(name) == 1) {
    throw PdfExistsException();
  } else {
    RooAddPdf* temp_pdf = new RooAddPdf(name.c_str(), name.c_str(), pdfs, coefs);
    if (ws_ == NULL) {
      pdfs_[name] = temp_pdf;
    } else {
      ws_->import(*temp_pdf, Silence());
      delete temp_pdf;
      pdfs_[name] = temp_pdf = dynamic_cast<RooAddPdf*>(ws_->pdf(name.c_str()));
    }
    return *temp_pdf;
  }
}

RooAbsPdf& doofit::builder::EasyPdf::Pdf(const std::string &name) {
  if (pdfs_.count(name) == 1) {
    return *pdfs_[name];
  } else {
    throw PdfNotExistingException();
  }
}
