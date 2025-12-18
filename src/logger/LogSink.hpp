#include "LogMessage.hpp"
#include "TagData.hpp"
#include <fstream>
#include <iostream>
#include <mutex>
#include <unistd.h>
#include <vector>

struct LogSink {
    virtual ~LogSink() = default;
    virtual void write(const LogMessage& msg) = 0;
};

class StdoutSink : public LogSink {
public:
    void write(const LogMessage& msg) override
    {
        if(msg.type == LogMsgType::TAG_DATA) {
            const TagData& t = msg.tag;
            std::cout << t.to_string() << std::endl;
        }
    }
};

class FileSink : public LogSink {
public:
    explicit FileSink(const std::string& path) { logfile_.open(path, std::ios::app); }
    ~FileSink()
    {
        if(logfile_.is_open()) { logfile_.close(); }
    }

    void write(const LogMessage& msg) override
    {
        if(!logfile_.is_open()) {
            perror("FileSink open failed");
            return;
        }

        if(msg.type == LogMsgType::TAG_DATA) {
            const TagData& t = msg.tag;
            logfile_ << t.to_string() << std::endl;
        }
    }
private:
    std::ofstream logfile_;
};

class TcpSink : public LogSink {
public:
    void add_client(int fd)
    {
        std::lock_guard<std::mutex> lock(mtx_);
        clients_.push_back(fd);
    }

    void write(const LogMessage& msg) override
    {
        if(msg.type != LogMsgType::TAG_DATA) return;

        std::string line = msg.tag.to_string();

        std::lock_guard<std::mutex> lock(mtx_);

        for(auto it = clients_.begin(); it != clients_.end();) {
            ssize_t n = ::write(*it, line.data(), line.size());
            if(n <= 0) {
                ::close(*it);
                it = clients_.erase(it);
            }
            else {
                ::write(*it, "\n", 1);
                ++it;
            }
        }
    }
private:
    std::vector<int> clients_;
    std::mutex mtx_;
};
