/** @file LogMessage.h
 */

#ifndef LOGMESSAGE_H
#define LOGMESSAGE_H

#include "TagData.hpp"
#include <cstdint>

enum class LogMsgType : uint32_t { TAG_DATA = 1, CONTROL = 2 };

enum class LogControl {
    SHUTDOWN = 0,
};

struct LogMessage {
    LogMsgType type;
    union {
        TagData tag;
        LogControl ctl;
    };
};

#endif /* LOGMESSAGE_H */

/*** end of file ***/
