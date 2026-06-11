#ifndef LIST_SEQUENCE_H
#define LIST_SEQUENCE_H

#include "sequence.h"
#include "linked_list.h"


template<class T> class mutable_list_sequence;
template<class T> class immutable_list_sequence;

template<class T>
class list_sequence : public sequence<T> {
protected:
    linked_list<T>* items_;
    virtual list_sequence<T>* instance() = 0;

public:
    list_sequence() : items_(new linked_list<T>()) {}
    list_sequence(const T* items, int count) : items_(new linked_list<T>(items, count)) {}
    explicit list_sequence(const linked_list<T>& list) : items_(new linked_list<T>(list)) {}
    list_sequence(const list_sequence& other) : items_(new linked_list<T>(*other.items_)) {}
    virtual ~list_sequence() {
        delete items_;
    }

    T get_first() const override {
        return items_->get_first();
    }
    T get_last() const override {
        return items_->get_last();
    }
    T get(int index) const override {
        return items_->get(index);
    }


    list_sequence<T>* get_subsequence(int start, int end) const override;

    int get_length() const override {
        return items_->get_length();
    }

    virtual list_sequence<T>* append_impl(const T& item) = 0;
    virtual list_sequence<T>* prepend_impl(const T& item) = 0;
    virtual list_sequence<T>* insert_at_impl(const T& item, int index) = 0;

    list_sequence<T>* concat(const sequence<T>& other) override {
        list_sequence<T>* result = instance();
        for (int i = 0; i < other.get_length(); ++i) {
            result = result->append_impl(other.get(i));
        }
        return result;
    }

    sequence<T>* clone() const override = 0;
};

template<class T>
class mutable_list_sequence : public list_sequence<T> {
public:
    mutable_list_sequence() : list_sequence<T>() {}
    mutable_list_sequence(const T* items, int count) : list_sequence<T>(items, count) {}
    explicit mutable_list_sequence(const linked_list<T>& list) : list_sequence<T>(list) {}
    mutable_list_sequence(const list_sequence<T>& other) : list_sequence<T>(other) {}

    list_sequence<T>* append(const T& item) override {
        return append_impl(item);
    }
    list_sequence<T>* prepend(const T& item) override {
        return prepend_impl(item);
    }
    list_sequence<T>* insert_at(const T& item, int index) override {
        return insert_at_impl(item, index);
    }
    list_sequence<T>* clone() const override {
        return new mutable_list_sequence<T>(*this);
    }

protected:
    list_sequence<T>* instance() override {
        return this;
    }
    list_sequence<T>* append_impl(const T& item) override {
        this->items_->append(item);
        return this;
    }
    list_sequence<T>* prepend_impl(const T& item) override {
        this->items_->prepend(item);
        return this;
    }
    list_sequence<T>* insert_at_impl(const T& item, int index) override {
        this->items_->insert_at(item, index);
        return this;
    }
};

template<class T>
class immutable_list_sequence : public list_sequence<T> {
public:
    immutable_list_sequence() : list_sequence<T>() {}
    immutable_list_sequence(const T* items, int count) : list_sequence<T>(items, count) {}
    explicit immutable_list_sequence(const linked_list<T>& list) : list_sequence<T>(list) {}
    immutable_list_sequence(const list_sequence<T>& other) : list_sequence<T>(other) {}

    list_sequence<T>* append(const T& item) override {
        return append_impl(item);
    }
    list_sequence<T>* prepend(const T& item) override {
        return prepend_impl(item);
    }
    list_sequence<T>* insert_at(const T& item, int index) override {
        return insert_at_impl(item, index);
    }
    list_sequence<T>* clone() const override {
        return new immutable_list_sequence<T>(*this);
    }

protected:
    list_sequence<T>* instance() override {
        return new immutable_list_sequence<T>(*this);
    }
    list_sequence<T>* append_impl(const T& item) override {
        immutable_list_sequence<T>* copy = new immutable_list_sequence<T>(*this);
        copy->items_->append(item);
        return copy;
    }
    list_sequence<T>* prepend_impl(const T& item) override {
        immutable_list_sequence<T>* copy = new immutable_list_sequence<T>(*this);
        copy->items_->prepend(item);
        return copy;
    }
    list_sequence<T>* insert_at_impl(const T& item, int index) override {
        immutable_list_sequence<T>* copy = new immutable_list_sequence<T>(*this);
        copy->items_->insert_at(item, index);
        return copy;
    }
};


template<class T>
list_sequence<T>* list_sequence<T>::get_subsequence(int start, int end) const {
    linked_list<T>* sub = this->items_->get_sub_list(start, end);
    list_sequence<T>* result = new mutable_list_sequence<T>(*sub);
    delete sub;
    return result;
}

#endif
