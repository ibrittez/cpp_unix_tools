/** @file MessageQueue.hpp
 *
 * @brief UNIX message queue class definition
 */

#ifndef MESSAGEQUEUE_H
#define MESSAGEQUEUE_H

#include <cstddef>
#include <cstring>
#include <stdexcept>
#include <string>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <type_traits>

class MessageQueueBase {
protected:
    MessageQueueBase(const std::string& key_file, char key_id, bool create)
        : msqid_(-1), key_file_(key_file), key_id_(key_id) {
        key_t key = ftok(key_file_.c_str(), key_id_);
        if(key == -1) throw std::runtime_error(strerror(errno));

        int flags = 0644;
        if(create) flags |= IPC_CREAT;

        msqid_ = msgget(key, flags);
        if(msqid_ == -1) throw std::runtime_error(strerror(errno));
    }

    /* no borra cola, cierra el handle. */
    ~MessageQueueBase() = default;

    /* prevengo la copia (cola recurso único) */
    MessageQueueBase(const MessageQueueBase&) = delete;
    MessageQueueBase& operator=(const MessageQueueBase&) = delete;

    /* permito mover la cola */
    MessageQueueBase(MessageQueueBase&& other) noexcept
        : msqid_(other.msqid_), key_file_(std::move(other.key_file_)), key_id_(other.key_id_) {
        other.msqid_ = -1;
    }

    MessageQueueBase& operator=(MessageQueueBase&& other) noexcept {
        if(this != &other) {
            msqid_ = other.msqid_;
            key_file_ = std::move(other.key_file_);
            key_id_ = other.key_id_;
            other.msqid_ = -1;
        }
        return *this;
    }

    int msqid_;
    std::string key_file_;
    char key_id_;
public:
    void remove() {
        if(msgctl(msqid_, IPC_RMID, NULL) == -1) {
            throw std::runtime_error("Failed to remove queue: " + std::string(strerror(errno)));
        }
    }
};

template<typename T> class MessageQueue : public MessageQueueBase {
    static_assert(std::is_trivially_copyable<T>::value, "MessageQueue<T>: T must be POD / trivially copyable");
public:
    MessageQueue(const std::string& key_file, char key_id, bool create = false)
        : MessageQueueBase(key_file, key_id, create) {}

    void send(const T& data, long type = 1) {
        struct {
            long mtype;
            T payload;
        } buf;

        buf.mtype = type;
        buf.payload = data;

        if(msgsnd(msqid_, &buf, sizeof(T), 0) == -1) throw std::runtime_error(strerror(errno));
    }

    T receive(long type = 0) {
        struct {
            long mtype;
            T payload;
        } buf;

        if(msgrcv(msqid_, &buf, sizeof(T), type, 0) == -1) throw std::runtime_error(strerror(errno));

        return buf.payload;
    }

    /* Recibir sin bloquear */
    bool try_receive(T& out, long type = 0) {
        struct {
            long mtype;
            T payload;
        } buf;

        ssize_t bytes = msgrcv(msqid_, &buf, sizeof(T), type, IPC_NOWAIT);
        if(bytes == -1) {
            if(errno == ENOMSG) {
                return false; // no había mensaje
            }
            throw std::runtime_error(strerror(errno));
        }

        out = buf.payload;
        return true;
    }
};

/* Especialización explica para string (es con la que veníamos trabajando) */
template<> class MessageQueue<std::string> : public MessageQueueBase {
public:
    static constexpr size_t MAX_MESSAGE_SIZE = 512;

    MessageQueue(const std::string& key_file, char key_id, bool create = false)
        : MessageQueueBase(key_file, key_id, create) {}

    void send(const std::string& msg, long type = 1) {
        if(msg.size() >= MAX_MESSAGE_SIZE) throw std::invalid_argument("message too long");

        struct {
            long mtype;
            char data[MAX_MESSAGE_SIZE];
        } buf;

        buf.mtype = type;
        std::strcpy(buf.data, msg.c_str());

        if(msgsnd(msqid_, &buf, std::strlen(buf.data) + 1, 0) == -1) throw std::runtime_error(strerror(errno));
    }

    std::string receive(long type = 0) {
        struct {
            long mtype;
            char data[MAX_MESSAGE_SIZE];
        } buf;

        if(msgrcv(msqid_, &buf, sizeof(buf.data), type, 0) == -1) throw std::runtime_error(strerror(errno));

        return std::string(buf.data);
    }

    /* Recibir sin bloquear */
    bool try_receive(std::string& out, long type = 0) {
        struct {
            long mtype;
            char mtext[MAX_MESSAGE_SIZE];
        } buf;

        ssize_t bytes = msgrcv(msqid_, &buf, MAX_MESSAGE_SIZE, type, IPC_NOWAIT);
        if(bytes == -1) {
            if(errno == ENOMSG) {
                return false; /* no había mensaje */
                ;
            }
            throw std::runtime_error("Failed to receive message: " + std::string(strerror(errno)));
        }

        out.assign(buf.mtext);
        return true;
    }
};

#endif /* MESSAGEQUEUE_H */

/*** end of file ***/
