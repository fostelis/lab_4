#ifndef STREAMS_H
#define STREAMS_H

#include <fstream>
#include <functional>
#include <string>
#include <vector>
#include "exceptions.h"
#include "sequence.h"
#include "lazy_sequence.h"

template<class T>
class read_only_stream {
private:
    std::vector<T> data_;
    std::size_t position_ = 0;
    bool opened_ = false;

public:
    read_only_stream() = default;

    explicit read_only_stream(const sequence<T>& seq) {
        for (int i = 0; i < seq.get_length(); ++i) data_.push_back(seq.get(i));
    }

    read_only_stream(const lazy_sequence<T>& seq, std::size_t count) {
        auto taken = seq.take(count);
        for (int i = 0; i < taken.get_length(); ++i) data_.push_back(taken.get(i));
    }

    read_only_stream(const std::string& file_name, std::function<T(const std::string&)> deserialize) {
        std::ifstream in(file_name);
        std::string line;
        while (std::getline(in, line)) data_.push_back(deserialize(line));
    }

    void open() { opened_ = true; }
    void close() { opened_ = false; }

    bool is_end_of_stream() const { return position_ >= data_.size(); }
    std::size_t get_position() const { return position_; }
    bool is_can_seek() const { return true; }
    bool is_can_go_back() const { return true; }

    std::size_t seek(std::size_t index) {
        if (index > data_.size()) throw index_out_of_range_exception("stream: invalid seek position");
        position_ = index;
        return position_;
    }

    T read() {
        if (!opened_) throw index_out_of_range_exception("stream: stream is not open");
        if (is_end_of_stream()) throw index_out_of_range_exception("stream: end of stream");
        return data_[position_++];
    }
};




template<class T>
class growing_file_read_stream {
private:
    std::string file_name_;
    std::function<T(const std::string&)> deserialize_;
    std::ifstream input_;
    std::size_t position_ = 0;
    bool opened_ = false;

public:
    growing_file_read_stream(std::string file_name, std::function<T(const std::string&)> deserialize)
        : file_name_(std::move(file_name)), deserialize_(std::move(deserialize)) {}

    void open() {
        if (opened_) {
            return;
        }
        input_.open(file_name_);
        if (!input_.is_open()) {
            throw index_out_of_range_exception("growing_file_stream: cannot open file");
        }
        opened_ = true;
    }

    void close() {
        if (input_.is_open()) {
            input_.close();
        }
        opened_ = false;
    }

    bool try_read(T& value) {
        if (!opened_) {
            throw index_out_of_range_exception("growing_file_stream: stream is not open");
        }
        std::string line;
        if (std::getline(input_, line)) {
            value = deserialize_(line);
            ++position_;
            return true;
        }
        input_.clear();
        return false;
    }

    T read() {
        T value{};
        if (!try_read(value)) {
            throw index_out_of_range_exception("growing_file_stream: no new data available");
        }
        return value;
    }

    std::size_t get_position() const { return position_; }
    bool is_can_seek() const { return false; }
    bool is_can_go_back() const { return false; }
};

template<class T>
class write_only_stream {
private:
    std::vector<T> data_;
    std::string file_name_;
    std::function<std::string(const T&)> serialize_;
    bool opened_ = false;

public:
    write_only_stream() = default;

    write_only_stream(std::string file_name, std::function<std::string(const T&)> serialize)
        : file_name_(std::move(file_name)), serialize_(std::move(serialize)) {}

    void open() { opened_ = true; }

    void close() {
        if (!file_name_.empty()) {
            std::ofstream out(file_name_);
            for (const auto& item : data_) out << serialize_(item) << '\n';
        }
        opened_ = false;
    }

    std::size_t write(const T& item) {
        if (!opened_) throw index_out_of_range_exception("stream: stream is not open");
        data_.push_back(item);
        return data_.size();
    }

    std::size_t get_position() const { return data_.size(); }
    const std::vector<T>& data() const { return data_; }
};

#endif
