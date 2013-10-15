#include <algorithm>
#include <iostream>
#include "TMath.h"
#include "RooAbsReal.h"
#include "P2VV/RooCubicSplineKnot.h"

using namespace std;
namespace RooCubicSplineKnot_aux {

  Double_t get(const RooArgList& b,int i) { return ((RooAbsReal&)b[i]).getVal() ; }
  Double_t get(const RooArgList& b,int i,int k) { return RooCubicSplineKnot_aux::get(b,i+k); }

  template <typename T> typename T::const_reference get(const T& t, int i, int j) { return t[4*i+j]; }
  template <typename T> void push_back(T& t, const typename T::value_type& a,
                                             const typename T::value_type& b,
                                             const typename T::value_type& c,
                                             const typename T::value_type& d) { t.push_back(a); t.push_back(b); t.push_back(c); t.push_back(d) ; }

}

void RooCubicSplineKnot::smooth(std::vector<double>& y, const std::vector<double>& dy, double lambda) const {
// implementation as described in D.S.G. Pollock, Smoothing Splines" 
// see http://r.789695.n4.nabble.com/file/n905996/SPLINES.PDF
    using namespace std;
    int n = y.size();
    vector<double> uu(n-2),vv(n-3),ww(n-4), q(n-2);
    assert( dy.size()==y.size());
    assert( size()==n);
    // lambda = 0 : no smoothing ; lambda -> 1: straight line (the ultimate smooth curve)
    if (lambda<0|| !(lambda<1)) {
        throw std::string("RooCubicSplineKnot::smooth: smoothing parameter must be in range [0,1)");
    }
    double mu =2*lambda/(3*(1-lambda));
    for (int i=0;i<n-2;++i) {
        q[i]  = r(i)*(y[i]-y[i+1])-r(i+1)*(y[i+1]-y[i+2]);
        uu[i] = p(i+1)+mu*(sqr(r(i)*dy[i])+sqr(f(i+1)*dy[i+1])+sqr(r(i+1)*dy[i+2]) );
        if (i>n-4) continue;
        vv[i] = h(i+1)+mu*(f(i+1)*r(i+1)*sqr(dy[i+1]) + f(i+1)*r(i+2)*sqr(dy[i+2]));
        if (i>n-5) continue ;
        ww[i] =        mu*r(i+1)*r(i+2)*sqr(dy[i+2]);
    }

    // Solve A b = q  with  A(i,i)=uu(i), A(i+1,i)=A(i,i+1) = vv(i), A(i+2,i),A(i,i+2)=ww(i)
    // start with factorization: A -> L D LT
    vv[0] /= uu[0]; ww[0] /= uu[0];
    uu[1] -= uu[0]*sqr(vv[0]);
    vv[1] -= uu[0]*vv[0]*ww[0];
    vv[1] /= uu[1]; ww[1] /= uu[1];
    for (int i=2;i<n-4;++i) {
        uu[i] -= uu[i-1]*sqr(vv[i-1])+uu[i-2]*sqr(ww[i-2]);
        vv[i] -= uu[i-1]*vv[i-1]*ww[i-1];  vv[i] /= uu[i];
        ww[i] /= uu[i];
    }
    uu[n-4] -= uu[n-5]*sqr(vv[n-5])+uu[n-6]*sqr(ww[n-6]);
    vv[n-4] -= uu[n-5]*vv[n-5]*ww[n-5];  vv[n-4] /= uu[n-4];
    uu[n-3] -= uu[n-4]*sqr(vv[n-4])+uu[n-5]*sqr(ww[n-5]);

    // forward substitution (i.e. solve L ( D LT  b )  = q  for D LT b -> q
    q[1]                         -= vv[0  ]*q[0  ];
    for (int i=2;i<n-2;++i) q[i] -= vv[i-1]*q[i-1]+ww[i-2]*q[i-2];
    // rescale  (i.e. solve D (LT b ) = q  for LT b
    for (int i=0;i<n-2;++i) { q[i] /= uu[i] ; }
    // backward substitution (i.e. solve LT b = q  for b -> q )
    q[n-4]                          -= vv[n-4]*q[n-3];
    for (int i=n-5;i>=0;--i) q[i  ] -= vv[i  ]*q[i+1]+ww[i]*q[i+2];
    // solve for y...
    int i=0;
    y[i]                 -=mu*sqr(dy[i])*(r(i)*q[i]                          ); ++i;
    y[i]                 -=mu*sqr(dy[i])*(r(i)*q[i]+f(i)*q[i-1]              ); ++i;
    for (;i<n-2;++i) y[i]-=mu*sqr(dy[i])*(r(i)*q[i]+f(i)*q[i-1]+r(i-1)*q[i-2]);
    y[i]                 -=mu*sqr(dy[i])*(          f(i)*q[i-1]+r(i-1)*q[i-2]); ++i;
    y[i]                 -=mu*sqr(dy[i])*(                      r(i-1)*q[i-2]);

}

// on input, y contains the values at the knot locations
// on output, it contains the b-spline coefficients
// Note: one element will be pre-pended, and one post-pended !!
void RooCubicSplineKnot::computeCoefficients(std::vector<double>& y, BoundaryConditions bc) const
{
 // see http://en.wikipedia.org/wiki/Spline_interpolation
 // for the derivation of the linear system...
 // see http://en.wikipedia.org/wiki/Tridiagonal_matrix_algorithm
 // for the O(N) algorithm to solve the relevant linear system...
    int n = size();
    assert(int(y.size())==size());

    // Weights of first and last spline are fully determined by values at knots
    double bf = y.front();
    double bb = y.back();

    // Set boundary conditions for linear system
    y.front() = bc.value[0] - bf * ( bc.secondDerivative[0] ? ( double(6) / sqr(h(0)) )  : ( -double(3) / h(0)   ) );
    y.back()  = bc.value[1] - bb * ( bc.secondDerivative[1] ? ( double(6) / sqr(h(n-2))) : (  double(3) / h(n-2) ) );

    // Solve linear system
    std::vector<double> c; c.reserve(n);
    c.push_back( mc(0, bc.secondDerivative) / mb(0, bc.secondDerivative) );
    y[0] /=  mb(0, bc.secondDerivative);
    // TODO: rewrite the special cases i==1 and i==n-1 (move latter out of loop)
    //       so we can avoid 'if' statements in the loop body...
    for (int i = 1; i < n; ++i) {
        double m = double(1) / ( mb(i, bc.secondDerivative ) - ma(i, bc.secondDerivative) * c.back() );
        c.push_back( mc(i,bc.secondDerivative) * m );
        y[i] -=  ma(i, bc.secondDerivative) * y[i - 1];
        y[i] *=  m;
    }
    for (int i = n-1 ; i-- > 0; ) y[i] -= c[i] * y[i + 1];
    y.push_back(bb); y.insert(y.begin(),bf); // ouch... expensive!
}




void RooCubicSplineKnot::fillPQRS() const {
    assert(_PQRS.empty());
    // P,Q,R,S only depend on the knot vector, so build at construction, and cache them...
    _PQRS.reserve(4*size());
    for (int i=0;i<size();++i) {
        _PQRS.push_back( h(i+1,i-2)*h(i+1,i-1)*h(i+1,i) );
        _PQRS.push_back( h(i+1,i-1)*h(i+2,i-1)*h(i+1,i) );
        _PQRS.push_back( h(i+2,i  )*h(i+2,i-1)*h(i+1,i) );
        _PQRS.push_back( h(i+2,i  )*h(i+3,i  )*h(i+1,i) );
    }

}

double RooCubicSplineKnot::evaluate(double x, const RooArgList& b) const {
    using RooCubicSplineKnot_aux::get; 
    int i = index(x); // location in knot vector: index of last knot less than x...
    assert(-1<=i && i<=size());
    // we're a 'natural' spline. If beyond first/last knot, Extrapolate using the derivative at the first (last) knot
    if (i==-1    ) { ++i; return get(b,i,0) - d(x,i)*r(i  )*(get(b,i,0)-get(b,i,1)); }
    if (i==size()) { --i; return get(b,i,2) + d(x,i)*r(i-1)*(get(b,i,2)-get(b,i,1)); }
    assert( u(i) <= x && x<= u(i+1) );
    return get(b,i,0)*A(x,i) // TODO: substitute A,B,C,D 'in situ'
         + get(b,i,1)*B(x,i)
         + get(b,i,2)*C(x,i)
         + get(b,i,3)*D(x,i);
}

double RooCubicSplineKnot::analyticalIntegral(const RooArgList& b) const {
    using RooCubicSplineKnot_aux::push_back;
    using RooCubicSplineKnot_aux::get;
    if (_IABCD.empty()) {
        // the integrals of A,B,C,D from u(i) to u(i+1) only depend on the knot vector...
        // so we create them 'on demand' and cache the result
        _IABCD.reserve(4*size());
        for (int j=0;j<size();++j) {
            push_back(_IABCD,   qua(h(j,j+1))/(4*P(j))
                            , - cub(h(j,j+1))*(3*u(j)-4*u(j-2)+u(j+1))/(12*P(j))
                              - sqr(h(j,j+1))*(3*sqr(u(j))-2*u(j-1)*u(j+1)+sqr(u(j+1))+u(j)*(-4*u(j-1)+2*u(j+1)-4*u(j+2)) +6*u(j-1)*u(j+2)-2*u(j+1)*u(j+2) )/(12*Q(j))
                              + sqr(h(j,j+1))*(3*sqr(u(j+1))+sqr(u(j  ))+2*u(j)*u(j+1)-8*u(j+1)*u(j+2)-4*u(j  )*u(j+2)+6*sqr(u(j+2)))/(12*R(j))
                            ,   sqr(h(j,j+1))*(3*sqr(u(j  ))+sqr(u(j+1))+2*u(j+1)*u(j)-8*u(j  )*u(j-1)-4*u(j-1)*u(j+1)+6*sqr(u(j-1)))/(12*Q(j))
                              - sqr(h(j,j+1))*(3*sqr(u(j+1))+sqr(u(j))-4*u(j-1)*u(j+1)+6*u(j-1)*u(j+2)-4*u(j+1)*u(j+2)-2*u(j)*(u(j-1)-u(j+1)+u(j+2)))/(12*R(j))
                              + cub(h(j,j+1))*(3*u(j+1)-4*u(j+3)+u(j))/(12*S(j))
                            ,   qua(h(j,j+1))/(4*S(j)) );
        }
    }
    // FIXME: this assumes the integration range goes from first knot to last knot...
    assert(b.getSize()-2==size());
    double norm(0);
    for (int i=0; i < size()-1; ++i) for (int k=0;k<4;++k) {
        norm += get(b,i,k)*RooCubicSplineKnot_aux::get(_IABCD,i,k) ;
    }
    return norm;
}

// location in knot vector: index of last knot less than x... except when beyond interval+ last +1 if to right, -1 if to left
int RooCubicSplineKnot::index(double u) const
{
    if (u>_u.back()) return size();
    std::vector<double>::const_iterator i = --std::upper_bound(_u.begin(),_u.end()-1,u);
    return std::distance(_u.begin(),i);
};

// S matrix for i-th interval
RooCubicSplineKnot::S_jk RooCubicSplineKnot::S_jk_sum(int i, const RooArgList& b) const
{
    if (_S_jk.empty()) {
        _S_jk.reserve(size()*4);
        for(int i=0;i<size();++i) {
            // This 'table' should be compatible with the definitions of A,B,C, and D...
            _S_jk.push_back( -RooCubicSplineKnot::S_jk(u(i+1),u(i+1),u(i+1))/P(i) ); // A
            _S_jk.push_back(  RooCubicSplineKnot::S_jk(u(i-2),u(i+1),u(i+1))/P(i)
                             +RooCubicSplineKnot::S_jk(u(i-1),u(i+1),u(i+2))/Q(i)
                             +RooCubicSplineKnot::S_jk(u(i  ),u(i+2),u(i+2))/R(i) ); // B
            _S_jk.push_back( -RooCubicSplineKnot::S_jk(u(i-1),u(i-1),u(i+1))/Q(i)
                             -RooCubicSplineKnot::S_jk(u(i-1),u(i  ),u(i+2))/R(i)
                             -RooCubicSplineKnot::S_jk(u(i  ),u(i  ),u(i+3))/S(i) ); // C
            _S_jk.push_back(  RooCubicSplineKnot::S_jk(u(i  ),u(i  ),u(i  ))/S(i) ); // D
        }
    }
    using RooCubicSplineKnot_aux::get;
    return get(_S_jk,i,0)*get(b,i,0)
         + get(_S_jk,i,1)*get(b,i,1)
         + get(_S_jk,i,2)*get(b,i,2)
         + get(_S_jk,i,3)*get(b,i,3);
}

RooCubicSplineKnot::S_edge::S_edge(const S_edge& other, double offset) :
    alpha(other.alpha), beta(other.beta)
{
    if (offset != 0) {
        std::cout << "RooCubicSplitKnot::S_edge: argument \"offset\" is not equal to 0" << std::endl;
        assert(offset==0);
    }
}

// S matrix for natural extrapolation beyond the first/last knot...
RooCubicSplineKnot::S_edge RooCubicSplineKnot::S_jk_edge(bool left, const RooArgList& b) const {
       using RooCubicSplineKnot_aux::get;
       if (left) {
         // efficiency = return evaluate(u(0),b) - d(x,0)*r(0)*(get(b,0,0)-get(b,0,1)); 
         double alpha = -r(0)*(get(b,0,0)-get(b,0,1));
         double beta = evaluate(u(0),b)-alpha*u(0);
         return S_edge( alpha, beta );
       } else {
         int i = size()-1;
         double alpha = r(i-1)*(get(b,i,2)-get(b,i,1));
         double beta = evaluate(u(i),b)-alpha*u(i);
         return S_edge(alpha,beta);
       }
}

namespace {
    // integral from lo to hi of (x-a)(x-b)(x-c)Exp[-gamma x]
    double eI(double lo, double hi, double a, double b, double c, double gamma) {
        assert(hi>=lo);
        assert(0<=lo);
          a*=gamma;b*=gamma;c*=gamma;
          lo*=gamma;hi*=gamma;
          double result = -a*b*c       *(TMath::Gamma(1,hi) - TMath::Gamma(1,lo))*TMath::Gamma(1) 
                        + (b*c+a*c+a*b)*(TMath::Gamma(2,hi) - TMath::Gamma(2,lo))*TMath::Gamma(2) 
                        - (a+b+c)      *(TMath::Gamma(3,hi) - TMath::Gamma(3,lo))*TMath::Gamma(3) 
                        +               (TMath::Gamma(4,hi) - TMath::Gamma(4,lo))*TMath::Gamma(4);

          return result / TMath::Power(gamma,4);
    }
    // integral from lo to hi of (x-a)(x-b)(x-c)
    double fI(double lo, double hi, double a, double b, double c, double ) {
        assert(hi>=lo);
        assert(0<=lo);
          double result = -a*b*c       *( hi - lo )
                        + (b*c+a*c+a*b)*( hi*hi - lo*lo ) / 2.
                        - (a+b+c)      *( hi*hi*hi - lo*lo*lo )/ 3.
                        +               ( hi*hi*hi*hi - lo*lo*lo*lo) / 4. ;

          return result ;
    }

#if 0
    // integral from lo to hi of   [ c + slope * (x - offset) ] Exp[-Gamma x ]
    //                           = [ c-slope*offset + slope*x ] Exp[-Gamma x ]
    double eIn(double lo, double hi, double c, double slope, double offset, double gamma) {
          assert(hi>=lo);
          assert(0<=lo);
          double a = c - slope*offset;
          double b = slope; //  ( a + b x ) Exp[ -Gamma x ]
                            //  (1/Gamma)  ( a Gamma      b y )      Exp[ -y ]



          return result / TMath::Power(gamma,4);
    }

#endif
}

#include "TMatrixD.h"
#include "TVectorD.h"
#include "TDecompBK.h"

// return integrals from lo to hi of basis spline . exp(-gamma x), for each basis spline...
// FIXME: # of splines != # of knots... so cannot return as TGraphErrors...

// return the chisquared , input hist + gamma, out : coefficients, errors
double  RooCubicSplineKnot::expIntegral(const TH1* hist, double gamma, TVectorD& coefficients, TMatrixD& covarianceMatrix) const {
    assert(hist!=0);
    int nknots = size();
    int nsplines = nknots+2;
    int nbins = hist->GetNbinsX();
    TMatrixD matrix(nsplines,nbins); for (int i=0;i<nsplines;++i) for (int j=0;j<nbins;++j) { matrix(i,j)=0; }
    TVectorD Y(nbins), DY(nbins);

    for (int i=0;i<nbins ;++i) {
        Y(i)  = hist->GetBinContent(1+i);
        DY(i) = hist->GetBinError(1+i);


        Y(i) /= DY(i)*DY(i);

        double lo = hist->GetBinLowEdge(1+i);
        double hi = lo + hist->GetBinWidth(1+i);

        double Norm =   (exp(-gamma*lo)-exp(-gamma*hi))/gamma;

        // go for flat parent distribution;
        // Norm = hi-lo;

#if 0
        // compute contributions beyond the first and last knot...
        if (lo < _u.front()) {
                // b-spline 0:  1 - d(x,0)*r(0)  // d(x,0) = x - u(0)
                // b-spline 1:    + d(x,0)*r(0) 
                matrix(0, 0) += eIn( lo,hi, 1., -r(0), u(0) )/Norm;
                matrix(1, 0) += eIn( lo,hi, 0., +r(0), u(0) )/Norm;

        }
        if (hi > _u.back()) {
                //                                           c + alpha  *    (x - x0) 
                matrix(nknots  , nbins-1) += eIn( lo, hi, 0., -r(nknots-2), u(nknots-1) )/Norm;
                matrix(nknots+1, nbins-1) += eIn( lo, hi, 1., +r(nknots-2), u(nknots-1) )/Norm;

        }
#else
        //  assume nothing beyond spline interval
        assert( lo >= _u.front() && hi <= _u.back() );
#endif

        // this bit is by construction fully contained between the first and last knot
        for (int j=0;j<nknots-1;++j) { 
            double l = std::max(lo,u(j));
            double h = std::min(hi,u(j+1));
            if (l>=h) continue;
            // in the knot interval [ u(j),  u(j+1) ], the splines j..j+3 contribute...
            matrix(j  ,i) += (-eI(l,h, u(j+1),u(j+1),u(j+1), gamma)/P(j) ) / Norm;
            matrix(j+1,i) += ( eI(l,h, u(j-2),u(j+1),u(j+1), gamma)/P(j) 
                              +eI(l,h, u(j-1),u(j+1),u(j+2), gamma)/Q(j) 
                              +eI(l,h, u(j  ),u(j+2),u(j+2), gamma)/R(j) ) / Norm;
            matrix(j+2,i) += (-eI(l,h, u(j-1),u(j-1),u(j+1), gamma)/Q(j) 
                              -eI(l,h, u(j-1),u(j  ),u(j+2), gamma)/R(j) 
                              -eI(l,h, u(j  ),u(j  ),u(j+3), gamma)/S(j) ) / Norm; 
            matrix(j+3,i) += ( eI(l,h, u(j  ),u(j  ),u(j  ), gamma)/S(j) ) / Norm; 
        }

    }

    TMatrixDSym D(nsplines); 
    for (int i=0;i<nsplines;++i) for (int j=0;j<=i;++j) {
        D(i,j) = 0;
        for (int k=0;k<nbins;++k) D(i,j) += matrix(i,k)*matrix(j,k)/(DY(k)*DY(k));
        if (i!=j) D(j,i) = D(i,j);
    }
    TDecompBK solver(D);

    Y *= matrix;
    TVectorD YY = Y;

    bool ok;
    coefficients.ResizeTo(nsplines);
    coefficients = solver.Solve(Y,ok);
    if (!ok) { 
        std::cout << "WARNING: bad linear system solution... " << std::endl;
        return -1;
    }

    D.Invert();
    covarianceMatrix.ResizeTo(D);
    covarianceMatrix = D;

    double chisq = 0;
    for (int i=0;i<nbins;++i) {
        double y = hist->GetBinContent(1+i);
        double dy = hist->GetBinError(1+i);
        double f = 0; for (int j=0;j<nsplines;++j) f+=matrix(j,i)*coefficients(j);
        double c = sqr( (y-f)/dy );
        // double lo = hist->GetBinLowEdge(1+i);
        // double hi = lo + hist->GetBinWidth(1+i);
        // std::cout << " bin " << i  << " [  " << lo << " , " << hi << " ]  y = " << y << " dy = " << dy << " f = " << f << " chi^2 = " << c << std::endl;
        chisq+=c;
    }

    return chisq;
}
