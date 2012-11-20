#include "doofit/config/DiscreteProbabilityDistribution.h"

// from STL

// from Project
#include "doocore/io/MsgStream.h"

using namespace std;

namespace doofit {
namespace config {
  using namespace doocore::io;
  
  void DiscreteProbabilityDistribution::Parse(string str) {
    vector<string> elements = DecomposeString(str);
    
    // number of elements must be odd (one simple and several pairs)
    if (elements.size()%2 != 1) {
      serr << "ERROR in DiscreteProbabilityDistribution::Parse(std::string): String '" << str << "' ends prematurely." << endmsg;
      throw;
    }
    
    vector<string>::iterator it=elements.begin();
    
    var_name_ = *(it);
    ++it;
    
    double cumuluated_prob = 0.0;
    
    while (it!=elements.end()) {
      double key = atof((*it).c_str());
      double value = atof((*(++it)).c_str());
      cumuluated_prob += value;    
      
      probabilities_.push_back(pair<double,double>(key,cumuluated_prob));
      ++it;
    }
    
    if (TMath::Abs((cumuluated_prob - 1.0)) > 1e-14) {
      serr << "ERROR in DiscreteProbabilityDistribution::Parse(string): Cumulative probability is not 1.0 (is " << cumuluated_prob << " - 1 = " << cumuluated_prob-1.0 << ")." << endmsg;
      throw;
    }
  }
  
  void DiscreteProbabilityDistribution::Print(std::ostream& os) const {
    os << "Variable: " << var_name_;
    double cumuluated_prob = 0.0;
    for (std::vector<pair<double,double> >::const_iterator itmap=probabilities_.begin(); itmap!=probabilities_.end(); ++itmap) {
      
      os << "; P(" << (*itmap).first << ") = " << (*itmap).second-cumuluated_prob;
      cumuluated_prob = (*itmap).second;
    }
  }
}
}
