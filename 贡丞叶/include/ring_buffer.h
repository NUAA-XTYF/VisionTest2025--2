#ifndef RING_BUFFER_H
#define RING_BUFFER_H

#include <array>
#include <stdexcept>
#include <cstddef>

template<typename T, size_t N>
class RingBuffer {
public:
    RingBuffer() : head_(0), tail_(0), size_(0) {}
    
    bool empty() const { return size_ == 0; }
    bool full() const { return size_ == N; }
    size_t size() const { return size_; }
    size_t capacity() const { return N; }
    
    void push(const T& value) {
        if (full()) {
            throw std::runtime_error("RingBuffer is full");
        }
        buffer_[tail_] = value;
        tail_ = (tail_ + 1) % N;
        ++size_;
    }
    
    T pop() {
        if (empty()) {
            throw std::runtime_error("RingBuffer is empty");
        }
        T value = buffer_[head_];
        head_ = (head_ + 1) % N;
        --size_;
        return value;
    }
    
    const T& front() const {
        if (empty()) {
            throw std::runtime_error("RingBuffer is empty");
        }
        return buffer_[head_];
    }
    
    const T& back() const {
        if (empty()) {
            throw std::runtime_error("RingBuffer is empty");
        }
        return buffer_[(tail_ + N - 1) % N];
    }
    
    const T& operator[](size_t index) const {
        if (index >= size_) {
            throw std::out_of_range("RingBuffer index out of range");
        }
        return buffer_[(head_ + index) % N];
    }
    
private:
    std::array<T, N> buffer_;
    size_t head_;
    size_t tail_;
    size_t size_;
};

#endif
