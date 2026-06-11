#ifndef DYNAMIC_ARRAY_H
#define DYNAMIC_ARRAY_H

#include "exceptions.h"
#include <algorithm>

template<class T>
class dynamic_array {
private:
    T* data_;
    int size_;

public:
    dynamic_array() : data_(nullptr), size_(0) {}

    dynamic_array(const T* items, int count) : size_(count) {
        if (count <= 0) {
            data_ = nullptr;
            size_ = 0;
            return;
        }
        data_ = new T[count];
        for (int i = 0; i < count; i++) {
            data_[i] = items[i];
        }
    }

    dynamic_array(int size) : size_(size) {
        if (size <= 0) {
            data_ = nullptr;
            size_ = 0;
            return;
        }
        data_ = new T[size];
    }

    dynamic_array(const dynamic_array& other) : size_(other.size_) {
        if (other.data_ == nullptr) {
            data_ = nullptr;
            return;
        }
        data_ = new T[size_];
        for (int i = 0; i < size_; i++) {
            data_[i] = other.data_[i];
        }
    }

    dynamic_array& operator=(const dynamic_array& other) {
        if (this != &other) {
            delete[] data_;
            size_ = other.size_;
            if (other.data_ == nullptr) {
                data_ = nullptr;
            }
            else {
                data_ = new T[size_];
                for (int i = 0; i < size_; i++) {
                    data_[i] = other.data_[i];
                }
            }
        }
        return *this;
    }

    ~dynamic_array() {
        delete[] data_;
    }

    T get(int index) const {
        if (index < 0 || index >= size_) {
            throw index_out_of_range_exception("index out of range");
        }
        return data_[index];
    }

    void set(int index, const T& value) {
        if (index < 0 || index >= size_) {
            throw index_out_of_range_exception("index out of range");
        }
        data_[index] = value;
    }

    int get_size() const {
        return size_;
    }

    void resize(int new_size) {
        if (new_size <= 0) {
            delete[] data_;
            data_ = nullptr;
            size_ = 0;
            return;
        }
        T* new_data = new T[new_size];
        int copy_size = std::min(new_size, size_);
        for (int i = 0; i < copy_size; i++) {
            new_data[i] = data_[i];
        }
        delete[] data_;
        data_ = new_data;
        size_ = new_size;
    }

    T& operator[](int index) {
        if (index < 0 || index >= size_) {
            throw index_out_of_range_exception("index out of range");
        }
        return data_[index];
    }

    const T& operator[](int index) const {
        if (index < 0 || index >= size_) {
            throw index_out_of_range_exception("index out of range");
        }
        return data_[index];
    }
};

#endif
