#ifndef GENERATION_WINDOW_H
#define GENERATION_WINDOW_H

#include <cstddef>
#include "exceptions.h"
#include "dynamic_array.h"
#include "array_sequence.h"



template<class T>
class generation_window {
private:
    dynamic_array<T> buffer_;
    std::size_t capacity_;
    std::size_t start_position_ = 0;
    std::size_t stored_count_ = 0;
    std::size_t first_stored_index_ = 0;

    std::size_t physical_position(std::size_t logical_offset) const {
        return (start_position_ + logical_offset) % capacity_;
    }

public:
    explicit generation_window(std::size_t capacity = 50)
        : buffer_(static_cast<int>(capacity == 0 ? 1 : capacity)),
          capacity_(capacity == 0 ? 1 : capacity) {}

    void push_back(const T& value) {
        if (stored_count_ < capacity_) {
            buffer_.set(static_cast<int>(physical_position(stored_count_)), value);
            ++stored_count_;
            return;
        }
        buffer_.set(static_cast<int>(start_position_), value);
        start_position_ = (start_position_ + 1) % capacity_;
        ++first_stored_index_;
    }

    bool contains(std::size_t global_index) const {
        return global_index >= first_stored_index_ &&
               global_index < first_stored_index_ + stored_count_;
    }

    T get(std::size_t global_index) const {
        if (!contains(global_index)) {
            throw index_out_of_range_exception("generation_window: index is outside window");
        }
        std::size_t logical_offset = global_index - first_stored_index_;
        return buffer_.get(static_cast<int>(physical_position(logical_offset)));
    }

    mutable_array_sequence<T> to_sequence() const {
        mutable_array_sequence<T> result;
        for (std::size_t i = 0; i < stored_count_; ++i) {
            result.append(buffer_.get(static_cast<int>(physical_position(i))));
        }
        return result;
    }

    std::size_t size() const {
        return stored_count_;
    }

    std::size_t capacity() const {
        return capacity_;
    }

    std::size_t first_index() const {
        return first_stored_index_;
    }

    std::size_t last_index() const {
        if (stored_count_ == 0) {
            throw empty_collection_exception("generation_window: empty");
        }
        return first_stored_index_ + stored_count_ - 1;
    }
};

#endif
