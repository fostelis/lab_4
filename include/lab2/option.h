#ifndef OPTION_H
#define OPTION_H


template<class T>
class option {
private:
    T value_;
    bool has_value_;
public:
    option() : has_value_(false) {}
    explicit option(const T& val) : value_(val), has_value_(true) {}

    bool is_some() const {
        return has_value_;
    }
    bool is_none() const {
        return !has_value_;
    }
    const T& value() const {
        return value_;
    }

    static option<T> none() {
        return option<T>();
    }
    static option<T> some(const T& val) {
        return option<T>(val);
    }
};

#endif
