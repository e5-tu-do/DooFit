// STL

// Boost

// ROOT

// RooFit
#include "RooWorkspace.h"

// from Project
#include "Pdf2Ws/Pdf2WsStd/Pdf2WsStdMass.h"

int main(){
  RooWorkspace* ws = new RooWorkspace("ws");
  Pdf2WsStd::Mass::Gaussian(ws, "test", "test","mass","mittelwert", "abweichung");
  
  ws->Print("t");
  return 0;
}
