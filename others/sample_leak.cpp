#include "RooRealVar.h"
#include "RooGaussian.h"
#include "RooDataSet.h"
#include "TDirectory.h"

using namespace ROOT;
using namespace RooFit;

int sample_leak() {
  RooRealVar m("m", "m", 5000, 6000);
  RooRealVar mean("mean", "mean", 5500, 5000, 6000);
  RooRealVar sigma("sigma", "sigma", 10, 0, 100);

  RooGaussian g("g", "g", m, mean, sigma);

  for (int i=0; i<10000; ++i) {
    RooDataSet* data = g.generate(RooArgSet(m), 10000);
    g.fitTo(*data, NumCPU(2), PrintLevel(3), Warnings(false), Timer(true));
    delete data;
    gDirectory->Print();
  }
}

int main() {
  sample_leak();
}
