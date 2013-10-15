#include "P2VV/ProgressDisplay.h"

void ProgressDisplay::restart(unsigned long expCount)
{
  _count = _nextTicCount = _tic = 0;
  _expCount = expCount;

  _os << _s1 << "0%   10   20   30   40   50   60   70   80   90   100%\n"
       << _s2 << "|----|----|----|----|----|----|----|----|----|----|"
       << std::endl  // endl implies flush, which ensures display
       << _s3;
  if (!_expCount) _expCount = 1;  // prevent divide by zero
}

unsigned long ProgressDisplay::increment(unsigned long incr)
{
  if ((_count += incr) >= _nextTicCount) displayTic();
  return _count;
}

void ProgressDisplay::displayTic()
{
  // use of floating point ensures that both large and small counts
  // work correctly.  static_cast<>() is also used several places
  // to suppress spurious compiler warnings. 
  unsigned int ticsNeeded =
    static_cast<unsigned int>((static_cast<double>(_count) / _expCount) * 50.);

  do {
    _os << '*' << std::flush;
  } while (++_tic < ticsNeeded);

  _nextTicCount = static_cast<unsigned long>((_tic / 50.0) * _expCount);
  if (_count == _expCount) {
    if (_tic < 51) _os << '*';
    _os << std::endl;
  }
}

