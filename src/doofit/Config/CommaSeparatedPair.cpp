#include "doofit/Config/CommaSeparatedPair.h"

// from STL

// from Project
#include "doocore/io/MsgStream.h"

using namespace std;

namespace doofit {
namespace Config {
  using namespace doocore::lutils; using namespace doocore::io;
  
  void CommaSeparatedPair::Parse(string str) {
    vector<string> elements = DecomposeString(str);
    
    // number of elements must be two
    if (elements.size() != 2) {
      serr << "ERROR in CommaSeparatedPair::Parse(std::string): String '" << str << "' does not contain exactly two comma-separated strings." << endmsg;
      throw;
    }
    
    first_ = elements[0];
    second_ = elements[1];
  }
  
  void CommaSeparatedPair::Print(std::ostream& os) const {
    os << first_ << "->" << second_;
  }
}
}