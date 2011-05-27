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

      po::options_description desc("Allowed options");
      desc.add_options()
          ("help", "produce help message")
          ("compression", po::value<int>(), "set compression level")
      ;
      
      po::options_description desc2("Allowed options 2");
      desc2.add_options()
          ("pleh2", "produce pleh2 message")
          ("compressione", po::value<int>(), "set compressione level")
      ;
          

      po::variables_map vm;
      po::parsed_options parsed = po::command_line_parser(ac, av).options(desc).allow_unregistered().run();
      
      po::store(parsed, vm);
      po::notify(vm);    

      vector<string> to_pass_further = po::collect_unrecognized(parsed.options, po::exclude_positional);
      
      for (int i=0; i<to_pass_further.size(); ++i){
        cout << to_pass_further[i] << endl;
      }
      
      po::variables_map vm2;
      po::parsed_options parsed2 = po::command_line_parser(to_pass_further).options(desc2).allow_unregistered().run();
      
      po::store(parsed2, vm2);
      po::notify(vm2);

      if (vm.count("help")) {
          cout << desc << "\n";
      }

      if (vm.count("compression")) {
          cout << "Compression level was set to " 
               << vm["compression"].as<int>() << ".\n";
      } else {
          cout << "Compression level was not set.\n";
      }
      
      if (vm2.count("pleh2")) {
          cout << desc2 << "\n";
      }

      if (vm2.count("compressione")) {
          cout << "Compressione level was set to " 
               << vm2["compressione"].as<int>() << ".\n";
      } else {
          cout << "Compressione level was not set.\n";
      }
      
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
