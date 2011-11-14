
// from STL
#include <iostream>
#include <string>



// from Project
#include "Builder/Standard/Constructor/Constructor.h"

using namespace std;
namespace bpt = boost::property_tree;

using namespace Builder::Standard;

int main( int argc, char *argv[] ){
  
  string filename = argv[1];
  
  Constructor builder;
  builder.load(filename);
  
  
  return 0;
}