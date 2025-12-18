#include "LogMessage.hpp"
#include "LogSink.hpp"
#include <vector>

class Logger {
public:
    void add_sink(LogSink* sink) { sinks_.push_back(sink); }

    void log(const LogMessage& msg)
    {
        for(auto* s: sinks_) { s->write(msg); }
    }
private:
    std::vector<LogSink*> sinks_;
};
