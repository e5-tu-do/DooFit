#include "MsgStream.h"

namespace DooFit {
int MsgStream::indent_ = 0;

MsgStream serr   (Utils::kTextRed);
MsgStream swarn  (Utils::kTextYellow);
MsgStream sinfo  (Utils::kTextGreen);
MsgStream scfg   (Utils::kTextBlue);
MsgStream sout   (Utils::kTextNone);
MsgStream sdebug (Utils::kTextNone);
};