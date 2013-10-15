#include <memory>
#include <cmath>

#include <P2VV/RooHessian.h>
#include <RooFunctor.h>
#include <RooRealVar.h>

//_______________________________________________________________________________
vector<pair<double, double> > gradient_with_errors(const RooAbsReal& f, const RooArgList& params,
                                                   vector<double>& stepsizes) {
   vector<pair<double, double> > grad(params.getSize());
   if (stepsizes.empty()) {
      stepsizes = initial_stepsizes(params);
   }
   using namespace NumDiv;
   std::auto_ptr<RooArgSet> func_params(f.getObservables(RooArgSet(params)));

   //Perform the differtiation.
   for(int r = 0; r < params.getSize(); r++) {
      double xo = static_cast<const RooAbsReal*>(params.at(r))->getVal();
      RooRealVar* x = dynamic_cast<RooRealVar*>(func_params->find(*params.at(r)));
      std::auto_ptr<RooFunctor> func(f.functor(*x, RooArgList(), RooArgSet()));

      double h = stepsizes[r];
      cout << "Calculating gradient for " << x->GetName() << " h = " << h << endl;
      CentralDifferenceGradient<RooFunctor> d(*func, x->getVal(), h);
      extrapolate_to_zero<CentralDifferenceGradient<RooFunctor> > e(d);
      grad[r] = e();
      stepsizes[r] = h * e.best_factor();
      x->setVal(xo);
   }
   return grad;
}

//_______________________________________________________________________________
vector<double> gradient(const RooAbsReal& f, const RooArgList& params,
                        vector<double>& stepsizes)
{
   vector<double> grad(params.getSize(), 0.);
   vector<pair<double, double> > ge = gradient_with_errors(f, params, stepsizes);
   for (int i = 0; i < params.getSize(); ++i) {
      grad[i] = ge[i].first;
   }
   return grad;
}

//_______________________________________________________________________________
vector<pair<double, double> > second_gradient_with_errors(const RooAbsReal& f, const RooArgList& params, 
                                                          vector<double>& stepsizes)
{
   if (stepsizes.empty()) {
      stepsizes = initial_stepsizes(params);
   }
   using namespace NumDiv;
   vector<pair<double, double> > grad2(params.getSize());

   std::auto_ptr<RooArgSet> func_params(f.getObservables(RooArgSet(params)));
   
   for (int i = 0; i < params.getSize(); i++) {
      double xo = static_cast<const RooAbsReal*>(params.at(i))->getVal();
      RooAbsRealLValue* x = dynamic_cast<RooAbsRealLValue*>(func_params->find(*params.at(i)));
      std::auto_ptr<RooFunctor> func(f.functor(RooArgList(*x), RooArgList(), RooArgSet()));
      double h = fabs(stepsizes[i]);
      cout << "Calculating second derivative for " << x->GetName() << ", h = " << h << endl;
      CentralDifferenceSecond<RooFunctor> curv(*func, x->getVal(), h);
      extrapolate_to_zero<CentralDifferenceSecond<RooFunctor> > e(curv);
      grad2[i] = e();
      x->setVal(xo);
      stepsizes[i] = h * e.best_factor();
   }
   return grad2;
}

//_______________________________________________________________________________
std::pair<TMatrixTSym<double>, TMatrixTSym<double> > cross_with_errors
(const RooAbsReal& f, const RooArgList& params, vector<double>& stepsizes)
{
   if (stepsizes.empty()) {
      stepsizes = initial_stepsizes(params);
   }
   using namespace NumDiv;
   TMatrixTSym<double> hess(params.getSize());
   TMatrixTSym<double> errors(params.getSize());

   std::auto_ptr<RooArgSet> func_params(f.getObservables(RooArgSet(params)));
   
   for (int r = 0; r < params.getSize(); r++) {
      for (int c = r + 1; c < params.getSize(); c++) {
         RooAbsRealLValue* x = dynamic_cast<RooAbsRealLValue*>(func_params->find(*params.at(r)));
         double xo = x->getVal();
         RooAbsRealLValue* y = dynamic_cast<RooAbsRealLValue*>(func_params->find(*params.at(c)));
         double yo = y->getVal();
                  
         RooArgList args(*x, *y);
         std::auto_ptr<RooFunctor> func(f.functor(args, RooArgList(), RooArgSet()));

         // Get the stepsizes
         double hx = fabs(stepsizes[r]);
         double hy = fabs(stepsizes[c]);

         // Calculate the cross terms
         cout << "Calculating cross term for " << x->GetName() << " " << xo << " "
              << y->GetName() << " " << yo << " "
              << ", hx = " << hx << ", hy = " << hy << endl;
         CentralCrossDifferenceSecond<RooFunctor> cross(*func, xo, yo, hx, hy);
         extrapolate_to_zero<CentralCrossDifferenceSecond<RooFunctor> > e(cross);
         std::pair<double, double> cd = e();
         hess[r][c] = hess[c][r] = cd.first;
         errors[r][c] = errors[c][r] = cd.second;
         x->setVal(xo);
         y->setVal(yo);
      }
   }
   return make_pair(hess, errors);
}

//_______________________________________________________________________________
vector<double> second_gradient(const RooAbsReal& f, const RooArgList& params, 
                               vector<double>& stepsizes){
   vector<pair<double, double> > grad2 = second_gradient_with_errors(f, params, stepsizes);
   vector<double> r(grad2.size());
   for (size_t i = 0; i < grad2.size(); ++i) {
      r[i] = grad2[i].first;
   }
   return r;
}

//_______________________________________________________________________________
std::pair<TMatrixTSym<double>, TMatrixTSym<double> > hessian_with_errors
(const RooAbsReal& f, const RooArgList& params, vector<double>& stepsizes) {
   using namespace NumDiv;
   TMatrixTSym<double> hess(params.getSize());
   TMatrixTSym<double> errors(params.getSize());

   std::auto_ptr<RooArgSet> func_params(f.getObservables(RooArgSet(params)));
   
   vector<pair<double, double> > grad2 = second_gradient_with_errors(f, params, stepsizes);
   for (size_t i = 0; i < grad2.size(); ++i) {
      hess[i][i] = grad2[i].first;
      errors[i][i] = grad2[i].second;
   }

   //Perform the cross differencing.
   std::pair<TMatrixTSym<double>, TMatrixTSym<double> > cd = cross_with_errors(f, params, stepsizes);
   hess += cd.first;
   errors += cd.second; 
   return make_pair(hess, errors);
}

//_______________________________________________________________________________
TMatrixTSym<double> hessian(const RooAbsReal& f, const RooArgList& params,
                            vector<double>& stepsizes) {
   std::pair<TMatrixTSym<double>, TMatrixTSym<double> > r = hessian_with_errors(f, params, stepsizes);
   return r.first;
}

//_______________________________________________________________________________
vector<double> initial_stepsizes(const RooArgList& params) {
   // Calculate the gradient first to get stepsizes
   vector<double> stepsizes(params.getSize());
   for(int r = 0; r < params.getSize(); r++) {
      RooRealVar* x = dynamic_cast<RooRealVar*>(params.at(r));
      assert(x);
      // Initial stepsize of 10% of range
      double h = 0;
      if (x->hasError()) {
         h = x->getError();
      } else {
         h = 0.1 * (x->getMax() - x->getMin());
      }
      stepsizes[r] = h;
   }
   return stepsizes;
}
