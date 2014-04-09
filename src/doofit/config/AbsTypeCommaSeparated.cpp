#include "doofit/config/AbsTypeCommaSeparated.h"

// from STL
#include <iostream>
#include <string>
#include <vector>

// from Boost
#include <boost/tokenizer.hpp>

using namespace std;
vector<string> doofit::config::AbsTypeCommaSeparated::DecomposeString(string str) const {
  vector<string> elements;
  
  using namespace boost;
  char_separator<char> sep(",");
  tokenizer<char_separator<char> > tok(str,sep);
  
  for (tokenizer<char_separator<char> >::iterator it=tok.begin(); it!=tok.end(); ++it) {
    elements.push_back(*it);
  }
  
  return elements;
}

std::istream& operator>>(std::istream& is, doofit::config::AbsTypeCommaSeparated& arg) {
  std::string s;
  is >> s;
  arg.Parse(s);
  return is;
}

std::ostream& operator<<(std::ostream& os, const doofit::config::AbsTypeCommaSeparated& arg) {
  arg.Print(os);
  return os;
}
