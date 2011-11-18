
// from STL
#include <iostream>
#include <string>



// from Project
#include "Builder/BobTheBuilder/Bob/Bob.h"

using namespace std;
namespace bpt = boost::property_tree;

using namespace Builder::BobTheBuilder;

int main( int argc, char *argv[] ){
  
  string filename = argv[1];
  
  Bob builder;
  builder.load(filename);
  
  
  return 0;
}
