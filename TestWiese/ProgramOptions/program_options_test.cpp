#include <boost/program_options.hpp>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>
#include <algorithm>

namespace po = boost::program_options;
using namespace std;

// A helper function to simplify the main part.
template<class T>
ostream& operator<<(ostream& os, const vector<T>& v)
{
  copy(v.begin(), v.end(), ostream_iterator<T>(cout, " ")); 
  return os;
}


int main(int ac, char* av[]) {
  try {
    
    int opt;
    po::options_description desc("Allowed options");
    desc.add_options()
    ("help", "produce help message")
    ("optimization", po::value<int>(&opt)->default_value(10), 
     "optimization level")
    ("include-path,I", po::value< vector<string> >(), 
     "include path")
    ("input-file", po::value< vector<string> >(), "input file");
    
    po::variables_map vm;        
    po::positional_options_description p;
    p.add("input-file", -1);
    
    po::store(po::command_line_parser(ac, av).
              options(desc).positional(p).run(), vm);
    po::notify(vm);
    
    if (vm.count("help")) {
      cout << desc << "\n";
      return 1;
    }
    
    if (vm.count("include-path"))
    {
      cout << "Include paths are: " 
      << vm["include-path"].as< vector<string> >() << "\n";
    }
    
    if (vm.count("input-file"))
    {
      cout << "Input files are: " 
      << vm["input-file"].as< vector<string> >() << "\n";
    }
    
    cout << "Optimization level is " << opt << "\n";
  }
  catch(exception& e) {
    cerr << "error: " << e.what() << "\n";
    return 1;
  }
  catch(...) {
    cerr << "Exception of unknown type!\n";
  }
  
  return 0;
}
