#ifndef MsgStream_h
#define MsgStream_h

#include "Utils.h"
#include <iostream>
#include <sstream> 

/*! \class MsgStream 
 * \brief A class for message output using different messages and colors.
 *
 * MsgStream is a handler class to output messages. It can be used like 
 * std::cout and similar streams. The following MsgStream objects are 
 * pre-defined for certain message levels: serr, swarn, sinfo, scfg and sout.
 *
 * Usage example:
 * \code 
 * swarn << "This is a warning message which will be printed in yellow. Value is " << value << endmsg;
 * \endcode
 *
 * The user can define their own MsgStream as well:
 * \code
 * MsgStream mymsgstream(Utils::kTextBlue);
 * mymsgstream << "My own stream" << endmsg;
 * \endcode
 */
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
extern MsgStream sout;

#endif
