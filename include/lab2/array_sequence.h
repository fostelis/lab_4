#ifndef ARRAY_SEQUENCE_H
#define ARRAY_SEQUENCE_H

#include "sequence.h"
#include "dynamic_array.h"
#include "linked_list.h"
#include "sequence_crtp.h"

template<class T> class mutable_array_sequence;
template<class T> class immutable_array_sequence;

template<class T>
class array_sequence : public sequence<T> {
protected:
    dynamic_array<T>* items_;
    virtual array_sequence<T>* instance() = 0;

public:
    array_sequence() : items_(new dynamic_array<T>()) {}
    array_sequence(const T* items, int count) : items_(new dynamic_array<T>(items, count)) {}
    explicit array_sequence(const dynamic_array<T>& arr) : items_(new dynamic_array<T>(arr)) {}
    explicit array_sequence(const linked_list<T>& list) : items_(new dynamic_array<T>()) {
        for (int i = 0; i < list.get_length(); ++i) {
            items_->resize(items_->get_size() + 1);
            items_->set(items_->get_size() - 1, list.get(i));
        }
    }
    array_sequence(const array_sequence& other) : items_(new dynamic_array<T>(*other.items_)) {}
    virtual ~array_sequence() {
        delete items_;
    }

    T get_first() const override {
        if (items_->get_size() == 0) {
            throw empty_collection_exception("empty");
        }
        return items_->get(0);
    }
    T get_last() const override {
        if (items_->get_size() == 0) {
            throw empty_collection_exception("empty");
        }
        return items_->get(items_->get_size() - 1);
    }
    T get(int index) const override {
        return items_->get(index);
    }

    array_sequence<T>* get_subsequence(int start, int end) const override;

    int get_length() const override {
        return items_->get_size();
    }

    virtual array_sequence<T>* append_impl(const T& item) = 0;
    virtual array_sequence<T>* prepend_impl(const T& item) = 0;
    virtual array_sequence<T>* insert_at_impl(const T& item, int index) = 0;

    array_sequence<T>* concat(const sequence<T>& other) override {
        array_sequence<T>* result = instance();
        int new_size = result->items_->get_size() + other.get_length();
        result->items_->resize(new_size);
        int pos = result->items_->get_size() - other.get_length();
        for (int i = 0; i < other.get_length(); ++i) {
            result->items_->set(pos + i, other.get(i));
        }
        return result;
    }
};

template<class T>
class mutable_array_sequence : public array_sequence<T> {
public:
    mutable_array_sequence() : array_sequence<T>() {}
    mutable_array_sequence(const T* items, int count) : array_sequence<T>(items, count) {}
    explicit mutable_array_sequence(const dynamic_array<T>& arr) : array_sequence<T>(arr) {}
    explicit mutable_array_sequence(const linked_list<T>& list) : array_sequence<T>(list) {}
    mutable_array_sequence(const array_sequence<T>& other) : array_sequence<T>(other) {}

    array_sequence<T>* append(const T& item) override {
        return append_impl(item);
    }
    array_sequence<T>* prepend(const T& item) override {
        return prepend_impl(item);
    }
    array_sequence<T>* insert_at(const T& item, int index) override {
        return insert_at_impl(item, index);
    }
    array_sequence<T>* clone() const override {
        return new mutable_array_sequence<T>(*this);
    }

protected:
    array_sequence<T>* instance() override {
        return this;
    }
    array_sequence<T>* append_impl(const T& item) override {
        this->items_->resize(this->items_->get_size() + 1);
        this->items_->set(this->items_->get_size() - 1, item);
        return this;
    }
    array_sequence<T>* prepend_impl(const T& item) override {
        return insert_at_impl(item, 0);
    }
    array_sequence<T>* insert_at_impl(const T& item, int index) override {
        if (index < 0 || index > this->items_->get_size()) {
            throw index_out_of_range_exception("index out of range");
        }
        int old = this->items_->get_size();
        this->items_->resize(old + 1);
        for (int i = old; i > index; --i) {
            this->items_->set(i, this->items_->get(i - 1));
        }
        this->items_->set(index, item);
        return this;
    }
};

template<class T>
class immutable_array_sequence : public array_sequence<T> {
public:
    immutable_array_sequence() : array_sequence<T>() {}
    immutable_array_sequence(const T* items, int count) : array_sequence<T>(items, count) {}
    explicit immutable_array_sequence(const dynamic_array<T>& arr) : array_sequence<T>(arr) {}
    explicit immutable_array_sequence(const linked_list<T>& list) : array_sequence<T>(list) {}
    immutable_array_sequence(const array_sequence<T>& other) : array_sequence<T>(other) {}

    array_sequence<T>* append(const T& item) override {
        return append_impl(item);
    }
    array_sequence<T>* prepend(const T& item) override {
        return prepend_impl(item);
    }
    array_sequence<T>* insert_at(const T& item, int index) override {
        return insert_at_impl(item, index);
    }
    array_sequence<T>* clone() const override {
        return new immutable_array_sequence<T>(*this);
    }

protected:
    array_sequence<T>* instance() override {
        return new immutable_array_sequence<T>(*this);
    }
    array_sequence<T>* append_impl(const T& item) override {
        auto* copy = new immutable_array_sequence<T>(*this);
        copy->items_->resize(copy->items_->get_size() + 1);
        copy->items_->set(copy->items_->get_size() - 1, item);
        return copy;
    }
    array_sequence<T>* prepend_impl(const T& item) override {
        return insert_at_impl(item, 0);
    }
    array_sequence<T>* insert_at_impl(const T& item, int index) override {
        if (index < 0 || index > this->items_->get_size()) {
            throw index_out_of_range_exception("index out of range");
        }
        auto* copy = new immutable_array_sequence<T>(*this);
        int old = copy->items_->get_size();
        copy->items_->resize(old + 1);
        for (int i = old; i > index; --i) {
            copy->items_->set(i, copy->items_->get(i - 1));
        }
        copy->items_->set(index, item);
        return copy;
    }
};

template<class T>
array_sequence<T>* array_sequence<T>::get_subsequence(int start, int end) const {
    if (start < 0 || end >= this->items_->get_size() || start > end) {
        throw index_out_of_range_exception("invalid indices");
    }
    T* sub = new T[end - start + 1];
    for (int i = start; i <= end; ++i) {
        sub[i - start] = this->items_->get(i);
    }
    array_sequence<T>* result = new mutable_array_sequence<T>(sub, end - start + 1);
    delete[] sub;
    return result;
}

template<class T>
class mutable_array_sequence_crtp : public sequence_crtp<T, mutable_array_sequence_crtp<T>> {
private:
    dynamic_array<T>* items_;
public:
    mutable_array_sequence_crtp() : items_(new dynamic_array<T>()) {}
    mutable_array_sequence_crtp(const T* items, int count) : items_(new dynamic_array<T>(items, count)) {}
    mutable_array_sequence_crtp(const mutable_array_sequence_crtp& other) : items_(new dynamic_array<T>(*other.items_)) {}
    ~mutable_array_sequence_crtp() {
        delete items_;
    }

    T get_first() const override {
        if (items_->get_size() == 0) {
            throw empty_collection_exception("empty");
        }
        return items_->get(0);
    }
    T get_last() const override {
        if (items_->get_size() == 0) {
            throw empty_collection_exception("empty");
        }
        return items_->get(items_->get_size() - 1);
    }
    T get(int index) const override {
        return items_->get(index);
    }
    mutable_array_sequence_crtp* get_subsequence(int s, int e) const override {
        if (s < 0 || e >= items_->get_size() || s > e) {
            throw index_out_of_range_exception("invalid indices");
        }
        T* sub = new T[e - s + 1];
        for (int i = s; i <= e; ++i) {
            sub[i - s] = items_->get(i);
        }
        auto* res = new mutable_array_sequence_crtp(sub, e - s + 1);
        delete[] sub;
        return res;
    }
    int get_length() const override {
        return items_->get_size();
    }

    sequence<T>* append_impl(const T& item) override {
        items_->resize(items_->get_size() + 1);
        items_->set(items_->get_size() - 1, item);
        return this;
    }
    sequence<T>* prepend_impl(const T& item) override {
        return insert_at_impl(item, 0);
    }
    sequence<T>* insert_at_impl(const T& item, int idx) override {
        if (idx < 0 || idx > items_->get_size()) {
            throw index_out_of_range_exception("index out of range");
        }
        int old = items_->get_size();
        items_->resize(old + 1);
        for (int i = old; i > idx; --i) {
            items_->set(i, items_->get(i - 1));
        }
        items_->set(idx, item);
        return this;
    }
    sequence<T>* concat_impl(const sequence<T>& other) override {
        int new_sz = items_->get_size() + other.get_length();
        items_->resize(new_sz);
        int pos = items_->get_size() - other.get_length();
        for (int i = 0; i < other.get_length(); ++i) {
            items_->set(pos + i, other.get(i));
        }
        return this;
    }
    sequence<T>* clone() const override {
        return new mutable_array_sequence_crtp(*this);
    }
};

template<class T>
class immutable_array_sequence_crtp : public sequence_crtp<T, immutable_array_sequence_crtp<T>> {
private:
    dynamic_array<T>* items_;
public:
    immutable_array_sequence_crtp() : items_(new dynamic_array<T>()) {}
    immutable_array_sequence_crtp(const T* items, int count) : items_(new dynamic_array<T>(items, count)) {}
    immutable_array_sequence_crtp(const immutable_array_sequence_crtp& other) : items_(new dynamic_array<T>(*other.items_)) {}
    ~immutable_array_sequence_crtp() {
        delete items_;
    }

    T get_first() const override {
        if (items_->get_size() == 0) {
            throw empty_collection_exception("empty");
        }
        return items_->get(0);
    }
    T get_last() const override {
        if (items_->get_size() == 0) {
            throw empty_collection_exception("empty");
        }
        return items_->get(items_->get_size() - 1);
    }
    T get(int index) const override {
        return items_->get(index);
    }
    immutable_array_sequence_crtp* get_subsequence(int s, int e) const override {
        if (s < 0 || e >= items_->get_size() || s > e) {
            throw index_out_of_range_exception("invalid indices");
        }
        T* sub = new T[e - s + 1];
        for (int i = s; i <= e; ++i) {
            sub[i - s] = items_->get(i);
        }
        auto* res = new immutable_array_sequence_crtp(sub, e - s + 1);
        delete[] sub;
        return res;
    }
    int get_length() const override {
        return items_->get_size();
    }

    sequence<T>* append_impl(const T& item) override {
        auto* copy = new immutable_array_sequence_crtp(*this);
        copy->items_->resize(copy->items_->get_size() + 1);
        copy->items_->set(copy->items_->get_size() - 1, item);
        return copy;
    }
    sequence<T>* prepend_impl(const T& item) override {
        return insert_at_impl(item, 0);
    }
    sequence<T>* insert_at_impl(const T& item, int idx) override {
        if (idx < 0 || idx > items_->get_size()) {
            throw index_out_of_range_exception("index out of range");
        }
        auto* copy = new immutable_array_sequence_crtp(*this);
        int old = copy->items_->get_size();
        copy->items_->resize(old + 1);
        for (int i = old; i > idx; --i) {
            copy->items_->set(i, copy->items_->get(i - 1));
        }
        copy->items_->set(idx, item);
        return copy;
    }
    sequence<T>* concat_impl(const sequence<T>& other) override {
        auto* result = new immutable_array_sequence_crtp(*this);
        int new_sz = result->items_->get_size() + other.get_length();
        result->items_->resize(new_sz);
        int pos = result->items_->get_size() - other.get_length();
        for (int i = 0; i < other.get_length(); ++i) {
            result->items_->set(pos + i, other.get(i));
        }
        return result;
    }
    sequence<T>* clone() const override {
        return new immutable_array_sequence_crtp(*this);
    }
};

#endif
