/******************************************************************************
** ProgressDisplay:                                                          **
**                                                                           **
** class "ProgressDisplay" is a modified copy of the class                   **
** "progress_display" from the Boost C++ Libraries (boost/progress.hpp)      **
**                                                                           **
** authors:                                                                  **
**   JvL, Jeroen van Leerdam, Nikhef, j.van.leerdam@nikhef.nl                **
**                                                                           **
******************************************************************************/

#ifndef PROGRESSDISPLAY_H
#define PROGRESSDISPLAY_H
#include <iostream>
#include <string>

class ProgressDisplay {
public:
  explicit ProgressDisplay(unsigned long expCount,
                           std::ostream & os = std::cout,
                           const std::string& s1 = "\n",
                           const std::string& s2 = "",
                           const std::string& s3 = "") :
    _os(os), _s1(s1), _s2(s2), _s3(s3) {restart(expCount);}

  void restart(unsigned long expCount);

  unsigned long  increment(unsigned long incr);

  unsigned long  operator+=(unsigned long incr) {return increment(incr);}
  unsigned long  operator++()                   {return increment(1);}
  unsigned long  count() const                  {return _count;}
  unsigned long  expCount() const               {return _expCount;}

private:
  void displayTic();

  std::ostream &    _os;
  const std::string _s1;
  const std::string _s2;
  const std::string _s3;

  unsigned long _count, _expCount, _nextTicCount;
  unsigned int  _tic;
};

#endif
