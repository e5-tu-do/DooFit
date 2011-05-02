#ifndef MsgStream_h
#define MsgStream_h

#include "Utils.h"
#include <iostream>
#include <sstream> 

class MsgStream {
public:
  MsgStream(Utils::TerminalColor color) : text_color_(color) {}
  MsgStream() : text_color_(Utils::kTextNone) {}
  
  std::ostringstream& stream() { return os_; }
  
  void flush() { os_.flush(); doOutput(); }
  MsgStream& doOutput() {
    Utils::SetTerminalColor(text_color_);
    std::cout << os_.str() << std::endl;
    Utils::ResetTerminal();
    os_.str("");
    return *this;
  }
  
  MsgStream& operator<<(std::ostream& (*_f)(std::ostream&)) {
    _f(os_);
    return *this;
  }
  
  MsgStream& operator<<(MsgStream& (*_f)(MsgStream&)) {
    _f(*this);
    return *this;
  }
  
  void Ruler() {
    os_ << "==============================================================================================================";
    doOutput();
  }
  
protected:
  std::ostringstream os_;
  Utils::TerminalColor text_color_;
};

inline MsgStream& endmsg(MsgStream& s) {
  return s.doOutput();
}

template<typename T>
inline MsgStream& operator<<(MsgStream& lhs, const T& arg) {
  lhs.stream() << arg;
  return lhs;
} 

extern MsgStream serr; 
extern MsgStream swarn;
extern MsgStream sinfo;
extern MsgStream scfg;

#endif
