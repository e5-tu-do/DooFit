#include "doofit/utils/MsgStream.h"

namespace doofit {
int MsgStream::indent_ = 0;

MsgStream serr   (utils::kTextRed);
MsgStream swarn  (utils::kTextYellow);
MsgStream sinfo  (utils::kTextGreen);
MsgStream scfg   (utils::kTextBlue);
MsgStream sout   (utils::kTextNone);
MsgStream sdebug (utils::kTextNone);
};