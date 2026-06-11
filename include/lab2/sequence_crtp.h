#ifndef SEQUENCE_CRTP_H
#define SEQUENCE_CRTP_H

#include "sequence.h"

template<class T, class Derived>
class sequence_crtp : public sequence<T> {
public:
    Derived& self() {
        return static_cast<Derived&>(*this);
    }
    const Derived& self() const {
        return static_cast<const Derived&>(*this);
    }

    sequence<T>* append(const T& item) override {
        return self().append_impl(item);
    }
    sequence<T>* prepend(const T& item) override {
        return self().prepend_impl(item);
    }
    sequence<T>* insert_at(const T& item, int index) override {
        return self().insert_at_impl(item, index);
    }
    sequence<T>* concat(const sequence<T>& other) override {
        return self().concat_impl(other);
    }

protected:
    virtual sequence<T>* append_impl(const T& item) = 0;
    virtual sequence<T>* prepend_impl(const T& item) = 0;
    virtual sequence<T>* insert_at_impl(const T& item, int index) = 0;
    virtual sequence<T>* concat_impl(const sequence<T>& other) = 0;
};

#endif
