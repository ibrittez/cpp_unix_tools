/** @file RingBuffer.h
 *
 * @brief Ring buffer module
 */

#ifndef RINGBUFFER_H
#define RINGBUFFER_H

#include <stdexcept>
#include <string>
#include <vector>
using namespace std;

class RingBuffer {
public:
    RingBuffer(int length): buffer_(length), head_(0), tail_(0), count_(0) {}

    void push(string item) {

        buffer_[head_] = item;
        head_ = (head_ + 1) % buffer_.size();

        if(count_ == buffer_.size()) { tail_ = (tail_ + 1) % buffer_.size(); }
        else { count_++; }
    }

    std::string pop() {

        if(count_ == 0) { throw std::runtime_error("empty buffer"); }

        std::string item = buffer_[tail_];
        tail_ = (tail_ + 1) % buffer_.size();
        count_--;

        return item;
    }

    std::string peek() const {

        if(count_ == 0) { throw std::runtime_error("empty buffer"); }
        return buffer_[tail_];
    }

    bool empty(void) const { return count_ == 0; }
    bool full(void) const { return count_ == buffer_.size(); }
    size_t count(void) const { return count_; }
    size_t capacity(void) const { return buffer_.size(); }

    void clear() {
        head_ = 0;
        tail_ = 0;
        count_ = 0;
    }
private:
    vector<string> buffer_;
    size_t head_;
    size_t tail_;
    size_t count_;
};

#endif /* RINGBUFFER_H */

/*** end of file ***/
