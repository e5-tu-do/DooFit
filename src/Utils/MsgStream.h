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
 * pre-defined for certain message levels: serr, swarn, sinfo, scfg, sout and 
 * sdebug.
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
  /**
   *  \brief Constructor for colored MsgStream
   *
   *  @param color The color to be used for this stream
   */
  MsgStream(Utils::TerminalColor color) : text_color_(color), is_active_(true), indent_(0) {}
  /**
   *  \brief Default constructor for standard uncolored output
   */
  MsgStream() : text_color_(Utils::kTextNone), is_active_(true), indent_(0) {}
  
  /**
   *  \brief Get the internal std::ostringstream
   *
   *  This function returns a reference to the std::ostringstream used to store
   *  output before it is flushed to the console.
   *
   *  \return{ internal std::ostringstream. }
   */
  std::ostringstream& stream() { return os_; }
  
  /**
   *  \brief Actually output the content of the MsgStream to std::cout. 
   *
   *  Normally not needed as endmsg() will force the output.
   *
   *  \return{ this MstStream object }
   *
   *  \todo Include support for other streams as well besides std::cout.
   */
  MsgStream& doOutput() {
    if (is_active_) {
      Utils::SetTerminalColor(text_color_);
      std::cout << os_.str() << std::endl;
      Utils::ResetTerminal();
    }
    os_.str("");
    
    os_ << std::string(indent_, ' ');
    
    return *this;
  }
  
  /**
   *  \brief Stream operator for std::ostream streams. 
   *
   *  Analogous to MsgStream::operator<<(MsgStream& (*)(MsgStream&)). See 
   *  iostreams documentation for reference.
   */
  MsgStream& operator<<(std::ostream& (*_f)(std::ostream&)) {
    _f(os_);
    return *this;
  }
  
  /**
   *  \brief Stream operator for MsgStream streams. 
   *
   *  Analogous to MsgStream::operator<<(std::ostream& (*)(std::ostream&)). See 
   *  iostreams documentation for reference.
   */
  MsgStream& operator<<(MsgStream& (*_f)(MsgStream&)) {
    _f(*this);
    return *this;
  }
  
  /**
   *  \brief Output a horizontal ruler
   */
  void Ruler() {
    os_ << "==============================================================================================================";
    doOutput();
  }
  
  /**
   *  \brief Set active state of this stream
   *
   *  Determine whether to print messages send to this stream on the console or
   *  not.
   *
   *  @param active_state determines whether printing is enabled or disabled
   */
  void set_active(bool active_state) { is_active_ = active_state; }
  
  /**
   *  \brief Set current indent for new lines
   *
   *  Every new line will be indented by @a indent number of spaces, starting 
   *  after the next endmsg().
   *
   *  @param indent number of spaces to insert at every new line
   */
  void set_indent(int indent) { 
    indent_ = indent; 
    if (os_.str().length() == 0) {
      os_ << std::string(indent_, ' ');
    }
  }
  
protected:
  /**
   *  \brief Flush the internal std::ostringstream and output.
   */
  void flush() { os_.flush(); doOutput(); }
  
  /// \brief Internal std::ostringstream for data.
  std::ostringstream os_;            
  /// \brief Text color for output.
  Utils::TerminalColor text_color_;  
  
  /// \brief determining if stream is active or not (i.e. printing)
  bool is_active_;
  
  /**
   *  \brief Indent for new lines.
   */
  int indent_;
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

inline MsgStream& operator<<(MsgStream& lhs, bool b) {
  if (b) {
    lhs.stream() << "True";
  } else {
    lhs.stream() << "False";
  }
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
/// MsgStream for debug messages. Color: None
extern MsgStream sdebug;

#endif
