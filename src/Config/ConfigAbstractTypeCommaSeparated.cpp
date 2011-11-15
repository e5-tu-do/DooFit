#include "Config/ConfigAbstractTypeCommaSeparated.h"

// from STL
#include <string>
#include <vector>

// from Boost
#include <boost/tokenizer.hpp>

using namespace std;
using namespace Config;

vector<string> ConfigAbstractTypeCommaSeparated::DecomposeString(string str) const {
  vector<string> elements;
  
  using namespace boost;
  char_separator<char> sep(",");
  tokenizer<char_separator<char> > tok(str,sep);
  
  for (tokenizer<char_separator<char> >::iterator it=tok.begin(); it!=tok.end(); ++it) {
    elements.push_back(*it);
  }
  
  return elements;
}

std::istream& operator>>(std::istream& is, ConfigAbstractTypeCommaSeparated& arg) {
  std::string s;
  is >> s;
  arg.Parse(s);
  return is;
}

std::ostream& operator<<(std::ostream& os, const ConfigAbstractTypeCommaSeparated& arg) {
  arg.Print(os);
  return os;
}