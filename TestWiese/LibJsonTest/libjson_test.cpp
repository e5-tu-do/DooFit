#include <iostream>

#include "libjson/libjson.h"

using namespace std;

int main(int ac, char* av[]) {
  JSONNode n(JSON_NODE);
  n.push_back(JSONNode("RootA", "Value in parent node"));
  JSONNode c(JSON_NODE);
  c.set_name("ChildNode");
  c.push_back(JSONNode("ChildA", "String Value"));
  c.push_back(JSONNode("ChildB", 42));
  n.push_back(c);
  std::string jc = n.write_formatted();
  std::cout << jc << std::endl;
  return 0;
}
