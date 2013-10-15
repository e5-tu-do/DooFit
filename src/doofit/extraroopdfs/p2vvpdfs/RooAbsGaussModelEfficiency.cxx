#include "P2VV/RooAbsGaussModelEfficiency.h"
#include "RooMath.h"
#include "TMath.h"

RooAbsGaussModelEfficiency::~RooAbsGaussModelEfficiency()  {};

namespace {
    static const Double_t rootpi(sqrt(TMath::Pi())) ;
    std::complex<double> evalApprox(Double_t x, const std::complex<double>& z) {
      // compute exp(-x^2)cwerf(-i(z-x)), cwerf(z) = exp(-z^2)erfc(-iz)
      // use the approximation: erfc(z) = exp(-z*z)/(sqrt(pi)*z)
      // to explicitly cancel the divergent exp(y*y) behaviour of
      // CWERF for z = x + i y with large negative y
      static const std::complex<double> mi(0,-1);
      std::complex<double> zp  = mi*(z-x);
      std::complex<double> zsq = zp*zp;
      std::complex<double> v = -zsq - x*x;
      std::complex<double> iz(z.imag()+x,z.real()-x); // ???
      return exp(v)*(exp(zsq)/(iz*rootpi) + 1.)*2. ;
    }

    // Calculate exp(-x^2) cwerf(i(z-x)), taking care of numerical instabilities
    std::complex<double> eval(Double_t x, const std::complex<double>& z) {
      Double_t re = z.real()-x;
      return (re>-5.0) ? RooMath::faddeeva_fast(std::complex<double>(-z.imag(),re))*exp(-x*x)
                       : evalApprox(x,z) ;
    }

  class N {
    public:
        N(double x, const std::complex<double>& z) ;
        std::complex<double> operator()(unsigned i) const { return _N[i]; }
    private:
        std::complex<double> _N[3];
  };

  class L {
      double _x;
  public:
      L(double x) : _x(x) { }
      double operator()(unsigned j, unsigned k) const ;
  };

}

N::N(double x, const std::complex<double>& z)
{
          _N[0] =  RooMath::erf(x);
          _N[1] =  exp(-x*x);
          _N[2] =  eval(x,z);
}

double
L::operator()(unsigned j, unsigned k) const
{
          assert(j<4);
          assert(k<3);
          switch(k) {
              case 0: return j==0 ? 1 : 0 ;
              case 1: switch(j) {
                      case 0 : return  0;
                      default : return 2*(*this)(j-1,2)/sqrt(TMath::Pi());
              }
              case 2: switch(j) {
                      case 0 : return -1.;
                      case 1 : return -2.*_x;
                      case 2 : return -2.*(2.*_x*_x-1.);
                      case 3 : return -4.*_x*(2.*_x*_x-3.);
                      // TODO: add higher orders!!!! Go up until (at least) six...
                      default : assert(1==0); return 0;
          }   }
          assert(1==0);
          return 0;
}

template <unsigned MaxOrder>
RooGaussModelAcceptance::M_n<MaxOrder>::M_n(double x, const std::complex<double>& z)
{
          L l(x); N n(x,z);
          for (unsigned i=0;i<MaxOrder;++i) _m[i] = n(0)*l(i,0) + n(1)*l(i,1) + n(2)*l(i,2);
}


#include <iostream>

std::complex<double>
RooGaussModelAcceptance::K_n::operator()(unsigned i) const {
          assert(0<=i&&i<=3);
          switch(i) {
              case 0 : return 0.5*_zi;
              case 1 : return 0.5*_zi*_zi;
              case 2 : return _zi*(_zi*_zi+1.0);
              case 3 : std::complex<double> _zi2 = _zi*_zi; return _zi2*(3.*_zi2+3.);
              // TODO: add higher orders!!!! Go up until six... (product of two cubic splines)
          }
          std::cerr << "K_n only implemented upto (and including) 3rd order" << std::endl;
          assert(1==0);
          return 0;
}

// explicitly instantiate some templates...
template class RooGaussModelAcceptance::M_n<1U>;
template class RooGaussModelAcceptance::M_n<2U>;
template class RooGaussModelAcceptance::M_n<3U>;
template class RooGaussModelAcceptance::M_n<4U>;
// template class RooGaussModelAcceptance::M_n<5U>;
// template class RooGaussModelAcceptance::M_n<6U>;
