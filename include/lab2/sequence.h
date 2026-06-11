#ifndef SEQUENCE_H
#define SEQUENCE_H

#include "option.h"
#include "exceptions.h"
#include <functional>
#include <utility>
#include <algorithm>

template<class T>
class mutable_array_sequence;

template<class T>
class sequence {
public:
    virtual ~sequence() = default;

    virtual T get_first() const = 0;
    virtual T get_last() const = 0;
    virtual T get(int index) const = 0;
    virtual sequence<T>* get_subsequence(int start, int end) const = 0;
    virtual int get_length() const = 0;
    virtual sequence<T>* append(const T& item) = 0;
    virtual sequence<T>* prepend(const T& item) = 0;
    virtual sequence<T>* insert_at(const T& item, int index) = 0;
    virtual sequence<T>* concat(const sequence<T>& other) = 0;
    virtual sequence<T>* clone() const = 0;

    template<class T2>
    sequence<T2>* map(T2 (*func)(const T&)) {
        sequence<T2>* result = new mutable_array_sequence<T2>();
        for (int i = 0; i < get_length(); ++i) {
            result = result->append(func(get(i)));
        }
        return result;
    }
    template<class T2>
    sequence<T2>* map(std::function<T2(const T&)> func) {
        sequence<T2>* result = new mutable_array_sequence<T2>();
        for (int i = 0; i < get_length(); ++i) {
            result = result->append(func(get(i)));
        }
        return result;
    }
    sequence<T>* where(std::function<bool(const T&)> pred) {
        sequence<T>* result = new mutable_array_sequence<T>();
        for (int i = 0; i < get_length(); ++i) {
            if (pred(get(i))) {
                result = result->append(get(i));
            }
        }
        return result;
    }
    sequence<T>* where(bool (*pred)(const T&)) {
        sequence<T>* result = new mutable_array_sequence<T>();
        for (int i = 0; i < get_length(); ++i) {
            if (pred(get(i))) {
                result = result->append(get(i));
            }
        }
        return result;
    }
    template<class T2>
    T2 reduce(std::function<T2(const T2&, const T&)> func, const T2& init) {
        T2 res = init;
        for (int i = 0; i < get_length(); ++i) {
            res = func(res, get(i));
        }
        return res;
    }
    template<class T2>
    T2 reduce(T2 (*func)(const T2&, const T&), const T2& init) {
        T2 res = init;
        for (int i = 0; i < get_length(); ++i) {
            res = func(res, get(i));
        }
        return res;
    }
    option<T> try_get_first(std::function<bool(const T&)> pred = nullptr) {
        for (int i = 0; i < get_length(); ++i) {
            T item = get(i);
            if (!pred || pred(item)) {
                return option<T>::some(item);
            }
        }
        return option<T>::none();
    }
    template<class T2>
    sequence<std::pair<T, T2>>* zip(const sequence<T2>& other) const {
        sequence<std::pair<T, T2>>* result = new mutable_array_sequence<std::pair<T, T2>>();
        int min_len = std::min(get_length(), other.get_length());
        for (int i = 0; i < min_len; ++i) {
            result = result->append(std::make_pair(get(i), other.get(i)));
        }
        return result;
    }
    sequence<T>* skip(int count) const {
        if (count < 0) {
            count = 0;
        }
        if (count >= get_length()) {
            return new mutable_array_sequence<T>();
        }
        sequence<T>* result = new mutable_array_sequence<T>();
        for (int i = count; i < get_length(); ++i) {
            result = result->append(get(i));
        }
        return result;
    }
    sequence<T>* take(int count) const {
        if (count < 0) {
            count = 0;
        }
        if (count > get_length()) {
            count = get_length();
        }
        sequence<T>* result = new mutable_array_sequence<T>();
        for (int i = 0; i < count; ++i) {
            result = result->append(get(i));
        }
        return result;
    }
    template<class T2>
    sequence<T2>* map_with_index(T2 (*func)(const T&, int)) const {
        sequence<T2>* result = new mutable_array_sequence<T2>();
        for (int i = 0; i < get_length(); ++i) {
            result = result->append(func(get(i), i));
        }
        return result;
    }
    template<class T2>
    sequence<T2>* map_with_index(std::function<T2(const T&, int)> func) const {
        sequence<T2>* result = new mutable_array_sequence<T2>();
        for (int i = 0; i < get_length(); ++i) {
            result = result->append(func(get(i), i));
        }
        return result;
    }
    sequence<sequence<T>*>* split(std::function<bool(const T&)> delim) const {
        sequence<sequence<T>*>* result = new mutable_array_sequence<sequence<T>*>();
        sequence<T>* current = new mutable_array_sequence<T>();
        for (int i = 0; i < get_length(); ++i) {
            if (delim(get(i))) {
                result = result->append(current);
                current = new mutable_array_sequence<T>();
            }
            else {
                current = current->append(get(i));
            }
        }
        if (current->get_length() > 0) {
            result = result->append(current);
        }
        else {
            delete current;
        }
        return result;
    }
    sequence<T>* slice(int idx, int count, const sequence<T>* repl = nullptr) {
        if (idx < 0) {
            idx = get_length() + idx;
        }
        if (idx < 0 || idx > get_length()) {
            throw index_out_of_range_exception("invalid index");
        }
        if (count < 0) {
            count = 0;
        }
        sequence<T>* result = new mutable_array_sequence<T>();
        for (int i = 0; i < idx; ++i) {
            result = result->append(get(i));
        }
        if (repl) {
            for (int i = 0; i < repl->get_length(); ++i) {
                result = result->append(repl->get(i));
            }
        }
        for (int i = idx + count; i < get_length(); ++i) {
            result = result->append(get(i));
        }
        return result;
    }
};

template<class T>
bool operator==(const sequence<T>& lhs, const sequence<T>& rhs) {
    if (lhs.get_length() != rhs.get_length()) {
        return false;
    }
    for (int i = 0; i < lhs.get_length(); ++i) {
        if (lhs.get(i) != rhs.get(i)) {
            return false;
        }
    }
    return true;
}
template<class T>
bool operator!=(const sequence<T>& lhs, const sequence<T>& rhs) {
    return !(lhs == rhs);
}
template<class T>
sequence<T>* operator+(const sequence<T>& lhs, const sequence<T>& rhs) {
    sequence<T>* result = lhs.clone();
    sequence<T>* concat = result->concat(rhs);
    if (concat != result) {
        delete result;
    }
    return concat;
}

#endif
