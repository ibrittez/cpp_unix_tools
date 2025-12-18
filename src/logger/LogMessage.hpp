/** @file LogMessage.h
 */

#ifndef LOGMESSAGE_H
#define LOGMESSAGE_H

#include "TagData.hpp"
#include <cstdint>
enum class LogMsgType : uint32_t { TAG_DATA = 1, CONTROL = 2 };

struct LogMessage {
    LogMsgType type;
    TagData tag;
};

struct LogControl {
    uint32_t command; // e.g. SHUTDOWN, FLUSH
};

#endif /* LOGMESSAGE_H */

/*** end of file ***/
