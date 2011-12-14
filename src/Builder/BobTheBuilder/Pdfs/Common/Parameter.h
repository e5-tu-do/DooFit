#ifndef BUILDER_BOBTHEBUILDER_PARAMETER_H
#define BUILDER_BOBTHEBUILDER_PARAMETER_H

// from STL
#include <iostream>
#include <string>

// forward declarations
class RooWorkspace;

namespace Builder{ namespace BobTheBuilder{
  
class Parameter{
 public:
  Parameter();
  Parameter( const std::string& string_to_parse);
  
  ~Parameter();
  
  void Parse( const std::string& string_to_parse );
  void Print( std::ostream& os) const;
  
  void AddToWorkspace( RooWorkspace* ws );
  
      
 protected:
    
 private:
  std::string name_;
  std::string desc_;

};

std::istream& operator>>(std::istream& is, Parameter& arg);
std::ostream& operator<<(std::ostream& os, const Parameter& arg);
  
}}

#endif // BUiLDER_BOBTHEBUILDER_PARAMETER_H
