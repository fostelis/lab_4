#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <exception>
#include <string>

class index_out_of_range_exception : public std::exception {
private:
    std::string message_;
public:
    index_out_of_range_exception(const std::string& msg) : message_(msg) {}
    const char* what() const noexcept override {
        return message_.c_str();
    }
};

class empty_collection_exception : public std::exception {
private:
    std::string message_;
public:
    empty_collection_exception(const std::string& msg) : message_(msg) {}
    const char* what() const noexcept override {
        return message_.c_str();
    }
};

#endif
