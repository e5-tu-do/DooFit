
// from STL
#include <iostream>
#include <string>

// from RooFit
#include "RooWorkspace.h"

// from Project
#include "DooFit/Builder/BobTheBuilder/Bob/Bob.h"

using namespace std;
using namespace DooFit;
namespace bpt = boost::property_tree;

using namespace Builder::BobTheBuilder;

int main( int argc, char *argv[] ){
  
  string filename = argv[1];
  
  Bob builder;
  builder.load(filename);
  
  RooWorkspace ws("workspace","workspace");
  builder.DumpToRooWorkspace(ws);
  
  ws.Print();
  
  return 0;
}
