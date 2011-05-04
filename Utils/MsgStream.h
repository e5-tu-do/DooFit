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
  /// \brief Constructor for colored MsgStream.
  /// \param color The color to be used for this stream.
  MsgStream(Utils::TerminalColor color) : text_color_(color) {}
  /// Constructor for standard uncolored output.
  MsgStream() : text_color_(Utils::kTextNone) {}
  
  /// \brief Get the internal std::ostringstream.
  /// \return Internal std::ostringstream.
  std::ostringstream& stream() { return os_; }
  
  /// \brief Actually output the content of the MsgStream to std::cout. 
  /// Normally not needed as endmsg() will force the output.
  /// \todo Include support for other streams as well besides std::cout.
  MsgStream& doOutput() {
    Utils::SetTerminalColor(text_color_);
    std::cout << os_.str() << std::endl;
    Utils::ResetTerminal();
    os_.str("");
    return *this;
  }
  
  /// \brief Stream operator for STL streams. 
  /// See iostreams documentation for reference.
  MsgStream& operator<<(std::ostream& (*_f)(std::ostream&)) {
    _f(os_);
    return *this;
  }
  
  /// \brief Stream operator for MsgStream streams. 
  /// Analogous to MsgStream::operator<<(std::ostream& (*)(std::ostream&)).
  MsgStream& operator<<(MsgStream& (*_f)(MsgStream&)) {
    _f(*this);
    return *this;
  }
  
  /// Output a horizontal ruler.
  void Ruler() {
    os_ << "==============================================================================================================";
    doOutput();
  }
  
protected:
  /// Flush the internal std::ostringstream.
  void flush() { os_.flush(); doOutput(); }
  
  /// \brief Internal std::ostringstream for data.
  std::ostringstream os_;            
  /// \brief Text color for output.
  Utils::TerminalColor text_color_;  
};

/// \brief MsgStream function to end a message (i.e. newline) and force the output. 
///
/// Not to be called directly but to be used together with 
/// MsgStream::operator<<() functions.
/// Usage example:
/// \code
/// swarn << "This is a warning message which will be printed in yellow. Value is " << value << endmsg;
/// \endcode
inline MsgStream& endmsg(MsgStream& s) {
  return s.doOutput();
}

/// \brief Stream operator for MsgStream streams and additional objects.
/// To be used with other objects like in STL iostreams.
template<typename T>
inline MsgStream& operator<<(MsgStream& lhs, const T& arg) {
  lhs.stream() << arg;
  return lhs;
} 

/// MsgStream for errors. Color: Red
extern MsgStream serr; 
/// MsgStream for warnings. Color: Yellow
extern MsgStream swarn;
/// MsgStream for info messages. Color: Green
extern MsgStream sinfo;
/// MsgStream for config messages. Color: Blue
extern MsgStream scfg;
/// MsgStream for standard messages. Color: None
extern MsgStream sout;

#endif
